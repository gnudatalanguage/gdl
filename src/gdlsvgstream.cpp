/* *************************************************************************
                          gdlsvgstream.cpp  -  graphic stream SVG
                             -------------------
    begin                : December 26 2008
    copyright            : (C) 2002 by Sylwester Arabas
    email                : slayoo@users.sf.net
 ***************************************************************************/

/* *************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "includefirst.hpp"
#include "gdlsvgstream.hpp"

using namespace std;

void GDLSVGStream::Init()
{
   plstream::init();
}

// this lookup table defines the base64 encoding
static const string svgBase64Table("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
static const char svgfillchar = '='; 

	string encodesvg(unsigned char buf[], unsigned int len) {
		string             ret;
		if(len==0)
			return "";
		ret.reserve((len-1)/3*4 + 4 + 1);
		
		for (string::size_type i = 0; i < len; ++i)
		{
			char c;
			c = (buf[i] >> 2) & 0x3f;
			ret.append(1, svgBase64Table[c]);
			c = (buf[i] << 4) & 0x3f;
			if (++i < len)
				c |= (buf[i] >> 4) & 0x0f;
			
			ret.append(1, svgBase64Table[c]);
			if (i < len)
			{
				c = (buf[i] << 2) & 0x3f;
				if (++i < len)
					c |= (buf[i] >> 6) & 0x03;
				
				ret.append(1, svgBase64Table[c]);
			}
			else
			{
				++i;
				ret.append(1, svgfillchar);
			}
			
			if (i < len)
			{
				c = buf[i] & 0x3f;
				ret.append(1, svgBase64Table[c]);
			}
			else
			{
				ret.append(1, svgfillchar);
			}
		}
		
		return(ret);
	}

#ifdef USE_PNGLIB
 
std::string GDLSVGStream::svg_to_png64(int width,int height,
	    png_byte *image, int bit_depth, int nbpp, int whattype, int *error) 
{
   static std::string tmpstr;
   tmpstr.clear();
   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   static const int np=pls->ncol0;
   int k;
   png_bytep *row_pointers;
   png_colorp  palette;
   char line[512];
   char filename[512];
   char *in,*out;
   int lin, lout;
   int fd;
   *error = 0;
   /* open a temporary file */
   sprintf(filename,"%sgdlsvgpng64.XXXXXX",getenv("IDL_TMPDIR")); //Insecure, check!
   fd=mkstemp(filename);

   if (fd==-1)
   {
     *error=1;
       cerr<<"unable to create temporary file \""<<filename<<"\" for svg image"<<endl;
       return NULL;
   } 
   
   fp = fdopen(fd,"w+");
   if (fp == NULL)
   {
     *error=1;
       cerr<<"unable to open temporary file \""<<filename<<"\" for svg image"<<endl;
       return NULL;
   } 
   
   /* Create and initialize the png_struct with the desired error handler
    * functions.  If you want to use the default stderr and longjump method,
    * you can supply NULL for the last three parameters.  We also check that
    * the library version is compatible with the one used at compile time,
    * in case we are using dynamically linked libraries.  REQUIRED.
    */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL); 

   if (png_ptr == NULL)
   {
       fclose(fp);
       unlink(filename);
     *error=1;
       return NULL;
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
       fclose(fp);
       unlink(filename);
       png_destroy_write_struct(&png_ptr,  (png_infopp)NULL);
     *error=1;
       return NULL;
   }

   /* Set error handling.  REQUIRED if you aren't supplying your own
    * error hadnling functions in the png_create_write_struct() call.
    */
   if (setjmp(png_jmpbuf(png_ptr)))
   {
       /* If we get here, we had a problem reading the file */
       fclose(fp);
       unlink(filename);
       png_destroy_write_struct(&png_ptr, &info_ptr);
     *error=1;
       return NULL;
   }


   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);

   /* Set the image information here.  Width and height are up to 2^31,
    * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
    * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
    * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
    * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
    * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
    * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
    */
   png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, whattype,
//either one of:		PNG_COLOR_TYPE_PALETTE,
//                      PNG_COLOR_TYPE_GRAY,
//                      PNG_COLOR_TYPE_RGB,
   PNG_INTERLACE_NONE, 
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE);

   /* set the palette if there is one.  REQUIRED for indexed-color images */
   bool haspalette=(whattype == PNG_COLOR_TYPE_PALETTE);
   if (haspalette) {
     palette = (png_colorp)malloc(np*sizeof(png_color));
   /* ... set palette colors ... */
   for (k=0;k<np;k++) {
     palette[k].red=pls->cmap0[k].r;
     palette[k].green=pls->cmap0[k].g;
     palette[k].blue=pls->cmap0[k].b;
     }
     png_set_PLTE(png_ptr, info_ptr, palette, np);
   }

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);

  /* The easiest way to write the image (you may have a different memory
    * layout, however, so choose what fits your needs best).  You need to
    * use the first method if you aren't handling interlacing yourself.
    */
   row_pointers=(png_bytepp)malloc(height*sizeof(png_bytep));
   for (k = 0; k < height; ++k) {
     row_pointers[k] = (png_bytep)(image + k*width*nbpp*sizeof(png_byte));
   }
   /* Write it */
   png_write_image(png_ptr, row_pointers);

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
   fflush(fp); /* just in case we core-dump before finishing... */

   /* if you malloced the palette, free it here */
   if (haspalette){
     free(palette);
   }
   free(row_pointers);  

   /* clean up after the write, and free any memory allocated */
   png_destroy_write_struct(&png_ptr, &info_ptr);
   /* rewind */
   rewind(fp);
   /* count size. Note that string functions do not work since NULL is valid */
   lout=0;
   while ((lin=(fread((void *)line, (size_t)1, (size_t)512, fp))) != 0) {
       lout+=lin; 
   }
   /* compute in and out length */
   /* allocate in */
   lin=lout+1;
   in=(char*)calloc(lin,sizeof(char));
   /* allocate out */
   lout=((lout + 2) / 3 * 4) + 1;
   out=(char*)calloc(lout,sizeof(char));
   /* bufread, convert, back to char */
   /* rewind */
   rewind(fp);
   lin=fread((void *)in, (size_t)1, (size_t)lin, fp);
   tmpstr = encodesvg((unsigned char*)in, lin);
   free(in);
   /* close and destroy fp, return */
   fclose(fp);
   unlink(filename);
   return tmpstr;
}
bool  GDLSVGStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel) {
  c_plflush();
  if (channel > 0) {
    cerr << "TV+SVG device: Value of CHANNEL (use TRUE instead) is out of allowed range. (FIXME!)" << endl;
    return false;
  }
  if (trueColorOrder > 1) {
    cerr << "TV+SVG device: True Color images must be [3,*,*] only. (FIXME!)" << endl;
    return false;
  }
  fprintf(pls->OutFile,"<image preserveAspectRatio=\"none\" x=\"%d\" y=\"%d\" width=\"%d\" height=\"%d\" xlink:href=\"data:image/svg;base64,",
          pos[0],pos[2],pos[1],pos[3]);
  int error;
  std::string ret ;      
  // do we need rotation handling? 
//  if ( pls->diorot == 1.0 ) {
//  } else {
//  }
  if ( channel == 0 ) {
    if ( trueColorOrder == 0 ) { //indexed value 0->255: image
      ret = GDLSVGStream::svg_to_png64( nx, ny, idata, 8, 1 ,PNG_COLOR_TYPE_PALETTE, &error );
      if ( error == 0 ) fprintf( pls->OutFile, ret.c_str( ) );
     } else {
      switch ( trueColorOrder ) {
        case 1:
          ret = GDLSVGStream::svg_to_png64( nx, ny, idata, 8, 3 ,PNG_COLOR_TYPE_RGB, &error );
          if ( error == 0 ) fprintf( pls->OutFile, ret.c_str( ) );
          break;
        case 2:
          break;
        case 3:
          break;
      }
    }
//  } else { //channel = 1 to 3
  }
  fprintf(pls->OutFile,"\"/>\n");
  return true;
#undef BUFLEN
}
#else
bool  GDLSVGStream::PaintImage(unsigned char *idata, PLINT nx, PLINT ny, DLong *pos,
		   DLong trueColorOrder, DLong channel) {return false;}
#endif
