/***************************************************************************
                          voigt.cpp  -  mathematical GDL library function
                             -------------------
    begin                : 26 May 2008
    copyright            : (C) 2007 by Nicolas Galmiche
    email                : n.galmiche _@_ gmail.com
 ***************************************************************************/


/***************************************************************************
 *   Alain Coulais 04 June 2008                                            *
 *   We put this software explicitely under the GPL 2 licence because      *
 *   we found it without explicit licence INSIDE a package UNDER GPL 2     *
 *   Please contact me if there is any problem with that !                 *
 *   (see up to date email in file AUTHORS)                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
///   To calculate the Faddeeva function with relative error less than 10^(-4).
///     (from http://www.atm.ox.ac.uk/user/wells/voigt.html)
///    http://dx.doi.org/10.1016/S0022-4073(97)00231-8
///
/// arguments:
///  x, y - Faddeeva/Voigt function arguments 
/// return value -- voigt

#include "math_fun_ng.hpp"


using namespace std;

namespace lib 
{

float humlik( float x, float y) 
  {

    static const float c[6] = { 1.0117281,     -0.75197147,      0.012557727, 
                                0.010022008,   -2.4206814e-4,    5.0084806e-7 };
    static const float s[6] = { 1.393237,       0.23115241,     -0.15535147, 
                                0.0062183662,   9.1908299e-5,   -6.2752596e-7 };
    static const float t[6] = { 0.31424038,     0.94778839,      1.5976826, 
                                2.2795071,      3.020637,        3.8897249 };

    const float rrtpi = 0.56418958; // 1/racines carre de (pi) 

    static float a0, d0, d2, e0, e2, e4, h0, h2, h4, h6, 
                 p0, p2, p4, p6, p8, z0, z2, z4, z6, z8; 
    static float mf[6], pf[6], mq[6], pq[6], xm[6], ym[6], xp[6], yp[6];
    static float old_y = -1.;
    static bool rg1, rg2, rg3;
    static float xlim0, xlim1, xlim2, xlim3, xlim4;
    static float yq, yrrtpi;
    if (y != old_y) {
        old_y = y;
        yq = y * y;				// y au carre
        yrrtpi = y * rrtpi;			// y * racine(Pi)
        rg1 = true, rg2 = true, rg3 = true;	// hypothese nom de region 
        if (y < 70.55) {
            xlim0 = sqrt(y * (40. - y * 3.6) + 15100.);                      //xlimX correspondent au limites de regions
            xlim1 = (y >= 8.425 ?  0. : sqrt(164. - y * (y * 1.8 + 4.3)));
            xlim2 = 6.8 - y;
            xlim3 = y * 2.4;
            xlim4 = y * 18.1 + 1.65;
            if (y <= 1e-6) 
                xlim2 = xlim1 = xlim0;
        }
    }

    float abx = fabs(x);
    float xq = abx * abx;

    if (abx >= xlim0 || y >= 70.55)         // Region 0 algorithm
        return yrrtpi / (xq + yq);

    else if (abx >= xlim1) {            //  Humlicek W4 Region 1
        if (rg1) {  // First point in Region 1
            rg1 = false;
            a0 = yq + 0.5;  //Region 1 y-dependents
            d0 = a0 * a0;
            d2 = yq + yq - 1.;
        }
        return rrtpi / (d0 + xq * (d2 + xq)) * y * (a0 + xq);
    } 
    
    else if (abx > xlim2) {  // Humlicek W4 Region 2
        if (rg2) {  //First point in Region 2
            rg2 = false;
            // Region 2 y-dependents
            h0 = yq * (yq * (yq * (yq + 6.) + 10.5) + 4.5) + 0.5625;
            h2 = yq * (yq * (yq * 4. + 6.) + 9.) - 4.5;
            h4 = 10.5 - yq * (6. - yq * 6.);
            h6 = yq * 4. - 6.;
            e0 = yq * (yq * (yq + 5.5) + 8.25) + 1.875;
            e2 = yq * (yq * 3. + 1.) + 5.25;
            e4 = h6 * 0.75;
        }
        return rrtpi / (h0 + xq * (h2 + xq * (h4 + xq * (h6 + xq)))) 
                 * y * (e0 + xq * (e2 + xq * (e4 + xq)));
    } 
    
    else if (abx < xlim3) { // Humlicek W4 Region 3
        if (rg3) {  // First point in Region 3
            rg3 = false;
            //Region 3 y-dependents
            z0 = y * (y * (y * (y * (y * (y * (y * (y * (y * (y 
                    + 13.3988) + 88.26741) + 369.1989) + 1074.409) 
                    + 2256.981) + 3447.629) + 3764.966) + 2802.87) 
                    + 1280.829) + 272.1014;
            z2 = y * (y * (y * (y * (y * (y * (y * (y * 5.  + 53.59518) 
                    + 266.2987) + 793.4273) + 1549.675) + 2037.31) 
                    + 1758.336) + 902.3066) + 211.678; 
            z4 = y * (y * (y * (y * (y * (y * 10. + 80.39278) + 269.2916) 
                    + 479.2576) + 497.3014) + 308.1852) + 78.86585;
            z6 = y * (y * (y * (y * 10. + 53.59518) + 92.75679) 
                    + 55.02933) + 22.03523;
            z8 = y * (y * 5. + 13.3988) + 1.49646;
            p0 = y * (y * (y * (y * (y * (y * (y * (y * (y * 0.3183291 
                    + 4.264678) + 27.93941) + 115.3772) + 328.2151) + 
                    662.8097) + 946.897) + 919.4955) + 549.3954) 
                    + 153.5168;
            p2 = y * (y * (y * (y * (y * (y * (y * 1.2733163 + 12.79458) 
                    + 56.81652) + 139.4665) + 189.773) + 124.5975) 
                    - 1.322256) - 34.16955;
            p4 = y * (y * (y * (y * (y * 1.9099744 + 12.79568) 
                    + 29.81482) + 24.01655) + 10.46332) + 2.584042;
            p6 = y * (y * (y * 1.273316 + 4.266322) + 0.9377051) 
                    - 0.07272979;
            p8 = y * .3183291 + 5.480304e-4;
        }
        return 1.7724538 / (z0 + xq * (z2 + xq * (z4 + xq * (z6 + 
                xq * (z8 + xq)))))
                  * (p0 + xq * (p2 + xq * (p4 + xq * (p6 + xq * p8))));
    } 
    
    else {  //  Humlicek CPF12 algorithm
        float ypy0 = y + 1.5;
        float ypy0q = ypy0 * ypy0;
        for (int j = 0; j <= 5; ++j) {
            float d = x - t[j];
            mq[j] = d * d;
            mf[j] = 1. / (mq[j] + ypy0q);
            xm[j] = mf[j] * d;
            ym[j] = mf[j] * ypy0;
            d = x + t[j];
            pq[j] = d * d;
            pf[j] = 1. / (pq[j] + ypy0q);
            xp[j] = pf[j] * d;
            yp[j] = pf[j] * ypy0;
        }
        float k = 0.;
        if (abx <= xlim4) // Humlicek CPF12 Region I
            for (int j = 0; j <= 5; ++j) 
                k += c[j] * (ym[j]+yp[j]) - s[j] * (xm[j]-xp[j]);
        else {           // Humlicek CPF12 Region II
            float yf = y + 3.;
            for (int j = 0; j <= 5; ++j) 
                k += (c[j] * (mq[j] * mf[j] - ym[j] * 1.5) 
                         + s[j] * yf * xm[j]) / (mq[j] + 2.25) 
                        + (c[j] * (pq[j] * pf[j] - yp[j] * 1.5) 
                           - s[j] * yf * xp[j]) / (pq[j] + 2.25);
            k = y * k + exp(-xq);
        }
        return k;
    }
  } 
} // namespace
