//
//  GSignalProcessor.h
//  Ghanni
//
//Copyright (c) 2012 Ghanni. All rights reserved.
//

#ifndef __Ghanni__GSignalProcessor__
#define __Ghanni__GSignalProcessor__

//#include "FFTReal.h"
#include "GFFT.h"
#include "GStructs.h"
#include "bcport.h"


class GSignalProcessor
{
private:
    //FFTReal	*m_FFT;
    GFFT *g_fft;
    double	*m_fftbuf[GF_WORKBUF_SIZE];
    int		m_cnt_xbuf;
    int		m_win_size;
    int		m_win_step;
    double	*m_hamming;
    double	*m_tmp_buf;
    double	*m_sliding_norm;
    double	*m_sliding_norm1;
    double	*m_sliding_norm2;
    //double	*m_sliding_fft[GF_SLIDING_NORM_SIZE];
    int 	m_sliding_norm_ptr;
    bool	m_sliding_norm_over;
    double	m_filt[256];
    double	m_center[20];
    double	m_left[20];
    double	m_right[20];
    
    // functions
    bool MakeFFTFromData(short* data, int dataLen, int dataPos, double* out);
    bool MakeXFromFFT(double *fftbuf, TSpectVector* out); // integrates bandwidth, spectral centroid and mfcc
    
    
public:
    TSpectVector	m_xbuf[GF_WORKBUF_SIZE];
    GSignalProcessor(int inWinSize, int inWinStep); // For 8000HZ signal inWinSize/inWinStep of 256/80 => 32ms/10ms (standard)
    ~GSignalProcessor();
    int get_cntxbuf();
    int TakePossibleData(short* data, int len);
    int WriteFeaturesToFile(char* name);
    //void NormalizeX(int inWinSize);
    
};

#endif /* defined(__Ghanni__GSignalProcessor__) */
