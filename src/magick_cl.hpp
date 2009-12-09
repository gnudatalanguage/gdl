/***************************************************************************
                          image_fun_cl.hpp  -  basic GDL library functions
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

#ifndef MAGICK_HPP_CL
#define MAGICK_HPP_CL

#include <Magick++.h>
namespace lib {

  using namespace Magick;
  using namespace std;
  using namespace antlr;
  //internal

  string GDLitos(int i);
  string GDLutos(unsigned int i);
  void magick_setup(void);
  unsigned int magick_id(void);
  Image& magick_image(EnvT * e,unsigned int mid);
  unsigned int magick_image(EnvT* e,Image &imImage);
  void magick_replace(EnvT* e, unsigned int mid, Image &imImage);
  //interface
  BaseGDL* magick_open(EnvT *e);
  BaseGDL* magick_create(EnvT *e);
  void magick_close(EnvT *e);

  BaseGDL* magick_read(EnvT *e);
  BaseGDL* magick_readindexes(EnvT *e);
  void magick_readcolormapRGB(EnvT *e);
  void magick_write(EnvT *e);
  void magick_writefile(EnvT *e);

  //Attributes
  BaseGDL * magick_IndexedColor(EnvT* e);
  BaseGDL * magick_rows(EnvT* e);
  BaseGDL * magick_columns(EnvT* e);
  BaseGDL * magick_colormapsize(EnvT* e);
  BaseGDL * magick_magick(EnvT* e);

  //manipulations
  void magick_flip(EnvT* e);
  void magick_matte(EnvT* e);
  void magick_interlace(EnvT* e);
  void magick_addNoise(EnvT* e);
  void magick_quantize(EnvT* e);
  void magick_writeIndexes(EnvT* e);
  void magick_writeColorTable(EnvT* e);
  void magick_quality(EnvT* e);

  //hmm
  void magick_display(EnvT* e);

  // SA: query/ping routines support
  BaseGDL * magick_ping(EnvT* e);
}
#endif
