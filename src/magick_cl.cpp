/***************************************************************************
                          magick_fun_cl.cpp  -  basic GDL library function
                             -------------------
    begin                : May 7 2004
    copyright            : (C) 2004 by Christopher Lee
    email                : leec_gdl@publius.co.uk
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

#ifndef HAVE_CONFIG_H
// #include <config.h>
// #else
// default: assume we have ImageMagick
#define USE_MAGICK 1
#define USE_MAGICK6 1
#endif


#ifdef USE_MAGICK

#include <string>
#include <fstream>
#include <memory>

#include "envt.hpp"
#include "magick_cl.hpp"
#include "graphicsdevice.hpp"

// If Magick has not been initialized, do it here, instead of initializing it in the main program (speedup and avoid strange backtraces)
// Also warn about limitations due to local implementation of Magick library.
//We should do more, by example hat octave people do in their code (they circumvent some other limitations)
#define START_MAGICK  \
    if (notInitialized ) { \
      notInitialized = false; \
        Magick::InitializeMagick(NULL);  \
              }

namespace lib {

  using namespace std;
  using namespace antlr;
  using namespace Magick;

  Image* gImage[40];
  unsigned int gValid[40];
  unsigned int gCount = 0;
  static bool notInitialized = true;
  static const std::string imageTypeList[]={"BilevelType","GrayscaleType","GrayscaleMatteType","PaletteType","PaletteMatteType","TrueColorType","TrueColorMatteType","ColorSeparationType","ColorSeparationMatteType","OptimizeType","UnknownType"};
  static const std::string imageClassList[]={"UndefinedClass","DirectClass","PseudoClass"};

  __attribute__ ((destructor)) static void destruct(void) { // This is GCC only. Possible memory leak with other compilers.
    int i;
    for (i = 0; i < 40; ++i)
      if (gValid[i] == 1)
        delete gImage[i];
  }

  void magick_setup() {
    int i;
    for (i = 0; i < 40; ++i) gValid[i] = 0;
  }

  Image *magick_image(EnvT *e, unsigned int mid) {
    if (gValid[mid] == 0) e->Throw("invalid ID.");
    Image* im=gImage[mid];
//    std::cerr<<im->depth()<<std::endl;
    return im;
  }

  unsigned int magick_image(EnvT* e, Image *imImage) {
    unsigned int mid = magick_id();
    gImage[mid] = imImage;
    return mid;
  }

  unsigned int magick_id(void) {
    unsigned int i, val;
    val = gCount;
    if (gCount == 0) magick_setup();
    for (i = 0; i < 40; ++i)
      if (gValid[i] == 0 && val > i) val = i;

    //    if(gCount==40) ERROR
    if (val >= gCount) gCount++;
    gValid[val] = 1;
    return val;
  }

  // magic_id=OPEN("Filename")
  BaseGDL* magick_open(EnvT* e) {
    START_MAGICK;
    try {
      DString filename;
      e->AssureScalarPar<DStringGDL>(0, filename);

      WordExp(filename);

      if (filename.length() == 0) e->Throw("Void file Name");

      Image *a = new Image();
      try {
        a->read(filename);
      } catch (WarningCoder &warning_) {
        cerr << warning_.what() << endl;
      }
      if ((a->rows() * a->columns()) == 0) e->Throw("Error reading image dimensions!");
      a->flip(); //necessary 
      unsigned int mid;
      mid = magick_image(e, a);
      return new DUIntGDL(mid);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }
//  a=magick_ping(name,type,"INFO", "CHANNELS", "DIMENSIONS", "HAS_PALETTE", "IMAGE_INDEX","NUM_IMAGES", "PIXEL_TYPE",/* "SUPPORTED_READ", "SUPPORTED_WRITE", */"TYPE")
  BaseGDL * magick_ping(EnvT* e) {
    // TODO!
    //if (e->KeywordPresent("SUPPORTED_READ") || e->KeywordPresent("SUPPORTED_WRITE"))
    //  e->Warning("SUPPORTED_READ and SUPPORTED_WRITE keywords not supported yet");

    // TODO: JPEG2000- and TIFF-related additional fields in the INFO structure
    START_MAGICK;
    SizeT nParam = e->NParam(1);

    try {
      DString filename;
      e->AssureScalarPar<DStringGDL>(0, filename);
      if (filename.length() == 0) return new DLongGDL(0);

      WordExp(filename);
      
      Image *a = new Image;
      try {
        a->ping(filename);
        //a->read(filename);
      } catch (WarningCoder &warning_) {
        cerr << warning_.what() << endl;
      }

      if (nParam == 2) {
        DString magick;
        e->AssureScalarPar<DStringGDL>(1, magick);
        if (a->magick() != magick) return new DLongGDL(0);
      }

      int debug = 0;
      if (debug == 1) {
        cout << "a->type()      :" << a->type() << endl;
        cout << "a->classType() :" << a->classType() << endl;
        cout << "a->matte()     :" << a->matte() << endl;
        // no useful info here:cout << "a->colorSpace()     :" << a->colorSpace() << endl;
        // Always 8:cout << "a->depth()     :" << a->depth() << endl;
        // Always 1: cout << "a->colorSpace() :" << a->colorSpace() << endl;
      }

      // AC 2012-May-10
      // http://www.graphicsmagick.org/Magick++/Image.html#type
      // relevant information that, in some cases, is provided after pinging:
      // a->matte(), a->classType() [and a->type() for Palette info only]
      DLong channels = 0;
      if (a->classType() == 1) channels = 3; // DirectColor
      if (a->classType() == 2) channels = 1; // PseudoColor
      if (channels == 0) cout << "no ClassType found for current Image" << endl;

      // AC 2012-May-10 this is NOT working with only a a->ping()
      // a->type() is FULLY reliable if and only if a->read() was done before !!! 
      //http://www.graphicsmagick.org/Magick++/Image.html#type
      // (should be OK with a a->read())
      /*
      channels = a->classType() == PseudoClass 
        ? 1      // color palette
        : a->type() == GrayscaleType 
          ? 1    // greyscale
          : a->type() == ColorSeparationType 
            ? 4  // CMYK
            : 3; // RGB
       */

      // AC 2012-May-10 this is OK (reliable), see example "589 Lavandula mono"
      if (a->matte()) channels += 1;

      // TODO! multiple images (using the Magick++ STL interface)
      DLong image_index, num_images;
      image_index = 0;
      num_images = 1; //a->scene()+1;

      DInt pixel_type;
      pixel_type = a->depth() == 16 ? 2 : 1;

      // AC 2012-May-10 Palette only if type == 4 OR 5
      // Despite Type is NOT useful without a a->read(), it is OK for Palette !
      // This should be reliable (OK with ImageMagick AND GraphicsMagick)
      DInt has_palette = 0;
      if ( a->classType() == PseudoClass) has_palette = 1;

      DString type;
      type = a->magick() == "PNM" ? "PPM" :
        a->magick() == "PGM" ? "PPM" :
        a->magick() == "DCM" ? "DICOM" :
        a->magick() == "JPC" ? "JPEG2000" :
        a->magick() == "JP2" ? "JPEG2000" :
        a->magick() == "JNG" ? "JPEG2000" :
        a->magick();

      if (debug == 1) cout << "Type (via a->magick()) : " << type << endl;
      bool addSBit=(a->magick() == "PNG"); //only one type?
      static int infoIx = e->KeywordIx("INFO");
      if (e->KeywordPresent(infoIx)) {
        e->AssureGlobalKW(infoIx);

        // creating the output anonymous structure
        DStructDesc* info_desc = new DStructDesc("$truct");
        SpDString aString;
        SpDLong aLong;
        SpDInt aInt;
        SpDLong aLongArr2(dimension(2));
        SpDByte aByteArr2(dimension(5));
        info_desc->AddTag("CHANNELS", &aLong);
        info_desc->AddTag("DIMENSIONS", &aLongArr2);
        info_desc->AddTag("HAS_PALETTE", &aInt);
        info_desc->AddTag("IMAGE_INDEX", &aLong);
        info_desc->AddTag("NUM_IMAGES", &aLong);
        info_desc->AddTag("PIXEL_TYPE", &aInt);
        if (addSBit) info_desc->AddTag("SBIT_VALUES", &aByteArr2);
        info_desc->AddTag("TYPE", &aString);
        DStructGDL* info = new DStructGDL(info_desc, dimension());

        // filling the info struct with data
        info->InitTag("CHANNELS", DLongGDL(channels));
        {
          DLongGDL dims(dimension(2));
          dims[0] = a->columns();
          dims[1] = a->rows();
          info->InitTag("DIMENSIONS", dims);
        }
        info->InitTag("HAS_PALETTE", DIntGDL(has_palette));
        info->InitTag("IMAGE_INDEX", DLongGDL(image_index));
        info->InitTag("NUM_IMAGES", DLongGDL(num_images));
        info->InitTag("PIXEL_TYPE", DIntGDL(pixel_type));
        info->InitTag("TYPE", DStringGDL(type));
        e->SetKW(infoIx, info);
      }

      static int channelsIx = e->KeywordIx("CHANNELS");
      if (e->KeywordPresent(channelsIx)) {
        e->AssureGlobalKW(channelsIx);
        e->SetKW(channelsIx, new DLongGDL(channels));
      }

      static int dimensionsIx = e->KeywordIx("DIMENSIONS");
      if (e->KeywordPresent(dimensionsIx)) {
        e->AssureGlobalKW(dimensionsIx);
        DLongGDL *dims = new DLongGDL(dimension(2));
        (*dims)[0] = a->columns();
        (*dims)[1] = a->rows();
        e->SetKW(dimensionsIx, dims);
      }

      static int has_paletteIx = e->KeywordIx("HAS_PALETTE");
      if (e->KeywordPresent(has_paletteIx)) {
        e->AssureGlobalKW(has_paletteIx);
        e->SetKW(has_paletteIx, new DIntGDL(has_palette));
      }

      static int image_indexIx = e->KeywordIx("IMAGE_INDEX");
      if (e->KeywordPresent(image_indexIx)) {
        e->AssureGlobalKW(image_indexIx);
        e->SetKW(image_indexIx, new DLongGDL(image_index));
      }

      static int num_imagesIx = e->KeywordIx("NUM_IMAGES");
      if (e->KeywordPresent(num_imagesIx)) {
        e->AssureGlobalKW(num_imagesIx);
        e->SetKW(num_imagesIx, new DLongGDL(num_images));
      }

      static int pixel_typeIx = e->KeywordIx("PIXEL_TYPE");
      if (e->KeywordPresent(pixel_typeIx)) {
        e->AssureGlobalKW(pixel_typeIx);
        e->SetKW(pixel_typeIx, new DIntGDL(pixel_type));
      }

      static int typeIx = e->KeywordIx("TYPE");
      if (e->KeywordPresent(typeIx)) {
        e->AssureGlobalKW(typeIx);
        e->SetKW(typeIx, new DStringGDL(type));
      }

      return new DLongGDL(1);
    } catch (Exception &error_) {
      return new DLongGDL(0);
    }
  }

  // magic_id=CREATE (columns, rows [, "colorname"]) 
  BaseGDL* magick_create(EnvT* e) {
    START_MAGICK;
    try {
      size_t nParam = e->NParam(2);
      DString col;
      DLong columns, rows;
      Geometry g;
      e->AssureScalarPar<DLongGDL>(0, columns);
      g.width(columns);
      e->AssureScalarPar<DLongGDL>(1, rows);
      g.height(rows);
      if (nParam == 3) { //truecolor
        e->AssureScalarPar<DStringGDL>(2, col);
        Image *a = new Image(g, Color(col));
        unsigned int mid;
        a->matte(false);
        mid = magick_image(e, a);
        return new DUIntGDL(mid);
      } else {
        Image *a = new Image(g, Color("black"));
        unsigned int mid;
        a->matte(false);
        mid = magick_image(e, a);
        return new DUIntGDL(mid);
      }

    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }
  
  //CLOSE, magic_id
  void magick_close(EnvT *e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);


      if (mid > gCount - 1) e->Throw("Invalid ID");
      else if (gValid[mid] == 0) e->Throw("ID not used");

      gValid[mid] = 0;
      delete gImage[mid];
      if (gCount - 1 == mid) gCount--;
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }

  //image=MAGICK_READINDEXES(mid) //read an indexed image.
  BaseGDL* magick_readindexes(EnvT *e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      unsigned int columns, rows;
      Image *image = magick_image(e, mid);
      if (image->classType() != PseudoClass) e->Throw("Not an indexed image: " + e->GetParString(0));

      columns = image->columns();
      rows = image->rows();
      SizeT c[2];
      c[0] = columns;
      c[1] = rows;
      dimension dim(c, 2);
#ifdef USE_MAGICK6
      if (image->modulusDepth() <= 8)
#else
      if (image->depth() <= 8)
#endif
      {
        DByteGDL *bImage = new DByteGDL(dim, BaseGDL::ZERO);
#ifndef HAS_IMAGEMAGICK
        PixelPacket *pixel_cache = image->getPixels(0,0,columns,rows); //magick command, without it writePixels do NOTHING!
        image->writePixels(IndexQuantum,(unsigned char*)(bImage->DataAddr()));
#else
        Quantum *pixel_cache = image->getPixels(0,0,columns,rows); //magick command, without it writePixels do NOTHING!
        image->writePixels(IndexQuantum,(unsigned char*)(bImage->DataAddr()));
#endif
        return bImage;
      }
#ifdef USE_MAGICK6
      else if (image->modulusDepth() <= 16)
#else     
      else if (image->depth() <= 16)
#endif
      {
        DUIntGDL *bImage = new DUIntGDL(dim, BaseGDL::NOZERO);
#ifndef HAS_IMAGEMAGICK
        PixelPacket *pixel_cache = image->getPixels(0, 0, columns, rows); //magick command, without it writePixels do NOTHING!
        image->writePixels(IndexQuantum, (unsigned char*) (bImage->DataAddr()));
#else
        Quantum *pixel_cache = image->getPixels(0, 0, columns, rows); //magick command, without it writePixels do NOTHING!
        image->writePixels(IndexQuantum, (unsigned char*) (bImage->DataAddr()));
#endif
        return bImage;
      }
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }

  //MAGICK_READCOLORMAPRGB, mid, red, green, blue
  void magick_readcolormapRGB(EnvT* e) {
    START_MAGICK;
    try {
      size_t nParam = e->NParam(1);
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      bool bgnotfound=true;
      
      if (image->classType() == PseudoClass) {
        //retrieve background color
        Color bg=image->backgroundColor();
        unsigned int cmapsize = image->colorMapSize();
        dimension cmap(cmapsize, 1);
        Color col;

#ifdef USE_MAGICK6
        if (image->modulusDepth() <= 8)
#else
        if (image->depth() <= 8)
#endif
        {
          DByteGDL* bgIndex=new DByteGDL(0);

          DByteGDL *R, *G, *B;

          R = new DByteGDL(cmap, BaseGDL::NOZERO);
          G = new DByteGDL(cmap, BaseGDL::NOZERO);
          B = new DByteGDL(cmap, BaseGDL::NOZERO);

          for (int i = 0; i < cmapsize; ++i) {
            col = image->colorMap(i);
            (*R)[i] = (col.redQuantum()) ;
            (*G)[i] = (col.greenQuantum()) ;
            (*B)[i] = (col.blueQuantum()) ;
            if (bgnotfound && (col==bg) ) {bgnotfound=false; (*bgIndex)[0]=i;}
          }
          if (nParam > 1) e->SetPar(1, R);
          if (nParam > 2) e->SetPar(2, G);
          if (nParam > 3) e->SetPar(3, B);
          if (e->KeywordPresent(0)) {e->AssureGlobalKW(0); e->SetKW(0, bgIndex);}
        }
#ifdef USE_MAGICK6
        else if (image->modulusDepth() <= 16)
#else     
        else if (image->depth() <= 16)
#endif
        {
          DUIntGDL* bgIndex=new DUIntGDL(0);
          DUIntGDL *R, *G, *B;
          R = new DUIntGDL(cmap, BaseGDL::NOZERO);
          G = new DUIntGDL(cmap, BaseGDL::NOZERO);
          B = new DUIntGDL(cmap, BaseGDL::NOZERO);

          for (int i = 0; i < cmapsize; ++i) {
            col = image->colorMap(i);
            (*R)[i] = (col.redQuantum()) ; 
            (*G)[i] = (col.greenQuantum()) ;
            (*B)[i] = (col.blueQuantum()) ; 
            if (bgnotfound && col==bg ) {bgnotfound=false; (*bgIndex)[0]=i;}
          }
          if (nParam > 1) e->SetPar(1, R);
          if (nParam > 2) e->SetPar(2, G);
          if (nParam > 3) e->SetPar(3, B);
          if (e->KeywordPresent(0)) {e->AssureGlobalKW(0); e->SetKW(0, bgIndex);}
        } else {
          e->Throw("Unknown Image type, too many colors");
        }
      } else {
        e->Throw("Not an indexed image: " + e->GetParString(0));
      }

    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
// iImage = MAGIC_READ(magic_id [,RGB=rgb_code] [,SUB_RECT=[a,b,c,d]] [,MAP=map_code])
// rgb_code is 0="BGR", 1="RGB", 2="RBG", 3="BRG", 4="GRB", 5="GBR"
// map_code is any of  combination or order of R = red, G = green, B = blue, A = alpha, C = cyan, Y = yellow M = magenta, and K = black.
// The ordering reflects the order of the pixels in the supplied pixel array.
  BaseGDL* magick_read(EnvT *e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      unsigned int columns, rows, lx, ly, wx, wy;
      Image* image = magick_image(e, mid);

      columns = image->columns();
      rows = image->rows();
      if ((rows * columns) == 0) e->Throw("Error reading image dimensions!");
      string map = "RGB";
      if (image->matte()) map = map + "A";

      if (e->GetKW(0) != NULL)//RGB
      {
        DInt rgb;
        e->ProvideScalarKW<DIntGDL>(0, rgb);
        if (rgb == 0) map = "BGR";
        else if (rgb == 1) map = "RGB";
        else if (rgb == 2) map = "RBG";
        else if (rgb == 3) map = "BRG";
        else if (rgb == 4) map = "GRB";
        else if (rgb == 5) map = "GBR";
        else {
          string s = "MAGICK_READ: RGB order type not supported (";
          s += i2s(rgb);
          s += "), using RGB ordering.";
          Message(s);
          map = "RGB";
        }
      }

      e->AssureStringScalarKWIfPresent(2, map);

      lx = 0;
      ly = 0;
      wx = columns;
      wy = rows;
      if (e->GetKW(1) != NULL)//SUB_RECT
      {
        BaseGDL* sr = e->GetKW(1);
        DULongGDL * subrect = static_cast<DULongGDL*> (sr->Convert2(GDL_ULONG, BaseGDL::COPY));
        if (subrect->N_Elements() != 4)
          e->Throw("Not enough elements in SUB_RECT, expected 4.");
        lx = (*subrect)[0]; //guaranteed to be >0
        ly = (*subrect)[1];
        wx = (*subrect)[2];
        wy = (*subrect)[3];

        if (wx > columns)
          e->Throw("Requested width exceeds number of columns, Either reduce the width or the X origin.");

        if (ly + wy > rows)
          e->Throw("Requested height exceeds number of rows. Either reduce the height or the Y origin.");
      }

      dimension dim;
      if (map.length() > 1) dim << map.length();
      dim << wx;
      dim << wy;

      if (image->depth() == 8) {
        DByteGDL *bImage = new DByteGDL(dim, BaseGDL::NOZERO);
        image->write(lx, ly, wx, wy, map, CharPixel, &(*bImage)[0]);
        return bImage;
      } else if (image->depth() == 16) {
        DUIntGDL* iImage = new DUIntGDL(dim, BaseGDL::NOZERO);
        image->write(lx, ly, wx, wy, map, ShortPixel, &(*iImage)[0]);
        return iImage;
      } else {
        e->Throw("Unsupported bit depth");
      }


    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }

  // MAGICK_WRITE, mid, gdlImageArray, rgb=rgb
  //general purpose writing array of pixels to internal format at magic number mid.
  void magick_write(EnvT* e) {
    START_MAGICK;
    try {

      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      int columns, rows, planes;
      //	StorageType ty;
      //	ty=CharPixel;
      BaseGDL* GDLimage = e->GetParDefined(1);

      string map = "BGR";
      if (GDLimage->Rank() == 3) {
        planes = GDLimage->Dim(0);
        columns = GDLimage->Dim(1);
        rows = GDLimage->Dim(2);
        if (planes == 2) {
          map = "IA";
        } else {
          if (e->GetKW(0) != NULL)//RGB
          {
            DInt rgb;
            e->ProvideScalarKW<DIntGDL>(0, rgb);

            if (rgb == 0) map = "BGR";
            else if (rgb == 1) map = "RGB";
            else if (rgb == 2) map = "RBG";
            else if (rgb == 3) map = "BRG";
            else if (rgb == 4) map = "GRB";
            else if (rgb == 5) map = "GBR";
            else {
              string s = "MAGICK_WRITE: RGB order type not supported (";
              s += i2s(rgb);
              s += "), using BGR ordering.";
              Message(s);
              map = "BGR";
            }
            if (image->matte()) map = map + "A";
          }
        }
        if (image->depth() == 8 || GDLimage->Type()==GDL_BYTE || !(IntType(GDLimage->Type()))) { //only INTeger values > BYTE can be saved as 16 bits see WRITE_PNG help
          DByteGDL * bImage = static_cast<DByteGDL*> (GDLimage->Convert2(GDL_BYTE, BaseGDL::COPY));
          Guard<DByteGDL> bImageGuard(bImage);
        // Ensure that there are no other references to this image.
          image->modifyImage();
          image->read(columns, rows, map, CharPixel, &(*bImage)[0]);
        } else if (image->depth() == 16) { //the only one able to support writng INTeger values > BYTES
            DUIntGDL * iImage = static_cast<DUIntGDL*> (GDLimage->Convert2(GDL_UINT, BaseGDL::COPY));
            Guard<DUIntGDL> iImageGuard(iImage);
        // Ensure that there are no other references to this image.
            image->modifyImage();
            image->read(columns, rows, map, ShortPixel, &(*iImage)[0]);
        } else {
          e->Throw("Unsupported bit depth");
        }
      } else {
        columns = GDLimage->Dim(0);
        rows = GDLimage->Dim(1);
        DByteGDL * bImage =
          static_cast<DByteGDL*> (GDLimage->Convert2(GDL_BYTE, BaseGDL::COPY));
        Guard<DByteGDL> bImageGuard(bImage);
        // Ensure that there are no other references to this image.
        image->modifyImage();
        // Set the image type to Palette.
        image->type(PaletteType);
//
        if (image->colorMapSize() < 1) e->Throw("GDL internal: destination image has no colormap!");
        image->size(Geometry(columns, rows));
        image->setPixels(0,0,columns, rows);
        image->read(columns, rows, "I", CharPixel, &(*bImage)[0]);
        image->syncPixels();
      }
      image->flip();
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }

  }
//MAGICK_WRITEFILE, mid, filename, imageType
  void magick_writefile(EnvT* e) {
    START_MAGICK;
    try {
      size_t nParam = e->NParam(2);
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);

      DString filename;
      e->AssureScalarPar<DStringGDL>(1, filename);
      WordExp(filename);

      if (nParam == 3) {
        DString imagetype;
        e->AssureScalarPar<DStringGDL>(2, imagetype);
        image->magick(imagetype);
      }
      image->write(filename);
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }

  //Attributes

  //ncolors=MAGICK_COLORMAPSIZE(mid)
  BaseGDL* magick_colormapsize(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      size_t nParam = e->NParam(1);
      if (nParam == 2) { //to be tested, works?
        DUInt ncol;
        e->AssureScalarPar<DUIntGDL>(1, ncol);
        image->colorMapSize(ncol);
        //magick_replace(e, mid, image);
      }

      return new DLongGDL(image->colorMapSize());
    } catch (Exception &error_) {
      return new DLongGDL(-1);
    }
    return NULL; //pacify -Wreturn-type
  }

  //magickNumber=MAGICK_MAGICK(mid [, "imageType"])
  BaseGDL* magick_magick(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      size_t nParam = e->NParam(1);
      if (nParam == 2) {
        DString format;
        e->AssureScalarPar<DStringGDL>(1, format);
        image->magick(format);
        //magick_replace(e, mid, image);
      }

      return new DStringGDL(image->magick());
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }

  //nrows=MAGICK_ROWS(mid)
  BaseGDL* magick_rows(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      return new DLongGDL(image->rows());
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }

  //ncols=MAGICK_COLUMNS(mid)
  BaseGDL* magick_columns(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      return new DLongGDL(image->columns());
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }
//bool=MAGICK_INDEXEDCOLOR(mid)
  BaseGDL* magick_IndexedColor(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);

      if (image->classType() == DirectClass)
        return new DIntGDL(0);
      if (image->classType() == PseudoClass)
        return new DIntGDL(1);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
    return NULL; //pacify -Wreturn-type
  }

  //MAGICK_QUALITY, mid, quality
  void magick_quality(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      DUInt q;
      size_t nParam = e->NParam(2);
      if (nParam == 2) e->AssureScalarPar<DUIntGDL>(1, q);
      if (nParam == 2) q = 75;
      //check before we do anything.

      Image* image = magick_image(e, mid);
      image->quality(q);
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }


  //manipulations
  //MAGIC_FLIP,mid
  void magick_flip(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      image->flip();
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
//MAGICK_MATTE, mid
  void magick_matte(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      image->matte(true);

      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
  //MAGICK_MATTECOLOR, mid, index
  void magick_mattecolor(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid, index;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      e->AssureScalarPar<DUIntGDL>(1, index);
      if (index >= 0 && index < image->colorMapSize()) {
        image->transparent(image->colorMap(index));

        //magick_replace(e, mid, image);
      }
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
//MAGICK_INTERLACE, mid, /NOINTERLACE, /LINEINTERLACE, /PLANEINTERLACE
  void magick_interlace(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);


      //NoInterlace.......Don't interlace image (RGBRGBRGBRGBRGBRGB...)
      //LineInterlace.....Use scanline interlacing (RRR...GGG...BBB...RRR...GGG...BBB...)
      //PlaneInterlace....Use plane interlacing (RRRRRR...GGGGGG...BBBBBB...)

      if (e->KeywordSet(0))//NoInterlace
        image->interlaceType(NoInterlace);
      else if (e->KeywordSet(1))//LineInterlace
        image->interlaceType(LineInterlace);
      else if (e->KeywordSet(2))//PlaneInterlace
        image->interlaceType(PlaneInterlace);

      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
//MAGICK_ADDNOISE,mid, /UNIFORMNOISE, /GAUSSIANNOISE, /MULTIPLICATIVEGAUSSIANNOISE, /IMPULSENOISE, /LAPLACIANNOISE",
 // /POISSONNOISE, NOISE=value
  void magick_addNoise(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);

      if (e->KeywordSet(0))//Uniform noise
        image->addNoise(UniformNoise);
      else if (e->KeywordSet(1))//Gaussian noise
        image->addNoise(GaussianNoise);
      else if (e->KeywordSet(2))//Multiplicative Gaussian noise
        image->addNoise(MultiplicativeGaussianNoise);
      else if (e->KeywordSet(3))//Impulse noise
        image->addNoise(ImpulseNoise);
      else if (e->KeywordSet(4))//Laplacian noise
        image->addNoise(LaplacianNoise);
      else if (e->KeywordSet(5))//Poisson noise
        image->addNoise(PoissonNoise);
      else if (e->GetKW(6) != NULL) {
        DInt noise;
        e->ProvideScalarKW<DIntGDL>(6, noise);

        if (noise == 0)//Uniform noise
          image->addNoise(UniformNoise);
        else if (noise == 1)//Gaussian noise
          image->addNoise(GaussianNoise);
        else if (noise == 2)//Multiplicative Gaussian noise
          image->addNoise(MultiplicativeGaussianNoise);
        else if (noise == 3)//Impulse noise
          image->addNoise(ImpulseNoise);
        else if (noise == 4)//Laplacian noise
          image->addNoise(LaplacianNoise);
        else if (noise == 5)//Poisson noise
          image->addNoise(PoissonNoise);
        else
          e->Throw("Unknown noise type requested.");
      } else//no keyword
        image->addNoise(UniformNoise);

      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
//MAGICK_QUANTIZE, mid [, ncolors] [,/TRUECOLOR] [,/YUV] [,/GRAYSCALE] [,/DITHER]
  void magick_quantize(EnvT* e) {
    START_MAGICK;
    try {
      size_t nParam = e->NParam();

      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      image->modifyImage();
      //set the number of colors;
      DLong ncol = (image->depth()<16)?256:65635;
      if (nParam > 1) e->AssureLongScalarPar(1, ncol);

      static int TRUECOLORIx = e->KeywordIx("TRUECOLOR");
      static int DITHERIx = e->KeywordIx("DITHER");
      bool dither=false;
      if (e->GetKW(DITHERIx) !=NULL) dither=e->KeywordSet(DITHERIx);
      static int YUVIx = e->KeywordIx("YUV");
      static int GRAYSCALEIx = e->KeywordIx("GRAYSCALE");
      
        if (e->KeywordSet(YUVIx)) //YUV
          image->quantizeColorSpace(YUVColorspace);
        else if (e->KeywordSet(GRAYSCALEIx)) //Grayscale
          image->quantizeColorSpace(GRAYColorspace);
        else
          image->quantizeColorSpace(RGBColorspace);
        
      image->quantizeColors(ncol);
      image->quantizeDither(dither);
      image->quantize();
//      image->quantizeTreeDepth(cube_depth);
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }
  
  //return type
  BaseGDL* magick_type(EnvT* e){
    START_MAGICK;
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0, mid);
    Image* image = magick_image(e, mid);
    return new DStringGDL(imageTypeList[image->type()]);
  }
  //return class
  BaseGDL* magick_class(EnvT* e){
    START_MAGICK;
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0, mid);
    Image* image = magick_image(e, mid);
    return new DStringGDL(imageClassList[image->classType()]);
  }

  //MAGICK_DISPLAY,mid

  void magick_display(EnvT* e) {
    START_MAGICK;
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0, mid);
    Image* image = magick_image(e, mid);
    image->display();

  }
// //NOT USED -- AND WHAT SOES IT DO?
//  void magick_writeIndexes(EnvT* e) {
//    START_MAGICK;
//    try {
//      DUInt mid;
//      e->AssureScalarPar<DUIntGDL>(0, mid);
//      BaseGDL* GDLimage = e->GetParDefined(1);
//      DByteGDL * bImage = static_cast<DByteGDL*> (GDLimage->Convert2(GDL_BYTE, BaseGDL::COPY));
//
//      Image* image = magick_image(e, mid);
//#ifndef HAS_IMAGEMAGICK
////      const MagicCore::PixelPacket* pixels;
//      IndexPacket* index;
//
//      unsigned int columns, rows;
//      columns = image->columns();
//      rows = image->rows();
//
////      pixels = image->setPixels(0, 0, columns, rows);
//      index = image->getIndexes();
//
//      SizeT nEl = columns*rows;
//      {
//        for (SizeT cx = 0; cx < nEl; ++cx) {
//          index[cx] = static_cast<unsigned int> ((*bImage)[cx]);
//        }
//      }
//#endif
//      image->syncPixels();
//
//      //magick_replace(e, mid, image);
//    } catch (Exception &error_) {
//      e->Throw(error_.what());
//    }
//  }

  //MAGICK_WRITECOLORTABLE,mid[,r,g,b]

  void magick_writeColorTable(EnvT* e) {
    START_MAGICK;
    try {
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0, mid);
      Image* image = magick_image(e, mid);
      SizeT nparam = e->NParam();
      if (nparam != 1 && nparam != 4) e->Throw("invalid number of parameters for MAGICK_WRITECOLORTABLE Procedure.");
//      unsigned int scale;
//      scale = 255;
      //these would be Palette type images I bet.
      image->type(PaletteType);

      if (nparam == 4) { //get passed LUT
        BaseGDL* GDLCol = e->GetParDefined(1);
        DByteGDL * Red = static_cast<DByteGDL*> (GDLCol->Convert2(GDL_BYTE, BaseGDL::COPY));
        //e->Guard( Red);
        Guard<BaseGDL> r_guard(Red);
        GDLCol = e->GetParDefined(2);
        DByteGDL * Green = static_cast<DByteGDL*> (GDLCol->Convert2(GDL_BYTE, BaseGDL::COPY));
        //e->Guard( Green);
        Guard<BaseGDL> g_guard(Green);
        GDLCol = e->GetParDefined(3);
        DByteGDL *Blue = static_cast<DByteGDL*> (GDLCol->Convert2(GDL_BYTE, BaseGDL::COPY));
        //e->Guard( Blue);
        Guard<BaseGDL> b_guard(Blue);

        if (Red->N_Elements() == Green->N_Elements() &&
          Red->N_Elements() == Blue->N_Elements()) {
          unsigned long n = Red->N_Elements();
          image->colorSpace(RGBColorspace);
          image->colorMapSize(n);
          image->quantize(n);
          for (unsigned long c = 0; c < n; ++c) image->colorMap(c, ColorRGB((double)(*Red)[c] / 255., (double)(*Green)[c] / 255., (double)(*Blue)[c] / 255.));
          }
      } else { //GET current LOADCT LUT
        PLINT r[ctSize], g[ctSize], b[ctSize];
        GraphicsDevice::GetDevice()->GetCT()->Get(r, g, b);
        unsigned long n = ctSize;
        image->colorSpace(RGBColorspace);
        image->colorMapSize(n);
        image->quantize(n);
        for (unsigned long c = 0; c < n; ++c) image->colorMap(c, ColorRGB((double)r[c] / 255., (double)g[c] / 255., (double)b[c] / 255.));
      }
      //magick_replace(e, mid, image);
    } catch (Exception &error_) {
      e->Throw(error_.what());
    }
  }


  /*
    void magick_template(EnvT* e)
  {
    try{
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0,mid);    
    Image image=magick_image(e,mid);

    magick_replace(e,mid,image);      
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }
   */

}

#endif
