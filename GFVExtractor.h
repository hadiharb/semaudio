//  Copyright (c) 2012 Ghanni. All rights reserved.
//



#ifndef GFVEXTRACTOR_H
#define GFVEXTRACTOR_H

//#include "FFTReal.h"
#include "GStructs.h"
#include "GSignalProcessor.h"

class GFVExtractor
{
private:
    float	*m_ITWhamming;
    int		m_cnt_featbuf;
    int		m_frac_used;
    int		m_intwin_size;
    int		m_intwin_total;
    int		m_intwin_overlap;
    
// functions
    
    bool MakeFeatureVectorFromX(TSpectVector* inBuf, int inBufSize, int inPos, TFeatureVector* out);
    int TakePossibleX(GSignalProcessor* gsignal);
    
public:
     TFeatureVector m_featbuf[GF_WORKBUF_SIZE];
    GFVExtractor(int inIntWinSize, int inFracUsed, float inOverlap);
    GFVExtractor();
    ~GFVExtractor();
    int get_cntfbuf();
    int TakeWavFile(char* name);
    float get_E();
    
};

#endif
