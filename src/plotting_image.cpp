/***************************************************************************
                       plotting_image.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "plotting.hpp"
#include "math_utl.hpp"

namespace lib {

  using namespace std;

  class tv_image_call : public plotting_routine_call {
  private:
    BaseGDL* image;
    DLong trueColor;
    DByteGDL* byteImage;
    Guard<BaseGDL> byteImageGuard;
    DLong imageWidth, imageHeight;
    DLong xSize, ySize;
    DLong botLeftPixelX, botLeftPixelY;
    DLong channel;
    SizeT rank;

    bool handle_args(EnvT* e) {

      trueColor = 0;

      image = e->GetParDefined(0);
      rank = image->Rank();
      e->AssureLongScalarKWIfPresent("TRUE", trueColor);
      if (rank < 1 || rank > 3) e->Throw("Image array must have rank 1, 2 or 3");
      if (rank <= 2 && trueColor != 0) e->Throw("Array must have 3 dimensions: " + e->GetParString(0));
      if (trueColor < 0 || trueColor > 3) e->Throw("Value of TRUE keyword is out of allowed range.");
      // to be changed    if (trueColor == 1 && xwd->depth < 24) e->Throw("Device depth must be 24 or greater for trueColore color display");

      DLong orderVal = SysVar::TV_ORDER();
      e->AssureLongScalarKWIfPresent("ORDER", orderVal);
      
      channel = 0;
      if (e->NParam(0) == 4) e->AssureLongScalarPar(3, channel);
      e->AssureLongScalarKWIfPresent("CHANNEL", channel);
      if (channel < 0 || channel > 3) e->Throw("Value of Channel is out of allowed range.");
      
      if (rank == 1) {
        byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
        byteImageGuard.Init(byteImage);
        if (orderVal != 0) {
          byteImage->Reverse(0);
        }
        imageWidth = byteImage->Dim(0);
        imageHeight = 1;
      } else if (rank == 2) {
        byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
        byteImageGuard.Init(byteImage);
        if (orderVal != 0) {
          byteImage->Reverse(1);
        }
        imageWidth = byteImage->Dim(0);
        imageHeight = byteImage->Dim(1);
      } else if (rank == 3) {
        if (trueColor == 1) {
          byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
          byteImageGuard.Init(byteImage);
          if (byteImage->Dim(0) < 3) e->Throw("Array <BYTE     Array[" + i2s(byteImage->Dim(0)) + "," +
                  i2s(byteImage->Dim(1)) + "," + i2s(byteImage->Dim(2))
                  + "]> does not have enough elements.");
          if (orderVal != 0) {
            byteImage->Reverse(2);
          }
          imageWidth = byteImage->Dim(1);
          imageHeight = byteImage->Dim(2);
        }
        if (trueColor == 2) {
          byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
          byteImageGuard.Init(byteImage);
          if (byteImage->Dim(1) < 3) e->Throw("Array <BYTE     Array[" + i2s(byteImage->Dim(0)) + "," +
                  i2s(byteImage->Dim(1)) + "," + i2s(byteImage->Dim(2))
                  + "]> does not have enough elements.");
          if (orderVal != 0) {
            byteImage->Reverse(2);
          }
          imageWidth = byteImage->Dim(0);
          imageHeight = byteImage->Dim(2);
        }
        if (trueColor == 3) {
          byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
          byteImageGuard.Init(byteImage);
          if (byteImage->Dim(2) < 3) e->Throw("Array <BYTE     Array[" + i2s(byteImage->Dim(0)) + "," +
                  i2s(byteImage->Dim(1)) + "," + i2s(byteImage->Dim(2))
                  + "]> does not have enough elements.");
          if (orderVal != 0) {
            byteImage->Reverse(1);
          }
          imageWidth = byteImage->Dim(0);
          imageHeight = byteImage->Dim(1);
        }
        if (trueColor == 0) { // here we have a rank =3
          byteImage = static_cast<DByteGDL*> (image->Convert2(GDL_BYTE, BaseGDL::COPY));
          byteImageGuard.Init(byteImage);
          if (byteImage->Dim(0) == 1) {
            imageWidth = byteImage->Dim(1);
            imageHeight = byteImage->Dim(2);
            if (orderVal != 0) {
              byteImage->Reverse(2);
            }
          } else if (byteImage->Dim(1) == 1) {
            imageWidth = byteImage->Dim(0);
            imageHeight = byteImage->Dim(2);
            if (orderVal != 0) {
              byteImage->Reverse(2);
            }
          } else {
            imageWidth = byteImage->Dim(0);
            imageHeight = byteImage->Dim(1);
            if (orderVal != 0) {
              byteImage->Reverse(1);
            }
          }
        }
      }
      return false;
    }

  private:

    void old_body(EnvT* e, GDLGStream * actStream) {

      enum {
        DATA = 0,
        NORMAL,
        DEVICE,
        NONE
      } coordinateSystem = NONE;
      //check presence of DATA,DEVICE and NORMAL options
      if (e->KeywordSet("DATA")) coordinateSystem = DATA;
      if (e->KeywordSet("DEVICE")) coordinateSystem = DEVICE;
      if (e->KeywordSet("NORMAL")) coordinateSystem = NORMAL;

      bool mapSet = false;
      
#ifdef USE_LIBPROJ4
      // Map Stuff (xtype = 3)

      get_mapset(mapSet);

      if (mapSet) {
        ref = map_init();
        if (ref == NULL) {
          e->Throw("Projection initialization failed.");
        }
      }
#endif

      SizeT nParam = e->NParam(1);

      DLong xPageSize = actStream->xPageSize();
      DLong yPageSize = actStream->yPageSize();

      DLong positionOnPage = 0;
      botLeftPixelX = 0;
      botLeftPixelY = 0;
      
      //DEVICE HAS FIXED PIXELS?
      bool fixedPixelSize = (((*static_cast<DLongGDL*> (SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0] & 1) == 0); 

      //image size. In DEV, CM or INCHES
      PLFLT devx, devy, x ,y;
      devx = 0;
      devy = 0;
      bool xSizeGiven, ySizeGiven;
      double aspect=static_cast<double>(imageWidth)/static_cast<double>(imageHeight);

        xSizeGiven=e->KeywordPresent("XSIZE");
        if (xSizeGiven) e->AssureDoubleScalarKWIfPresent("XSIZE", devx);
        ySizeGiven=e->KeywordPresent("YSIZE");
        if (ySizeGiven) e->AssureDoubleScalarKWIfPresent("YSIZE", devy);

        //interpret size:
        if (xSizeGiven || ySizeGiven) {
          PLFLT x1,y1, x2 ,y2;
          if (coordinateSystem == DATA) {
            actStream->world2device(0, 0, x1,y1);
            actStream->world2device(devx, devy, x2,y2);
            devx=x2-x1; 
            devy=y2-y1;
          } else if (coordinateSystem == NORMAL) {
            actStream->NormedDeviceToDevice(0, 0, x1,y1);
            actStream->NormedDeviceToDevice(devx, devy, x2,y2);
            devx=(x2-x1); 
            devy=(y2-y1);
          } else if (coordinateSystem == DEVICE) {
          } else {
            if (e->KeywordSet("INCHES")) {
              devx *= (10 * 2.54);
              devy *= (10 * 2.54);
              actStream->mm2device(devx, devy, x,y);
              devx=x;
              devy=y;
            } else if (e->KeywordSet("CENTIMETERS")) {
              devx *= (10.);
              devy *= (10.);
              actStream->mm2device(devx, devy, x,y);
              devx=x;
              devy=y;
            }
          }
          if (xSizeGiven && !ySizeGiven) {devy=devx/aspect; ySizeGiven=true;}
          if (!xSizeGiven && ySizeGiven) {devx=devy*aspect; xSizeGiven=true;}
        } else {
          double pageaspect=xPageSize/yPageSize;
          if (aspect>pageaspect){
            devx=xPageSize*0.9; //10% margin 
            devy=devx/aspect;
          } else {
            devy=yPageSize*0.9; //10% margin
            devx=devy*aspect;
          }
        }

      if (nParam == 2) {
        int nPerPageX, nPerPageY, ix, iy;
        e->AssureLongScalarPar(1, positionOnPage);
        if (!fixedPixelSize && !(xSizeGiven && ySizeGiven)) e->Throw("XSIZE and/or YSIZE needed for proper positioning.");
        nPerPageX = xPageSize / imageWidth;
        nPerPageY = yPageSize / imageHeight;
        // AC 2011/11/06, bug 3433502
        if (nPerPageX > 0) {
          ix = positionOnPage % nPerPageX;
        } else ix = 0;
        if (nPerPageY > 0) {
          iy = (positionOnPage / nPerPageX) % nPerPageY;
        } else iy = 0;
        botLeftPixelX = imageWidth*ix;
        botLeftPixelY = yPageSize - imageHeight * (iy + 1);
      } else if (nParam >= 3) {
        PLFLT xLLf, yLLf;
        e->AssureDoubleScalarPar(1, xLLf); //may be DATA or NORM or DEVICE. DEV,NORM and DATA preferred on INCH and CENTIM. INCH and CM imply DEVICE
        e->AssureDoubleScalarPar(2, yLLf); //idem
        //convert to device Pixel:
        if (coordinateSystem == DATA) {
//          actStream->WorldToDevice(xLLf, yLLf, x,y);
          actStream->world2device(xLLf, yLLf, x,y);
          botLeftPixelX=x; 
          botLeftPixelY=y;
        } else if (coordinateSystem == NORMAL) {
          actStream->NormedDeviceToDevice(xLLf, yLLf, x,y);
          botLeftPixelX=x;
          botLeftPixelY=y;
        } else if (coordinateSystem == DEVICE) {
          botLeftPixelX = xLLf;
          botLeftPixelY = yLLf;
        } else {
          if (e->KeywordSet("INCHES")) {
            xLLf *= (10 * 2.54);
            yLLf *= (10 * 2.54);
            actStream->mm2device(xLLf, yLLf, x,y);
            botLeftPixelX=x;
            botLeftPixelY=y;
          } else if (e->KeywordSet("CENTIMETERS")) {
            xLLf *= (10.);
            yLLf *= (10.);
            actStream->mm2device(xLLf, yLLf, x,y);
            botLeftPixelX=x;
            botLeftPixelY=y;
          } else {
            botLeftPixelX = xLLf;
            botLeftPixelY = yLLf;
          }
        }
      }

      xSize = devx;
      ySize = devy;

      int debug=0;
      if (debug == 1) {
        std::cout << "==================== " << std::endl;
        std::cout << "trueColor " << trueColor <<std::endl;
        std::cout << "Rank " << rank <<std::endl;
        std::cout << "Channel " << channel <<std::endl;
        std::cout << "xSize " << xSize <<std::endl;
        std::cout << "ySize " << ySize <<std::endl;
        std::cout << "botLeftPixelX " << botLeftPixelX <<std::endl;
        std::cout << "botLeftPixelY " << botLeftPixelY <<std::endl;
        std::cout << "imageWidth " << imageWidth <<std::endl;
        std::cout << "imageHeight " << imageHeight <<std::endl;
      }
   
    }

  private:

    void call_plplot(EnvT* e, GDLGStream * actStream) {

      //pass physical position of image
      DLong devicebox[4] = {botLeftPixelX, xSize, botLeftPixelY, ySize};
//  cout << devicebox[0] << "," << devicebox[1] << "," << devicebox[2] << ","<< devicebox[3] << endl;

      Guard<BaseGDL> chan_guard;
      if (channel == 0) {
        if (!actStream->PaintImage(&(*byteImage)[0], imageWidth, imageHeight, devicebox, trueColor, channel)) e->Throw("device does not support Paint");
      } else if (rank == 3) {
        // Rank == 3 w/channel
        SizeT dims[2];
        dims[0] = imageWidth;
        dims[1] = imageHeight;
        dimension dim(dims, 2);
        DByteGDL* byteImage_chan = new DByteGDL(dim, BaseGDL::ZERO);
        for (SizeT i = (channel - 1); i < byteImage->N_Elements(); i += 3) {
          (*byteImage_chan)[i / 3] = (*byteImage)[i];
        }
        // Send just single channel
        if (!actStream->PaintImage(&(*byteImage_chan)[0], imageWidth, imageHeight, devicebox, trueColor, channel)) e->Throw("device does not support Paint");
        chan_guard.Init(byteImage_chan); // delete upon exit
      } else if (rank == 2) {
        // Rank = 2 w/channel
        if (!actStream->PaintImage(&(*byteImage)[0], imageWidth, imageHeight, devicebox, trueColor, channel)) e->Throw("device does not support Paint");
      }
    }



  private:

    void post_call(EnvT*, GDLGStream * actStream) // {{{
    {
//      if (doT3d) {
//        plplot3d_guard.Reset(plplot3d);
//        actStream->stransform(NULL, NULL);
//      }
    }
  };

  void tv_image(EnvT* e) {
    tv_image_call tv_image;
    tv_image.call(e, 1);
  }

} // namespace
