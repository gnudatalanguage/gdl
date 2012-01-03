/* *************************************************************************
                          deviceps.hpp  -  postscript device
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DEVICEPS_HPP_
#  define DEVICEPS_HPP_

#  include "gdlpsstream.hpp"
#  include "plotting.hpp" // get_axis_crange for TV()
#  include "initsysvar.hpp"
#  include <gsl/gsl_const_mksa.h> // GSL_CONST_MKSA_INCH

#  ifdef USE_PSLIB
#    include <stdio.h> // tmpnam
#    include <sys/utsname.h> // uname
#    include <libps/pslib.h>
#  endif

#  ifdef HAVE_OLDPLPLOT
#    define SETOPT SetOpt
#  else
#    define SETOPT setopt
#  endif

class DevicePS: public Graphics
{
  std::string      fileName;
  GDLPSStream*     actStream;
  float            XPageSize;
  float            YPageSize;
  float            XOffset;
  float            YOffset;
  int              color;
  int              decomposed; // false -> use color table
  bool	           orient_portrait; 
  bool             encapsulated;
  float	           scale;

  static const int dpi = 72;
  static const float cm2in = .01 / GSL_CONST_MKSA_INCH;

  void InitStream()
  {
    delete actStream;

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;

    actStream = new GDLPSStream( nx, ny, SysVar::GetPFont(), encapsulated);

    actStream->sfnam( fileName.c_str());

    // zeroing offsets (xleng and yleng are the default ones but they need to be specified 
    // for the offsets to be taken into account by spage(), works with plplot >= 5.9.9)
    actStream->spage(dpi, dpi, 540, 720, 0, 0);

    // as setting the offsets and sizes with plPlot is (extremely) tricky, and some of these setting
    // are hardcoded into plplot (like EPS header, and offsets in older versions of plplot)
    // here we only specify the aspect ratio - size an offset are handled by pslib when device,/close is called
    char as[32];
    sprintf(as, "%f", XPageSize / YPageSize);
    actStream->SETOPT( "a", as);

    // plot orientation
    actStream->sori(orient_portrait ? 1 : 2);

    // no pause on destruction
    actStream->spause( false);

    // extended fonts
    actStream->fontld( 1);

    // set color map
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    //    actStream->scmap0( r, g, b, ctSize); 
    actStream->scmap1( r, g, b, ctSize); 
    actStream->scolbg(255,255,255); // white background

    // default: black+white (IDL behaviour)
    //actStream->scolor( color); // has no effect
    if (color == 0) 
      actStream->SETOPT( "drvopt","text=0,color=0"); 
    else 
      actStream->SETOPT( "drvopt","text=0,color=1");
    color=0;

    actStream->Init();
    
    // load font
    actStream->font( 1);
    actStream->DefaultCharSize();

    //    (*pMulti)[ 0] = 0;
    actStream->adv(0);
  }

private:
  void pslibHacks()
  {
#  ifndef USE_PSLIB
    Warning("Warning: pslib support is mandatory for the PostScript driver to handle the following");
    Warning("         keywords: [X,Y]SIZE, [X,Y]OFFSET, SCALE_FACTOR, LANDSCAPE, PORTRAIT, ENCAPSULATED");
#  else
    PSDoc *ps = PS_new(); 
    if (ps == NULL)
    {
      Warning("Warning: pslib failed to allocate memory.");
      return;
    }
    FILE *fp = tmpfile(); // this creates a file which should be deleted automaticaly when it is closed
    if (fp == NULL) 
    {
      Warning("Warning: failed to create temporary PostScript file.");
      PS_delete(ps);
      return;
    }
    if (PS_open_fp(ps, fp) == -1) 
    { 
      Warning("Warning: pslib failed to open a new PostScript file.");
      goto cleanup;
    }
    
    PS_set_parameter(ps, "imagereuse", "false");
    PS_set_info(ps, "Title", "Graphics produced by GDL"); 
    PS_set_info(ps, "Orientation", orient_portrait ? "Portrait" : "Landscape"); 
    {
      struct utsname uts;
      uname(&uts);
      string tmp;
      tmp = "GDL Version " + string(VERSION) + ", " + string(uts.sysname) + " " + string(uts.machine);
      PS_set_info(ps, "Creator", tmp.c_str()); 
      char* login = getlogin();
      if (login == NULL) Warning("Warning: getlogin() failed!");
      tmp = (login == NULL ? "?" : login) + string("@") + uts.nodename;
      PS_set_info(ps, "Author", tmp.c_str());
    }

    // TODO
    //psfont = PS_findfont(ps, "Helvetica", "", 0); 
    //PS_setfont(ps, psfont, 8.0); 

    {
      PS_begin_page(ps, XPageSize * cm2in * dpi, YPageSize * cm2in * dpi);
      {
        int psimage = PS_open_image_file(ps, "eps", fileName.c_str(), NULL, 0);
        if (psimage == 0)
        {
          Warning("Warning: pslib failed to load plPlot output file.");
          goto cleanup;
        }

        float scl = orient_portrait
          ? (XPageSize * cm2in * dpi) / (PS_get_value(ps, "imagewidth", (float) psimage))
          : (YPageSize * cm2in * dpi) / (PS_get_value(ps, "imagewidth", (float) psimage));
        PS_place_image(ps, psimage, 
          XOffset * cm2in * dpi,
          YOffset * cm2in * dpi, 
          scale * scl
        );
        PS_close_image(ps, psimage); 
      }
      PS_end_page(ps);
      PS_close(ps);
    }
    
    // write contents to fileName
    {
      rewind(fp);
      FILE *fp_plplot = fopen(fileName.c_str(), "w");
      if (fp_plplot == NULL)
      {
        Warning("Warning: failed to open plPlot-generated file");
        goto cleanup;
      }
      const size_t buflen=4096;
      unsigned char buff[buflen];
      while (true)
      {
        size_t cnt = fread(&buff, 1, buflen, fp);
        if (!cnt) break;
        if (fwrite(&buff, 1, cnt, fp_plplot) < cnt)
        {
          Warning("Warning: failed to overwrite the plPlot-generated file with pslib output");
        }
      }
      fclose(fp_plplot);
    }

    cleanup:
    PS_delete(ps);
    fclose(fp); // this deletes the temporary file as well
    // PSlib changes locale - bug no. 3428043
#    ifdef HAVE_LOCALE_H
    setlocale(LC_ALL, "C");
#    endif
#  endif
  }

public:
  DevicePS(): Graphics(), fileName( "gdl.ps"), actStream( NULL), color(0), 
    decomposed( 0), encapsulated(false), scale(1.)
  {
    name = "PS";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( 17780)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( 12700)); 
    dStruct->InitTag("X_VSIZE",    DLongGDL( 640)); 
    dStruct->InitTag("Y_VSIZE",    DLongGDL( 512)); 
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 0)); 
    dStruct->InitTag("X_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( 1000.0)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 0)); 
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 266807)); 
    dStruct->InitTag("ORIGIN",     origin); 
    dStruct->InitTag("ZOOM",       zoom); 

    SetPortrait();

#  ifdef USE_PSLIB
    PS_boot();
    // PSlib changes locale - bug no. 3428043
#    ifdef HAVE_LOCALE_H
    setlocale(LC_ALL, "C");
#    endif
#  endif
  }
  
  ~DevicePS()
  {
    delete actStream;
#  ifdef USE_PSLIB
    PS_shutdown();
    // PSlib changes locale - bug no. 3428043
#    ifdef HAVE_LOCALE_H
    setlocale(LC_ALL, "C");
#    endif
#  endif
  }

  GDLGStream* GetStream( bool open=true)
  {
    if( actStream == NULL) 
      {
	if( !open) return NULL;
	InitStream();
      }
    return actStream;
  }

  bool SetFileName( const std::string& f)
  {
    fileName = f;
    return true;
  }

  bool CloseFile()
  {
    if (actStream != NULL)
    {
      delete actStream;
      actStream = NULL;
      if (!encapsulated) pslibHacks(); // needs to be called after the plPlot-generated file is closed
    }
    return true;
  }

  bool SetXOffset( const float xo) // xo [cm]
  {
    XOffset=xo;
    return true;
  }

  bool SetYOffset( const float yo) // yo [cm]
  {
    YOffset=yo;
    return true;
  }

  bool SetXPageSize( const float xs) // xs [cm]
  {
    XPageSize=xs;
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_SIZE"))))[0] 
      = DLong(floor(0.5+
        xs * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_PX_CM"))))[0]
      ));
    return true;
  }

  bool SetYPageSize( const float ys) // ys [cm]
  {
    YPageSize=ys;
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_SIZE"))))[0] 
      = DLong(floor(0.5+
        ys * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_PX_CM"))))[0]
      ));
    return true;
  }

  bool SetColor()
  {
    color=1;
    return true;
  }

  bool SetPortrait()
  {
    orient_portrait = true;
    XPageSize = 7 * 100. * GSL_CONST_MKSA_INCH;
    YPageSize = 5 * 100. * GSL_CONST_MKSA_INCH; 
    XOffset = .75 * 100. * GSL_CONST_MKSA_INCH; 
    YOffset = 3 * 100. * GSL_CONST_MKSA_INCH; // TODO: this is different from IDL docs
    return true;
  }

  bool SetLandscape()
  {
    orient_portrait = false;
    XPageSize = 10 * 100. * GSL_CONST_MKSA_INCH; 
    YPageSize = 7 * 100. * GSL_CONST_MKSA_INCH; 
    XOffset = .5 * 100. * GSL_CONST_MKSA_INCH; 
    YOffset = .75 * 100. * GSL_CONST_MKSA_INCH;
    return true;
  }

  bool SetScale(float value)
  {
    scale = value;
    return true;
  }

  bool SetEncapsulated(bool val)
  {
    // TODO ?: change XPageSize, YPageSize, XOffset, YOffset
    encapsulated = val;
    return true;
  }

  bool Decomposed( bool value)           
  {   
    decomposed = value;
    return true;
  }

  DLong GetDecomposed()        
  {
    return decomposed;  
  }

  // TODO: SA: this TV() should be merged with TV() in DeviceX and DeviceZ!
  // TODO: SA: just a draft - a lot more needs to be done...
  void TV( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 

    GDLGStream* actStream = GetStream();

    // TODO: use it is XSIZE and YSIZE is not specified!
    //DLong xsize = (*static_cast<DLongGDL*>( dStruct->GetTag( xSTag, 0)))[0];
    //DLong ysize = (*static_cast<DLongGDL*>( dStruct->GetTag( ySTag, 0)))[0];

    DLong pos=0; // TODO: handle it!
    DDouble xmin, ymin;
    {
      DDouble null;
      lib::get_axis_crange("X", xmin, null);
      lib::get_axis_crange("Y", ymin, null);
    }
    if (nParam == 2) {
      e->AssureLongScalarPar( 1, pos);
    } else if (nParam >= 3) {
      if (e->KeywordSet("NORMAL")) 
      {
        e->Throw("NORMAL keyword not supported yet");
	//e->AssureDoubleScalarPar( 1, xmin);
	//e->AssureDoubleScalarPar( 2, ymin);
	//xLL = (DLong) rint(xLLf * xsize);
	//yLL = (DLong) rint(yLLf * ysize);
      } 
      else if (e->KeywordSet("DEVICE")) 
      {
        e->Throw("DEVICE keyword not supported yet");
      }
      else // aka DATA
      {
	e->AssureDoubleScalarPar( 1, xmin);
	e->AssureDoubleScalarPar( 2, ymin);
      }
    }

    DByteGDL* p0B = e->GetParAs<DByteGDL>( 0);
    SizeT rank = p0B->Rank();

    int width, height;
    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (rank == 2) 
      {
	if (tru != 0)
	  e->Throw( "Array must have 3 dimensions: "+
		    e->GetParString(0));
	width  = p0B->Dim(0);
	height = p0B->Dim(1);
      } 
    else if( rank == 3) 
      {
	if (tru == 1) {
	  width = p0B->Dim(1);
	  height = p0B->Dim(2);
	} else if (tru == 2) {
	  width = p0B->Dim(0);
	  height = p0B->Dim(2);
	} else if (tru == 3) {
	  width = p0B->Dim(0);
	  height = p0B->Dim(1);
	} else {
	  e->Throw( "TRUE must be between 1 and 3");
	}
      } else {
	e->Throw( "Image array must have rank 2 or 3");
      }
    if (tru != 0) e->Throw("Decomposed images not supported yet with PostScript + TV() (FIXME)"); // TODO!

    /* TODO...
    if( width + xLL > xsize || height + yLL > ysize)
      e->Throw( "Value of image coordinates is out of allowed range.");
    */

    class grid2d {
      public: PLFLT** data;
      private: GDLGStream *pls;
      private: int w, h;
      public: grid2d(GDLGStream *actStream, int w, int h) 
        : pls(actStream), w(w), h(h) { pls->Alloc2dGrid(&data, w, h); }
      public: ~grid2d() { pls->Free2dGrid(data, w, h); }
    } idata(actStream, width, height);
    for (int x=0; x < width; ++x)
      for (int y=0; y < height; ++y)
        idata.data[x][y] = (*p0B)[x + y * width]; 

    PLFLT xmax, ymax;
    if (e->KeywordSet("XSIZE")) 
    {
      DDouble tmp;
      e->AssureDoubleScalarKW("XSIZE", tmp);
      xmax = xmin + tmp;
    }
    else e->Throw("Specification of XSIZE is mandatory for PostScript/TV() (FIXME!)"); // TODO!
    if (e->KeywordSet("YSIZE")) 
    {
      DDouble tmp;
      e->AssureDoubleScalarKW("YSIZE", tmp);
      ymax = ymin + tmp;
    }
    else e->Throw("Specification of YSIZE is mandatory for PostScript/TV() (FIXME!)"); // TODO!

    // TODO: map projection (via the last two arguments - same as was done in CONTOUR e.g.)
    bool mapSet = false;
#ifdef USE_LIBPROJ4
    //get_mapset(mapSet);
#endif
    if (mapSet) e->Throw("PostScript + TV() + mapping cobination not available yet (FIXME!)");

    actStream->imagefr(idata.data, width, height, xmin, xmax, ymin, ymax, 0., 255., 0., 255., NULL, NULL); 
  }

};

#endif
