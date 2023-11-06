//  Copyright (c) 2012 Ghanni. All rights reserved.
//



#include "HHNN.h"
#include "math.h"
#include <iostream>
#include <stdarg.h>

bool notsaved=true;
// *********************************************************************************************************

NNoutput *HHNN::getAverage(int n, ...)
{
    NNoutput *NNout, *current;
    int winner = -1;
    NNout = new NNoutput;
    current = new NNoutput;
    float maxx=0;
    
    
    va_list vl;
    va_start(vl,n);

    for(int i=1;i<out+1;i++)
    {
        NNout->semantic[i] = 0;
    }
    
    for(int j= 0; j<n; j++)
    {
        current = va_arg(vl, NNoutput*);
        for (int i=1;i<out+1;i++) {
            NNout->semantic[i] += current->semantic[i];
        }
    }
    for(int i=1;i<out+1;i++)
    {
        NNout->semantic[i] /= n;
    }
    
    va_end(vl);
    
    for(int i=1;i<out+1;i++)
    {
        if(NNout->semantic[i]>maxx)
        {
            maxx=NNout->semantic[i];
            winner=i;
        }
    }
    NNout->winnerIndex=winner;
    return NNout;
}

/*
NNoutput *HHNN::getAverage(NNoutput *n1, NNoutput *n2, NNoutput *n3, NNoutput *n4)
{
    NNoutput *NNout;
    int winner = -1;
    NNout = new NNoutput;
    float maxx=0;
    for(int i=1;i<out+1;i++)
    {
        NNout->semantic[i] = (n1->semantic[i]+n2->semantic[i]+n3->semantic[i]+n4->semantic[i])/4;
    }
    
    for(int i=1;i<out+1;i++)
    {
        if(NNout->semantic[i]>maxx)
        {
            maxx=NNout->semantic[i];
            winner=i;
        }
    }
    NNout->winnerIndex=winner;
    return NNout;
}
*/
// *********************************************************************************************************

HHNN::HHNN()
{
    in=80;hid=50;out=2;numinst=80;
    for(int i=0;i<500;i++){
        for(int j=0;j<500;j++){
            w[i][j]=0;
        }
    }
    for (int i = 0; i<GF_MAXCLASSES; i++) {
        strcpy(classnames[i], "");
    }
}

// *********************************************************************************************************

HHNN::HHNN(int In, int Hid, int Out)
{
    in=In;hid=Hid;out=Out;numinst=80;
    for(int i=0;i<500;i++){
        for(int j=0;j<500;j++){
            w[i][j]=0;
        }
    }
    for (int i = 0; i<GF_MAXCLASSES; i++) {
        strcpy(classnames[i], "");
    }
}

// *********************************************************************************************************

float HHNN::f(float a)
{
 float ex=0;
 if (-a<20) {
    ex=1/(1+exp(-a));
 }
 return ex;
}

// *********************************************************************************************************

void HHNN::output(int n)
{
 int j;
 o[in+1]=-1;

 for (j=1;j<in+1;j++)
  {
   o[j]=(xinput[j][n]);
  }
 for (j=in+2;j<in+hid+out+1;j++)
  {
   float buff=0;
   teta[j]=-w[j][in+1];
   for (int i=1;i<j;i++)
    {
     buff=buff+w[j][i]*o[i];
     }
    o[j]=this->f(buff-teta[j]);
   }
  for(int i=1;i<out+1;i++)
  {lbl[n][i]=o[in+hid+i];}

 }

// *********************************************************************************************************

void HHNN::delta(int m)
{

 for(int i=1;i<out+1;i++)
 {d[in+hid+i]=o[in+hid+i]*(1-o[in+hid+i])*(t[m][i]-o[in+hid+i]);}

 for(int j=in+hid;j>1;j--)
  {
    float buffer=0;
    for (int k=j+1;k<in+hid+out+1;k++)
     {
      buffer=buffer+d[k]*w[k][j];
     }
     d[j]=o[j]*(1-o[j])*buffer;
    }
}

// *********************************************************************************************************

void HHNN::weight(float beta)
{
  int i,j;
  for(i=1;i<in+1;i++)
  {
   for(j=i+in;j<in+hid+out+1;j++)
    {
     w[j][i] = w[j][i] + beta* d[j] * o[i];
     }
   }
   for(i=in+1;i<in+hid+1;i++)
   {
    for(j=i+1;j<in+hid+out+1;j++)
    {
     w[j][i] = w[j][i] + beta* d[j] * o[i];
     }
    }
}

// *********************************************************************************************************

void HHNN::initlearn(int offset, int numberOfSpeakers, GFVExtractor **Pgm, AnsiString *classNames, int type)
{
    numinst=offset*numberOfSpeakers;
    switch (type) {
        case TYPE_MFCC:
            in = 2*GF_MFCC;
            break;
        case TYPE_MFSC:
           in = 2*GF_MFSC;
            break;
        case TYPE_MEL:
            in = 2*GF_MEL;
            break;
        case TYPE_PGM:
            in = 2*GF_PASSFREQ;
            break;
        case TYPE_SCBA:
            in = 6;
            break;
        case TYPE_ALL:
            in = 2*GF_MFCC+2*GF_MFSC+2*GF_MEL+2*GF_PASSFREQ;
            break;
        default:
            in = 2*GF_PASSFREQ;
            break;
    }
    
    for(int i=1;i<in+hid+1;i++)
    {
        for(int j=i+1;j<in+hid+out+1;j++)
        {
            w[j][i]=0.03;
        }
    }
    for(int i=1;i<numberOfSpeakers+1;i++)
    {
        for(int k=1;k<offset+1;k++)
        {
            t[k+(i-1)*offset][i]=1;
        }
    }
    for(int i=1;i<numberOfSpeakers+1;i++)
    {
        for(int k=1;k<offset+1;k++)
        {
            for(int f=1;f<in/2+1;f++)
            {
                switch (type) {
                    case TYPE_MFCC:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMFCC[f-1];
                        break;
                    case TYPE_MFSC:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMFSC[f-1];
                        break;
                    case TYPE_MEL:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMEL[f-1];
                        break;
                    case TYPE_PGM:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mean[f-1];
                        break;
                    default:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mean[f-1];
                        break;
                }
            }
            for(int f=in/2+1;f<in+1;f++)
            {
                xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMFCC[f-in/2-1];
                switch (type) {
                    case TYPE_MFCC:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMFCC[f-in/2-1];
                        break;
                    case TYPE_MFSC:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMFSC[f-in/2-1];
                        break;
                    case TYPE_MEL:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMEL[f-in/2-1];
                        break;
                    case TYPE_PGM:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].var[f-in/2-1];
                        break;
                    default:
                        xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].var[f-in/2-1];
                        break;
                }
            }
            if (type == TYPE_PGM) {
                xinput[in+1][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mE/10;
                xinput[in+2][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vE/10;
            }
            if (type == TYPE_SCBA) {
                xinput[1][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mSc/GF_PASSFREQ;
                xinput[2][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vSc/(10*GF_PASSFREQ);
                xinput[3][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mBa/(2*GF_PASSFREQ);
                xinput[4][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vBa/(10*GF_PASSFREQ);
                xinput[5][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mE/10;
                xinput[6][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vE/10;
            }
            if (type == TYPE_ALL) {
                for(int f=1;f<GF_PASSFREQ+1;f++)
                {
                    xinput[f][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mean[f-1];
                    xinput[f+GF_PASSFREQ][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].var[f-1];
                }
                for(int f=1;f<GF_MEL+1;f++)
                {
                    xinput[f+2*GF_PASSFREQ][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMEL[f-1];
                    xinput[f+2*GF_PASSFREQ+GF_MEL][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMEL[f-1];
                }
                for(int f=1;f<GF_MFSC+1;f++)
                {
                    xinput[f+2*GF_PASSFREQ+2*GF_MEL][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMFSC[f-1];
                    xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMFSC[f-1];
                }
                for(int f=1;f<GF_MFCC+1;f++)
                {
                    xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].mMFCC[f-1];
                    xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][k+(i-1)*offset]=Pgm[i-1]->m_featbuf[k-1].vMFCC[f-1];
                }
            }
            
        }
    }
    if (type == TYPE_PGM) {
        in += 2;
    }
    if (type == TYPE_MFCC||type == TYPE_MEL||type == TYPE_MFSC||type == TYPE_PGM) {
        for(int wi=1;wi<numinst+1;wi++)
        {
            maxxs=0;
            for(int f=1;f<in/2+1;f++)
            {
                if(fabs(xinput[f][wi])>maxxs)maxxs=fabs(xinput[f][wi]);
            }
            for(int f=1;f<in/2+1;f++)
            {
                xinput[f][wi]/=maxxs+0.0000000001;
            }
            maxxs=0;
            
            for(int f=in/2+1;f<in+1;f++)
            {
                if(fabs(xinput[f][wi])>maxxs)maxxs=fabs(xinput[f][wi]);
            }
            for(int f=in/2+1;f<in+1;f++)
            {
                xinput[f][wi]/=maxxs+0.0000000001;
            }
        }
    }
    
    if (type == TYPE_ALL) {
        float maxm = 0;
        float maxv = 0;
        for(int wi=1;wi<numinst+1;wi++)
        {
            maxm=maxv=0;
            for(int f=1;f<GF_PASSFREQ+1;f++)
            {
                if(fabs(xinput[f][wi])>maxm)maxm=fabs(xinput[f][wi]);
                if(fabs(xinput[f+GF_PASSFREQ][wi])>maxv)maxv=fabs(xinput[f+GF_PASSFREQ][wi]);
            }
            for(int f=1;f<GF_PASSFREQ+1;f++)
            {
                xinput[f][wi]/=maxm+0.0000000001;
                xinput[f+GF_PASSFREQ][wi]/=maxv+0.0000000001;
            }
            
            maxm=maxv=0;
            for(int f=1;f<GF_MEL+1;f++)
            {
                if(fabs(xinput[f+2*GF_PASSFREQ][wi])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ][wi]);
                if(fabs(xinput[f+2*GF_PASSFREQ+GF_MEL][wi])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+GF_MEL][wi]);
            }
            for(int f=1;f<GF_MEL+1;f++)
            {
                xinput[f+2*GF_PASSFREQ][wi]/=maxm+0.0000000001;
                xinput[f+2*GF_PASSFREQ+GF_MEL][wi]/=maxv+0.0000000001;
            }
            
            maxm=maxv=0;
            for(int f=1;f<GF_MFSC+1;f++)
            {
                if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL][wi])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL][wi]);
                if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][wi])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][wi]);
            }
            for(int f=1;f<GF_MFSC+1;f++)
            {
                xinput[f+2*GF_PASSFREQ+2*GF_MEL][wi]/=maxm+0.0000000001;
                xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][wi]/=maxv+0.0000000001;
            }
            
            maxm=maxv=0;
            for(int f=1;f<GF_MFCC+1;f++)
            {
                if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][wi])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][wi]);
                if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][wi])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][wi]);
            }
            for(int f=1;f<GF_MFCC+1;f++)
            {
                xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][wi]/=maxm+0.0000000001;
                xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][wi]/=maxv+0.0000000001;
            }
        }
    }
/*
    for(int wi=1;wi<numinst+1;wi++)
    {
        maxxs=0;
        for(int f=1;f<in/2+1;f++)
        {
            if(fabs(xinput[f][wi])>maxxs)maxxs=fabs(xinput[f][wi]);
        }
        for(int f=1;f<in/2+1;f++)
        {
            xinput[f][wi]/=maxxs+0.0000000001;
        }
        maxxs=0;
        
        for(int f=in/2+1;f<in+1;f++)
        {
            if(fabs(xinput[f][wi])>maxxs)maxxs=fabs(xinput[f][wi]);
        }
        for(int f=in/2+1;f<in+1;f++)
        {
            xinput[f][wi]/=maxxs+0.0000000001;
        }
    }
*/
    for (int i = 1; i<numberOfSpeakers+1; i++) {
        strcpy(classnames[i], classNames[i-1].c_str());
    }
}

// *********************************************************************************************************

NNoutput * HHNN::getOutput(int wi, GFVExtractor *pgm, int type)
{
    float *semantic; int winner=-1;
    NNoutput * NNout;
    NNout=new NNoutput;
    semantic=new float[out+1];
    int newin = in;
    if (type == TYPE_PGM) {
        newin = in - 2;
    }
     for(int f=1;f<newin/2+1;f++)
     {
         switch (type) {
             case TYPE_MFCC:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].mMFCC[f-1];
                 break;
             case TYPE_MFSC:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].mMFSC[f-1];
                 break;
             case TYPE_MEL:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].mMEL[f-1];
                 break;
             case TYPE_PGM:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].mean[f-1];
                 break;
             default:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].mean[f-1];
                 break;
         }
     }
     for(int f=newin/2+1;f<newin+1;f++)
     {
         switch (type) {
             case TYPE_MFCC:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].vMFCC[f-in/2-1];
                 break;
             case TYPE_MFSC:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].vMFSC[f-in/2-1];
                 break;
             case TYPE_MEL:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].vMEL[f-in/2-1];
                 break;
             case TYPE_PGM:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].var[f-in/2-1];
                 break;
             default:
                 xinput[f][numinst+1]=pgm->m_featbuf[wi].var[f-in/2-1];
                 break;
         }

     }
    if (type == TYPE_PGM) {
        xinput[newin+1][numinst+1]=pgm->m_featbuf[wi].mE/10;
        xinput[newin+2][numinst+1]=pgm->m_featbuf[wi].vE/10;
    }
    if (type == TYPE_SCBA) {
        xinput[1][numinst+1]=pgm->m_featbuf[wi].mSc/GF_PASSFREQ;
        xinput[2][numinst+1]=pgm->m_featbuf[wi].vSc/(10*GF_PASSFREQ);
        xinput[3][numinst+1]=pgm->m_featbuf[wi].mBa/(2*GF_PASSFREQ);
        xinput[4][numinst+1]=pgm->m_featbuf[wi].vBa/(10*GF_PASSFREQ);
        xinput[5][numinst+1]=pgm->m_featbuf[wi].mE/10;
        xinput[6][numinst+1]=pgm->m_featbuf[wi].vE/10;
    }
    if (type == TYPE_ALL) {
        for(int f=1;f<GF_PASSFREQ+1;f++)
        {
            xinput[f][numinst+1]=pgm->m_featbuf[wi].mean[f-1];
            xinput[f+GF_PASSFREQ][numinst+1]=pgm->m_featbuf[wi].var[f-1];
        }
        for(int f=1;f<GF_MEL+1;f++)
        {
            xinput[f+2*GF_PASSFREQ][numinst+1]=pgm->m_featbuf[wi].mMEL[f-1];
            xinput[f+2*GF_PASSFREQ+GF_MEL][numinst+1]=pgm->m_featbuf[wi].vMEL[f-1];
        }
        for(int f=1;f<GF_MFSC+1;f++)
        {
            xinput[f+2*GF_PASSFREQ+2*GF_MEL][numinst+1]=pgm->m_featbuf[wi].mMFSC[f-1];
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][numinst+1]=pgm->m_featbuf[wi].vMFSC[f-1];
        }
        for(int f=1;f<GF_MFCC+1;f++)
        {
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][numinst+1]=pgm->m_featbuf[wi].mMFCC[f-1];
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][numinst+1]=pgm->m_featbuf[wi].vMFCC[f-1];
        }
    }
     //xinput[in][numinst+1]=pgm->m_featbuf[wi].mBa/100;
     //xinput[in-1][numinst+1]=pgm->m_featbuf[wi].vBa/1000;
     //xinput[in-2][numinst+1]=pgm->m_featbuf[wi].mSc/100;
     //xinput[in-3][numinst+1]=pgm->m_featbuf[wi].vSc/100;
    if (type == TYPE_MFCC||type == TYPE_MEL||type == TYPE_MFSC||type == TYPE_PGM) {
        maxxs=0;
        for(int f=1;f<in/2+1;f++)
        {
            if(fabs(this->xinput[f][numinst+1])>maxxs)maxxs=fabs(this->xinput[f][numinst+1]);
        }
        for(int f=1;f<in/2+1;f++)
        {
            this->xinput[f][numinst+1]/=maxxs+0.0000000001;
        }
        maxxs=0;
        for(int f=in/2+1;f<in+1;f++)
        {
            if(fabs(this->xinput[f][numinst+1])>maxxs)maxxs=fabs(this->xinput[f][numinst+1]);
        }
        for(int f=in/2+1;f<in+1;f++)
        {
            this->xinput[f][numinst+1]/=maxxs+0.0000000001;
        }
    }
    
    if (type == TYPE_ALL) {
        float maxm = 0;
        float maxv = 0;
        maxm=maxv=0;
        for(int f=1;f<GF_PASSFREQ+1;f++)
        {
            if(fabs(xinput[f][numinst+1])>maxm)maxm=fabs(xinput[f][numinst+1]);
            if(fabs(xinput[f+GF_PASSFREQ][numinst+1])>maxv)maxv=fabs(xinput[f+GF_PASSFREQ][numinst+1]);
        }
        for(int f=1;f<GF_PASSFREQ+1;f++)
        {
            xinput[f][numinst+1]/=maxm+0.0000000001;
            xinput[f+GF_PASSFREQ][numinst+1]/=maxv+0.0000000001;
        }
        
        maxm=maxv=0;
        for(int f=1;f<GF_MEL+1;f++)
        {
            if(fabs(xinput[f+2*GF_PASSFREQ][numinst+1])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ][numinst+1]);
            if(fabs(xinput[f+2*GF_PASSFREQ+GF_MEL][numinst+1])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+GF_MEL][numinst+1]);
        }
        for(int f=1;f<GF_MEL+1;f++)
        {
            xinput[f+2*GF_PASSFREQ][numinst+1]/=maxm+0.0000000001;
            xinput[f+2*GF_PASSFREQ+GF_MEL][numinst+1]/=maxv+0.0000000001;
        }
        
        maxm=maxv=0;
        for(int f=1;f<GF_MFSC+1;f++)
        {
            if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL][numinst+1])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL][numinst+1]);
            if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][numinst+1])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][numinst+1]);
        }
        for(int f=1;f<GF_MFSC+1;f++)
        {
            xinput[f+2*GF_PASSFREQ+2*GF_MEL][numinst+1]/=maxm+0.0000000001;
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+GF_MFSC][numinst+1]/=maxv+0.0000000001;
        }
        
        maxm=maxv=0;
        for(int f=1;f<GF_MFCC+1;f++)
        {
            if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][numinst+1])>maxm)maxm=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][numinst+1]);
            if(fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][numinst+1])>maxv)maxv=fabs(xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][numinst+1]);
        }
        for(int f=1;f<GF_MFCC+1;f++)
        {
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC][numinst+1]/=maxm+0.0000000001;
            xinput[f+2*GF_PASSFREQ+2*GF_MEL+2*GF_MFSC+GF_MFCC][numinst+1]/=maxv+0.0000000001;
        }
    }
/*
    maxxs=0;
    for(int f=1;f<in/2+1;f++)
    {
        if(fabs(this->xinput[f][numinst+1])>maxxs)maxxs=fabs(this->xinput[f][numinst+1]);
    }
    for(int f=1;f<in/2+1;f++)
    {
        this->xinput[f][numinst+1]/=maxxs+0.0000000001;
    }
    maxxs=0;
    for(int f=in/2+1;f<in+1;f++)
    {
        if(fabs(this->xinput[f][numinst+1])>maxxs)maxxs=fabs(this->xinput[f][numinst+1]);
    }
    for(int f=in/2+1;f<in+1;f++)
    {
        this->xinput[f][numinst+1]/=maxxs+0.0000000001;
    }
 */
    
    output(numinst+1);
    float maxx=0;
    for(int i=1;i<out+1;i++)
    {
        if(o[in+hid+i]>maxx)
        {
            maxx=o[in+hid+i];
            winner=i;
        }
        lbl[numinst+1][i]=o[in+hid+i];
        semantic[i]=o[in+hid+i];
        NNout->semantic[i]=o[in+hid+i];
    }
    NNout->winnerIndex=winner;
    return NNout;
}

// *********************************************************************************************************

float HHNN::start(float beta)
{
 int s;
    err=0;
 for(s=1;s<numinst+1;s++)
  {
   this->output(s);
   this->delta(s);
   this->weight(beta);
   for(int i=1;i<out+1;i++)
   {err+=(t[s][i]-o[in+hid+i])*(t[s][i]-o[in+hid+i]);}
   }
  err/=out;
 return sqrt(err/numinst);
}

// *********************************************************************************************************

int HHNN::nclasses()
{
    return out;
}

// *********************************************************************************************************

AnsiString HHNN::getClassName(int c)
{
    return AnsiString(classnames[c+1]);
}

// *********************************************************************************************************

void HHNN::save(char *fname){
    //AnsiString brk="\n",fpext=".hnn";
    //AnsiString FileNamefp=AnsiString(fname).SubString(0, AnsiString(fname).Length()-4)+fpext;
    AnsiString FileNamefp=AnsiString(fname);
    
    FILE* f = fopen(FileNamefp.c_str(),"wb+");
    fwrite((void *)&in,1,sizeof(int),f);
    fwrite((void *)&hid,1,sizeof(int),f);
    fwrite((void *)&out,1,sizeof(int),f);
    fwrite((void *)&numinst,1,sizeof(int),f);
    for(int i=0;i<500;i++){
        for(int j=0;j<500;j++){
            fwrite((void *)&w[i][j],1,sizeof(float),f);
        }
    }
    for (int i = 0; i<GF_MAXCLASSES; i++) {
        fwrite(classnames[i],50,1,f);
    }
    //fwrite((void *)this,1,sizeof(HHNN),f);
    
    fclose(f);
}

// *********************************************************************************************************

void HHNN::open(char *fname){
    
    FILE* f = fopen(fname,"rb");
    //fseeko(f, 0, SEEK_END);
    //int e=ftello(f);
    //fseeko(f, 0, SEEK_SET); 
    //int b=ftell(f);
    //int fsize=e-b;
    if(f){
    fread((void *)&in,1,sizeof(int),f);
    fread((void *)&hid,1,sizeof(int),f);
    fread((void *)&out,1,sizeof(int),f);
    fread((void *)&numinst,1,sizeof(int),f);
    for(int i=0;i<500;i++){
        for(int j=0;j<500;j++){
            fread((void *)&w[i][j],1,sizeof(float),f);
        }
    }
    for (int i = 0; i<GF_MAXCLASSES; i++) {
            fread(classnames[i],50,1,f);
    }
    //fread((void *)this,1,sizeof(HHNN),f);
    
    fclose(f);
    }
}


// *********************************************************************************************************

