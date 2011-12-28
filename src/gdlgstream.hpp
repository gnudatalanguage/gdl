/* *************************************************************************
                          gdlgstream.hpp  -  graphic stream
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

#ifndef GDLGSTREAM_HPP_
#define GDLGSTREAM_HPP_

#include <plplot/plstream.h>
#include <plplot/plstrm.h>
#ifndef HAVE_X
#else
#  include <plplot/plxwd.h>
#endif
#include <plplot/plplot.h>
#include <string>
#include <iostream>
#include "typedefs.hpp"
#include "gdlexception.hpp"

using namespace std;

class GDLGStream: public plstream
{
  void init(); // prevent plstream::init from being called directly
	
protected:
  bool valid;

public:
  GDLGStream( int nx, int ny, const char *driver, const char *file="/dev/null")
    : plstream( nx, ny, driver, file), valid( true)
  {
    if (!checkPlplotDriver(driver))
      ThrowGDLException(std::string("PLplot installation lacks the requested driver: ") + driver);
  }

  virtual ~GDLGStream()
  {
// 	plend();
  }

// 	void PlstreamInit()
// 	{
// 	    if( !plstreamInitCalled)
// 		{
// 			this->plstream::init();
// 			plstreamInitCalled = true;
// 		}
// 	}

  static bool checkPlplotDriver(const char *driver)
  {
    int numdevs_plus_one = 64;
#ifdef HAVE_OLDPLPLOT
    char **devlongnames = NULL;
    char **devnames = NULL;
#else
    const char **devlongnames = NULL;
    const char **devnames = NULL;
#endif

    static vector<std::string> devNames;;

    // do only once
    if( devNames.empty())
    {
      // acquireing a list of drivers from plPlot
      for (int maxnumdevs = numdevs_plus_one;; numdevs_plus_one = maxnumdevs += 16)
      {
#ifdef HAVE_OLDPLPLOT
        devlongnames = static_cast<char**>(realloc(devlongnames, maxnumdevs * sizeof(char*)));
        devnames = static_cast<char**>(realloc(devnames, maxnumdevs * sizeof(char*)));
#else
        devlongnames = static_cast<const char**>(realloc(devlongnames, maxnumdevs * sizeof(char*)));
        devnames = static_cast<const char**>(realloc(devnames, maxnumdevs * sizeof(char*)));
#endif
        plgDevs(&devlongnames, &devnames, &numdevs_plus_one);
        numdevs_plus_one++;
        if (numdevs_plus_one < maxnumdevs) break;
        else Message("The above PLPlot warning message, if any, can be ignored");
      } 
      free(devlongnames); // we do not need this information

      for( int i = 0; i < numdevs_plus_one - 1; ++i)
        devNames.push_back(std::string(devnames[ i]));
    
      free(devnames);
    }

    // for debug
    std::vector<std::string> devnamesDbg = devNames;

    return std::find( devNames.begin(), devNames.end(), std::string( driver)) != devNames.end();
  }

  static void SetErrorHandlers();

  virtual void Init()=0;
  virtual void EventHandler() {}
  virtual void GetGeometry( long& xSize, long& ySize, long& xoff, long& yoff);

  virtual void eop()          { plstream::eop();}

  virtual void Raise()         {}
  virtual void Lower()        {}
  virtual void Clear()         {}
  virtual void Clear( DLong bColor)          {}

  bool Valid() { return valid;}

  //  void Clear();
  void Color( ULong c, DLong decomposed=0, UInt ix=1);
  void Background( ULong c, DLong decomposed=0);

  void DefaultCharSize();
  void NextPlot( bool erase=true); // handles multi plots

  void NoSub(); // no subwindows (/NORM, /DEVICE)

  // SA: overloading plplot methods in order to handle IDL-plplot extended
  // text formating syntax conversion
  const char * TranslateFormatCodes(const char *text);
  void mtex( const char *side, PLFLT disp, PLFLT pos, PLFLT just,
                         const char *text);
  void ptex( PLFLT x, PLFLT y, PLFLT dx, PLFLT dy, PLFLT just,
                         const char *text);

};

#endif
