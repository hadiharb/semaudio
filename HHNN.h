//  Copyright (c) 2012 Ghanni. All rights reserved.
//


#ifndef HHNN_H
#define HHNN_H

#include "GFVExtractor.h"
#include <stdio.h>

struct NNoutput
{
 float semantic[GF_MAXCLASSES];
 int winnerIndex;
} ;


class HHNN
{
 private:
    int epoch;
    int T,numinst,ninst;
    int in,hid,out,han;
    float t[GF_MAXSAMPLES][GF_MAXCLASSES+1],lbl[GF_MAXSAMPLES][GF_MAXCLASSES];
    float o[GF_MAXSAMPLES];
    float xinput[GF_MAXIN][GF_MAXSAMPLES];
    float d[GF_MAXSAMPLES];
    float w[500][500];
    float maxxs;
    float teta[GF_MAXSAMPLES];
    float err;
    float b;
    int pasfreq;
    char classnames[GF_MAXCLASSES][50];

 public:
    HHNN();
    HHNN(int In, int Hid, int Out);
    float f(float a);
    void output(int n);
    void delta(int m);
    void weight(float beta);
    //void initlearn(int offset, int numberOfSpeakers, GFVExtractor **Pgm); // numinst is offset*numberOfSpeakers
    void initlearn(int offset, int numberOfSpeakers, GFVExtractor **Pgm, AnsiString *classNames, int type); // 0 => MFCC, 1 => MFSC, 2 => MEL, 3 => PGM
    void save(char *fname);
    void open(char *fname);
    int nclasses();
    AnsiString getClassName(int c);
    NNoutput * getOutput(int offset, GFVExtractor *pgm);
    NNoutput * getOutput(int offset, GFVExtractor *pgm, int type); // 0 => MFCC, 1 => MFSC, 2 => MEL, 3 => PGM
   // NNoutput * getAverage(NNoutput *n1, NNoutput *n2, NNoutput *n3, NNoutput *n4);
    NNoutput * getAverage(int n, ...);
    float start(float beta);

};

#endif
