//  Copyright (c) 2012 Ghanni. All rights reserved.
//

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "GFFT.h"
#include "math.h"

GFFT::GFFT(long length){
    len = length;
    xc = new Complex[len];
}

GFFT::~GFFT(){
    delete [] xc;
}

void GFFT::fft(CArray& x) {
    const size_t N = x.size();
    if (N <= 1) return;
    
    CArray even = x[std::slice(0, N/2, 2)];
    CArray  odd = x[std::slice(1, N/2, 2)];
    
    fft(even);
    fft(odd);
    
    for (size_t k = 0; k < N/2; ++k)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        x[k] = even[k] + t;
        x[k+N/2] = even[k] - t;
    }
}

void GFFT::make_fft(double f[], const double x[]){
    for (int i=0; i<len; i++) {
        xc[i] = Complex(x[i],0.0f);
    }
    CArray data(xc, len);
    fft(data);
    for (int i = 0; i< len/2; i++) {
        f[i] = data[i].real();
        f[i+len/2] = data[i].imag();
    }
}
