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
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
// default: assume we have ImageMagick
#define USE_MAGICK 1
#define USE_MAGICK6 1
#endif


#ifdef USE_MAGICK

#include "includefirst.hpp"

#include <string>
#include <fstream>
#include <memory>
//#include <Magick++.h>

#include "datatypes.hpp"
#include "envt.hpp"
#include "dpro.hpp"
#include "dinterpreter.hpp"
#include "basic_fun_cl.hpp"
#include "terminfo.hpp"
#include "typedefs.hpp"
#include "math_utl.hpp"
#include "magick_cl.hpp"

#define GDL_DEBUG
//#undef GDL_DEBUG

namespace lib {

  using namespace std;
  using namespace antlr;
  using namespace Magick;

  Image gImage[40];
  unsigned int gValid[40];
  unsigned int gCount=0;

  string GDLutos(unsigned int i)
  {
    int mema=3;
    char *n=new char(mema);
    while (snprintf(n, sizeof n, "%u", i) >= sizeof n)
      {			delete n;mema++; n=new char(mema);   }
    string s=n;
    delete n;
    return s;
  }

  
  string GDLitos(int i)
  {
    int mema=3;
    char *n=new char(mema);
    while (snprintf(n, sizeof n, "%d", i) >= sizeof n)
      {			delete n;mema++; n=new char(mema);   }
    string s=n;
    delete n;
    return s;
  }


  void magick_setup()
  {
    int i;
    for (i=0;i<40;++i) gValid[i]=0;
  }

  Image& magick_image(EnvT *e,unsigned int mid)
  {
    if(gValid[mid]==0) e->Throw("invalid ID.");
    
    return gImage[mid];
  }

  unsigned int magick_image(EnvT* e,Image &imImage)
  {
    unsigned int mid=magick_id();
    gImage[mid]=imImage;    
    return mid;
  }
    
  void magick_replace(EnvT* e, unsigned int mid, Image &imImage)
  {
    gImage[mid]=imImage;
  }

  unsigned int magick_id(void)
  {
    unsigned int i,val;
    val=gCount;
    if (gCount ==0) magick_setup();
    for (i=0;i<40;++i) 
      if(gValid[i]==0 && val >i) val=i;

    //    if(gCount==40) ERROR
    if(val >= gCount) gCount++;
    gValid[val]=1;
    return val;
  }


  BaseGDL* magick_open(EnvT* e)
  {
    try{
      DString filename;
      e->AssureScalarPar<DStringGDL>(0,filename);
      
      Image a;
      a.read(filename);
      a.flip();
      unsigned int mid;
      mid=magick_image(e, a);
      return new DUIntGDL (mid);
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }


  BaseGDL * magick_ping(EnvT* e)
  {
    // TODO!
    //if (e->KeywordPresent("SUPPORTED_READ") || e->KeywordPresent("SUPPORTED_WRITE"))
    //  e->Warning("SUPPORTED_READ and SUPPORTED_WRITE keywords not supported yet");

    // TODO: JPEG2000- and TIFF-related additional fields in the INFO structure

    SizeT nParam=e->NParam(1);
 
    try 
    {
      DString filename;
      e->AssureScalarPar<DStringGDL>(0, filename);
      Image a;
      try 
      {
        a.ping(filename);
      }
      catch (WarningCoder &warning_ )
      {
        cerr << warning_.what() << endl;
      }

      if (nParam == 2)
      {
        DString magick;
        e->AssureScalarPar<DStringGDL>(1, magick);
        if (a.magick() != magick) return new DLongGDL(0); 
      }
  
      DInt has_palette, pixel_type;
      DLong channels, num_images, image_index;
      DString type;
     
      // relevant information that, in some cases, is provided after pinging:
      // a.type(), a.matte(), a.classType(), a.colorSpace()
      channels = a.classType() == PseudoClass 
        ? 1      // color palette
        : a.type() == GrayscaleType 
          ? 1    // greyscale
          : a.type() == ColorSeparationType 
            ? 4  // CMYK
            : 3; // RGB
      if (a.matte()) channels += 1;

      // TODO! multiple images (using the Magick++ STL interface)
      image_index = 0; 
      num_images = 1;

      pixel_type = a.depth() == 16 ? 2 : 1;

      has_palette = a.classType() == PseudoClass ? 1 : 0;

      // TODO: 
      // - DCM->DICOM... ?
      // - JP2->JPEG2000 ?
      type = a.magick() == "PNM" ? "PPM" : a.magick();

      static int infoIx = e->KeywordIx("INFO");
      if (e->KeywordPresent(infoIx))
      {
        e->AssureGlobalKW(infoIx);

        // creating the output anonymous structure
        DStructDesc* info_desc = new DStructDesc("$truct");
        SpDString aString;
        SpDLong aLong;
        SpDInt aInt;
        SpDLong aLongArr2(dimension(2));
        info_desc->AddTag("CHANNELS", &aLong);
        info_desc->AddTag("DIMENSIONS", &aLongArr2);
        info_desc->AddTag("HAS_PALETTE", &aInt);
        info_desc->AddTag("IMAGE_INDEX", &aLong);
        info_desc->AddTag("NUM_IMAGES", &aLong);
        info_desc->AddTag("PIXEL_TYPE", &aInt);
        info_desc->AddTag("TYPE", &aString);
        DStructGDL* info = new DStructGDL(info_desc, dimension());

        // filling the info struct with data
        info->InitTag("CHANNELS", DLongGDL(channels));
        {
          DLongGDL dims(dimension( 2));
          dims[0] = a.columns();
          dims[1] = a.rows();
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
      if (e->KeywordPresent(channelsIx))
      {
        e->AssureGlobalKW(channelsIx);
        e->SetKW(channelsIx, new DLongGDL(channels));
      }

      static int dimensionsIx = e->KeywordIx("DIMENSIONS");
      if (e->KeywordPresent(dimensionsIx))
      {
        e->AssureGlobalKW(dimensionsIx);
        DLongGDL *dims = new DLongGDL(dimension(2)); 
        (*dims)[0] = a.columns();
        (*dims)[1] = a.rows();
        e->SetKW(dimensionsIx, dims);
      }

      static int has_paletteIx = e->KeywordIx("HAS_PALETTE");
      if (e->KeywordPresent(has_paletteIx))
      {
        e->AssureGlobalKW(has_paletteIx);
        e->SetKW(has_paletteIx, new DIntGDL(has_palette));
      }

      static int image_indexIx = e->KeywordIx("IMAGE_INDEX");
      if (e->KeywordPresent(image_indexIx))
      {
        e->AssureGlobalKW(image_indexIx);
        e->SetKW(image_indexIx, new DLongGDL(image_index));
      }

      static int num_imagesIx = e->KeywordIx("NUM_IMAGES");
      if (e->KeywordPresent(num_imagesIx))
      {
        e->AssureGlobalKW(num_imagesIx);
        e->SetKW(num_imagesIx, new DLongGDL(num_images));
      }

      static int pixel_typeIx = e->KeywordIx("PIXEL_TYPE");
      if (e->KeywordPresent(pixel_typeIx))
      {
        e->AssureGlobalKW(pixel_typeIx);
        e->SetKW(pixel_typeIx, new DIntGDL(pixel_type));
      }

      static int typeIx = e->KeywordIx("TYPE");
      if (e->KeywordPresent(typeIx))
      {
        e->AssureGlobalKW(typeIx);
        e->SetKW(typeIx, new DStringGDL(type));
      }

      return new DLongGDL(1);
    }
    catch (Exception &error_ )
    {
      return new DLongGDL(0);
    }
  }

  BaseGDL* magick_create(EnvT* e)
  {
    try{
      size_t nParam=e->NParam(2);
      DString col;
      DLong columns,rows;
      Geometry g;
      e->AssureScalarPar<DLongGDL>(0,columns);
      g.width(columns);
      e->AssureScalarPar<DLongGDL>(1,rows);
      g.height(rows);
      if(nParam == 3) 
	{
	  e->AssureScalarPar<DStringGDL>(2,col);
	  Image a(g,Color(col));
	  unsigned int mid;
	  a.matte(false);
	  mid=magick_image(e, a);
	  return new DUIntGDL (mid);
	}
      else
	{
	  Image a(g,Color("black"));
	  unsigned int mid;
	  a.matte(false);
	  mid=magick_image(e, a);
	  return new DUIntGDL (mid);
	}

    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  void magick_close(EnvT *e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);
	
	
	if(mid > gCount-1) e->Throw("Invalid ID");
	else if(gValid[mid] == 0) e->Throw("ID not used");
	
	gValid[mid]=0;
	gImage[mid]=NULL;
	if(gCount-1 == mid) gCount--;      
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }


  BaseGDL* magick_readindexes(EnvT *e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);
	unsigned int columns,rows;
	Image image=magick_image(e,mid);
	if(image.classType()==DirectClass)
	  e->Throw("Not an indexed image: " +e->GetParString(0));

	columns=image.columns();
	rows=image.rows();
	SizeT c[2];
	c[0]=columns;
	c[1]=rows;
	dimension dim(c,2);
	DByteGDL *bImage=new DByteGDL(dim,BaseGDL::NOZERO);

	PixelPacket* pixel;
	IndexPacket* index;
	pixel=image.getPixels(0,0,columns,rows);
	index=image.getIndexes();
	unsigned int cx, cy;
	for (cy=0;cy<rows;++cy)
	  for (cx=0;cx<columns;++cx)
	    (*bImage)[cx+(rows-cy-1)*columns]= index[cx+(cy)*columns];
	
	return bImage;
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }


  void magick_readcolormapRGB(EnvT* e)
  {
    try{
      size_t nParam=e->NParam(1);
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      if(image.classType()==DirectClass)
	e->Throw("Not an indexed image: " +e->GetParString(0));
      

      if(image.classType()==PseudoClass)
	{
	  unsigned int Quant,scale,i;
	  if(QuantumDepth == 16) Quant=65535;
	  if(QuantumDepth == 8) Quant=255;
	  
	  unsigned int cmapsize=image.colorMapSize();
	  dimension cmap(cmapsize,1);
	  Color col;

#ifdef USE_MAGICK6
	  if(image.modulusDepth()<=8)
#else
	    if(image.depth()<=8)
#endif
	    {

	      scale=255;
	      DByteGDL *R,*G,*B;

	      R=new DByteGDL(cmap,BaseGDL::NOZERO);
	      G=new DByteGDL(cmap,BaseGDL::NOZERO);
	      B=new DByteGDL(cmap,BaseGDL::NOZERO);

	      for (i=0;i<cmapsize;++i)
		{
		  col=image.colorMap(i);
		  (*R)[i]=(col.redQuantum())*scale/Quant;
		  (*G)[i]=(col.greenQuantum())*scale/Quant;
		  (*B)[i]=(col.blueQuantum())*scale/Quant;
		}
	      if(nParam > 1) e->SetPar(1,R);
	      if(nParam > 2) e->SetPar(2,G);
	      if(nParam > 3) e->SetPar(3,B);
	    }
#ifdef USE_MAGICK6
	  else if(image.modulusDepth()<=16)
#else	    
	  else if(image.depth()<=16)
#endif
	    {
	      scale=65536;
	      DUIntGDL *R,*G,*B;
	      R=new DUIntGDL(cmap,BaseGDL::NOZERO);
	      G=new DUIntGDL(cmap,BaseGDL::NOZERO);
	      B=new DUIntGDL(cmap,BaseGDL::NOZERO);

	      for (i=0;i<cmapsize;++i)
		{
		  col=image.colorMap(i);
		  (*R)[i]=(col.redQuantum())*scale/Quant;
		  (*G)[i]=(col.greenQuantum())*scale/Quant;
		  (*B)[i]=(col.blueQuantum())*scale/Quant;
		}
	      if(nParam > 1) e->SetPar(1,R);
		if(nParam > 2) e->SetPar(2,G);
		if(nParam > 3) e->SetPar(3,B);
	    }
	    else
	      {
		e->Throw("Uknown Image type, too many colors");
	      }
	  
	}
      else
	{
	  e->Throw("Not an indexed image: " +e->GetParString(0));
	}
  
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  BaseGDL* magick_read(EnvT *e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);
	unsigned int columns,rows,lx,ly,wx,wy;
	Image image=magick_image(e,mid);
	
	columns=image.columns();
	rows=image.rows();
	SizeT c[3];
	if(image.matte()) c[0]=4;
	else c[0]=3;

	
	string map="BGR";
	if(e->GetKW(0) != NULL)//RGB
	  {
	    DInt rgb;
	    e->AssureScalarKW<DIntGDL>(0,rgb);
	    if(rgb==0) map="BGR";
	    else if(rgb==1) map="RGB";
	    else if(rgb==2) map="RBG";
	    else if(rgb==3) map="BRG";
	    else if(rgb==4) map="GRB";
	    else if(rgb==5) map="GBR";
	    else 
	      {
		string s="MAGICK_READ: RGB order type not supported (";
		s+=GDLutos(rgb);
		s+="), using BGR ordering.";
		Message(s);		
		map="BGR";
	      }
	    if(image.matte()) map=map+"A";
	  }

	if(e->KeywordSet(2)) //MAP
	  e->AssureScalarPar<DStringGDL>(0,map);    

	lx=0;ly=0;wx=columns;wy=rows;
	if(e->GetKW(1) != NULL)//SUB_RECT
	  {
	    BaseGDL* sr=e->GetKW(1);
	    DULongGDL * subrect=static_cast<DULongGDL*>(sr->Convert2(ULONG,BaseGDL::COPY));
	    if(subrect->N_Elements() != 4)
	      e->Throw("Not enough elements in SUB_RECT, expected 4.");
	    lx=(*subrect)[0];//guaranteed to be >0
	    ly=(*subrect)[1];
	    wx=(*subrect)[2];
	    wy=(*subrect)[3];

	    if(wx > columns) 
	      e->Throw("Requested width exceeds number of columns, Either reduce the width or the X origin.");

	    if(ly+wy > rows) 
	      e->Throw("Requested height exceeds number of rows. Either reduce the height or the Y origin.");


	    
	  }	

	c[0]=map.length();
	c[1]=wx;
	c[2]=wy;
	dimension dim(c,3);	  

	if(image.depth() == 8)
	  {
	    DByteGDL *bImage=new DByteGDL(dim,BaseGDL::NOZERO);
	    image.write(lx,ly,wx,wy,map, CharPixel,&(*bImage)[0]);
	    return bImage;
	  }
	else if(image.depth() == 16)
	  {
	    DUIntGDL* iImage=new DUIntGDL(dim,BaseGDL::NOZERO);
	    image.write(lx,ly,wx,wy,map, ShortPixel,&(*iImage)[0]);
	    return iImage;
	  }
	else
	  {
	    e->Throw("Unsupported bit depth");
	  }
	

      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }


  void magick_write(EnvT* e)
  {
    try
      {

	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);
	Image image=magick_image(e,mid);
	int columns,rows;
	StorageType ty;
	ty=CharPixel;
	BaseGDL* GDLimage=e->GetParDefined(1);

	string map="BGR";
	if(GDLimage->Rank() == 3)
	  {
	    columns=GDLimage->Dim(1);
	    rows=GDLimage->Dim(2);
	    if(e->GetKW(0) != NULL)//RGB
	      {
		DInt rgb;
		e->AssureScalarKW<DIntGDL>(0,rgb);

		if(rgb==0) map="BGR";
		else if(rgb==1) map="RGB";
		else if(rgb==2) map="RBG";
		else if(rgb==3) map="BRG";
		else if(rgb==4) map="GRB";
		else if(rgb==5) map="GBR";
		else
		{
		  string s="MAGICK_WRITE: RGB order type not supported (";
		  s+=GDLutos(rgb);
		  s+="), using BGR ordering.";
		  Message(s);		
		  map="BGR";
		}
		if(image.matte()) map=map+"A";
	      }
	    /*	    if(image.depth() == 8)
		    {*/

		DByteGDL * bImage=
		  static_cast<DByteGDL*>(GDLimage->Convert2(BYTE,BaseGDL::COPY));
		image.read(columns,rows,map, CharPixel,&(*bImage)[0]);
		/*	      }
	    else if(image.depth() == 16)
	      {
		DUIntGDL * iImage=
		  static_cast<DUIntGDL*>(GDLimage->Convert2(UINT,BaseGDL::COPY));

		image.read(columns,rows,map, ShortPixel,&(*iImage)[0]);
	      }
	    else
	      {
		e->Throw("Unsupported bit depth");
		}*/
	    
	  }
	else 
	  {
	    e->Throw("2D Not yet supported");
	  }
	image.flip();
	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }

  }

  void magick_writefile(EnvT* e)
  {
    try
      {
	size_t nParam=e->NParam(2);
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);
	
	DString filename;
	e->AssureScalarPar<DStringGDL>(1,filename);    
	if(nParam==3)
	  {
	    DString imagetype;
	    e->AssureScalarPar<DStringGDL>(2,imagetype);    
	    image.magick(imagetype);
	  }
	image.write(filename);
	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }
    
  //Attributes
  BaseGDL* magick_colormapsize(EnvT* e)
  {
    try{
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      size_t nParam=e->NParam(1);
      if(nParam == 2) 
	{
	  DUInt ncol;
	  e->AssureScalarPar<DUIntGDL>(1,ncol);    
	  image.colorMapSize(ncol);
	  magick_replace(e,mid,image);
	}

      return new DLongGDL(image.colorMapSize());
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
    }

  BaseGDL* magick_magick(EnvT* e)
  {
    try{
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      size_t nParam=e->NParam(1);
      if(nParam == 2) 
	{
	  DString format;
	  e->AssureScalarPar<DStringGDL>(1,format);    
	  image.magick(format);
	  magick_replace(e,mid,image);
	}

      return new DStringGDL(image.magick());
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
    }


  BaseGDL* magick_rows(EnvT* e)
  {
    try{
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      return new DLongGDL(image.rows());
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
    }

  BaseGDL* magick_columns(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);
	return new DLongGDL(image.columns());
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
    }

    
  BaseGDL* magick_IndexedColor(EnvT* e)
  {
    try{
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      
      if(image.classType()==DirectClass)
	return new DIntGDL(0);
      if(image.classType()==PseudoClass)
	return new DIntGDL(1);
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }

  }

  void magick_quality(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	DUInt q;
	size_t nParam=e->NParam(2);
	if(nParam == 2) e->AssureScalarPar<DUIntGDL>(1,q);
	if(nParam == 2) q=75;
	//check before we do anything.

	Image image=magick_image(e,mid);
	image.quality(q);
	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }


    //manipulations
  void magick_flip(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);
	image.flip();
	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  void magick_matte(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);
	if(e->KeywordSet(1)) image.matte(false);
	else image.matte(true);

	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  void magick_interlace(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);

	
	//NoInterlace.......Don't interlace image (RGBRGBRGBRGBRGBRGB...)
	//LineInterlace.....Use scanline interlacing (RRR...GGG...BBB...RRR...GGG...BBB...)
	//PlaneInterlace....Use plane interlacing (RRRRRR...GGGGGG...BBBBBB...)

	if(e->KeywordSet(0))//NoInterlace
	  image.interlaceType(NoInterlace);
	else if(e->KeywordSet(1))//LineInterlace
	  image.interlaceType(LineInterlace);
	else if(e->KeywordSet(2))//PlaneInterlace
	  image.interlaceType(PlaneInterlace);

	magick_replace(e,mid,image);
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  void magick_addNoise(EnvT* e)
  {
    try
      {
	DUInt mid;
	e->AssureScalarPar<DUIntGDL>(0,mid);    
	Image image=magick_image(e,mid);
	
	if(e->KeywordSet(0))//Uniform noise
	  image.addNoise(UniformNoise);
	else if(e->KeywordSet(1))//Gaussian noise
	  image.addNoise(GaussianNoise);
	else if(e->KeywordSet(2))//Multiplicative Gaussian noise
	  image.addNoise(MultiplicativeGaussianNoise);
	else if(e->KeywordSet(3))//Impulse noise
	  image.addNoise(ImpulseNoise);
	else if(e->KeywordSet(4))//Laplacian noise
	  image.addNoise(LaplacianNoise);
	else if(e->KeywordSet(5))//Poisson noise
	  image.addNoise(PoissonNoise);
	else if(e->GetKW(6) !=NULL)
	  {
	    DInt noise;
	    e->AssureScalarKW<DIntGDL>(6,noise);
	    
	    if(noise==0)//Uniform noise
	      image.addNoise(UniformNoise);
	    else if(noise==1)//Gaussian noise
	      image.addNoise(GaussianNoise);
	    else if(noise==2)//Multiplicative Gaussian noise
	      image.addNoise(MultiplicativeGaussianNoise);
	    else if(noise==3)//Impulse noise
	      image.addNoise(ImpulseNoise);
	    else if(noise==4)//Laplacian noise
	      image.addNoise(LaplacianNoise);
	    else if(noise==5)//Poisson noise
	      image.addNoise(PoissonNoise);
	    else 
	      e->Throw("Unknown noise type requested.");
	  }
	else//no keyword
	  image.addNoise(UniformNoise);
	
	magick_replace(e,mid,image);      
      }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
      }	
    
  void magick_quantize(EnvT* e)
  {
    try{
      size_t nParam=e->NParam(2);
      DUInt mid;
      e->AssureScalarPar<DUIntGDL>(0,mid);    
      Image image=magick_image(e,mid);
      DLong ncol;      
      //set the number of colors;
      if(!e->KeywordSet(0)) 
	{
	  if(nParam>1)
	    e->AssureLongScalarPar(1,ncol);
	  else
	      ncol=256;

	  image.quantizeColors(ncol);
	  if(e->KeywordSet(1))//YUV
	    image.quantizeColorSpace(YUVColorspace);
	  else if(e->KeywordSet(2))//Grayscale
	    image.quantizeColorSpace(GRAYColorspace);
	  else
	    image.quantizeColorSpace(RGBColorspace);
	  if(e->KeywordSet(3)) 
	    image.quantizeDither(true);
	  image.quantize();
	  image.classType(PseudoClass);
	}
      else
	{
	  //truecolor
	  if(nParam>1)
	    e->AssureLongScalarPar(1,ncol);
	  else
	    ncol=256;

	  image.quantizeColors(ncol);
	  image.quantizeColorSpace(RGBColorspace);
	  image.quantizeColors((long)256*(long)256*(long)256-1);
	  if(e->KeywordSet(3)) 
	    image.quantizeDither(true);
	  image.quantize();
	  image.classType(DirectClass);
	  
	}
      
      
      magick_replace(e,mid,image);      
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  //
  void magick_display(EnvT* e)
  {
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0,mid);    
    Image image=magick_image(e,mid);
    image.display();
 
  }

  void magick_writeIndexes(EnvT* e)
  {

    try{
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0,mid);    
    BaseGDL* GDLimage=e->GetParDefined(1);
    DByteGDL * bImage=static_cast<DByteGDL*>(GDLimage->Convert2(BYTE,BaseGDL::COPY));

    Image image=magick_image(e,mid);

    PixelPacket* pixels;
    IndexPacket* index;

    unsigned int columns, rows, cx, cy;
    columns=image.columns();
    rows=image.rows();

    pixels=image.setPixels(0,0,columns,rows);
    index=image.getIndexes();

SizeT nEl = columns*rows;
#pragma omp parallel if (nEl >= CpuTPOOL_MIN_ELTS && (CpuTPOOL_MAX_ELTS == 0 || CpuTPOOL_MAX_ELTS <= nEl))
{
#pragma omp for
    for(cx=0;cx<nEl;++cx)
      {
	    index[cx]=(unsigned int)(*bImage)[cx];
/*	    *index=(unsigned int)(*bImage)[cx];
	    index++;*/
      }
}
    image.syncPixels();

    magick_replace(e,mid,image);      
    }
    catch (Exception &error_ )
      {
        e->Throw(error_.what());
      }
  }

  void magick_writeColorTable(EnvT* e)
  {
    try{
    DUInt mid;
    e->AssureScalarPar<DUIntGDL>(0,mid);    
    Image image=magick_image(e,mid);

    BaseGDL* GDLCol=e->GetParDefined(1);
    DByteGDL * Red=static_cast<DByteGDL*>(GDLCol->Convert2(BYTE,BaseGDL::COPY));
    //e->Guard( Red);
    auto_ptr<BaseGDL> r_guard( Red);
    GDLCol=e->GetParDefined(2);
    DByteGDL * Green=static_cast<DByteGDL*>(GDLCol->Convert2(BYTE,BaseGDL::COPY));
    //e->Guard( Green);
    auto_ptr<BaseGDL> g_guard( Green);
    GDLCol=e->GetParDefined(3);
    DByteGDL *Blue= static_cast<DByteGDL*>(GDLCol->Convert2(BYTE,BaseGDL::COPY));
    //e->Guard( Blue);
    auto_ptr<BaseGDL> b_guard( Blue);

    if(Red->N_Elements() == Green->N_Elements() && 
       Red->N_Elements() == Blue->N_Elements())
      {
	unsigned long n=Red->N_Elements();
	unsigned long c;
	unsigned int Quant,scale;
	if(QuantumDepth == 16) Quant=65535;
	if(QuantumDepth == 8) Quant=255;
	scale=255;
	Color col;
	for(c=0;c<n;++c)
	  {
	    col.redQuantum((*Red)[c]);
	    col.greenQuantum((*Green)[c]);
	    col.blueQuantum((*Blue)[c]);
	  }
      }

    magick_replace(e,mid,image);      
    }
    catch (Exception &error_ )
      {
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
