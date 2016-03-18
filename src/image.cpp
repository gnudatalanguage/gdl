/***************************************************************************
                          image.cpp  -  GDL image routines
                             -------------------
    begin                : Jul 20 2004
    copyright            : (C) 2004 by Joel Gales
    email                : jomoga@users.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// PLplot is used for direct graphics

#include "includefirst.hpp"

#include "graphicsdevice.hpp"
#include "image.hpp"
#ifdef HAVE_LIBWXWIDGETS
#include "gdlwxstream.hpp"
#endif

using namespace std;

namespace lib {


  //  BaseGDL* tvrd( EnvT* e)
  //  {
  //      // when !d.name == Null  we do nothing !
  //      GDLGStream* actStream = GraphicsDevice::GetDevice()->GetStream();
  //      if (actStream == NULL) e->Throw("Unable to create window.");
  //
  //#ifdef HAVE_LIBWXWIDGETS
  //      if (actStream->HasImage()) 
  //        return GetImage(e);
  //      else
  //#endif
  //        return GraphicsDevice::GetDevice()->TVRD( e);
  //  }

  BaseGDL* tvrd( EnvT* e){
    GDLGStream* actStream = GraphicsDevice::GetDevice()->GetStream();
    if (actStream == NULL) e->Throw("Unable to create window.");
 
    if (e->KeywordSet("WORDS")) e->Throw( "WORDS keyword not yet supported.");
    DLong orderVal=SysVar::TV_ORDER();
    e->AssureLongScalarKWIfPresent( "ORDER", orderVal);

    DLong tru=0;
    e->AssureLongScalarKWIfPresent( "TRUE", tru);
    if (tru > 3 || tru < 0) e->Throw("Value of TRUE keyword is out of allowed range.");
    //GetBitMapData is device-dependent and insures that image is by default ORDER=0 now.
    DByteGDL *bitmap = static_cast<DByteGDL*>(actStream->GetBitmapData());
    if (bitmap==NULL)  e->Throw("Unable to read from current device: "+GraphicsDevice::GetDevice()->Name()+"."); //need to GDLDelete bitmap on exit after this line.

    long nx=bitmap->Dim(0);
    long ny=bitmap->Dim(1);
    long x_gdl=0;
    long y_gdl=0;
    long nx_gdl=nx;
    long ny_gdl=ny;

    bool error=false;
    bool hasXsize=false;
    bool hasYsize=false;
    int nParam = e->NParam();
    if (nParam >= 4) {
      DLongGDL* Ny = e->GetParAs<DLongGDL>(3);
      ny_gdl=(*Ny)[0];
      hasYsize=true;
    }
    if (nParam >= 3) {
      DLongGDL* Nx = e->GetParAs<DLongGDL>(2);
      nx_gdl=(*Nx)[0];
      hasXsize=true;
    }
    if (nParam >= 2) {
      DLongGDL* y0 = e->GetParAs<DLongGDL>(1);
      y_gdl=(*y0)[0];
    }
    if (nParam >= 1) {
      DLongGDL* x0 = e->GetParAs<DLongGDL>(0);
      x_gdl=(*x0)[0];
    }
    DLong channel=-1;
    if (nParam == 5) {
      DLongGDL* ChannelGdl = e->GetParAs<DLongGDL>(4);
      channel=(*ChannelGdl)[0]; 
    }
    e->AssureLongScalarKWIfPresent( "CHANNEL", channel);
    if (channel > 3) {GDLDelete(bitmap); e->Throw("Value of Channel is out of allowed range.");}

    if (!(hasXsize))nx_gdl-=x_gdl; 
    if (!(hasYsize))ny_gdl-=y_gdl;
    
    DLong xref,xval,xinc,yref,yval,yinc,xmax11,ymin11;
    int x_11=0;
    int y_11=0;
    xref=0;xval=0;xinc=1;
    yref=0;yval=0;yinc=1;
    
    x_11=xval+(x_gdl-xref)*xinc;
    y_11=yval+(y_gdl-yref)*yinc;
    xmax11=xval+(x_gdl+nx_gdl-1-xref)*xinc;    
    ymin11=yval+(y_gdl+ny_gdl-1-yref)*yinc;
    if (y_11 < 0 || y_11 > ny-1) error=true;
    if (x_11 < 0 || x_11 > nx-1) error=true;
    if (xmax11 < 0 || xmax11 > nx-1) error=true;
    if (ymin11 < 0 || ymin11 > ny-1) error=true;
    if (error)  {GDLDelete(bitmap); e->Throw("Value of Area is out of allowed range.");}

    SizeT dims[3];
    DByteGDL* res;

    if ( tru == 0 ) {
      dims[0] = nx_gdl;
      dims[1] = ny_gdl;
      dimension dim( dims, (SizeT) 2 );
      res = new DByteGDL( dim, BaseGDL::ZERO );
      //set again dimension since tvrd does not want the 1-sized dimensions purged! (like in a=tvrd(0,0,1,1) a=Array[1,1]
      static_cast<BaseGDL*>(res)->SetDim(dim);
      if ( channel <= 0 ) { //channel not given, return max of the 3 channels
	DByte mx, mx1;
	for ( SizeT i =0; i < nx_gdl ; ++i ) {
	  for ( SizeT j = 0; j < ny_gdl ; ++j ) {
	    mx = (*bitmap)[3 * ((j+y_11) * nx + (i+x_11)) + 0]; 
	    mx1 = (*bitmap)[3 * ((j+y_11) * nx + (i+x_11)) + 1];
	    if ( mx1 > mx ) mx = mx1;
	    mx1 = (*bitmap)[3 * ((j+y_11) * nx + (i+x_11)) + 2];
	    if ( mx1 > mx ) mx = mx1;
	    (*res)[j * nx_gdl + i] = mx;         
	  }
	}
      } else {
	for ( SizeT i =0; i < nx_gdl ; ++i ) {
	  for ( SizeT j = 0; j < ny_gdl ; ++j ) {
	    (*res)[j * nx_gdl + i] = (*bitmap)[3 * ((j+y_11) * nx + (i+x_11)) + channel]; 
	  }
	}
      }
      GDLDelete(bitmap);
      // Reflect about y-axis
      if ( orderVal == 1 ) res->Reverse( 1 );
      return res;

    } else {
      dims[0] = 3;
      dims[1] = nx_gdl;
      dims[2] = ny_gdl;
      dimension dim( dims, (SizeT) 3 );
      res = new DByteGDL( dim, BaseGDL::NOZERO );
      //set again dimension since tvrd does not want the 1-sized dimensions purged! (like in a=tvrd(0,0,1,1,/tru) a=Array[3,1,1]
      static_cast<BaseGDL*>(res)->SetDim(dim);
      for ( SizeT i =0; i < nx_gdl ; ++i ) {
	for ( SizeT j = 0; j < ny_gdl ; ++j ) {
	  for ( SizeT k = 0 ; k < 3 ; ++k) (*res)[3 * (j * nx_gdl + i) + k] = (*bitmap)[3 * ((j+y_11) * nx + (i+x_11)) + k]; 
	}
      }
      GDLDelete(bitmap);
      // Reflect about y-axis
      if ( orderVal == 1 ) res->Reverse( 2 );

      if ( tru == 1 ) {
	return res;
      } else if ( tru == 2 ) {
	DUInt* perm = new DUInt[3];
	perm[0] = 1;
	perm[1] = 0;
	perm[2] = 2;
	return res->Transpose( perm );
      } else if ( tru == 3 ) {
	DUInt* perm = new DUInt[3];
	perm[0] = 1;
	perm[1] = 2;
	perm[2] = 0;
	return res->Transpose( perm );
      }
    }
    assert( false );
    return NULL;
  }

  void loadct( EnvT* e) // = LOADCT_INTERNALGDL for exclusive use by LOADCT
  {
    SizeT nCT = GraphicsDevice::N_CT();

    static int get_namesIx = e->KeywordIx( "GET_NAMES"); 
    bool get_names = e->KeywordPresent( get_namesIx);
    if( get_names)
      {
	e->AssureGlobalKW( get_namesIx);

	DStringGDL* names = new DStringGDL( nCT, BaseGDL::NOZERO);
	for( SizeT i=0; i<nCT; ++i)
	  (*names)[ i] = GraphicsDevice::GetCT( i)->Name();

	e->SetKW( get_namesIx, names);
	return; //correct behaviour.
      }

    if( e->NParam() == 0) return; //FIXME should list tables names, promt for number and load it!

    DLong iCT;

    DByte r[ctSize], g[ctSize], b[ctSize];
    PLINT rint[ctSize], gint[ctSize], bint[ctSize];
    //load original table
    GDLCT* actCT = GraphicsDevice::GetCT();
    actCT->Get(rint,gint,bint,ctSize);


    e->AssureLongScalarPar( 0, iCT);
    if( iCT < 0 || iCT >= nCT)
      e->Throw( "Table number must be from 0 to "+i2s(nCT-1));
    
    GraphicsDevice* actDevice = GraphicsDevice::GetDevice();
    GDLGStream* actStream = actDevice->GetStream( false); // no open

    GraphicsDevice::LoadCT( iCT);
    //new table is:
    actCT = GraphicsDevice::GetCT();
    //actually update colors in col0 if needed:
    DLong bottom=0;
    DLong ncolors=ctSize;
    if ( e->KeywordSet ( "BOTTOM" ) ) e->AssureLongScalarKWIfPresent ( "BOTTOM", bottom );
    if ( e->KeywordSet ( "NCOLORS" ) ) e->AssureLongScalarKWIfPresent ( "NCOLORS", ncolors );
    if (bottom < 0) bottom=0;
    if (bottom > ctSize-1) bottom=ctSize-1;
    if (ncolors < 1) ncolors=1;
    if (ncolors > ctSize) ncolors=ctSize;
    if (bottom+ncolors > ctSize) ncolors=ctSize-bottom;
    for( SizeT i=0, j=bottom ; j<bottom+ncolors; ++i, ++j) {
      actCT->Get( i, r[ i], g[ i], b[ i]);
      //update section of colors
      rint[j] = (PLINT) r[i];
      gint[j] = (PLINT) g[i];
      bint[j] = (PLINT) b[i];
    }

    static int rgbtableIx = e->KeywordIx( "RGB_TABLE");
    if( e->KeywordPresent( rgbtableIx) )
      {
	e->AssureGlobalKW( rgbtableIx);
	DByteGDL* rgbtable = new DByteGDL( dimension(ncolors, 3), BaseGDL::NOZERO);
	for( SizeT i=0, j=bottom ; j<bottom+ncolors; ++i, ++j)
	  {
	    (*rgbtable)[i] = rint[j];
	    (*rgbtable)[i+ncolors] = gint[j];
	    (*rgbtable)[i+2*ncolors] = bint[j];
	  }
	e->SetKW( rgbtableIx, rgbtable);
	return; //correct behaviour.
    }
    // GD:
    // new colormap must be given to *all* streams since plplot has a colormap PER STREAM.
    // (IDL has a colormap PER Device, i.e., all Windows of multiWindow devices.)
    // However plplot using the crazy old rw color scheme, uses heavily XAllocColor which is a killer for X11 speed.
    // SetColorMap0() is thus overloaded for X11 devices to *do nothing* (devicex do tricks behind plplot's back most of the time)
    int nbActiveStreams = actDevice->MaxWin();
    for (int i = 0; i < nbActiveStreams; ++i) {
      actStream = actDevice->GetStreamAt(i);
      if (actStream != NULL) actStream->SetColorMap0(rint, gint, bint, ctSize);
    }
  }
} // namespace
     
