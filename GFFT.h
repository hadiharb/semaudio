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

#ifndef GFFT_H
#define GFFT_H

#include <complex>
#include <iostream>
#include <valarray>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class GFFT {
public:
    GFFT(long length);
    ~GFFT();
    void make_fft (double f [], const double x []);
private:
    void fft(CArray& x);
    long len;
    Complex *xc;
};

    
#endif