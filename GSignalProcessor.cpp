//
//  GSignalProcessor.cpp
//  Ghanni
//  Copyright (c) 2012 Ghanni. All rights reserved.
//



#include "bcport.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "GSignalProcessor.h"

int myMel[15]={1,2,3,4,5,6,8,10,13,17,21,26,31,40};
//int myMel[15]={1,9,18,27,40};
int detailedMel[20]={240,360,480,600,720,840,1000,1150,1300,1450,1600,1800,2000,2200,2400,2700,3000,3300,3620,4100};

// *********************************************************************************************************
GSignalProcessor::GSignalProcessor(int inWinSize, int inWinStep)
{
    m_win_size = inWinSize;
    m_win_step = inWinStep;
    m_cnt_xbuf=0;
    if (m_win_size<=256)
    {
        //m_FFT = new FFTReal(256);
        m_win_size = 256;
    } else {
        //m_FFT = new FFTReal(512);
        m_win_size = 512;
    }
    g_fft = new GFFT(m_win_size);
    
    for (int i=0; i<GF_WORKBUF_SIZE; i++)
        m_fftbuf[i] = new double[m_win_size];
    /*
    for (int i=0; i<GF_SLIDING_NORM_SIZE; i++) {
        m_sliding_fft[i]= new double[m_win_size/2];
    }
     */
	
    // init signal Hamming window
    m_hamming = new double[m_win_size];
    for (int i=0; i<m_win_size; i++)
        m_hamming[i] = 0.54 - 0.46*cos(2*M_PI*(double)i/(double)(m_win_size-1));
	
    m_tmp_buf = new double[m_win_size];
    m_sliding_norm = new double[GF_SLIDING_NORM_SIZE];
    m_sliding_norm1 = new double[GF_SLIDING_NORM_SIZE];
    m_sliding_norm2 = new double[GF_SLIDING_NORM_SIZE];
    for (int i=0; i<GF_SLIDING_NORM_SIZE; i++)
    {
        m_sliding_norm[i] = 0.0;
        m_sliding_norm1[i] = 0.0;
        m_sliding_norm2[i] = 0.0;
    }
    
    m_sliding_norm_ptr = 0;
    m_sliding_norm_over = false;
    
    /*
    for (int i=0; i<13; i++) m_filt[i]=0.003;
    for (int i=13; i<113; i++) m_filt[i]=1;
    for (int i=113; i<200; i++) m_filt[i]=0.003;
    */
    for (int i=0; i<6-3; i++) m_filt[i]=0.003;
    for (int i=6-3; i<213; i++) m_filt[i]=1;
    for (int i=213; i<256; i++) m_filt[i]=0.003;

    for (int i=0; i<GF_MEL; i++) m_center[i]=detailedMel[i];
    m_left[0]=180;
    for(int i=0;i<GF_MEL;i++)
    {
        m_left[i+1]=m_center[i];
    }
    for(int i=0;i<GF_MEL;i++)
    {
        m_right[i]=(2*m_center[i])-m_left[i];
    }

}

// *********************************************************************************************************

GSignalProcessor::~GSignalProcessor()
{
    delete m_sliding_norm1;
    delete m_sliding_norm2;
    delete m_sliding_norm;
    delete m_tmp_buf;
    delete m_hamming;
    for (int i=0; i<GF_WORKBUF_SIZE; i++)
        delete m_fftbuf[i];
    /*
    for (int i=0; i<GF_SLIDING_NORM_SIZE; i++) {
        delete m_sliding_fft[i];
    }
     */
    //delete m_FFT;
    delete g_fft;
}

// *********************************************************************************************************

bool GSignalProcessor::MakeFFTFromData(short* data, int dataLen, int dataPos, double* out)
{
    if (!data || dataLen-dataPos<m_win_size || !out) return false;
    //double tmp = 0.0;
    for (int i=0; i<m_win_size; i++)
        m_tmp_buf[i] = (double)data[i+dataPos]*m_hamming[i];
    
    //m_FFT->do_fft(out, m_tmp_buf);
    g_fft->make_fft(out, m_tmp_buf);
    int j = m_win_size/2;
    for (int i=0; i<j; i++)
    {
        out[i]/=(double)m_win_size;
        out[i+j]/=(double)m_win_size;
        if (fabs(out[i])>0.01 && fabs(out[i+j])>0.01)
        {
            out[i+j] = (out[i]*out[i]+out[i+j]*out[i+j]);
            out[i] = sqrt(out[i+j]);
        }
        else
        {
            out[i] = 0.0;
            out[i+j] = 0.0;
        }
    }
    
    /*
    if (j==256) {
        for (int i=0; i<2*j; i+=2) {
            tmp = out[i]+out[i+1];
            out[i/2] = tmp;
        }
    }
     */
    m_sliding_norm_ptr++;
    if (m_sliding_norm_ptr>=GF_SLIDING_NORM_SIZE) { m_sliding_norm_ptr = 0; m_sliding_norm_over = true; }
    
    double norm = 0.0, norm1 = 0.0, norm2 = 0.0;
    for (int i=0; i<j; i++)
    {
        norm+=out[i+j];
        norm1+=pow(m_filt[i]*out[i],2);
        if (out[i]>norm2) {
            norm2 = out[i];
        }
        //m_sliding_fft[m_sliding_norm_ptr][i]=out[i];
    }
    m_sliding_norm[m_sliding_norm_ptr] = norm;
    m_sliding_norm1[m_sliding_norm_ptr] = norm1;
    m_sliding_norm2[m_sliding_norm_ptr] = norm2;
    
    return true;
}


// *********************************************************************************************************

bool GSignalProcessor::MakeXFromFFT(double *fftbuf, TSpectVector* out)
{
    // Basic spectral vector computation
    if (!fftbuf || !out) return false;
    int j = m_win_size/2;
    double norm = 0.0, norm1 = 0.0, norm2 = 0.0, norm3 = 0.0;// norm4 = 0.0;
    for (int i=0; i<GF_SLIDING_NORM_SIZE; i++)
    {
        norm += m_sliding_norm[i];
        norm1 += m_sliding_norm1[i];
        if (m_sliding_norm2[i]>norm2) {
            norm2 = m_sliding_norm2[i];
        }
    }
    /*
    for (int k=0; k<j; k++) {
        for (int i=0; i<GF_SLIDING_NORM_SIZE; i++)
        {
            norm4 += m_sliding_fft[i][k];
        }
        norm4 /=GF_SLIDING_NORM_SIZE;
        fftbuf[k] -= norm4;
        norm4 = 0;
    }
     */
    double ratio = (norm1>0.0001)? norm/norm1 : 1.0;
    double ratio2 = (norm2>0.0001)? norm2 : 1.0;
    //ratio2 = 1.0;
    //    printf("%f ",ratio);
    for (int i=0; i<j; i++)
        fftbuf[i] = ratio * fftbuf[i] * m_filt[i];

    for (int i=0; i<j; i++)
        norm3 += fftbuf[i] * fftbuf[i];

    if(sqrt(norm3)>1) norm3 = log(sqrt(norm3));
    else norm3 =0.00001;
    (*out).E = norm3;

    for (int i=0; i<j; i++)
        fftbuf[i] = fftbuf[i]/ratio2;
	
    for (int k=0; k<GF_PASSFREQ; k++)
    {
        float tmp = 0.0f;
        for (int kk = 3*k; kk<3*(k+1); kk++)
            tmp+=fftbuf[kk+0*6];
        (*out).X[k] = tmp;
    }

    
    // Bandwidth and spectral centroid computation
    float a=0,b=0;
    for(int k=0;k<GF_PASSFREQ;k++)
    {
        a+=k*(*out).X[k];
        b+=(*out).X[k];
    }
    if(b<0.001)b=0.001;
    (*out).Sc=a/b;
    a=b=0;
    for(int k=0;k<GF_PASSFREQ;k++)
    {
        a+=pow((k-(*out).Sc),2)*(*out).X[k];
        b+=(*out).X[k];
    }
    if(b<0.001)b=0.001;
    (*out).Ba=a/b;
    
    
    // MFCC computation
    float buf1=0;
    for(int j=0;j<GF_MFSC;j++)
    {
        for(int b=myMel[j];b<myMel[j+1];b++)
        {
            buf1+=(*out).X[b];
        }
        (*out).MFSC[j]=buf1;
        buf1=0;
    }
    
    int l,c;
    double total,deltaf;
    deltaf=2*15.625;
    for(int i=0;i<GF_MEL;i++)
    {
        total=0;
        l = (m_left[i]/deltaf)+1;
        c = m_center[i]/deltaf;
        
        for(int j=l;j<c+1;j++)
        {
            total = total+(fftbuf[j]*((j*deltaf)-m_left[i])/(m_center[i]-m_left[i]));
        }
        l = (m_center[i]/deltaf)+1;
        c =m_right[i]/deltaf;
        if(c>127) c=127;
        for(int j=l;j<c+1;j++)
        {
            total = total+(fftbuf[j]*((j*deltaf)-m_right[i])/(m_center[i]-m_right[i]));
        }
        (*out).MEL[i]=total;
        
    }
    
    double temp,temp2,pin;
    pin=3.14159265359;
	
    for(int i=0;i<GF_MFCC;i++)
    {
        temp=0;
        //for(int j=0;j<GF_MFSC;j++)
        for(int j=0;j<GF_MFSC;j++)
        {
            //temp2=pin*i/GF_MFSC;
            temp2=pin*i/GF_MFSC;
            temp2=temp2*(j-0.5);
            temp2=cos(temp2);
            if((*out).MFSC[j]>0)
            {temp=temp+(log((*out).MFSC[j])*temp2);}
            //if((*out).MEL[j]>0)
            //{temp=temp+(log((*out).MEL[j])*temp2);}
        }
        //temp=temp*sqrt(2/double(GF_MFSC));
        temp=temp*sqrt(2/double(GF_MFSC));
        (*out).MFCC[i]=temp;
    }
    
    
    
    return true;
}


// *********************************************************************************************************
/*
void GSignalProcessor::NormalizeX(int inWinSize)
{
    float maxf = 0;
    for(int s=0;s<GF_PASSFREQ;s++)
    {
        for(int n=0;n<m_cnt_xbuf/inWinSize;n+=4)
        {
            for(int l=n*inWinSize;l<(n+4)*inWinSize+1;l++)
            {
                if(m_xbuf[l].X[s]>maxf) {maxf=m_xbuf[l].X[s];}
            }
            
            for(int l=n*inWinSize;l<(n+4)*inWinSize+1;l++)
            {
                m_xbuf[l].X[s]=m_xbuf[l].X[s]/maxf;
            }
            maxf=0;
        }
    }
}
*/

// *********************************************************************************************************

int GSignalProcessor::TakePossibleData(short* data, int len)
{
    if (!data || len<m_win_size) return 0;
    
    int np = (len-m_win_size+m_win_step)/m_win_step;
    if (np > GF_WORKBUF_SIZE-m_cnt_xbuf) np = GF_WORKBUF_SIZE-m_cnt_xbuf;
    if (np<=0) return 0;
    
    if (!m_sliding_norm_over)
    {
        for (int i=0; i<np; i++)
            MakeFFTFromData(data, len, i*m_win_step, m_fftbuf[i]);
        for (int i=0; i<np; i++)
            MakeXFromFFT(m_fftbuf[i], &(m_xbuf[m_cnt_xbuf++]));
    } else {
        for (int i=0; i<np; i++)
        {
            MakeFFTFromData(data, len, i*m_win_step, m_fftbuf[i]);
            MakeXFromFFT(m_fftbuf[i], &(m_xbuf[m_cnt_xbuf++]));
        }
    }
    /*int a = TakePossibleX();
    if (a>0 && a<=m_cnt_xbuf)
    {
        memmove(&(m_xbuf[0]),&(m_xbuf[a]),sizeof(TSpectVector)*(m_cnt_xbuf-a));
        m_cnt_xbuf-=a;
    }*/
    return np*m_win_step;
}

// *********************************************************************************************************

int GSignalProcessor::get_cntxbuf()
{
    return m_cnt_xbuf;
}

// *********************************************************************************************************

int GSignalProcessor::WriteFeaturesToFile(char* name)
{
    AnsiString brk="\n",fpext=".arff";
    AnsiString FileNamefp=AnsiString(name)+fpext;
    AnsiString txt = "";
    
    FILE* f = fopen(FileNamefp.c_str(),"w+");
    if(!f)
        return 0;
    
    for(int s=0;s<m_cnt_xbuf;s++)
    {
        for (int i = 0; i<GF_MFCC; i++) {
            txt = txt + AnsiString(m_xbuf[s].MFCC[i])+",";
        }
        txt = txt + "\n";
        fputs(txt.c_str(), f);
        txt = "";
    }
    
    fclose(f);
    return 1;
}

// *********************************************************************************************************



