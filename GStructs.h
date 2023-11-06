//
//  GStructs.h
//  Ghanni
//
//Copyright (c) 2012 Ghanni. All rights reserved.
//

#ifndef Ghanni_GStructs_h
#define Ghanni_GStructs_h


#define GF_WORKBUF_SIZE		4096
#define GF_ITW_WORKBUF_SIZE		800
#define GF_PASSFREQ		40
#define GF_MFSC		14
#define GF_MFCC		14
#define GF_MEL		19
#define GF_SLIDING_NORM_SIZE	300
#define GF_ITW_SIZE	200
#define GF_ITW_FRAC	100
#define GF_ITW_OVERLAP	0.2f
#define GF_NEXPERTS 5
#define GF_MAXSAMPLES 20000
#define GF_MAXCLASSES 100
#define GF_MAXIN 200
#define TYPE_MFCC 0
#define TYPE_MFSC 1
#define TYPE_MEL 2
#define TYPE_PGM 3
#define TYPE_SCBA 4
#define TYPE_ALL 5


typedef struct
{
    float	value;
    int		index;
} TPitch;

typedef float TDCTVector[128];
typedef float TXVector[GF_PASSFREQ];
typedef float TMFSCVector[GF_MFSC];
typedef float TMFCCVector[GF_MFCC];
typedef float TMELVector[GF_MEL];

typedef struct {
    TXVector X;
    TMFSCVector MFSC;
    TMFCCVector MFCC;
    TMELVector MEL;
    float	Sc;
    float	Ba;
    float       E;
} TSpectVector;

typedef struct {
    float	mean[GF_PASSFREQ];
    float	var[GF_PASSFREQ];
    float mMFSC[GF_MFSC];
    float vMFSC[GF_MFSC];
    float mMFCC[GF_MFCC];
    float vMFCC[GF_MFCC];
    float mMEL[GF_MEL];
    float vMEL[GF_MEL];
    float mSc;
    float vSc;
    float mBa;
    float vBa;
    float mE;
    float vE;
    TPitch	maxmeanLow;
    TPitch	maxmeanHi;
    TPitch	maxmean;
} TFeatureVector;

typedef struct {
    float semFeaturesM[11][GF_NEXPERTS];
    float semFeaturesV[11][GF_NEXPERTS];
    float PGMM[14][5];
    float PGMV[14][5];
    float MFCCM[6];
    float MFCCV[6];
    float SCM;
    float SCV;
    float BAM;
    float BAV;
} TSgnVector;

typedef char TFingerPrintCol[19];




#endif
