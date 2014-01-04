/***************************************************************************
                          convol_inc1.cpp  -  convol function EDGE_WRAP
                             -------------------
    begin                : Sep 19 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// to be included from convol.cpp

#ifdef INCLUDE_CONVOL_INC_CPP

// for all result elements
for (SizeT a = 0; a < nA; ++aInitIx[1]) {
    bool regular = true;
    for (SizeT aSp = 1; aSp < nDim;) {
        if (aInitIx[ aSp] < this->dim[ aSp]) {
            regArr[ aSp] = aInitIx[aSp] >= aBeg[aSp] && aInitIx[aSp] < aEnd[ aSp];
            if (regular) {
                for (; aSp < nDim; ++aSp) {
                    if (!regArr[ aSp]) {
                        regular = false;
                        break;
                    }
                }
                break;
            }
        }
        aInitIx[ aSp] = 0;
        regArr[ aSp] = !aBeg[ aSp];
        if (aBeg[ aSp]) regular = false;
        ++aInitIx[ ++aSp];
    }

    if (regular) { //regular
        for (long aInitIx0 = 0; aInitIx0 < aBeg0; ++aInitIx0, ++a) { // 0-dim beginning regular
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif

            SizeT counter = 0;

            long m_aInitIx0 = -aInitIx0;
            long* kIx = kIxArr;
            for (SizeT k = 0; k < nK; ++k) {
                SizeT aLonIx = aInitIx0 + kIx[0];
                bool doit = true;
                if (kIx[0] < m_aInitIx0) {
#if defined (CONVOL_EDGE_WRAP)
                    aLonIx = aInitIx0 + kIx[0] + dim0;
#elif defined(CONVOL_EDGE_TRUNCATE)
                    aLonIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                    aLonIx = 0;
                    doit = false;
#endif
                }
                if (doit) {
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];
                    }
                    //was        res_a += ddP[ aLonIx] * ker[ k];
                    if (doInvalid) {
                        if (ddP[ aLonIx] != invalidValue) {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx])) {
                                    res_a += ddP[ aLonIx] * ker[ k];
                                    counter++;
                                    if(normalize)  { curScale += absker[k];
                                    otfBias += biasker[k];}
                                }
                            } else {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        }
                    } else {
                        if (doNan) {
                            if (gdlValid(ddP[ aLonIx])) {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        } else {
                            res_a += ddP[ aLonIx] * ker[ k];
                            counter++;
                        }
                    }
                }
                kIx += nDim;
            }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            CONVERT_CONVOL_TO_ORIG;
#endif
        } // 0-dim beginning regular

        if (center) { //regular, center
            for (long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a) { // 0-dim body regular center

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
            SizeT counter = 0;

                long* kIx = kIxArr;
                for (SizeT k = 0; k < nK; k += kDim0) {
                    SizeT aLonIx = aInitIx0 + kIx[0];
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];
                    }
                    for (SizeT k0 = 0; k0 < kDim0; ++k0) {
                        //was            res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                        if (doInvalid) {
                            if (ddP[ aLonIx + k0] != invalidValue) {
                                if (doNan) {
                                    if (gdlValid(ddP[ aLonIx + k0])) {
                                        res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0];
                                        otfBias += biasker[ k + k0];}
                                    }
                                } else {
                                    res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                    counter++;
                                    if(normalize)  { curScale += absker[ k + k0];
                                    otfBias += biasker[ k + k0];}
                                }
                            }
                        } else {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx + k0])) {
                                    res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                    counter++;
                                    if(normalize)  { curScale += absker[ k + k0];
                                    otfBias += biasker[ k + k0];}
                                }
                            } else {
                                res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                counter++;
                            }
                        }
                    }
                    kIx += kDim0_nDim;
                }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
                CONVERT_CONVOL_TO_ORIG;
#endif
            } //body regular center
        } else { //not center
            for (long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a) { // regular body not center

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
                SizeT counter = 0;

                long* kIx = kIxArr;
                for (SizeT k = 0; k < nK; k += kDim0) {
                    SizeT aLonIx = aInitIx0 + kIx[0];
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];
                    }
                    for (SizeT k0 = 0; k0 < kDim0; ++k0) {
                        // was:           res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                        if (doInvalid) {
                            if (ddP[ aLonIx + k0] != invalidValue) {
                                if (doNan) {
                                    if (gdlValid(ddP[ aLonIx - k0])) {
                                        res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0];
                                        otfBias += biasker[ k + k0];}
                                    }
                                } else {
                                    res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                    counter++;
                                    if(normalize)  { curScale += absker[ k + k0];
                                    otfBias += biasker[ k + k0];}
                                }
                            }
                        } else {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx - k0])) {
                                    res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                    counter++;
                                    if(normalize)  { curScale += absker[ k + k0];
                                    otfBias += biasker[ k + k0];}
                                }
                            } else {
                                res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                counter++;
                            }
                        }
                    }
                    kIx += kDim0_nDim;
                }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
                CONVERT_CONVOL_TO_ORIG;
#endif
            } //regular body not center 
        } //not center

        // 0-dim end
        for (long aInitIx0 = aEnd0; aInitIx0 < dim0; ++aInitIx0, ++a) { // 0-dim regular end
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
            SizeT counter = 0;

            long* kIx = kIxArr;
            for (SizeT k = 0; k < nK; ++k) {
                SizeT aLonIx = aInitIx0 + kIx[0];
                bool doit = true;
                if (aLonIx >= dim0) {
#if defined (CONVOL_EDGE_WRAP)
                    aLonIx -= dim0;
#elif defined(CONVOL_EDGE_TRUNCATE)
                    aLonIx = dim0_1;
#elif defined(CONVOL_EDGE_ZERO)
                    aLonIx = dim0_1;
#endif
                }
                if (doit) {
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        aLonIx += (aInitIx[ rSp] + kIx[ rSp]) * aStride[ rSp];
                    }
                    //was        res_a += ddP[ aLonIx] * ker[ k];
                    if (doInvalid) {
                        if (ddP[ aLonIx] != invalidValue) {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx])) {
                                    res_a += ddP[ aLonIx] * ker[ k];
                                    counter++;
                                    if(normalize)  { curScale += absker[k];
                                    otfBias += biasker[k];}
                                }
                            } else {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        }
                    } else {
                        if (doNan) {
                            if (gdlValid(ddP[ aLonIx])) {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        } else {
                            res_a += ddP[ aLonIx] * ker[ k];
                            counter++;
                        }
                    }
                }
                kIx += nDim;
            }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            CONVERT_CONVOL_TO_ORIG;
#endif
        } //0-dim regular end
    } else { //not regular
        // 0-dim beginning
        for (long aInitIx0 = 0; aInitIx0 < aBeg0; ++aInitIx0, ++a) { //beginning, not regular
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
            SizeT counter = 0;

            long m_aInitIx0 = -aInitIx0;
            long* kIx = kIxArr;
            for (SizeT k = 0; k < nK; ++k) {
                SizeT aLonIx = aInitIx0 + kIx[0];
                bool doit = true;
                if (kIx[0] < m_aInitIx0) {
#if defined (CONVOL_EDGE_WRAP)
                    aLonIx = aInitIx0 + kIx[0] + dim0;
#elif defined(CONVOL_EDGE_TRUNCATE)
                    aLonIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                    aLonIx = 0;
#endif
                }

                for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                    long aIx = aInitIx[ rSp] + kIx[ rSp];
                    if (aIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
                        aIx += this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                        aIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                        aIx = 0;
                        doit = false;
#endif 
                    }
                    else if (aIx >= this->dim[ rSp]) {
#if defined (CONVOL_EDGE_WRAP)
                        aIx -= this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                        aIx = this->dim[ rSp] - 1;
#elif defined(CONVOL_EDGE_ZERO)
                        aIx = this->dim[ rSp] - 1;
                        doit = false;
#endif
                    }
                    aLonIx += aIx * aStride[ rSp];
                }

                if (doit) {
                    //was        res_a += ddP[ aLonIx] * ker[ k];
                    if (doInvalid) {
                        if (ddP[ aLonIx] != invalidValue) {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx])) {
                                    res_a += ddP[ aLonIx] * ker[ k];
                                    counter++;
                                    if(normalize)  { curScale += absker[k];
                                    otfBias += biasker[k];}
                                }
                            } else {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        }
                    } else {
                        if (doNan) {
                            if (gdlValid(ddP[ aLonIx])) {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        } else {
                            res_a += ddP[ aLonIx] * ker[ k];
                            counter++;
                        }
                    }
                }
                kIx += nDim;
            }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            CONVERT_CONVOL_TO_ORIG;
#endif
        } //beginning, not regular

        if (center) { //center, not regular 
            for (long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a) { //body,center, not regular
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
                SizeT counter = 0;

                long* kIx = kIxArr;
                bool doit = true;
                for (SizeT k = 0; k < nK; k += kDim0) {
                    SizeT aLonIx = aInitIx0 + kIx[0];
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        long aIx = aInitIx[ rSp] + kIx[ rSp];
                        if (aIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
                            aIx += this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                            aIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                            aIx = 0;
                            doit = false;
#endif
                        } else if (aIx >= this->dim[ rSp]) {
#if defined (CONVOL_EDGE_WRAP)
                            aIx -= this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                            aIx = this->dim[ rSp] - 1;
#elif defined(CONVOL_EDGE_ZERO)
                            aIx = this->dim[ rSp] - 1;
                            doit = false;
#endif
                        }
                        aLonIx += aIx * aStride[ rSp];
                    }
                    if (doit) {
                        for (SizeT k0 = 0; k0 < kDim0; ++k0) {
                            //was            res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                            if (doInvalid) {
                                if (ddP[ aLonIx + k0] != invalidValue) {
                                    if (doNan) {
                                        if (gdlValid(ddP[ aLonIx + k0])) {
                                            res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                            counter++;
                                            if(normalize)  { curScale += absker[ k + k0];
                                            otfBias += biasker[ k + k0];}
                                        }
                                    } else {
                                        res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0]; 
                                        otfBias += biasker[ k + k0];}
                                    }
                                }
                            } else {
                                if (doNan) {
                                    if (gdlValid(ddP[ aLonIx + k0])) {
                                        res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0];
                                        otfBias += biasker[ k + k0];}
                                    }
                                } else {
                                    res_a += ddP[ aLonIx + k0] * ker[ k + k0];
                                    counter++;
                                }
                            }
                        }
                    }
                    kIx += kDim0_nDim;
                }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
                CONVERT_CONVOL_TO_ORIG;
#endif
            } //body, center, not regular
        } else { //not center, not regular
            for (long aInitIx0 = aBeg0; aInitIx0 < aEnd0; ++aInitIx0, ++a) { //body, not center, not regular
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
                SizeT counter = 0;

                long* kIx = kIxArr;
                bool doit = true;
                for (SizeT k = 0; k < nK; k += kDim0) {
                    SizeT aLonIx = aInitIx0 + kIx[0];
                    for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                        long aIx = aInitIx[ rSp] + kIx[ rSp];
                        if (aIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
                            aIx += this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                            aIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                            aIx = 0;
                            doit = false;
#endif
                        } else if (aIx >= this->dim[ rSp]) {
#if defined (CONVOL_EDGE_WRAP)
                            aIx -= this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                            aIx = this->dim[ rSp] - 1;
#elif defined(CONVOL_EDGE_ZERO)
                            aIx = this->dim[ rSp] - 1;
                            doit = false;
#endif
                        }
                        aLonIx += aIx * aStride[ rSp];
                    }
                    if (doit) {
                        for (SizeT k0 = 0; k0 < kDim0; ++k0) {
                            //was            res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                            if (doInvalid) {
                                if (ddP[ aLonIx + k0] != invalidValue) {
                                    if (doNan) {
                                        if (gdlValid(ddP[ aLonIx - k0])) {
                                            res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                            counter++;
                                            if(normalize)  { curScale += absker[ k + k0];
                                            otfBias += biasker[ k + k0];}
                                        }
                                    } else {
                                        res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0];
                                        otfBias += biasker[ k + k0];}
                                    }
                                }
                            } else {
                                if (doNan) {
                                    if (gdlValid(ddP[ aLonIx - k0])) {
                                        res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                        counter++;
                                        if(normalize)  { curScale += absker[ k + k0];
                                        otfBias += biasker[ k + k0];}
                                    }
                                } else {
                                    res_a += ddP[ aLonIx - k0] * ker[ k + k0];
                                    counter++;
                                }
                            }
                        }
                    }
                    kIx += kDim0_nDim;
                }
            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
                CONVERT_CONVOL_TO_ORIG;
#endif
            }//body, not center, not regular
        } //not center, not regular

        // 0-dim end
        for (long aInitIx0 = aEnd0; aInitIx0 < dim0; ++aInitIx0, ++a) { //end, not regular
#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            DLong res_a = 0;
            DLong otfBias = 0;
            DLong curScale = 0;
#else
            Ty& res_a = (*res)[ a];
            Ty otfBias = this->zero;
            Ty curScale = this->zero;
#endif
            SizeT counter = 0;

            long* kIx = kIxArr;
            bool doit = true;
            for (SizeT k = 0; k < nK; ++k) {
                SizeT aLonIx = aInitIx0 + kIx[0];
                if (aLonIx >= dim0) {
#if defined (CONVOL_EDGE_WRAP)
                    aLonIx -= dim0;
#elif defined(CONVOL_EDGE_TRUNCATE)
                    aLonIx = dim0_1;
#elif defined(CONVOL_EDGE_ZERO)
                    aLonIx = dim0_1;
#endif
                }
                for (SizeT rSp = 1; rSp < nDim; ++rSp) {
                    long aIx = aInitIx[ rSp] + kIx[ rSp];
                    if (aIx < 0) {
#if defined (CONVOL_EDGE_WRAP)
                        aIx += this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                        aIx = 0;
#elif defined(CONVOL_EDGE_ZERO)
                        aIx = 0;
                        doit = false;
#endif
                    } else if (aIx >= this->dim[ rSp]) {
#if defined (CONVOL_EDGE_WRAP)
                        aIx -= this->dim[ rSp];
#elif defined(CONVOL_EDGE_TRUNCATE)
                        aIx = this->dim[ rSp] - 1;
#elif defined(CONVOL_EDGE_ZERO)
                        aIx = this->dim[ rSp] - 1;
                        doit = false;
#endif
                    }
                    aLonIx += aIx * aStride[ rSp];
                }

                if (doit) {
                    //was         res_a += ddP[ aLonIx] * ker[ k];
                    if (doInvalid) {
                        if (ddP[ aLonIx] != invalidValue) {
                            if (doNan) {
                                if (gdlValid(ddP[ aLonIx])) {
                                    res_a += ddP[ aLonIx] * ker[ k];
                                    counter++;
                                    if(normalize)  { curScale += absker[k];
                                    otfBias += biasker[k];}
                                }
                            } else {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        }
                    } else {
                        if (doNan) {
                            if (gdlValid(ddP[ aLonIx])) {
                                res_a += ddP[ aLonIx] * ker[ k];
                                counter++;
                                if(normalize)  { curScale += absker[k];
                                otfBias += biasker[k];}
                            }
                        } else {
                            res_a += ddP[ aLonIx] * ker[ k];
                            counter++;
                        }
                    }
                }
                kIx += nDim;
            }

            if (normalize && (doInvalid||doNan)) { 
                scale = curScale;
#if defined(CONVOL_BYTE__) || defined (CONVOL_UINT__)
                CONVERT_MODIFY_BIAS;
#else
                bias=this->zero;
#endif
            }
            if (counter == 0) res_a = missingValue; 
            else 
            {
                res_a /= scale;
                res_a += bias;
             }

#if (defined(CONVOL_BYTE__) || defined (CONVOL_UINT__) || defined (CONVOL_INT__))
            CONVERT_CONVOL_TO_ORIG;
#endif
        } //end, not regular
    } // if( regular) else
} // for(...)



#endif
