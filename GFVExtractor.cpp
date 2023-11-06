//
//  GFPExtactor
//  Ghanni FingerPrint extractor class
//
//  Copyright (c) 2012 Ghanni. All rights reserved.
//



#include <math.h>
#include <stdio.h>
#include <string.h>

#include "GFVExtractor.h"


// *********************************************************************************************************

GFVExtractor::GFVExtractor(int inIntWinSize, int inFracUsed, float inOverlap)
{
    
    m_intwin_size = inIntWinSize;
    m_frac_used = inFracUsed;
    m_intwin_overlap = (int)(inOverlap*m_intwin_size);
    m_cnt_featbuf = 0;
    
// init ITW Hamming window
    m_ITWhamming = new float[m_intwin_size + 2*m_intwin_overlap];
    m_intwin_total = m_intwin_size + 2*m_intwin_overlap;
    for (int i=0; i<m_intwin_total; i++)
    {
	if (i>=m_intwin_overlap && i<m_intwin_size+m_intwin_overlap)
	    m_ITWhamming[i] = 1.0f;
	else
	    m_ITWhamming[i] = 0.54f - 0.46*cos(2*M_PI*(float)i / (float)(m_intwin_total-1));
    }
	 
}

// *********************************************************************************************************

GFVExtractor::GFVExtractor()
{
    
    m_intwin_size = GF_ITW_SIZE;
    m_frac_used = GF_ITW_FRAC;
    m_intwin_overlap = (int)(GF_ITW_OVERLAP*m_intwin_size);
    m_cnt_featbuf = 0;
    
    // init ITW Hamming window
    m_ITWhamming = new float[m_intwin_size + 2*m_intwin_overlap];
    m_intwin_total = m_intwin_size + 2*m_intwin_overlap;
    for (int i=0; i<m_intwin_total; i++)
    {
        if (i>=m_intwin_overlap && i<m_intwin_size+m_intwin_overlap)
            m_ITWhamming[i] = 1.0f;
        else
            m_ITWhamming[i] = 0.54f - 0.46*cos(2*M_PI*(float)i / (float)(m_intwin_total-1));
    }
    
}

// *********************************************************************************************************

GFVExtractor::~GFVExtractor()
{
    delete m_ITWhamming;
}

// *********************************************************************************************************

int GFVExtractor::get_cntfbuf()
{
    return m_cnt_featbuf;
}

// *********************************************************************************************************

bool GFVExtractor::MakeFeatureVectorFromX(TSpectVector* inBuf, int inBufSize, int inPos, TFeatureVector* out)
{
    if (inBufSize-inPos < m_intwin_total || !out || !inBuf) return false;
    
    float mean, var;
    for (int k=0; k<GF_PASSFREQ; k++)
    {
        mean = var = 0.0f;
        for (int l=inPos; l<inPos+m_intwin_total; l+=1)
        {
            float tmp = inBuf[l].X[k]*m_ITWhamming[l-inPos];
            mean += tmp;
            var += tmp*tmp;
        }
        out->mean[k] = mean / m_intwin_size;
        out->var[k] = (var / m_intwin_size) - pow (out->mean[k],2);
    }
    for (int k=0; k<GF_MFSC; k++)
    {
        mean = var = 0.0f;
        for (int l=inPos; l<inPos+m_intwin_total; l+=1)
        {
            float tmp = inBuf[l].MFSC[k]*m_ITWhamming[l-inPos];
            mean += tmp;
            var += tmp*tmp;
        }
        out->mMFSC[k] = mean / m_intwin_size;
        out->vMFSC[k] = (var / m_intwin_size) - pow (out->mMFSC[k],2);
    }
    for (int k=0; k<GF_MFCC; k++)
    {
        mean = var = 0.0f;
        for (int l=inPos; l<inPos+m_intwin_total; l+=1)
        {
            float tmp = inBuf[l].MFCC[k]*m_ITWhamming[l-inPos];
            mean += tmp;
            var += tmp*tmp;
        }
        out->mMFCC[k] = mean / m_intwin_size;
        out->vMFCC[k] = (var / m_intwin_size) - pow (out->mMFCC[k],2);
    }
    for (int k=0; k<GF_MEL; k++)
    {
        mean = var = 0.0f;
        for (int l=inPos; l<inPos+m_intwin_total; l+=1)
        {
            float tmp = inBuf[l].MEL[k]*m_ITWhamming[l-inPos];
            mean += tmp;
            var += tmp*tmp;
        }
        out->mMEL[k] = mean / m_intwin_size;
        out->vMEL[k] = (var / m_intwin_size) - pow (out->mMEL[k],2);
    }
    mean = var = 0.0f;
    for (int l=inPos; l<inPos+m_intwin_total; l+=1)
    {
        float tmp = inBuf[l].Ba*m_ITWhamming[l-inPos];
        mean += tmp;
        var += tmp*tmp;
    }
    out->mBa = mean / m_intwin_size;
    out->vBa = (var / m_intwin_size) - pow (out->mBa,2);
    
    mean = var = 0.0f;
    for (int l=inPos; l<inPos+m_intwin_total; l+=1)
    {
        float tmp = inBuf[l].Sc*m_ITWhamming[l-inPos];
        mean += tmp;
        var += tmp*tmp;
    }
    out->mSc = mean / m_intwin_size;
    out->vSc = (var / m_intwin_size) - pow (out->mSc,2);

    mean = var = 0.0f;
    for (int l=inPos; l<inPos+m_intwin_total; l+=1)
    {
        float tmp = inBuf[l].E*m_ITWhamming[l-inPos];
        mean += tmp;
        var += tmp*tmp;
    }
    out->mE = mean / m_intwin_size;
    out->vE = (var / m_intwin_size) - pow (out->mE,2);
    
    return true;
}

// *********************************************************************************************************

int GFVExtractor::TakePossibleX(GSignalProcessor* gsignal)
{
    int m_cnt_xbuf=gsignal->get_cntxbuf();
    
    if (m_cnt_xbuf<m_intwin_total) return 0;
    int np = (m_cnt_xbuf-m_intwin_total+m_frac_used)/m_frac_used;
    if (np > GF_WORKBUF_SIZE-m_cnt_featbuf) np = GF_WORKBUF_SIZE-m_cnt_featbuf;
    if (np<=0) return 0;

    for (int i=0; i<np; i++)
    {
        MakeFeatureVectorFromX(gsignal->m_xbuf, m_cnt_xbuf, i*m_frac_used, &(m_featbuf[m_cnt_featbuf++]));
    }
    //int a = TakePossibleFV();
    /*
    int a = np;
    if (a>0 && a<=m_cnt_featbuf)
    {
        memmove(&(m_featbuf[0]),&(m_featbuf[a]),sizeof(TFeatureVector)*(m_cnt_featbuf-a));
        m_cnt_featbuf-=a;
    }
     */
    return np*m_frac_used;
}

// *********************************************************************************************************
float GFVExtractor::get_E()
{
    float buf = 0.0;
    for(int i=0;i<m_cnt_featbuf;i++)
    {
        buf += m_featbuf[i].mE;
    }
    return buf/(m_cnt_featbuf+1);
}

// *********************************************************************************************************

int GFVExtractor::TakeWavFile(char* name) // 16 bits, 8KHz or 16KHz mono
{
    FILE *f;
    char nbchannel, bitres;
    int freq, size, l;
    unsigned char header[44];
    
    
    f=fopen(name,"rb");
    if(!f)
        return 0;
    
    fread(header,1,44,f);
    nbchannel=header[22];
    bitres=header[34];
    freq=header[24]+256*header[25];
    size=16777216*header[7]+65536*header[6]+256*header[5]+header[4];
    size=size-44;
    l = int(size/2);
    //len=int((1000/float(freq))*(size/(nbchannel*(bitres/8)))); //in ms
    
    //nbperwindow=freq*0.032;
    //nbslide=freq*0.01;
    //minstart=11; //in ms
    //maxstart=len-21; //in ms
    
    if (bitres != 16 || nbchannel != 1 || (freq != 8000 && freq != 16000)) {
        return 0;
    }
    
    int BS = 300000;
    short* buf = new short[BS];
    int lptr = 0;
    int t = 0;
    GSignalProcessor* signalprocessor=NULL;
     // to change 256, 80 into an auto mode

    while (l>0 || t>0)
    {
        int r = (l>(BS-lptr))?(BS-lptr):l;
        l-=r;
        fread(buf,2,r,f);
        if (freq == 16000) {
            signalprocessor= new GSignalProcessor(512,160);
        }
        else {
            signalprocessor= new GSignalProcessor(256,80);
        }
        
        t = signalprocessor->TakePossibleData(buf, r);
        if (t==0) {
            l=-1;
        }
        TakePossibleX(signalprocessor);
        delete signalprocessor;
        memmove(buf, buf+t, (lptr+r-t)*2);
        lptr = (lptr+r-t);
    }
    delete buf;
    
    fclose(f);
    return 1;
}
