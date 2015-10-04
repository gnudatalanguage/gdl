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
#define DEVICEPS_HPP_

#include "gdlpsstream.hpp"
#include "plotting.hpp" // get_axis_crange for TV()
#include "initsysvar.hpp"
#include <gsl/gsl_const_mksa.h> // GSL_CONST_MKSA_INCH

#include "objects.hpp"


#  ifdef USE_PSLIB
#    include <stdio.h> // tmpnam
#    if !defined(_WIN32) || defined(__CYGWIN__)
#        include <sys/utsname.h> // uname
#    endif
#    include <libps/pslib.h>
#  endif

#  ifdef HAVE_OLDPLPLOT
#    define SETOPT SetOpt
#  else
#    define SETOPT setopt
#  endif

#ifdef _MSC_VER
#define CM2IN (.01 / GSL_CONST_MKSA_INCH) // This is not good, but works
#define in2cm ( GSL_CONST_MKSA_INCH * 100)
#define DPI 72.0 //in dpi;
#define RESOL 1000.0
#else
  static const float CM2IN = .01 / GSL_CONST_MKSA_INCH;
  static const float in2cm = GSL_CONST_MKSA_INCH*100;
  static const PLFLT DPI = 72.0 ; //in dpi;
  static const float RESOL = 1000.0;
#endif

class DevicePS: public GraphicsDevice
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
  int              bitsPerPix;
  float	           scale;

  GDLStream  *psUnit;

  void InitStream()
  {
    delete actStream;

    DLongGDL* pMulti = SysVar::GetPMulti();
    DLong nx = (*pMulti)[ 1];
    DLong ny = (*pMulti)[ 2];

    if( nx <= 0) nx = 1;
    if( ny <= 0) ny = 1;
    actStream = new GDLPSStream( nx, ny, (int)SysVar::GetPFont(), encapsulated, color, bitsPerPix);

    actStream->sfnam( fileName.c_str());

    // trying to solve bug report 3611898
    // AC 29-Avril-2013: the way I found to link GDLPSStream* and GDLStream*
    DLong lun=GetLUN();
    psUnit = &fileUnits[ lun-1];
    psUnit->Open(fileName,fstream::out,false,false,false,
		 defaultStreamWidth,false,false);
    (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("UNIT"))))[0]=lun;

    // zeroing offsets (xleng and yleng are the default ones but they need to be specified 
    // for the offsets to be taken into account by spage(), works with plplot >= 5.9.9)    
    actStream->spage(DPI, DPI, 540, 720, 0, 0); //plplot default: portrait!

    // as setting the offsets and sizes with plPlot is (extremely) tricky, and some of these setting
    // are hardcoded into plplot (like EPS header, and offsets in older versions of plplot)
    // here we play only with the aspect ratio - size and offset are handled by pslib when device,/close is called

    // patch 3611949 by Joanna, 29 Avril 2013
    PLFLT pageRatio=XPageSize/YPageSize;
    std::string as = i2s( pageRatio);
    actStream->SETOPT( "a", as.c_str());
    
    // plot orientation
    //std::cout  << "orientation : " << orient_portrait<< std::endl;
    
    actStream->sdiori(orient_portrait ? 1 : 2);
    
    // no pause on destruction
    actStream->spause( false);

    // extended fonts
    actStream->fontld( 1);
    
    PLINT r[ctSize], g[ctSize], b[ctSize];
    actCT.Get( r, g, b);
    actStream->scmap0( r, g, b, ctSize);
    actStream->scmap1( r, g, b, ctSize);
    // default: black+white (IDL behaviour)
    short font=((int)SysVar::GetPFont()>-1)?1:0;
    string what="text="+i2s(font)+",color="+i2s(color);
    actStream->SETOPT( "drvopt",what.c_str());
    actStream->scolbg(255,255,255); // start with a white background

    actStream->Init();
    
    // need to be called initially. permit to fix things
    actStream->ssub(1,1);
    actStream->adv(0);
    // load font
    actStream->font( 1);
    actStream->vpor(0,1,0,1);
    actStream->wind(0,1,0,1);
    actStream->DefaultCharSize();
   //in case these are not initalized, here is a good place to do it.
    if (actStream->updatePageInfo()==true)
    {
        actStream->GetPlplotDefaultCharSize(); //initializes everything in fact..

    }
    PLFLT xp, yp;
    PLINT xleng, yleng, xoff, yoff;
    actStream->gpage(xp, yp, xleng, yleng, xoff, yoff);
    // to mimic IDL we must scale char so that the A4 charsize is constant whatever the size of the plot
    PLFLT size = (XPageSize>YPageSize)?XPageSize:YPageSize;
    PLFLT refsize= (xleng/xp>yleng/yp)?xleng/xp:yleng/yp;
    PLFLT charScale=(refsize*in2cm)/size;
    PLFLT defhmm, scalhmm;
    plgchr(&defhmm, &scalhmm); // height of a letter in millimetres
    actStream->RenewPlplotDefaultCharsize(defhmm * charScale);
  }
    
private:
  void pslibHacks()
  {
#  ifndef USE_PSLIB
    Warning("Warning: pslib support is mandatory for the PostScript driver to handle correctly the following");
    Warning("         keywords:  [X,Y]OFFSET, [X,Y]SIZE, SCALE_FACTOR");
#  else
    PSDoc *ps = PS_new(); 
    GDLGuard<PSDoc> psGuard( ps, PS_delete);
    
    if (ps == NULL)
    {
      Warning("Warning: pslib failed to allocate memory.");
      return;
    }
    
    FILE *fp = tmpfile(); // this creates a file which should be deleted automaticaly when it is closed
    FILEGuard fpGuard( fp, fclose);
    
    if (fp == NULL) 
    {
      Warning("Warning: failed to create temporary PostScript file.");
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
      string tmp;
#if !defined(_WIN32) || defined(__CYGWIN__)
      struct utsname uts;
      uname(&uts);
      tmp = "GDL Version " + string(VERSION) + ", " + string(uts.sysname) + " " + string(uts.machine);
      PS_set_info(ps, "Creator", tmp.c_str()); 
      char* login = getlogin();
      if (login == NULL) Warning("Warning: getlogin() failed!");
      tmp = (login == NULL ? "?" : login) + string("@") + uts.nodename;
      PS_set_info(ps, "Author", tmp.c_str());
#else
      tmp = "GDL Version " + string(VERSION) + ", Microsoft Windows x32";
      PS_set_info(ps, "Creator", tmp.c_str());

      WCHAR username[257];
      char cusername[257];
      DWORD username_len = 257;
      GetUserNameW(username, &username_len);

      WideCharToMultiByte(CP_ACP, 0, username, username_len, cusername, username_len, NULL, NULL);

      PS_set_info(ps, "Author", cusername);
#endif
    }
    //bug: PSLIB does not return the correct boundingbox, it forgets offx and offy. Try to get it
    //back (using pslib own code!)!
        char *bb;
        FILE *feps;
        char buffer[1024]; //largely sufficient
        int nbytes;
        feps=fopen(fileName.c_str(), "r");
        nbytes=fread(buffer,sizeof(char),1023,feps);
        fclose(feps);
        buffer[1023]=0;
	bb = strstr(buffer, "%%BoundingBox:");
        float offx, offy, width, height;
	if(bb) {
            bb += 15;
            sscanf(bb, "%f %f %f %f", &offx, &offy, &width, &height);
	} else {
            offx=0;
            offy=0;
            width=500;
            height=500; //silly values, will be replaced afterwards hopefully.
        }

    // TODO
    //psfont = PS_findfont(ps, "Helvetica", "", 0); 
    //PS_setfont(ps, psfont, 8.0); 

      char bbstr [20], offstr [20];
      int bbXSize, bbYSize;
    {

      int bbXoff = XOffset*CM2IN*DPI;
      int bbYoff = YOffset*CM2IN*DPI;
      bbXSize = orient_portrait ? bbXoff + XPageSize*CM2IN*DPI*scale : bbXoff + YPageSize*CM2IN*DPI*scale;
      bbYSize = orient_portrait ? bbYoff + YPageSize*CM2IN*DPI*scale : bbYoff + XPageSize*CM2IN*DPI*scale;
      sprintf(bbstr,"%i %i %i %i",bbXoff,bbYoff,bbXSize,bbYSize);
      sprintf(offstr,"%i %i",bbXoff,bbYoff);
    
      PS_set_info(ps,"BoundingBox",bbstr);
      PS_begin_page(ps, bbXSize, bbYSize);
      {
        int psimage = PS_open_image_file(ps, "eps", fileName.c_str(), NULL, 0);
        if (psimage == 0)
        {
          Warning("Warning: pslib failed to load plPlot output file.");
          goto cleanup;
        }
	
        float scl = 0.98*min((bbXSize-bbXoff) / (width-offx), (bbYSize-bbYoff) / (height-offy) );
	int margx = ((bbXSize-bbXoff) - scl*(width-offx))/2;
	int margy = ((bbYSize-bbYoff) - scl*(height-offy))/2;
        PS_place_image(ps, psimage, 
		       bbXoff-offx*scl + margx,
		       bbYoff-offy*scl + margy,
		       scl
        );
        PS_close_image(ps, psimage); 
      }
      PS_end_page(ps);
      PS_close(ps);
    }
    // Replace PageBoundingBox and CropBox and write contents to fileName
    // To do that we need to remove a few chars of the line of 21 "%" at the begining of the plplot file,
    // since we ADD characters and that appears to be unsafe (? version dependent? compiler?).
    // if 21 "%" are not found, it's best to DO NOTHING!
    // the "%%%%%%%%%%%%%%%%%%%%%" is largely before offset 12000, thus in the first fread.  
    {
      rewind(fp);
      FILE *fp_plplot = fopen(fileName.c_str(), "w");
      FILEGuard fp_plplotGuard( fp_plplot, fclose);
      if (fp_plplot == NULL)
      {
        Warning("Warning: failed to open plPlot-generated file");
        goto cleanup;
      }

      // When multiple pages are supported, PageBoundingBox and the cropbox
      // will appear more than once. Then this section will need to be redone.

      // Edit: change the two 0's after the PageBoundingBox
      string pbstr=string("%%PageBoundingBox: ")+offstr;
      long added=pbstr.length()-22; //number of chars to replace, compensated by
      //removal of equivalent number of "%" elsewhere. 
      const size_t buflen=12000;
#ifdef _MSC_VER
      char *buff = (char*)alloca(sizeof(char)*buflen);
#else      
      char buff[buflen];
#endif

      //do the first read:
      size_t cnt = fread(&buff, 1, buflen, fp);
      std::string sbuff;
      sbuff.assign(buff,cnt);
      //if "%%%%%%%%%%%%%%%%%%%%%" is not found, or 21 chars too small, do nothing:
      size_t junkbufferloc=sbuff.find("%%%%%%%%%%%%%%%%%%%%%");
      bool doIt=((junkbufferloc != string::npos) && (added < 22) );
      
      // find the PageBoundingBox statement
      size_t pos = sbuff.find("%%PageBoundingBox: 0 0");
      if (doIt && pos != string::npos) { 
        //shrink "%%%..." by the amount of added chars...
        sbuff.erase(junkbufferloc,added);
        //replace, adding some chars:
        sbuff.replace(pos,22,pbstr); 
      }
      pos = sbuff.find("0 setlinecap");
      if (doIt && pos != string::npos) { 
        sbuff.replace(pos,1,"1"); 
      }
      pos = sbuff.find("0 setlinejoin");
      if (doIt && pos != string::npos) { 
        sbuff.replace(pos,1,"1"); 
      }
      // PSlib outputs pdfmarks which resize the PDF to the size of the boundingbox
      // this is nice, but not IDL behaviour (and anyway, the two 0's are wrong)
      char mychar[60];
      sprintf(mychar,"[ /CropBox [0 0 %i.00 %i.00] /PAGE pdfmark",bbXSize,bbYSize);
      string pdfstr=string(mychar); 
      pos = sbuff.find(pdfstr);
      // this replacement will shrink only the size of sbuff and is thus safe
      if (pos != string::npos) sbuff.erase(pos,pdfstr.length());

      // write the first buflen to file
      strcpy(buff,sbuff.c_str());
      if (fwrite(&buff, 1, sbuff.size(), fp_plplot) < sbuff.size()) //and NOT buflen!
        {
          Warning("Warning: failed to overwrite the plPlot-generated file with pslib output");
        }

      // read the rest of fp and write to file
      while (true)
      {
       cnt = fread(&buff, 1, buflen, fp);
         if (!cnt) break;
        if (fwrite(&buff, 1, cnt, fp_plplot) < cnt)
        {
          Warning("Warning: failed to overwrite the plPlot-generated file with pslib output");
        }
      }
//       fclose(fp_plplot);
    }

    cleanup: //all closing done by FileGuard now!
    // PSlib changes locale - bug no. 3428043
#    ifdef HAVE_LOCALE_H
    setlocale(LC_ALL, "C");
#    endif
#  endif
  }

private:
  void epsHacks()
  {
    // using namespace std;
    //PLPLOT outputs a strange boundingbox; this hack directly edits the eps file.  
    //if the plplot bug ever gets fixed, this hack won't be needed.
    char *bb;
    FILE *feps;
    const size_t buflen=2048;//largely sufficient
    char buffer[buflen]; 
    int cnt;
    feps=fopen(fileName.c_str(), "r");
    cnt=fread(buffer,sizeof(char),buflen,feps);

    //read original boundingbox
    bb = strstr(buffer, "%%BoundingBox:");
    if (bb==NULL) {
      Warning("Warning: failed to read temporary PostScript file.");
      fclose(feps);
      return;
    }
    
    string sbuff = string(buffer);
    stringstream searchstr,replstr;
    size_t pos;
    int extralen=0;

// Do not change bonding box. It is good now.
//    int offx, offy, width, height;
//    bb += 15;
//    sscanf(bb, "%i %i %i %i", &offx, &offy, &width, &height);
//    float hsize = XPageSize*CM2IN*DPI*scale;
//    float vsize = YPageSize*CM2IN*DPI*scale;
//    float newwidth = (width - offx), newheight = (height - offy);
//    float hscale = (orient_portrait ? hsize : vsize)/newwidth/5.0;
//    float vscale = (orient_portrait ? vsize : hsize)/newheight/5.0;
////    hscale = min(hscale,vscale)*0.98;
//    hscale = min(hscale,vscale);
//    vscale = hscale;
//    float hoff = -5.*offx*hscale + ((orient_portrait ? hsize : vsize) - 5.0*hscale*newwidth)*0.5;
//    float voff = -5.*offy*vscale + ((orient_portrait ? vsize : hsize) - 5.0*vscale*newheight)*0.5;
//
//    //replace with a more sensible boundingbox
//    searchstr << "BoundingBox: " << offx << " " << offy << " " << width << " " << height;
//    replstr << "BoundingBox: 0 0 " << floor((orient_portrait ? hsize : vsize)+0.5) << " " << floor((orient_portrait ? vsize : hsize)+0.5);
//      pos = sbuff.find(searchstr.str());
//    if (pos != string::npos) {
//      sbuff.replace(pos,searchstr.str().length(),replstr.str()); 
//      extralen = replstr.str().length()-searchstr.str().length();
//    }

    //replace the values of linecap and linejoin to nice round butts (sic!) more pleasing to the eye.
    searchstr.str("");
    searchstr << "0 setlinecap" << endl << "    0 setlinejoin";
    replstr.str("");
    replstr << "1 setlinecap" << endl << "    1 setlinejoin";
    pos = sbuff.find(searchstr.str());
    if (pos != string::npos) {
      sbuff.replace(pos,searchstr.str().length(),replstr.str()); 
      extralen = extralen + replstr.str().length()-searchstr.str().length();
    }
    
    //add landscape
    if (!orient_portrait) {
    searchstr.str("%%Page: 1 1");
    replstr.str("");
    replstr << "%%Page: 1 1" << endl << "%%PageOrientation: Landscape" << endl;
    pos = sbuff.find(searchstr.str());
    if (pos != string::npos) {
      sbuff.replace(pos,searchstr.str().length(),replstr.str()); 
      extralen = extralen + replstr.str().length()-searchstr.str().length();
    }
    }

    //open temp file
    FILE *fp = tmpfile(); // this creates a file which should be deleted automaticaly when it is closed
    FILEGuard fpGuard( fp, fclose);
    if (fp == NULL) { 
      Warning("Warning: failed to create temporary PostScript file.");
      fclose(feps);
      return;
    }

    // write the first buflen to temp file
    fwrite(sbuff.c_str(), 1, buflen+extralen, fp); 
    
    // read the rest of feps and write to temp file
    while (true)
      {
    	cnt = fread(&buffer, 1, buflen, feps);
    	if (!cnt) break;
        if (fwrite(&buffer, 1, cnt, fp) < cnt)
    	  {
    	    Warning("Warning: failed to write to temporary file");
    	  }
      }
    fclose(feps);

    // copy temp file to fileName
    rewind(fp);
    FILE *fp_plplot = fopen(fileName.c_str(), "w");
    FILEGuard fp_plplotGuard( fp_plplot, fclose);
    if (fp_plplot == NULL) {
      Warning("Warning: failed to open plPlot-generated file");
      return;
    }
    while (true)
      {
    	cnt = fread(&buffer, 1, buflen, fp);
    	if (!cnt) break;
        if (fwrite(&buffer, 1, cnt, fp_plplot) < cnt)
    	  {
    	    Warning("Warning: failed to overwrite the plPlot-generated file with pslib output");
    	  }
      }

  }

public:
  DevicePS(): GraphicsDevice(), fileName( "gdl.ps"), actStream( NULL),
    XPageSize(17.78), YPageSize(12.7), XOffset(0.75),YOffset(5.0),
    color(0), decomposed( 0), encapsulated(false), scale(1.), orient_portrait(true), bitsPerPix(8)
  {
    name = "PS";

    DLongGDL origin( dimension( 2));
    DLongGDL zoom( dimension( 2));
    zoom[0] = 1;
    zoom[1] = 1;

    dStruct = new DStructGDL( "!DEVICE");
    dStruct->InitTag("NAME",       DStringGDL( name)); 
    dStruct->InitTag("X_SIZE",     DLongGDL( XPageSize*scale*RESOL)); 
    dStruct->InitTag("Y_SIZE",     DLongGDL( YPageSize*scale*RESOL));
    dStruct->InitTag("X_VSIZE",    DLongGDL( XPageSize*scale*RESOL));
    dStruct->InitTag("Y_VSIZE",    DLongGDL( YPageSize*scale*RESOL));
    dStruct->InitTag("X_CH_SIZE",  DLongGDL( 360));
    dStruct->InitTag("Y_CH_SIZE",  DLongGDL( 360));
    dStruct->InitTag("X_PX_CM",    DFloatGDL( RESOL)); 
    dStruct->InitTag("Y_PX_CM",    DFloatGDL( RESOL)); 
    dStruct->InitTag("N_COLORS",   DLongGDL( 256)); 
    dStruct->InitTag("TABLE_SIZE", DLongGDL( 256)); 
    dStruct->InitTag("FILL_DIST",  DLongGDL( 1));
    dStruct->InitTag("WINDOW",     DLongGDL( -1)); 
    dStruct->InitTag("UNIT",       DLongGDL( 0)); 
    dStruct->InitTag("FLAGS",      DLongGDL( 266791)); //266807 if color, 266791 if monochrome.
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
    // trying to solve bug report 3611898
    // this is needed to decrement Lun number ...
    (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("UNIT"))))[0]=0;
    if (actStream != NULL)
    {
      psUnit->Close();
      psUnit->Free();
      psUnit=NULL;

      delete actStream;
      actStream = NULL;
      if (!encapsulated) pslibHacks(); else epsHacks(); // needs to be called after the plPlot-generated file is closed
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
   (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("X_VSIZE"))))[0]
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
    (*static_cast<DLongGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_VSIZE"))))[0]
      = DLong(floor(0.5+
        ys * (*static_cast<DFloatGDL*>(dStruct->GetTag(dStruct->Desc()->TagIndex("Y_PX_CM"))))[0]
      ));
    return true;
  }

  bool SetColor(const long hascolor)
  {
    if (hascolor==1) color=1; else color=0;
      if (hascolor==1) 
      {
        DLong FLAG=(*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0];
        (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0]=FLAG|16; //set colored device
      } else {
      DLong FLAG=(*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0];
        (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0]=FLAG&(~16); //set monochrome device
      }
      //trick, to be repeated in Decomposed()
      DLong FLAG=(*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("FLAGS"))))[0];
      if (decomposed==1 && color==1) (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0]= FLAG&(~512); //remove flag 'printer' since logic does not work with ps drive
      else (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0]= FLAG|(512); //set Flag printer
    return true;
  }

  bool SetPortrait()
  {
      // no need to update !D
    orient_portrait = true;
//    nb: IDL defaults to:
//    SetXPageSize(7 * in2cm);
//    SetYPageSize(5 * in2cm);
//    SetXOffset(.75 * in2cm);
//    SetYOffset(5 * in2cm); 
    return true;
  }

  bool SetLandscape()
  {
      // no need to update !D
    orient_portrait = false;
//    SetXPageSize(9.5 * in2cm);
//    SetYPageSize(7.0 * in2cm);
//    SetXOffset(.75 * in2cm);
//    SetYOffset(10.25 * in2cm);
    return true;
  }

  bool SetScale(float value)
  {
      //no effect for postscript in IDL up to 8 (?)
    scale = value;
    return true;
  }

  bool SetEncapsulated(bool val)
  {
    encapsulated = val;
    return true;
  }
  
  bool SetBPP(const int val)
  {
    int bpp = max(min(8,val),1);
    if (bpp > 4) bpp = 8;
    else if (bpp > 2) bpp = 4;
    bitsPerPix = bpp;
    return true;
  }
  
  bool Decomposed( bool value)           
  {   
    decomposed = value;
    if (decomposed==1) (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256*256*256;
    else (*static_cast<DLongGDL*>( dStruct->GetTag(dStruct->Desc()->TagIndex("N_COLORS"))))[0]=256;
    DLong FLAG=(*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0];
    //trick, to be repeated in SetColor(). To compensate a problem in ps driver. Other possibilities: use only the psc driver and do the black & white directly ourselves. 
    if (decomposed==1 && color==1) { (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0]= FLAG&(~512); //remove flag 'printer' since logic does not work with ps drive
    } else (*static_cast<DLongGDL*>(SysVar::D()->GetTag(SysVar::D()->Desc()->TagIndex("FLAGS"), 0)))[0]= FLAG|(512); //set Flag printer
    return true;
  }

  DLong GetDecomposed()        
  {
    return decomposed;  
  }

  DIntGDL* GetPageSize()
  {
    DIntGDL* res;
    res = new DIntGDL(2, BaseGDL::NOZERO);
    (*res)[0]= XPageSize;
    (*res)[1]= YPageSize;
    return res;
  }
};

#endif
