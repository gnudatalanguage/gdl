/***************************************************************************
                             widget.cpp  -  GDL WIDGET_XXX library functions
                                            the actual library routines
                                            (widget system in gdlwidget.cpp)
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002 by Marc Schellens
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

#include <iostream>
// not yet ready? version problems? #include <wx/display.h>

#include "datatypes.hpp"
#include "envt.hpp"
#include "dinterpreter.hpp"

#ifdef HAVE_LIBWXWIDGETS
#include "gdlwidget.hpp"
#include "graphicsdevice.hpp"
#endif

#ifdef HAVE_LIBWXWIDGETS

wxBitmap * GetBitmapFromPassedBytes(EnvT* e, DByteGDL* passedBytes, bool doMask=false) {
  wxBitmap * bitmap=NULL; 
  if (passedBytes->Rank() == 0) return NULL; //protect against call of this function with undefined bitmaps.
  if (passedBytes->Rank() < 2 || passedBytes->Rank() > 3) e->Throw("Array must be a [X,Y] or [X,Y,3] array.");
  if (passedBytes->Rank() == 3 && ((passedBytes->Dim(2) < 3) || (passedBytes->Dim(2) > 4))) e->Throw("Array must be a [X,Y] or [X,Y,3] or [X,Y,4] array.");
  if (passedBytes->Rank() == 2) {
    bitmap = new wxBitmap(static_cast<char*> (passedBytes->DataAddr()), passedBytes->Dim(0)*8, passedBytes->Dim(1), 1);
  } else {
    DUInt perm[3] = {2, 0, 1};
    if (passedBytes->Dim(2) == 3) {
      BaseGDL* tempcopy = passedBytes->Transpose(perm);
      tempcopy->Reverse(2); //necessary 
      wxImage tryImage = wxImage(tempcopy->Dim(1), tempcopy->Dim(2), static_cast<unsigned char*> (tempcopy->DataAddr()), TRUE); //STATIC DATA I BELIEVE.
      bitmap = new wxBitmap(tryImage);
      GDLDelete(tempcopy); //deletion AFTER bitmap has been created!
    } else {
      //separate Alpha channel. Awkward.
      SizeT byteSize = 3 * passedBytes->Dim(0) * passedBytes->Dim(1);
      DByteGDL* theBytes = new DByteGDL(dimension(passedBytes->Dim(0), passedBytes->Dim(1), 3), BaseGDL::NOZERO);
      Guard<DByteGDL> g(theBytes);
      theBytes->Assign(passedBytes, byteSize);

      BaseGDL* tempcopy = (theBytes->Transpose(perm));
      tempcopy->Reverse(2); //necessary 
      //need to reverse the mask bytes also
      DByteGDL* mask = new DByteGDL(dimension(passedBytes->Dim(0), passedBytes->Dim(1)), BaseGDL::NOZERO);
      Guard<DByteGDL> m(mask);
      for (SizeT i=0; i<passedBytes->Dim(0)*passedBytes->Dim(1); ++i) (*mask)[i]=static_cast<unsigned char*>(static_cast<unsigned char*>(passedBytes->DataAddr())+byteSize)[i] ;
      mask->Reverse(1);
      
      wxImage tryImage = wxImage(tempcopy->Dim(1), tempcopy->Dim(2), static_cast<unsigned char*> (tempcopy->DataAddr()),TRUE);
      tryImage.SetAlpha(static_cast<unsigned char*> (mask->DataAddr()), TRUE); //the Alpha channel is available
      bitmap = new wxBitmap(tryImage);
      GDLDelete(tempcopy); //deletion AFTER bitmap has been created!
      doMask=false; //since it exists already
    }
  }
  if (doMask) {
    wxImage tryImage = bitmap->ConvertToImage();
    unsigned char r = tryImage.GetRed(0, 0);
    unsigned char g = tryImage.GetGreen(0, 0);
    unsigned char b = tryImage.GetBlue(0, 0);
    wxMask* m = new wxMask(*bitmap, wxColour(r, g, b));
    bitmap->SetMask(m);
  }
  return bitmap;
}


wxRealPoint GetRequestedUnitConversionFactor( EnvT* e){
  int the_units = 0;
  static int unitsIx = e->KeywordIx( "UNITS" );
  e->AssureLongScalarKWIfPresent( unitsIx, the_units );
  //convert unit to the factor in pixels
  DDouble sx=wxGetDisplaySizeMM().x;
  DDouble sy=wxGetDisplaySizeMM().y;
  sx=wxGetDisplaySize().x/sx; //pix per mm
  sy=wxGetDisplaySize().y/sy;

  if (the_units==0) return wxRealPoint(1,1);
  if (the_units==1) return wxRealPoint(sx*25.4,sy*25.4);
  if (the_units==2) return wxRealPoint(sx*10.0,sy*10.0);
  return wxRealPoint(0,0); //never reached -- pacifier.
}

void GDLWidget::ChangeUnitConversionFactor( EnvT* e)
{
  int the_units = 0;
  static int unitsIx = e->KeywordIx( "UNITS" );
  e->AssureLongScalarKWIfPresent( unitsIx, the_units );
  //convert unit to the factor in pixels
  DDouble sx=wxGetDisplaySizeMM().x;
  DDouble sy=wxGetDisplaySizeMM().y;
  sx=wxGetDisplaySize().x/sx; //pix per mm
  sy=wxGetDisplaySize().y/sy;

  if (the_units==0) unitConversionFactor=wxRealPoint(1,1);
  if (the_units==1) unitConversionFactor=wxRealPoint(sx*25.4,sy*25.4);
  if (the_units==2) unitConversionFactor=wxRealPoint(sx*10.0,sy*10.0);
}
void GDLWidget::DefaultValuesInAbsenceofEnv() //needed to create widgets directly from inside gdl.
{
}
void GDLWidget::GetCommonKeywords( EnvT* e)
{
  static int frameIx = e->KeywordIx( "FRAME" );
  static int event_funcIx = e->KeywordIx( "EVENT_FUNC" );
  static int event_proIx = e->KeywordIx( "EVENT_PRO" );
  static int func_get_valueIx = e->KeywordIx( "FUNC_GET_VALUE" );
  static int pro_set_valueIx = e->KeywordIx( "PRO_SET_VALUE" );
  static int notify_realizeIx = e->KeywordIx( "NOTIFY_REALIZE" );
  static int kill_notifyIx = e->KeywordIx( "KILL_NOTIFY" );
  static int group_leaderIx = e->KeywordIx( "GROUP_LEADER" );
  static int no_copyIx = e->KeywordIx( "NO_COPY" );
  static int scr_xsizeIx = e->KeywordIx( "SCR_XSIZE" );
  static int scr_ysizeIx = e->KeywordIx( "SCR_YSIZE" );
  static int scrollIx = e->KeywordIx( "SCROLL" );
  static int sensitiveIx = e->KeywordIx( "SENSITIVE" );
  static int unameIx = e->KeywordIx( "UNAME" );
  static int uvalueIx = e->KeywordIx( "UVALUE" );
  static int xoffsetIx = e->KeywordIx( "XOFFSET" );
  static int xsizeIx = e->KeywordIx( "XSIZE" );
  static int yoffsetIx = e->KeywordIx( "YOFFSET" );
  static int ysizeIx = e->KeywordIx( "YSIZE" );
  static int ALIGN_CENTER = e->KeywordIx( "ALIGN_CENTER" );
  static int ALIGN_LEFT = e->KeywordIx( "ALIGN_LEFT" );
  static int ALIGN_RIGHT = e->KeywordIx( "ALIGN_RIGHT" );
  static int ALIGN_TOP = e->KeywordIx( "ALIGN_TOP" );
  static int ALIGN_BOTTOM = e->KeywordIx( "ALIGN_BOTTOM" );
  static int FONT = e->KeywordIx( "FONT" );
//  static int RESOURCE_NAME = e->KeywordIx( "RESOURCE_NAME" ); // std::string

  std::string inputfont="";
  e->AssureStringScalarKWIfPresent( FONT, inputfont );
  if (inputfont.length() > 0) {
     font=GDLWidget::defaultFont;
//      std::cerr <<"DEFAULT BEFORE: "<< font.GetNativeFontInfoUserDesc() << std::endl;
      bool ok = font.SetNativeFontInfoUserDesc(wxString(inputfont.c_str(), wxConvLibc));
//      std::cerr <<"DEFAULT AFTER: "<< font.GetNativeFontInfoUserDesc() << std::endl;
      if (ok) { //it seems to be always OK with wxWidgets, that gives back its defaultFont in bad cases.Thus: behaviuor not as IDL.
//        std::cerr <<"FINAL DESC: "<< font.GetNativeFontInfoDesc() << std::endl;
      } else font=GDLWidget::systemFont;  //defining a bad font goes back to the system font.
  }
  alignment=gdlwALIGN_NOT;
  if (e->KeywordSet(ALIGN_LEFT)) alignment|=gdlwALIGN_LEFT;
  if (e->KeywordSet(ALIGN_CENTER)) alignment|=gdlwALIGN_CENTER;
  if (e->KeywordSet(ALIGN_RIGHT)) alignment|=gdlwALIGN_RIGHT;
  if (e->KeywordSet(ALIGN_TOP)) alignment|=gdlwALIGN_TOP;
  if (e->KeywordSet(ALIGN_BOTTOM)) alignment|=gdlwALIGN_BOTTOM;
  
  scrolled = e->KeywordSet( scrollIx );
//  if (scrolled && this->IsLabel()) e->Throw("Keyword SCROLL not allowed in call to: WIDGET_LABEL"); //does not work: next: remove SCROLL from CommonKW.

  sensitive=e->BooleanKeywordAbsentOrSet( sensitiveIx); //defaults to true if absent

  groupLeader = 0;
  if (e->KeywordPresent( group_leaderIx )){
    e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader );
    if (groupLeader != 0) {
      GDLWidget *widget = GDLWidget::GetWidget( groupLeader );
      if ( widget == NULL ) {
        e->Throw( "Invalid widget identifier:" + i2s( groupLeader ) );
      }
    }
  }

  frameWidth = 0;
  e->AssureLongScalarKWIfPresent( frameIx, frameWidth );
  int the_units = 0;
  static int unitsIx = e->KeywordIx( "UNITS" );
  e->AssureLongScalarKWIfPresent( unitsIx, the_units );
  //convert unit to the factor in pixels
  DDouble sx=wxGetDisplaySizeMM().x;
  DDouble sy=wxGetDisplaySizeMM().y;
  sx=wxGetDisplaySize().x/sx; //pix per mm
  sy=wxGetDisplaySize().y/sy;
    
  if (the_units==0) unitConversionFactor=wxRealPoint(1,1);
  if (the_units==1) unitConversionFactor=wxRealPoint(sx*25.4,sy*25.4);
  if (the_units==2) unitConversionFactor=wxRealPoint(sx*10.0,sy*10.0);

  e->AssureLongScalarKWIfPresent( scr_xsizeIx, wScreenSize.x ); if (wScreenSize.x<=0) wScreenSize.x=wxDefaultSize.x;
  e->AssureLongScalarKWIfPresent( xsizeIx, wSize.x );           if (wSize.x<=0) wSize.x=wxDefaultSize.x;
  e->AssureLongScalarKWIfPresent( scr_ysizeIx, wScreenSize.y ); if (wScreenSize.y<=0) wScreenSize.y=wxDefaultSize.y;
  e->AssureLongScalarKWIfPresent( ysizeIx, wSize.y );           if (wSize.y<=0) wSize.y=wxDefaultSize.y;
  e->AssureLongScalarKWIfPresent( xoffsetIx, wOffset.x );       if (wOffset.x<=0) wOffset.x=wxDefaultPosition.x;
  e->AssureLongScalarKWIfPresent( yoffsetIx, wOffset.y );       if (wOffset.y<=0) wOffset.y=wxDefaultPosition.y;

  uValue = e->GetKW( uvalueIx ); //IDL: a !NULL uvalue is ignored.
  if ( uValue != NULL ) {
    bool no_copy = e->KeywordSet( no_copyIx );
    if ( no_copy )
      e->GetTheKW( uvalueIx ) = NULL;
    else
      uValue = uValue->Dup( );
  }
  e->AssureStringScalarKWIfPresent( event_funcIx, eventFun );
  StrUpCaseInplace( eventFun );
  e->AssureStringScalarKWIfPresent( event_proIx, eventPro );
  StrUpCaseInplace( eventPro );
  e->AssureStringScalarKWIfPresent( kill_notifyIx, killNotify );
  StrUpCaseInplace( killNotify );
  e->AssureStringScalarKWIfPresent( notify_realizeIx, notifyRealize );
  StrUpCaseInplace( notifyRealize );
  e->AssureStringScalarKWIfPresent( pro_set_valueIx, proValue );
  StrUpCaseInplace( proValue );
  e->AssureStringScalarKWIfPresent( func_get_valueIx, funcValue );
  StrUpCaseInplace( funcValue );
  e->AssureStringScalarKWIfPresent( unameIx, uName );
  // no case change
}
#endif


// non library functions
// these reside here because gdlwidget.hpp is only included if wxWidgets are used
// and hence putting them there would cause a compiler error without wxWidgets

//GD not sure if the environment at the end of called PRO or FUNC is OK (nested procedures?) TBC.
BaseGDL* CallEventFunc( const std::string& f, BaseGDL* ev)
{
  StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack( ) );

  int funIx = GDLInterpreter::GetFunIx( f );

  ProgNodeP callingNode = NULL; //BaseGDL::interpreter->GetRetTree();

  EnvUDT* newEnv = new EnvUDT( callingNode, funList[ funIx], NULL );
  newEnv->SetNextPar( ev ); // pass as local

  BaseGDL::interpreter->CallStack( ).push_back( newEnv );

  // make the call
  newEnv->SetCallContext( EnvUDT::RFUNCTION );
  BaseGDL* res = BaseGDL::interpreter->call_fun( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );
  return res;
}

void CallEventPro( const std::string& p, BaseGDL* p0, BaseGDL* p1 ) {
  //due to the introduction of AUTO_PRINT_EXPR unfound procedures will crash GDL if this is not properly done.
#ifdef 	AUTO_PRINT_EXPR
    if ( GDLInterpreter::CheckProExist( p )) {
#endif      
    StackGuard<EnvStackT> guard( BaseGDL::interpreter->CallStack( ) );
    int proIx = GDLInterpreter::GetProIx( p );

    ProgNodeP callingNode = NULL; //BaseGDL::interpreter->GetRetTree();

    EnvUDT* newEnv = new EnvUDT( callingNode, proList[ proIx], NULL );
    newEnv->SetNextPar( p0 ); // pass as local
    if ( p1 != NULL )
      newEnv->SetNextPar( p1 ); // pass as local

    BaseGDL::interpreter->CallStack( ).push_back( newEnv );

    // make the call
    BaseGDL::interpreter->call_pro( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );
#ifdef 	AUTO_PRINT_EXPR
  } else std::cerr<<"Attempt to call undefined procedure: '"+p+"'."<<std::endl;
#endif      
}

DStructGDL* CallEventHandler( DStructGDL* ev ) {
  // Must work in good harmony with WIDGET_EVENT requirements.
  // for one event, start from the originating widget and go through the list of parents, 
  // and process the first event-related procedure associated.
  // If the event handling found is a PROCEDURE, do it, and return NULL.
  // If the event handling found is FUNCTION, use it and return the result.
  // If the top of the hierarchy is attained without ev being swallowed by an event handler, return ev.
  // Empty events (success) are returned in any other case.
#ifdef HAVE_LIBWXWIDGETS
  

  static int idIx = 0 ; //ev->Desc( )->TagIndex( "ID" ); // 0
  static int topIx = 1; //ev->Desc( )->TagIndex( "TOP" ); // 1
  static int handlerIx = 2; //ev->Desc( )->TagIndex( "HANDLER" ); // 2

  DLong actID = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];

  //run-time errors (throws by interpreter etc but in widget's loop)
  if (ev->Desc( )->Name( ) == "*WIDGET_RUNTIME_ERROR*" ) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "CallEventHandler: *WIDGET_RUNTIME_ERROR*" << std::endl;
#endif
    static int messIx = ev->Desc( )->TagIndex( "MESSAGE" );
    std::string mess = (*static_cast<DStringGDL*> (ev->GetTag( messIx, 0 )))[0];
    GDLDelete(ev);
    EnvUDT* e = GDLInterpreter::CallStackBack(); e->Throw(mess);
    return NULL;
  } else if (ev->Desc( )->Name( ) == "*TOPLEVEL_DESTROYED*" ) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "CallEventHandler: *TOPLEVEL_DESTROYED*: TLB Widget: "+i2s(actID)+" has been destroyed."<< std::endl;
#endif
    GDLDelete(ev);
    return NULL;
  }
//Do we also protect against noevent widgets??
//  if ( ev->Desc( )->Name( ) == "WIDGET_NOEVENT" ) return ev;
  
  // note that such a struct name is illegal in GDL
  // therefore it cannot be used in user code.
  // This is safer than choosing a legal name
  // as it could collide with user code
  else if ( ev->Desc( )->Name( ) == "*WIDGET_DESTROY*" ) {
    GDLWidget* widget = GDLWidget::GetWidget( actID );
    if ( widget == NULL ) {
#ifdef GDL_DEBUG_WIDGETS
      Warning( "CallEventHandler: *WIDGET_DESTROY*: Internal error: Destroy request for already destroyed widget. ID: " + i2s( actID ) );
#endif
      GDLDelete(ev);
      return NULL;
    }
    
    static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" );
    DLong handlerCode = (*static_cast<DLongGDL*> (ev->GetTag( handlerIx, 0 )))[0];

    GDLDelete( ev );

    assert( handlerCode == 0 ); // only '0' -> Destroy for now

    assert( widget->IsBase( ) );
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "CallEventHandler: *WIDGET_DESTROY*: Deleting widget: "+i2s(actID) << std::endl;
#endif
    delete widget; // removes itself from widgetList

    return NULL; //= OK 
  }
  
  //No handler yet: set value to 0
  (*static_cast<DLongGDL*> (ev->GetTag(handlerIx, 0)))[0] = 0;
  do {
#ifdef GDL_DEBUG_WIDGETS          
    std::cout << "searching event handler with: " + i2s(actID) << std::endl;
#endif
    GDLWidget *widget = GDLWidget::GetWidget(actID);
    if (widget == NULL) {
#ifdef GDL_DEBUG_WIDGETS
      Warning("CallEventHandler: Widget no longer valid. ID: " + i2s(actID));
#endif
      actID = GDLWidget::NullID;
      GDLDelete(ev);
      ev = NULL;
      break; //out of while
    }
    DString eventHandlerPro = widget->GetEventPro();
    if (eventHandlerPro != "") {
      (*static_cast<DLongGDL*> (ev->GetTag(handlerIx, 0)))[0] = actID; //handler ID marked.
#ifdef GDL_DEBUG_WIDGETS          
      std::cout << "CallEventPro: " + eventHandlerPro + " on " + i2s(actID) << std::endl;
#endif
      CallEventPro(eventHandlerPro, ev); // swallows ev according to the doc, thus:
      ev = NULL; // note: ev is already deleted at this point when returning.
      break; // out of while
    }
    DString eventHandlerFun = widget->GetEventFun();
    if (eventHandlerFun != "") {
      //this a posteriori (not issued in gdlwidgeteventhandler, where handler=topFrame is the default) will define me (actID) as the handler of this event,
      //which is OK as long as the ID of the originating event is eitehr me or one of my children..
      (*static_cast<DLongGDL*> (ev->GetTag(handlerIx, 0)))[0] = actID; //handler ID marked.
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "CallEventFunc: " + eventHandlerFun + " on " + i2s(actID) << std::endl;
#endif
	  DStructGDL* evstart=(DStructGDL*)(ev)->Dup();
      BaseGDL* retVal = CallEventFunc(eventHandlerFun, ev); // grabs ev
      // note: ev is already deleted at this point when returning.
	  //will test if ev is unchanged:
	  
      if (retVal->Type() == GDL_STRUCT) {
        ev = static_cast<DStructGDL*> (retVal);
        if (ev->Desc()->TagIndex("ID") != idIx || ev->Desc()->TagIndex("TOP") != topIx || ev->Desc()->TagIndex("HANDLER") != handlerIx) {
          GDLDelete(ev);
          throw GDLException(eventHandlerFun + ": Event handler return struct must contain ID, TOP, HANDLER as first tags.");
        }
		bool doReturn = ( (*static_cast<DLongGDL*> (ev->GetTag(handlerIx, 0)))[0] == (*static_cast<DLongGDL*> (evstart->GetTag(handlerIx, 0)))[0] );
		GDLDelete(evstart);
		if (doReturn) return ev; //apparently, this is a case where a function should return, at least this patch solves PLOTMAN's panel problem see #1685
      } else { //not a struct, same as a procedure, has swallowed the event
        ev = NULL;
        break; 
      }
      // returned struct is a new ev:
      // FUNCTION --> no break, will go up to the top or exit if consumed.!
    }
    actID = widget->GetParentID(); //go upper in hierarchy
  } while (actID != GDLWidget::NullID);
#endif
  return ev;
}

template< typename T>
T* GetKeywordAs( EnvT* e, int ix)
{
  BaseGDL* kwBaseGDL = e->GetTheKW( ix );
  if ( kwBaseGDL == NULL )
    return NULL;

  if ( kwBaseGDL->Type( ) != T::t )
    return static_cast<T*> (kwBaseGDL->Convert2( T::t, BaseGDL::COPY ));

  bool stolen = e->StealLocalKW( ix );
  if ( stolen )
    return static_cast<T*> (kwBaseGDL);

  return static_cast<T*> (kwBaseGDL->Dup( ));
}

#ifdef HAVE_LIBWXWIDGETS

DStringGDL*  GetTableValueAsString(EnvT* e, BaseGDL* &value, DStringGDL* format,  int &majority, bool acceptSingle=false) {
  DStringGDL* valueAsStrings;

  if (value == NULL) { //set valueAsString. 
	//local check of size given, changes number of lines/columns
	DLong xSize, ySize;
	static int xsizeIx = e->KeywordIx("XSIZE");
	static int ysizeIx = e->KeywordIx("YSIZE");
	xSize = -1;
	e->AssureLongScalarKWIfPresent(xsizeIx, xSize);
	ySize = -1;
	e->AssureLongScalarKWIfPresent(ysizeIx, ySize);
	SizeT dims[2];
	dims[0] = (xSize > 0) ? xSize : 6;
	dims[1] = (ySize > 0) ? ySize : 6;
	dimension dim(dims, 2);
	valueAsStrings = new DStringGDL(dim);
	majority=GDLWidgetTable::NONE_MAJOR;
  } else {
	//test of non-conformity for value
	value = value->Dup();
	if (value->Rank() > 2) e->Throw("Value has greater than 2 dimensions.");
	if (value->Rank() == 0) {
	  if (acceptSingle) value->SetDim(dimension(1)) ; //convert to 1D array
	  else e->Throw("Expression must be an array in this context: " + e->GetParString(0));
	}
	if (value->Type() == GDL_STRUCT) {
	  if (value->Rank() > 1) e->Throw("Multi dimensional arrays of structures not allowed.");
	  if (majority == GDLWidgetTable::NONE_MAJOR) majority = GDLWidgetTable::ROW_MAJOR;
	  //convert to STRING
	  DStructGDL *input = static_cast<DStructGDL*> (value);
	  SizeT nTags = input->NTags();
	  //further check:
	  for (SizeT iTag = 0; iTag < nTags; ++iTag) {
		BaseGDL* tested = input->GetTag(iTag);
		if (tested->Rank() > 0 || tested->Type() == GDL_STRUCT) e->Throw("Structures cannot include arrays or other structures.");
	  }
	  SizeT nEl = input->N_Elements();
	  stringstream os;
	  input->ToStreamRaw(os);
	  SizeT dims[2];
	  dims[0] = nTags;
	  dims[1] = nEl;
	  dimension dim(dims, 2);
	  valueAsStrings = new DStringGDL(dim, BaseGDL::NOZERO);

	  valueAsStrings->FromStream(os); //simple as that if we manage the dimensions and transpose accordingly....
	  if (majority == GDLWidgetTable::COLUMN_MAJOR) valueAsStrings = static_cast<DStringGDL*> (valueAsStrings->Transpose(NULL));
	} else if (value->Type() == GDL_STRING) {
	  //no conversion and besides, prevent problem with NULL strings
	  valueAsStrings = static_cast<DStringGDL*>(value)->Dup();
	} else {
	  //convert to STRING using FORMAT.
	  valueAsStrings = CallStringFunction(value, format);
	  majority = GDLWidgetTable::NONE_MAJOR;
	}
  }
  return valueAsStrings;
}
#endif


namespace lib {
using namespace std;

BaseGDL* widget_table( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget* parent = GDLWidget::GetWidget( parentID );
  if ( parent == NULL )  e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  static int ALIGNMENT = e->KeywordIx( "ALIGNMENT" );
  static int BACKGROUND_COLOR = e->KeywordIx( "BACKGROUND_COLOR" );
  static int COLUMN_LABELS = e->KeywordIx( "COLUMN_LABELS" );
  static int COLUMN_MAJOR = e->KeywordIx( "COLUMN_MAJOR" );
  static int ROW_MAJOR = e->KeywordIx( "ROW_MAJOR" );
  static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
  static int DISJOINT_SELECTION = e->KeywordIx( "DISJOINT_SELECTION" );
  static int EDITABLE = e->KeywordIx( "EDITABLE" );
  static int FOREGROUND_COLOR = e->KeywordIx( "FOREGROUND_COLOR" );
  static int FORMAT = e->KeywordIx( "FORMAT" );
//  static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
  static int NO_COLUMN_HEADERS = e->KeywordIx( "NO_COLUMN_HEADERS" );
  static int NO_HEADERS = e->KeywordIx( "NO_HEADERS" );
  static int NO_ROW_HEADERS = e->KeywordIx( "NO_ROW_HEADERS" );
  static int RESIZEABLE_COLUMNS = e->KeywordIx( "RESIZEABLE_COLUMNS" );
  static int RESIZEABLE_ROWS = e->KeywordIx( "RESIZEABLE_ROWS" );
  static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
  static int ROW_LABELS = e->KeywordIx( "ROW_LABELS" );
  static int VALUE = e->KeywordIx( "VALUE" );
  
  bool disjointSelection = e->KeywordSet( DISJOINT_SELECTION );
//  bool ignoreAccelerators = e->KeywordSet( IGNORE_ACCELERATORS );
  bool noHeaders = e->KeywordSet( NO_HEADERS );
  bool noColumnHeaders = e->KeywordSet( NO_COLUMN_HEADERS ) || noHeaders;
  bool noRowHeaders = e->KeywordSet( NO_ROW_HEADERS ) || noHeaders;
  bool resizeableColumns = e->KeywordSet( RESIZEABLE_COLUMNS );
  bool resizeableRows = e->KeywordSet( RESIZEABLE_ROWS );

  DByteGDL* editable = GetKeywordAs<DByteGDL>(e, EDITABLE );
  DByteGDL* alignment = GetKeywordAs<DByteGDL>(e, ALIGNMENT);
  //not checked by idl
  //if (alignment) {if (!(alignment->Rank()==0 ||alignment->Rank()==2)) e->Throw("Table grid color attribute has the wrong dimensions.");}
 

  DByteGDL* backgroundColor = GetKeywordAs<DByteGDL>(e, BACKGROUND_COLOR);
  DByteGDL* foregroundColor = GetKeywordAs<DByteGDL>(e, FOREGROUND_COLOR);
  //exit if problems
  if (backgroundColor) {if (backgroundColor->N_Elements()%3 != 0) e->Throw("Table grid color attribute has the wrong dimensions.");}
  if (foregroundColor) {if (foregroundColor->N_Elements()%3 != 0) e->Throw("Table grid color attribute has the wrong dimensions.");}
 
  DStringGDL* columnLabels = GetKeywordAs<DStringGDL>(e, COLUMN_LABELS);
  DLongGDL* columnWidth = GetKeywordAs<DLongGDL>(e, COLUMN_WIDTHS);

  static int AM_PM = e->KeywordIx( "AM_PM" );
  DStringGDL* amPm = GetKeywordAs<DStringGDL>(e, AM_PM);
  //check
  if (amPm) {if (amPm->N_Elements()!=2) e->Throw("% Keyword array parameter AM_PM must have 2 elements.");}
  static int DAYS_OF_WEEK = e->KeywordIx( "DAYS_OF_WEEK" );
  DStringGDL* daysOfWeek = GetKeywordAs<DStringGDL>(e, DAYS_OF_WEEK);
  //check
  if (daysOfWeek) {if (daysOfWeek->N_Elements()!=2) e->Throw("% Keyword array parameter DAYS_OF_WEEK must have 7 elements.");}
  static int MONTHS = e->KeywordIx( "MONTHS" );
  DStringGDL* month = GetKeywordAs<DStringGDL>(e, MONTHS);
  //check
  if (month) {if (month->N_Elements()!=12) e->Throw("% Keyword array parameter MONTH must have 12 elements.");}

  DStringGDL* format = GetKeywordAs<DStringGDL>(e, FORMAT);

  DLongGDL* rowHeights = GetKeywordAs<DLongGDL>(e, ROW_HEIGHTS);
  DStringGDL* rowLabels = GetKeywordAs<DStringGDL>(e, ROW_LABELS);

//  DLong tabMode = 0;
//  e->AssureLongScalarKWIfPresent( TAB_MODE, tabMode );

  BaseGDL* value = e->GetKW( VALUE ); //value may not exist!!!! Dangerous!!!
  bool isstruct=( value!=NULL && value->Type()==GDL_STRUCT);
  int majority = GDLWidgetTable::NONE_MAJOR;
  if (e->KeywordSet(COLUMN_MAJOR) && ! isstruct) e->Throw("The COLUMN_MAJOR keyword can only be used on structure values.");
  if (e->KeywordSet(ROW_MAJOR) && ! isstruct) e->Throw("The ROW_MAJOR keyword can only be used on structure values.");
  if (e->KeywordSet(COLUMN_MAJOR)) majority = GDLWidgetTable::COLUMN_MAJOR; //
  if (e->KeywordSet(ROW_MAJOR)) majority = GDLWidgetTable::ROW_MAJOR; //order of preference

  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  DLong x_scroll_size = 0;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  DLong y_scroll_size = 0;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );

  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int ALL_EVENTS = e->KeywordIx( "ALL_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool allevents = e->KeywordSet( ALL_EVENTS );
  bool kbrdfocusevents = e->KeywordSet( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordSet( CONTEXT_EVENTS );

  if (allevents)  eventFlags |= GDLWidget::EV_ALL;
  if (editable != NULL)  eventFlags |= GDLWidget::EV_KEYBOARD; //use this for handling editable events
  if (kbrdfocusevents) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (contextevents) eventFlags |= GDLWidget::EV_CONTEXT;

  DStringGDL* valueAsStrings=GetTableValueAsString(e, value, format, majority);
 
  GDLWidgetTable* table = new GDLWidgetTable( parentID, e,
  alignment,
  amPm,
  backgroundColor,
  foregroundColor,
  columnLabels,
  majority,
  columnWidth,
  daysOfWeek,
  disjointSelection,
  editable,
  format,
//  ignoreAccelerators,
  month,
  noColumnHeaders,
  noRowHeaders,
  resizeableColumns,
  resizeableRows,
  rowHeights,
  rowLabels,
//  tabMode,
  value,
  x_scroll_size,
  y_scroll_size,
  valueAsStrings,
  eventFlags
  );
  if (table->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )  table->SetWidgetType( GDLWidget::WIDGET_TABLE );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_TABLE "+i2s(table->GetWidgetID( ))+" OK.\n";
#endif
  // return widget ID
  return new DLongGDL( table->GetWidgetID( ) );
#endif
} // widget_table


BaseGDL* widget_tree( EnvT* e)
{
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget* parent = GDLWidget::GetWidget( parentID );
  if ( parent == NULL )  e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if ( !(parent->IsBase()) && !(parent->IsTree()))  e->Throw( "Parent is of incorrect type.");
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->IsBase() && parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  static int BITMAP = e->KeywordIx( "BITMAP" );

  static int DRAGGABLE = e->KeywordIx( "DRAGGABLE" );
  static int EXPANDED = e->KeywordIx( "EXPANDED" );
  static int FOLDER = e->KeywordIx( "FOLDER" );
  static int INDEX = e->KeywordIx( "INDEX" );
  static int TOP = e->KeywordIx( "TOP" ); //obsoleted in 6.4 use INDEX=0
  
  static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY" );
  //  static int TAB_MODE = e->KeywordIx( "TAB_MODE" );

  static int VALUE = e->KeywordIx( "VALUE" );
//
  DLong treeindex=-1;
  if (e->KeywordPresent( INDEX )) {
    e->AssureLongScalarKWIfPresent( INDEX, treeindex );
  } else if (e->KeywordSet( TOP )) { treeindex=0; }
   
  DLong draggability=-1; //inherit
  if (e->KeywordPresent( DRAGGABLE )) e->AssureLongScalarKWIfPresent( DRAGGABLE, draggability );
  bool folder = e->KeywordSet( FOLDER );
  bool expanded = (folder && e->KeywordSet( EXPANDED ));

  static int MASK = e->KeywordIx( "MASK" );
  bool mask = e->KeywordSet( MASK );

  //
  //common for all widgets
  DULong eventFlags=0;
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool context = e->KeywordSet( CONTEXT_EVENTS );
  if (context) eventFlags |= GDLWidget::EV_CONTEXT;

  static int DROP_EVENTS = e->KeywordIx( "DROP_EVENTS" );
  DLong dropability = -1; //inherit
  if (e->KeywordPresent( DROP_EVENTS )) {
    bool dropevents=e->KeywordSet( DROP_EVENTS); //get a zero or one
    dropability=dropevents?1:0;
  }
  
  DByteGDL* testByte=NULL;
  wxBitmap* bitmap=NULL;
  if (e->KeywordPresentAndDefined(BITMAP)) { //must be 16 x 16 x 3 but we do not care about the 16x16
    testByte = e->GetKWAs<DByteGDL>(BITMAP);
    if (testByte) {
      bitmap = GetBitmapFromPassedBytes(e, testByte, mask);
    }
  }

//
//  DLong tabMode = 0;
//  e->AssureLongScalarKWIfPresent( TAB_MODE, tabMode );
  
  DString dragNotify=(parent->IsBase())?"<default>":"<inherit>";
  if (e->KeywordPresent(DRAG_NOTIFY)) {
    e->AssureStringScalarKWIfPresent( DRAG_NOTIFY, dragNotify );
    dragNotify=StrUpCase(dragNotify); //UPPERCASE
  } 
  DString strvalue=""; //important to init to a zero-length string!!!
  e->AssureStringScalarKWIfPresent( VALUE, strvalue ); //important to init to a zero-length string!!!
  DStringGDL* value=new DStringGDL(strvalue);


    GDLWidgetTree* tree = new GDLWidgetTree(parentID, e, value, eventFlags
      , bitmap
      , draggability
      , dropability
      , expanded
      , folder
      , treeindex
      , dragNotify
      );
  // after creation, set mask:
    if (mask) tree->SetMask(true); 
  if (tree->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN ) tree->SetWidgetType( GDLWidget::WIDGET_TREE );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_TREE "+i2s(tree->GetWidgetID( ))+" OK.\n";
#endif
  // return widget ID
  return new DLongGDL( tree->GetWidgetID( ) );
#endif
} // widget_tree

BaseGDL* widget_draw( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );
  static int COLOR_MODEL = e->KeywordIx( "COLOR_MODEL" );

// probably never implemented:
//  static int CLASSNAME = e->KeywordIx( "CLASSNAME" ); // string
//  static int COLORS = e->KeywordIx( "COLORS" ); // long
//  static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY" ); //string
//  static int GRAPHICS_LEVEL = e->KeywordIx( "GRAPHICS_LEVEL" );
//  static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
//  static int RENDERER = e->KeywordIx( "RENDERER" );
//  static int RETAIN = e->KeywordIx( "RETAIN" );

  static int APP_SCROLL = e->KeywordIx( "APP_SCROLL" );
  bool app_scroll = e->KeywordSet(APP_SCROLL);
  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  DLong x_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  DLong y_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );

  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );
  
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;
  
  static int DROP_EVENTS = e->KeywordIx( "DROP_EVENTS" );
  static int EXPOSE_EVENTS = e->KeywordIx( "EXPOSE_EVENTS" );
  static int MOTION_EVENTS = e->KeywordIx( "MOTION_EVENTS" );
  static int VIEWPORT_EVENTS = e->KeywordIx( "VIEWPORT_EVENTS" );
  static int WHEEL_EVENTS = e->KeywordIx( "WHEEL_EVENTS" );
  static int BUTTON_EVENTS = e->KeywordIx( "BUTTON_EVENTS" );
  static int KEYBOARD_EVENTS = e->KeywordIx( "KEYBOARD_EVENTS" );

  bool drop_events = e->KeywordSet( DROP_EVENTS );
  bool expose_events = e->KeywordSet( EXPOSE_EVENTS );
  bool motion_events = e->KeywordSet( MOTION_EVENTS );
  bool viewport_events = e->KeywordSet( VIEWPORT_EVENTS );
  bool wheel_events = e->KeywordSet( WHEEL_EVENTS );
  bool button_events = e->KeywordSet( BUTTON_EVENTS );
  bool keyboard_events = e->KeywordPresent( KEYBOARD_EVENTS );

  if (drop_events) eventFlags |= GDLWidget::EV_DROP;
  if (expose_events) eventFlags |= GDLWidget::EV_EXPOSE;
  if (motion_events)  eventFlags |= GDLWidget::EV_MOTION;
  if (viewport_events) eventFlags |=  GDLWidget::EV_VIEWPORT;
  if (wheel_events) eventFlags |=  GDLWidget::EV_WHEEL;
  if (button_events) eventFlags |=  GDLWidget::EV_BUTTON;
  if (keyboard_events) {
    DLong val= (*e->GetKWAs<DLongGDL>(KEYBOARD_EVENTS))[0];
    if (val==2) { eventFlags |=  GDLWidget::EV_KEYBOARD2;}
    else if (val==1)  {eventFlags |=  GDLWidget::EV_KEYBOARD;}
  }
  DStringGDL* tooltipgdl=NULL;
  if (e->KeywordPresentAndDefined(TOOLTIP)) tooltipgdl = e->GetKWAs<DStringGDL>(TOOLTIP) ;
  GDLWidgetDraw* draw=new GDLWidgetDraw( parentID, e, -1, x_scroll_size, y_scroll_size, app_scroll, eventFlags, tooltipgdl);
  if (e->KeywordPresent(COLOR_MODEL)) static_cast<gdlwxDrawPanel*>(draw->GetWxWidget())->SetUndecomposed(); 
  if (draw->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN ) draw->SetWidgetType( GDLWidget::WIDGET_DRAW );
//  if (keyboard_events) draw->SetFocus(); //cannot set focus on this one when there are others. Not here anyway!
  #ifdef GDL_DEBUG_WIDGETS
    cerr<<"WIDGET_DRAW "+i2s(draw->GetWidgetID( ))+" OK.\n";
  #endif
  // return widget ID
  return new DLongGDL( draw->GetWidgetID( ) );
#endif  
} // widget_draw

  
  
    
  BaseGDL* widget_base( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( );

  WidgetIDT parentID = GDLWidget::NullID;
  if ( nParam == 1 ) // no TLB
    e->AssureLongScalarPar( 0, parentID );

  if (e->KeywordPresent("FONT")) e->Throw("FONT keyword not accepted by WIDGET_BASE.");  //forbidden for widget_base 
  // handle some more keywords over widget
  
  static int mbarIx = e->KeywordIx( "MBAR" );
  static int obsolete_app_mbarIx = e->KeywordIx( "APP_MBAR" );
  static int modalIx = e->KeywordIx( "MODAL" );
  static int base_align_bottomIx = e->KeywordIx( "BASE_ALIGN_BOTTOM" );
  static int base_align_centerIx = e->KeywordIx( "BASE_ALIGN_CENTER" );
  static int base_align_leftIx = e->KeywordIx( "BASE_ALIGN_LEFT" );
  static int base_align_rightIx = e->KeywordIx( "BASE_ALIGN_RIGHT" );
  static int base_align_topIx = e->KeywordIx( "BASE_ALIGN_TOP" );
  static int columnIx = e->KeywordIx( "COLUMN" );
  static int rowIx = e->KeywordIx( "ROW" );
  static int context_eventsIx = e->KeywordIx( "CONTEXT_EVENTS" );
  static int context_menuIx = e->KeywordIx( "CONTEXT_MENU" );
  static int exclusiveIx = e->KeywordIx( "EXCLUSIVE" );
  static int nonexclusiveIx = e->KeywordIx( "NONEXCLUSIVE" );
  static int floatingIx = e->KeywordIx( "FLOATING" );
  static int grid_layoutIx = e->KeywordIx( "GRID_LAYOUT" );
  static int kbrd_focus_eventsIx = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int mapIx = e->KeywordIx( "MAP" );
  static int spaceIx = e->KeywordIx( "SPACE" );
  static int titleIx = e->KeywordIx( "TITLE" );
//  static int tlb_frame_attrIx = e->KeywordIx( "TLB_FRAME_ATTR" );
  static int tlb_iconify_eventsIx = e->KeywordIx( "TLB_ICONIFY_EVENTS" );
  static int tlb_kill_request_eventsIx = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS" );
  static int tlb_move_eventsIx = e->KeywordIx( "TLB_MOVE_EVENTS" );
  static int tlb_size_eventsIx = e->KeywordIx( "TLB_SIZE_EVENTS" );
  static int tlb_frame_attrIx = e->KeywordIx("TLB_FRAME_ATTR");
  static int tlb_bitmapIx = e->KeywordIx("BITMAP");
//  static int toolbarIx = e->KeywordIx( "TOOLBAR" );
  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int xpadIx = e->KeywordIx( "XPAD" );
  static int x_scroll_sizeIx = e->KeywordIx( "X_SCROLL_SIZE" );
  static int ypadIx = e->KeywordIx( "YPAD" );
  static int y_scroll_sizeIx = e->KeywordIx( "Y_SCROLL_SIZE" );
  static int display_nameIx = e->KeywordIx( "DISPLAY_NAME" );
  static int rname_mbarIx = e->KeywordIx( "RNAME_MBAR" );

  long children_alignment=GDLWidget::gdlwALIGN_NOT;
  if (e->KeywordSet(base_align_topIx )) children_alignment|=GDLWidget::gdlwALIGN_TOP;
  if (e->KeywordSet(base_align_bottomIx )) children_alignment|=GDLWidget::gdlwALIGN_BOTTOM;
  if (e->KeywordSet(base_align_leftIx )) children_alignment|=GDLWidget::gdlwALIGN_LEFT;
  if (e->KeywordSet(base_align_centerIx)) children_alignment|=GDLWidget::gdlwALIGN_CENTER;
  if (e->KeywordSet(base_align_rightIx)) children_alignment|=GDLWidget::gdlwALIGN_RIGHT;

  bool modal = e->KeywordSet( modalIx );

  bool context_events = e->KeywordSet( context_eventsIx );
  bool IsContextMenu = e->KeywordSet( context_menuIx );

  bool exclusive = e->KeywordSet( exclusiveIx );
  bool nonexclusive = e->KeywordSet( nonexclusiveIx );

  bool floating = e->KeywordSet( floatingIx );
  bool grid_layout = e->KeywordSet( grid_layoutIx );
  bool kbrd_focus_events = e->KeywordSet( kbrd_focus_eventsIx );

  bool mapWid = true;
  if ( e->KeywordPresent( mapIx ) ) if ( !e->KeywordSet( mapIx ) )  mapWid = false;
  //    std::cout << "Map in widget_base: " << mapWid << std::endl;

  //     bool scroll = e->KeywordSet( scrollIx); //in getcommonkw.
//  bool tlb_frame_attr = e->KeywordSet( tlb_frame_attrIx );
  bool tlb_iconify_events = e->KeywordSet( tlb_iconify_eventsIx );
  bool tlb_kill_request_events = e->KeywordSet( tlb_kill_request_eventsIx );
  bool tlb_move_events = e->KeywordSet( tlb_move_eventsIx );
  bool tlb_size_events = e->KeywordSet( tlb_size_eventsIx );
//  bool toolbar = e->KeywordSet( toolbarIx );
  
  if (e->KeywordPresent(tlb_bitmapIx)) Warning("BITMAP option not available for WIDGET_BASE (fixme).");
  DLong frame_attr=0;
  e->AssureLongScalarKWIfPresent( tlb_frame_attrIx, frame_attr );
  DLong x_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( x_scroll_sizeIx, x_scroll_size );
  DLong y_scroll_size = -1;
  e->AssureLongScalarKWIfPresent( y_scroll_sizeIx, y_scroll_size );

  bool mbarPresent = e->KeywordPresent( mbarIx )||e->KeywordPresent( obsolete_app_mbarIx );

  // consistency
  if ( nonexclusive && exclusive ) e->Throw( "Conflicting keywords: [NON]EXCLUSIVE" );
  //exclusive and non-exclusive bases ignore the xoffset and yoffset keyword. Furthermore, the layout is always set (/COL or /ROW, but not "nothing").
  //besides, SPACE, XPAD and YPAD are ignored.
  //according to doc, Exclusive and non-exclusive base admit only button widget children, but simple tests show it is not the case for IDL up to now.

  //xpad, ypad and space default to gdlPAD if not precised:

  DLong space=gdlSPACE;
  if ( e->KeywordPresent(spaceIx) && !nonexclusive && !exclusive ) e->AssureLongScalarKWIfPresent( spaceIx, space );
  DLong xpad = space;
  if ( !nonexclusive && !exclusive ) e->AssureLongScalarKWIfPresent( xpadIx, xpad );
  DLong ypad = space;
  if ( !nonexclusive && !exclusive ) e->AssureLongScalarKWIfPresent( ypadIx, ypad );
  
  DLong column = 0;
  e->AssureLongScalarKWIfPresent( columnIx, column );
  DLong row = 0;
  e->AssureLongScalarKWIfPresent( rowIx, row );

  if (column>0 && row>0) e->Throw( "Conflicting keywords: row vs. col" );

  DString resource_name = "";
  DString rname_mbar = "";
  e->AssureStringScalarKWIfPresent( rname_mbarIx, rname_mbar );

  DString title = "";
  e->AssureStringScalarKWIfPresent( titleIx, title );

  DString display_name = "";
  e->AssureStringScalarKWIfPresent( display_nameIx, display_name );



  if ( mbarPresent ) {
    if ( parentID != GDLWidget::NullID )
      e->Throw( "Only top level bases allow a menubar." );
    if ( e->KeywordPresent( mbarIx ) ) { //prefer MBAR over obsolete app_mbar
      e->AssureGlobalKW( mbarIx );
    } else if ( e->KeywordPresent( obsolete_app_mbarIx ) ) {
      e->AssureGlobalKW( obsolete_app_mbarIx );
    }

  }

  if ( modal || floating ) {
    //we must test groupleader even before it is set up by SetCommonKeywords.
    DLong groupLeader = 0;
    static int group_leaderIx = e->KeywordIx( "GROUP_LEADER" );
    e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader );
    if ( groupLeader == 0 ) {
      if (modal) e->Throw( "MODAL top level bases must have a group leader specified." );
      if (floating) e->Throw( "FLOATING top level bases must have a group leader specified." );
    }
    if ( parentID != GDLWidget::NullID )
      e->Throw( "Only top level bases can be MODAL." );
  }
  
  if ( parentID != GDLWidget::NullID ) {
    GDLWidget* p = GDLWidget::GetWidget( parentID );
    if ( p == NULL )
      e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
    if ( !IsContextMenu && !p->IsBase( ) && !p->IsTab( ) ) e->Throw( "Parent is of incorrect type." );
    if ( IsContextMenu && ( p->IsContextBase() || p->IsButton() || p->IsComboBox() || p->IsDropList() || p->IsLabel() || p->IsSlider() || p->IsTab()) ) e->Throw( "Parent is of incorrect type." );
  }
  
  if ( nParam == 0 && IsContextMenu ) e->Throw( "Incorrect number of arguments." );

  // generate widget
  WidgetIDT mBarID = mbarPresent ? 1 : 0;

  int exclusiveMode = GDLWidget::BGNORMAL;
  if ( exclusive ) { exclusiveMode = GDLWidget::BGEXCLUSIVE;} //space xpad ypad ignored if mode=exclusive or nonexclusive
  if ( nonexclusive ) { exclusiveMode = GDLWidget::BGNONEXCLUSIVE;}

  //events:
  //CONTEXT_EVENTS
  //KBRD_FOCUS_EVENTS
  //TLB_ICONIFY_EVENTS
  //TLB_KILL_REQUEST_EVENTS
  //TLB_MOVE_EVENTS
  //TLB_SIZE_EVENTS
  //TRACKING_EVENTS (done above)

  if (context_events) eventFlags |= GDLWidget::EV_CONTEXT;
  if (kbrd_focus_events) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (tlb_move_events)  {
    if ( parentID == GDLWidget::NullID ) eventFlags |= GDLWidget::EV_MOVE;
  }
  if (tlb_size_events) {
    if ( parentID == GDLWidget::NullID ) eventFlags |= GDLWidget::EV_SIZE;
  }
  if (tlb_iconify_events) {
    if ( parentID == GDLWidget::NullID ) eventFlags |= GDLWidget::EV_ICONIFY;
  }
  if (tlb_kill_request_events) {
    if ( parentID != GDLWidget::NullID ) e->Throw( "Only top level bases allow the TLB_KILL_REQUEST_EVENTS keyword." );
    eventFlags |= GDLWidget::EV_KILL;
  }
  //xpad, ypad, space all have meaning only if a sizer is used, i.e. col or row >0:
  if (column<1 && row<1) {
    xpad=0; ypad=0; space=0;
  }
  GDLWidgetBase* base; 
  if ( parentID == GDLWidget::NullID ) {
    base = new GDLWidgetTopBase( e, eventFlags, mapWid, /*ref*/ mBarID, modal, frame_attr, column, row, exclusiveMode, floating, resource_name, rname_mbar, title, display_name, xpad, ypad, x_scroll_size, y_scroll_size, grid_layout , children_alignment, space);

      // some more properties
      if (mbarPresent) {
        if (e->KeywordPresent(mbarIx)) { //prefer MBAR over obsolete app_mbar
          e->SetKW(mbarIx, new DLongGDL(mBarID));
        } else if (e->KeywordPresent(obsolete_app_mbarIx)) {
          e->SetKW(obsolete_app_mbarIx, new DLongGDL(mBarID));
        }
      }
      
  } else if (IsContextMenu) {
    GDLWidgetContextBase* ctx = new GDLWidgetContextBase( parentID, e, eventFlags,
    mapWid,
    column, row,
    exclusiveMode,
    resource_name, rname_mbar,
    title,
    display_name,
    xpad, ypad,
    x_scroll_size, y_scroll_size, grid_layout , children_alignment, space);
    return new DLongGDL( ctx->GetWidgetID( ) );
  } else if ( static_cast<GDLWidgetBase*>(GDLWidget::GetWidget( parentID ))->IsTab() ) {
    base = new GDLWidgetTabbedBase( parentID, e, eventFlags,
    mapWid,
    column, row,
    exclusiveMode,
    resource_name, rname_mbar,
    title,
    display_name,
    xpad, ypad,
    x_scroll_size, y_scroll_size, grid_layout , children_alignment, space);
  } else {
    base = new GDLWidgetNormalBase( parentID, e, eventFlags,
    mapWid,
    column, row,
    exclusiveMode,
    resource_name, rname_mbar,
    title,
    display_name,
    xpad, ypad,
    x_scroll_size, y_scroll_size, grid_layout , children_alignment, space);
  }

  
  if (base->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   base->SetWidgetType( GDLWidget::WIDGET_BASE );

#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_BASE "+i2s(base->GetWidgetID( ))+" OK.\n";
#endif 
  // return widget ID
  return new DLongGDL( base->GetWidgetID( ) );
#endif
}


// WIDGET_BUTTON
  BaseGDL* widget_button( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  static SizeT buttonNumber=1; //for default value
  //"HELP" is not supported (yet).
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
// Note that when parent is a button but not a menu button, is reported in GDLWidgetButton()
  if (!parent->IsBase() && !parent->IsMenuBar() && !parent->IsMenu()) e->Throw( "Parent is of incorrect type." );

  static int bitmapIx = e->KeywordIx( "BITMAP" );
  static int menuIx = e->KeywordIx( "MENU" );
  bool isMenu =  e->KeywordSet( menuIx );
  static int SeparatorIx = e->KeywordIx( "SEPARATOR" );
  bool hasSeparatorAbove= e->KeywordSet(SeparatorIx) ;
  static int TOOLTIP = e->KeywordIx( "TOOLTIP" );
  static int valueIx = e->KeywordIx( "VALUE" );
  
  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int dynamicResizeIx = e->KeywordIx( "DYNAMIC_RESIZE" );
  bool dynres = e->KeywordSet( dynamicResizeIx );

  static int imageIx = e->KeywordIx( "IMAGE" );
  bool hasImage=false;
  if (isMenu || parent->IsMenu() ) {
    if (e->KeywordSet(imageIx)) hasImage=true;
  }

  static int checkIx = e->KeywordIx( "CHECKED_MENU" );
  bool checked=false;
  if (parent->IsMenu() && !isMenu) {
    checked=(e->KeywordSet(checkIx));
  }
// TBD:  "HELP", "INPUT_FOCUS", "X_BITMAP_EXTRA", "FLAT", "NO_RELEASE", "ACCELERATOR","TAB_MODE" 
  
  
//  To get the equivalent of pushbutton_events (push and release) with wxWidgets and have a better coverage, use ToggleButtons (wx 2.9 and after)
//  static int PUSHBUTTON_EVENTS = e->KeywordIx( "PUSHBUTTON_EVENTS" );
//  bool pushbuttonevents = e->KeywordSet( PUSHBUTTON_EVENTS );
  
  BaseGDL* invalue = e->GetKW( valueIx );

  bool isBitmap =  e->KeywordSet( bitmapIx );
  if (isBitmap && parent->IsMenuBar()) e->Throw("Menu bars items cannot be images");

  DStringGDL* tooltipgdl = NULL;
  GDLWidgetButton* button;
  if (e->KeywordPresentAndDefined(TOOLTIP)) tooltipgdl = e->GetKWAs<DStringGDL>(TOOLTIP);  
  
  
  DString strvalue = "button"+i2s(buttonNumber++); //tested default!

  wxBitmap * bitmap=NULL;
  //value=filename if /BITMAP present. Otherwise value must be string, although if array of correct size, is bitmap!
  //Note BITMAP and RadioButtons are not possible directly.
  DByteGDL* passedBytes=NULL;
  if (invalue) { //IMAGE KW is ignored when VALUE specifies an image.
      if (invalue->Type() == GDL_STRING) { //value is a string
        e->AssureStringScalarKW(valueIx, strvalue);
        // BITMAP is ignored when invalue is of the correct type. Otherwise if string, the bitmap must be present.
        if (isBitmap) {
          hasImage = false; //image will be ignored.
          WordExp(strvalue);
          bitmap = new wxBitmap(wxString(strvalue.c_str(), wxConvUTF8), wxBITMAP_TYPE_ANY);
          if (bitmap->IsOk()) {
          strvalue.clear();
          } else {
            Warning("WIDGET_BUTTON: Can't open bitmap file: " + strvalue);
            delete bitmap;
            bitmap=NULL; //necessary!
          }
        }
      } else { //value is an image
        passedBytes=e->GetKWAs<DByteGDL>(valueIx); //test it
        hasImage = false; //image will be ignored.
      }

      // if hasImage is still true, it is the image KW that contains an image:
      if (hasImage) passedBytes = e->GetKWAs<DByteGDL>(imageIx); //value must be a text and image is in IMAGE=xx KW
          
      //whatever the passedBytes it must be OK:
      if (passedBytes) {
        bitmap = GetBitmapFromPassedBytes(e, passedBytes);
        if (!hasImage) strvalue.clear();
      }
    }
  
  DStringGDL* value=new DStringGDL(strvalue);

  //Separate Button Types depending on parent container type:
  if (parent->IsBase()) {
  if (isMenu) button = new GDLWidgetMenuButton( parentID, e, value, eventFlags, bitmap, tooltipgdl);
    else button = new GDLWidgetNormalButton( parentID, e, value, eventFlags, bitmap, tooltipgdl);
  } else if (parent->IsMenuBar()) {
#ifdef PREFERS_MENUBAR
    button = new GDLWidgetMenuBarButton( parentID, e, value, eventFlags, tooltipgdl);
#else
    button = new GDLWidgetMenuBarButton( parentID, e, value, eventFlags, bitmap, tooltipgdl);
#endif
  } else if (parent->IsMenu()) {    
    if (e->KeywordPresent(TOOLTIP)) e->Throw("Tooltips are not available for menu items.");
    if (isMenu)  button = new GDLWidgetSubMenu( parentID, e, value, eventFlags, hasSeparatorAbove, bitmap);
    else  button = new GDLWidgetMenuEntry( parentID, e, value, eventFlags, hasSeparatorAbove, bitmap, checked);
  } 
  
  if (button->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN ) button->SetWidgetType( GDLWidget::WIDGET_BUTTON );
  if (dynres) button->authorizeDynamicResize();
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_BUTTON "+i2s(button->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( button->GetWidgetID( ) );
#endif
}

// // WIDGET CW_BGROUP
// BaseGDL* widget_bgroup( EnvT* e)
// {
// #ifndef HAVE_LIBWXWIDGETS
//     e->Throw("GDL was compiled without support for wxWidgets");
//     return NULL; // avoid warning
// #else
//     SizeT nParam=e->NParam(1);
// 
//     //SizeT nParam = e->NParam();
//     DLongGDL* p0L = e->GetParAs<DLongGDL>( 0);
//     WidgetIDT parentID = (*p0L)[0];
// 
//     DStringGDL* names = e->GetParAs<DStringGDL>(1);
// 
//     GDLWidget *widget = GDLWidget::GetWidget( parentID);
// 
//     DLong xsize = -1;
//     static int xsizeIx = e->KeywordIx( "XSIZE");
//     e->AssureLongScalarKWIfPresent( xsizeIx, xsize);
// 
//     DLong ysize = -1;
//     static int ysizeIx = e->KeywordIx( "YSIZE");
//     e->AssureLongScalarKWIfPresent( ysizeIx, ysize);
// 
//     static int buttonuvalueIx = e->KeywordIx( "BUTTON_UVALUE");
//     DString buttonuvalue = "";
//     e->AssureStringScalarKWIfPresent(buttonuvalueIx, buttonuvalue);
// 
//     static int uvalueIx = e->KeywordIx( "UVALUE");
//     BaseGDL* uvalue = e->GetKW( uvalueIx);
//     if( uvalue != NULL)
//         uvalue = uvalue->Dup();
// 
//     static int labelIx = e->KeywordIx( "LABEL_TOP");
//     DString labeltop = "";
//     e->AssureStringScalarKWIfPresent( labelIx, labeltop);
// 
//     GDLWidgetBGroup::BGroupMode mode = GDLWidgetBGroup::NORMAL;
//     static int modeIx = e->KeywordIx( "EXCLUSIVE");
//     if(e->KeywordSet( modeIx)) {
//         mode = GDLWidgetBGroup::EXCLUSIVE;
//     } else {
//         modeIx = e->KeywordIx( "NONEXCLUSIVE");
//         if(e->KeywordSet(modeIx)) {
//             mode = GDLWidgetBGroup::NONEXCLUSIVE;
//         }
//     }
// 
//     GDLWidgetBGroup::BGroupReturn ret = GDLWidgetBGroup::RETURN_ID;
//     static int retIx = e->KeywordIx( "RETURN_INDEX");
//     if(e->KeywordSet( retIx)) {
//         ret = GDLWidgetBGroup::RETURN_INDEX;
//     } else {
//         retIx = e->KeywordIx( "RETURN_NAME");
//         if(e->KeywordSet(retIx)) {
//             ret = GDLWidgetBGroup::RETURN_NAME;
//         }
//     }
// 
//     DLong rows = -1;
//     static int rowIx = e->KeywordIx("ROW");
//     e->AssureLongScalarKWIfPresent( rowIx, rows);
// 
//     DLong cols = -1;
//     static int colIx = e->KeywordIx("COLUMN");
//     e->AssureLongScalarKWIfPresent( colIx, cols);
// 
//     GDLWidgetBGroup* group = new GDLWidgetBGroup( parentID, names,
//             uvalue, buttonuvalue,
//             xsize, ysize, labeltop,
//             rows, cols, mode, ret);
//     group->SetWidgetType("GROUP");
// 
//     return new DLongGDL( group->WidgetID());
// 
// #endif
// }
// 



// WIDGET_LIST

BaseGDL* widget_list( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL ) value = value->Dup( ); else value = new DStringGDL(""); //protect!

  static int multipleIx = e->KeywordIx( "MULTIPLE" );
  bool multiple = e->KeywordSet( multipleIx );
  
  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;
  
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool contextevents = e->KeywordSet( CONTEXT_EVENTS );
  if (contextevents) eventFlags |= GDLWidget::EV_CONTEXT;

  DLong style = multiple ? wxLB_EXTENDED : wxLB_SINGLE;  //|wxLB_MULTIPLE crash in windows.
  GDLWidgetList* list = new GDLWidgetList( parentID, e, value, style, eventFlags );
  if (list->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   list->SetWidgetType( GDLWidget::WIDGET_LIST );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_LIST "+i2s(list->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( list->GetWidgetID( ) );
#endif
}

// WIDGET_DROPLIST
  BaseGDL* widget_droplist( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int titleIx = e->KeywordIx( "TITLE" );
  DString title = "";
  e->AssureStringScalarKWIfPresent( titleIx, title );

  static int dynamicResizeIx = e->KeywordIx( "DYNAMIC_RESIZE" );
  bool dynres = e->KeywordSet( dynamicResizeIx );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL )  value = value->Dup( ); else value = new DStringGDL(""); //protect!

  DLong style = 0;
  GDLWidgetDropList* droplist = new GDLWidgetDropList( parentID, e, value, eventFlags, title, style);
  if (droplist->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   droplist->SetWidgetType( GDLWidget::WIDGET_DROPLIST );
  if (dynres) droplist->authorizeDynamicResize();
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_DROPLIST "+i2s(droplist->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( droplist->GetWidgetID( ) );
#endif
}


// WIDGET_COMBOBOX

BaseGDL* widget_combobox( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* value = e->GetKW( valueIx );
  if ( value != NULL ) value = value->Dup( );
  else value=new DStringGDL("");
  
  static int editableIx = e->KeywordIx( "EDITABLE" );
  bool editable = e->KeywordSet( editableIx );
  static int dynamicResizeIx = e->KeywordIx( "DYNAMIC_RESIZE" );
  bool dynres = e->KeywordSet( dynamicResizeIx );
 
  //common for all widgets. Only that for combobox.
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;


  DLong style = wxCB_SIMPLE;
  if ( !editable ) style |= wxCB_READONLY; else style |= wxTE_PROCESS_ENTER;
  GDLWidgetComboBox* combobox = new GDLWidgetComboBox( parentID, e, value, eventFlags, style );
  if (combobox->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   combobox->SetWidgetType( GDLWidget::WIDGET_COMBOBOX );
  if (dynres) combobox->authorizeDynamicResize();
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_COMBOBOX "+i2s(combobox->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( combobox->GetWidgetID( ) );
#endif
}

 
  BaseGDL* widget_tab( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  DLong multiline = 0;
  static int multilineIx = e->KeywordIx( "MULTILINE" );
  e->AssureLongScalarKWIfPresent( multilineIx, multiline );

  DLong location = 0;
  static int locationIx = e->KeywordIx( "LOCATION" );
  e->AssureLongScalarKWIfPresent( locationIx, location );

  static int scrollIx = e->KeywordIx( "SCROLL" );
  if (e->KeywordPresent(scrollIx)) e->Throw("Keyword SCROLL not allowed in call to: WIDGET_TAB");

  GDLWidgetTab* tab = new GDLWidgetTab( parentID, e, eventFlags, location, multiline );
  
  if (tab->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   tab->SetWidgetType( GDLWidget::WIDGET_TAB );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_TAB "+i2s(tab->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( tab->GetWidgetID( ) );
#endif
}

BaseGDL* widget_slider( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  DLong minimum = 0;
  static int minimumIx = e->KeywordIx( "MINIMUM" );
  e->AssureLongScalarKWIfPresent( minimumIx, minimum );
  DLong maximum = 100;
  static int maximumIx = e->KeywordIx( "MAXIMUM" );
  e->AssureLongScalarKWIfPresent( maximumIx, maximum );

  DLong value = minimum;
  static int valueIx = e->KeywordIx( "VALUE" );
  e->AssureLongScalarKWIfPresent( valueIx, value );

  static int dragIx = e->KeywordIx( "DRAG" );
  if (e->KeywordSet( dragIx )) eventFlags |= GDLWidget::EV_DRAG;
  

  static int verticalIx = e->KeywordIx( "VERTICAL" );
  bool vertical = e->KeywordSet( verticalIx );

  static int suppressValueIx = e->KeywordIx( "SUPPRESS_VALUE" );
  bool suppressValue = e->KeywordSet( suppressValueIx );

  DString title;
  static int titleIx = e->KeywordIx( "TITLE" );
  e->AssureStringScalarKWIfPresent( titleIx, title );

  GDLWidgetSlider* sl = new GDLWidgetSlider( parentID, e, value, eventFlags,
  minimum, maximum,
  vertical,
  suppressValue,
  title
  );
  if (sl->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   sl->SetWidgetType( GDLWidget::WIDGET_SLIDER );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_SLIDER "+i2s(sl->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( sl->GetWidgetID( ) );
#endif
}

// WIDGET_TEXT
  BaseGDL* widget_text( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );
  
  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  static int ALL_EVENTS = e->KeywordIx( "ALL_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  bool allevents = e->KeywordSet( ALL_EVENTS );
  bool kbrdfocusevents = e->KeywordSet( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordSet( CONTEXT_EVENTS );

  if (allevents)  eventFlags |= GDLWidget::EV_ALL;
  if (kbrdfocusevents) eventFlags |= GDLWidget::EV_KBRD_FOCUS;
  if (contextevents) eventFlags |= GDLWidget::EV_CONTEXT;

  static int noNewLineIx = e->KeywordIx( "NO_NEWLINE" );
  bool noNewLine = e->KeywordSet( noNewLineIx );

  DStringGDL* valueStr = NULL;
  static int valueIx = e->KeywordIx( "VALUE" );
  BaseGDL* valueKW = e->GetKW( valueIx );
  if ( valueKW != NULL ) {
    if ( valueKW->Type( ) != GDL_STRING )
      e->Throw( "VALUE must be a STRING." );
    valueStr = static_cast<DStringGDL*> (valueKW);
    bool success = e->StealLocalKW( valueIx );
    if ( !success )
      valueStr = valueStr->Dup( );
  } else valueStr=new DStringGDL(""); //important to init to a zero-length string!!!

  DLong edit = 0;
  static int editableIx = e->KeywordIx( "EDITABLE" );
  e->AssureLongScalarKWIfPresent( editableIx, edit );
  bool editable = (edit == 1);
  
  GDLWidgetText* text = new GDLWidgetText( parentID, e, valueStr, eventFlags, noNewLine, editable);
  if (text->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   text->SetWidgetType( GDLWidget::WIDGET_TEXT );
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_TEXT "+i2s(text->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( text->GetWidgetID( ) );
#endif
}


// WIDGET_LABEL
  BaseGDL* widget_label( EnvT* e)
  {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL; // avoid warning
#else
  SizeT nParam = e->NParam( 1 );

  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
  WidgetIDT parentID = (*p0L)[0];
  GDLWidget *parent = GDLWidget::GetWidget( parentID );
  if (parent==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
  if (!parent->IsBase()) e->Throw( "Parent is of incorrect type." );
  if (parent->IsContextBase()) e->Throw( "Parent is of incorrect type." );

  if (parent->GetExclusiveMode() != GDLWidget::BGNORMAL ) e->Throw( "Parent is of incorrect type." );

  static int valueIx = e->KeywordIx( "VALUE" );
  DString value = "label"; //tested default!
  e->AssureStringScalarKWIfPresent( valueIx, value );
  
  static int sunkenIx = e->KeywordIx( "SUNKEN_FRAME" );
  bool isSunken=e->KeywordSet(sunkenIx);
  
  static int dynamicResizeIx = e->KeywordIx( "DYNAMIC_RESIZE" );
  bool dynres = e->KeywordSet( dynamicResizeIx );  

  //common for all widgets
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  bool trackingevents = e->KeywordSet( TRACKING_EVENTS );
  DULong eventFlags=0;
  if (trackingevents)  eventFlags |= GDLWidget::EV_TRACKING;

  GDLWidgetLabel* label = new GDLWidgetLabel( parentID, e, value , eventFlags, isSunken);
  if (label->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   label->SetWidgetType( GDLWidget::WIDGET_LABEL );
  if (dynres) label->authorizeDynamicResize();
#ifdef GDL_DEBUG_WIDGETS
  cerr<<"WIDGET_LABEL "+i2s(label->GetWidgetID( ))+" OK.\n";
#endif
  return new DLongGDL( label->GetWidgetID( ) );
#endif
}


// WIDGET_INFO
//TBD: % WIDGET_INFO: Keyword parameter PARENT not allowed in call to: WIDGET_INFO
BaseGDL* widget_info( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
  return NULL;
#else
  SizeT nParam = e->NParam( );

  DLongGDL* p0L = NULL;
  SizeT nEl = 0;
  SizeT rank = 0;
  if ( nParam > 1 ) {
    e->Throw("Incorrect number of arguments.");
  }
  static int TAB_MODE = e->KeywordIx( "TAB_MODE" );
  if (e->KeywordSet(TAB_MODE)) return new DLongGDL(1); //pretend that tab_mode works
  static int UPDATE = e->KeywordIx( "UPDATE" );
  if (e->KeywordSet(UPDATE)) return new DLongGDL(1); //pretend that update works always (fixme: yet another property to add, get,set to GDLWidget::)

  static int activeIx = e->KeywordIx( "ACTIVE" );
  bool active = e->KeywordSet( activeIx );
  static int sensIx = e->KeywordIx( "SENSITIVE" );
  bool sens = e->KeywordSet( sensIx );
  static int debugIx = e->KeywordIx( "DEBUG" );
  bool debug = e->KeywordSet( debugIx );
  
  static int validIx = e->KeywordIx( "VALID_ID" );
  bool valid = e->KeywordSet( validIx );

  static int modalIx = e->KeywordIx( "MODAL" );
  bool modal = e->KeywordSet( modalIx );

  static int mapIx = e->KeywordIx( "MAP" );
  bool is_mapped=e->KeywordSet( mapIx );
  static int displayIx = e->KeywordIx( "DISPLAY" );
  bool isdisplayed=e->KeywordSet( displayIx );
  static int managedIx = e->KeywordIx( "MANAGED" );
  bool managed = e->KeywordSet( managedIx );

  static int realizedIx = e->KeywordIx( "REALIZED" );
  bool realized = e->KeywordSet( realizedIx );

  static int xmanagerBlockIx = e->KeywordIx( "XMANAGER_BLOCK" );
  bool xmanagerBlock = e->KeywordSet( xmanagerBlockIx );

  static int childIx = e->KeywordIx( "CHILD" );
  bool child = e->KeywordSet( childIx );
  
  static int nchildIx = e->KeywordIx( "N_CHILDREN" );
  bool nchildren = e->KeywordSet( nchildIx );
  static int allchildIx = e->KeywordIx( "ALL_CHILDREN" );
  bool allchildren = e->KeywordSet( allchildIx );

  static int versionIx = e->KeywordIx( "VERSION" );
  bool version = e->KeywordSet( versionIx );
  
  static int geometryIx = e->KeywordIx( "GEOMETRY" );
  bool geometry = e->KeywordSet( geometryIx );
  
  static int nameIx = e->KeywordIx( "NAME");
  bool name = e->KeywordSet( nameIx );
  static int typeIx = e->KeywordIx( "TYPE");
  bool type = e->KeywordSet( typeIx );

  static int eventproIx = e->KeywordIx( "EVENT_PRO" );
  bool eventpro = e->KeywordSet( eventproIx );

  static int eventfunIx = e->KeywordIx( "EVENT_FUNC" );
  bool eventfun = e->KeywordSet( eventfunIx );

  static int unameIx = e->KeywordIx( "UNAME");
  bool uname = e->KeywordSet( unameIx );

  static int findbyunameIx = e->KeywordIx( "FIND_BY_UNAME");
  bool findbyuname = e->KeywordPresent( findbyunameIx );

  static int fontnameIx = e->KeywordIx( "FONTNAME");
  bool fontname = e->KeywordSet( fontnameIx );

  static int buttonsetIx = e->KeywordIx( "BUTTON_SET");
  bool buttonset = e->KeywordSet( buttonsetIx );
  
  static int parentIx = e->KeywordIx( "PARENT");
  bool parent = e->KeywordSet( parentIx );
  
  static int textselectIx = e->KeywordIx( "TEXT_SELECT");
  bool textselect = e->KeywordPresent( textselectIx );
  
  static int tabledisjointselectionIx = e->KeywordIx( "TABLE_DISJOINT_SELECTION"); 
  bool tabledisjointselection = e->KeywordSet( tabledisjointselectionIx );

  static int tableselectIx = e->KeywordIx( "TABLE_SELECT");
  bool tableselect = e->KeywordSet( tableselectIx );

  static int USE_TABLE_SELECT = e->KeywordIx( "USE_TABLE_SELECT");

  static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
  bool giveColumnWidths = e->KeywordSet(COLUMN_WIDTHS);
  static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
  bool giveRowHeights = e->KeywordSet(ROW_HEIGHTS);
     
  static int SYSTEM_COLORS = e->KeywordIx( "SYSTEM_COLORS" );
  bool giveSystemColors = e->KeywordSet(SYSTEM_COLORS);

  static int TREE_SELECT = e->KeywordIx( "TREE_SELECT");
  bool treeselect = e->KeywordSet(TREE_SELECT);
  static int TREE_CHECKBOX = e->KeywordIx( "TREE_CHECKBOX");
  bool treecheckbox = e->KeywordSet(TREE_CHECKBOX);
  static int TREE_CHECKED = e->KeywordIx( "TREE_CHECKED");
  bool treechecked = e->KeywordSet(TREE_CHECKED);
  static int TREE_DRAG_SELECT = e->KeywordIx( "TREE_DRAG_SELECT");
  bool treedragselect = e->KeywordSet(TREE_DRAG_SELECT);
  static int TREE_INDEX = e->KeywordIx( "TREE_INDEX");
  bool treeindex = e->KeywordSet(TREE_INDEX);
  static int TREE_FOLDER = e->KeywordIx( "TREE_FOLDER");
  bool treefolder = e->KeywordSet(TREE_FOLDER);
  static int TREE_EXPANDED = e->KeywordIx( "TREE_EXPANDED");
  bool treeexpanded = e->KeywordSet(TREE_EXPANDED);
  static int TREE_ROOT = e->KeywordIx( "TREE_ROOT" );
  bool treeroot = e->KeywordSet(TREE_ROOT);
  static int TREE_BITMAP = e->KeywordIx( "TREE_BITMAP");
  bool treebitmap = e->KeywordSet(TREE_BITMAP);
  static int MASK = e->KeywordIx( "MASK");
  bool treemask = e->KeywordSet(MASK);
  static int DRAGGABLE = e->KeywordIx( "DRAGGABLE");
  bool draggable = e->KeywordSet(DRAGGABLE);
  static int DRAG_NOTIFY = e->KeywordIx( "DRAG_NOTIFY");
  bool dragnotify = e->KeywordSet(DRAG_NOTIFY);
  static int DROP_EVENTS = e->KeywordIx( "DROP_EVENTS");
  bool dropevents = e->KeywordSet(DROP_EVENTS);
  static int TOOLTIP = e->KeywordIx( "TOOLTIP");
  bool tooltip = e->KeywordSet(TOOLTIP);

  static int DRAW_BUTTON_EVENTS   = e->KeywordIx( "DRAW_BUTTON_EVENTS" );
  static int DRAW_EXPOSE_EVENTS   = e->KeywordIx( "DRAW_EXPOSE_EVENTS" );
  static int DRAW_KEYBOARD_EVENTS = e->KeywordIx( "DRAW_KEYBOARD_EVENTS" );
  static int DRAW_MOTION_EVENTS   = e->KeywordIx( "DRAW_MOTION_EVENTS" );
  static int DRAW_VIEWPORT_EVENTS = e->KeywordIx( "DRAW_VIEWPORT_EVENTS" );
  static int DRAW_WHEEL_EVENTS    = e->KeywordIx( "DRAW_WHEEL_EVENTS" );
  bool draw_button_events   = e->KeywordPresent(DRAW_BUTTON_EVENTS);
  bool draw_expose_events   = e->KeywordPresent(DRAW_EXPOSE_EVENTS);
  bool draw_keyboard_events = e->KeywordPresent(DRAW_KEYBOARD_EVENTS);
  bool draw_motion_events   = e->KeywordPresent(DRAW_MOTION_EVENTS);
  bool draw_wheel_events    = e->KeywordPresent(DRAW_WHEEL_EVENTS);
  bool draw_viewport_events = e->KeywordPresent(DRAW_VIEWPORT_EVENTS);
  bool drop_events          = e->KeywordPresent(DROP_EVENTS);
    
  static int LIST_SELECT = e->KeywordIx( "LIST_SELECT");
  bool listselect = e->KeywordSet(LIST_SELECT);

  static int DROPLIST_SELECT = e->KeywordIx( "DROPLIST_SELECT");
  bool droplistselect = e->KeywordSet(DROPLIST_SELECT);

  static int COMBOBOX_GETTEXT = e->KeywordIx( "COMBOBOX_GETTEXT");
  bool comboboxgettext = e->KeywordSet(COMBOBOX_GETTEXT);
  
  static int TAB_NUMBER = e->KeywordIx( "TAB_NUMBER");
  bool tabnumber = e->KeywordSet(TAB_NUMBER);
  static int TAB_CURRENT = e->KeywordIx( "TAB_CURRENT");
  bool tabcurrent = e->KeywordSet(TAB_CURRENT);
  static int TAB_MULTILINE = e->KeywordIx( "TAB_MULTILINE");
  bool tabmultiline = e->KeywordSet(TAB_MULTILINE);

  static int unitsIx = e->KeywordIx( "UNITS" );
  bool unitsGiven = e->KeywordPresent ( unitsIx );
  

  static int tlb_iconify_eventsIx = e->KeywordIx( "TLB_ICONIFY_EVENTS" );
  bool tlb_iconify_events=e->KeywordSet(tlb_iconify_eventsIx);
  static int tlb_kill_request_eventsIx = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS" );
  bool tlb_kill_request_events=e->KeywordSet(tlb_kill_request_eventsIx);
  static int tlb_move_eventsIx = e->KeywordIx( "TLB_MOVE_EVENTS" );
  bool tlb_move_events=e->KeywordSet(tlb_move_eventsIx);
  static int tlb_size_eventsIx = e->KeywordIx( "TLB_SIZE_EVENTS" );
  bool tlb_size_events=e->KeywordSet(tlb_size_eventsIx);

  
  static int STRING_SIZE=e->KeywordIx("STRING_SIZE"); bool getStringSize=e->KeywordPresent(STRING_SIZE);
  static int SIBLING=e->KeywordIx("SIBLING"); bool sibling=e->KeywordPresent(SIBLING);

  //find a string, return a long
  if (findbyuname) {
    DStringGDL* myUname = e->GetKWAs<DStringGDL>(findbyunameIx);
    if (myUname == NULL) return new DLongGDL( 0 );
    DLongGDL* list = static_cast<DLongGDL*>( GDLWidget::GetWidgetsList( ) );
    Guard<BaseGDL> guard_list(list);
    for (SizeT i=0; i< list->N_Elements(); ++i) {
      GDLWidget* widget = GDLWidget::GetWidget( (*list)[i] );
      if ( widget != NULL ){
        if ((*myUname)[0] == widget->GetUname() ) return new DLongGDL(widget->GetWidgetID());
      }
    }
    return new DLongGDL( 0 );
  }
  
  if ( nParam > 0 ) {
    p0L = e->GetParAs<DLongGDL>(0);
    nEl = p0L->N_Elements( );
    rank = p0L->Rank( );
  } else {
  //only possible with ACTIVE, VERSION or MANAGED.
    if (!(active || managed || version  || xmanagerBlock || isdisplayed || debug ) ) e->Throw("Specified keyword requires ID argument.");
  // special case of MANAGED without any widget number
    if ( managed ) {
      return GDLWidget::GetManagedWidgetsList( );
    }
  }
  
  // XMANAGER_BLOCK keyword - NOTE: returns 1 if a (managed) Top Widget BLOCKS
  if ( xmanagerBlock ) {
    return new DLongGDL( GDLWidget::IsXmanagerBlocking( ) );
  }
  // End /XMANAGER_BLOCK

  if (active) {
    return new DLongGDL( GDLWidget::IsActive( ) );
  }
  
  if (isdisplayed) {
//the code below has problems on MacOSX , better to just answer "YES" :
    return new DLongGDL(1);
//#if wxCHECK_VERSION(3,1,7)
//#ifdef __WXMSW__
//    return new DLongGDL(1);
//#else
//    wxDisplay w;
//    if (w.GetCount() > 0) return new DLongGDL(1); else return new DLongGDL(0);
//#endif
//#else
//    return new DLongGDL(1);
//#endif
  }
  
  if (is_mapped) {
    //must return 1 if the widget is visible, which is normally because the grand parent is mapped.
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        if (widget->GetMyTopLevelBaseWidget()->GetMap()) return new DLongGDL(1); else return new DLongGDL(0);
      }
    } else {
      // Array Input
      DLongGDL* res = new DLongGDL( p0L->Dim( ) );
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        if ( widget != NULL ) {
          atLeastOneFound=true;
          if (widget->GetMyTopLevelBaseWidget()->GetMap()) (*res)[i] = 1;
        }
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }
  
  //debug is used for the moment to list all windows hierarchy for debug purposes.
  if (debug) {
      DLongGDL* wid = static_cast<DLongGDL*>( GDLWidget::GetWidgetsList( ) );
      Guard<BaseGDL> guard_wid(wid);
      DStringGDL* res = new DStringGDL(wid->N_Elements());
      for ( SizeT i = 0; i < wid->N_Elements(); i++ ) {
        GDLWidget *widget = GDLWidget::GetWidget( (*wid)[i] );
        //protect against internal error where wid is not up-to-date
        wxObject* wxWidgetAddr=NULL;
        DString wxWidgetName="Zombie";
        if (widget) {
          wxWidgetAddr=widget->GetWxWidget();
          wxWidgetName=widget->GetWidgetName();
        }
        std::stringstream os;
        os<<(*wid)[i]<<"("<<wxWidgetName<<"@"<< std::hex << wxWidgetAddr << "), "; 
        (*res)[i]=os.str();
      }
      return res;
  }
  
  // Returns a String, empty if no result:
  // UNAME, FONTNAME keywords
  if ( uname || fontname || name ||eventpro || eventfun) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        if (uname) return new DStringGDL(widget->GetUname());
        else if (name) return new DStringGDL(widget->GetWidgetName());
        else if (eventfun) return new DStringGDL(widget->GetEventFun());
        else if (eventpro) return new DStringGDL(widget->GetEventPro());
        else if (fontname) { wxWindow* ww=dynamic_cast<wxWindow*>(widget->GetWxWidget()); 
          if (ww) return new DStringGDL( std::string(ww->GetFont().GetNativeFontInfoUserDesc().mb_str()) );}
      }
    } else {
      // Array Input
      DStringGDL* res = new DStringGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        DString result = "";
        if ( widget != NULL ) {
          atLeastOneFound=true;
          if (uname) result = widget->GetUname();
          else if (name) result = widget->GetWidgetName();
          else if (eventfun) result = widget->GetEventFun();
          else if (eventpro) result = widget->GetEventPro();
          else if (fontname)  { wxWindow* ww=dynamic_cast<wxWindow*>(widget->GetWxWidget()); 
          if (ww) result = std::string(ww->GetFont().GetNativeFontInfoUserDesc().mb_str());}
        }
        (*res)[i] = result;
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }

  // returns a long where 0 is "no info"
  // PARENT, CHILD keyword
  if ( child || parent || type || nchildren || sens || sibling ) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        DLong result = 0;
        if (parent) result = widget->GetParentID(); //but parent is always defined...
        else if (type) result = widget->GetWidgetType();
        else if (sens) result = widget->GetSensitive();
        else if (sibling) result = widget->GetSibling();
        else { // child || nchildren
          DLong nchild = widget->NChildren();
          if (nchildren) {
            result = nchild;
          } else if (child) {
            if (nchild > 0) result = widget->GetChild(0);
            else result = 0;
          }
        }
        return new DLongGDL(result);
      }
    } else {
      // Array Input
      DLongGDL* res = new DLongGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        if ( widget == NULL ) {
          (*res)[ i] = (DLong) 0;
        } else {
          atLeastOneFound=true;
          DLong result=0;
          if (parent)  result = widget->GetParentID( ); //but parent is always defined...
          else if (type)  result = widget->GetWidgetType( );
          else if (sens)  result = widget->GetSensitive( ); 
          else if (sibling)  result = widget->GetSibling( );
          else { // child || nchildren
            DLong nchild = widget->NChildren();
            if (nchildren) {
              result = nchild;
            } else if (child) {
              if (nchild > 0) result = widget->GetChild(0);
              else result = 0;
            }
          }
          ( *res )[ i] = result;
        }
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }

    if (getStringSize) {
      DStringGDL* gs = e->GetKWAs<DStringGDL>(STRING_SIZE);
      if (gs->N_Elements()> 2) e->Throw("Keyword array parameter STRING_SIZE must have from 1 to 2 elements.");
      DString s=(*gs)[0];
      DString font("");
      if (gs->N_Elements()>1) font=((*gs)[1]);
      if (rank == 0) {
        // Scalar Input
        WidgetIDT widgetID = (*p0L)[0];
        GDLWidget *widget = GDLWidget::GetWidget(widgetID);
        if (widget == NULL) {
          e->Throw("Invalid widget identifier:" + i2s(widgetID));
        } else {
          DLongGDL* res = new DLongGDL(dimension(2));
          wxFont myFont=widget->getFont();
          if (font.size() > 0) {
            bool ok = myFont.SetNativeFontInfoUserDesc(wxString(font.c_str(), wxConvLibc));
            if (!ok) myFont=widget->getFont(); //will not provide the same result as IDL as IDL return [0,0] when font is not known.
          }
          wxSize val=widget->calculateTextScreenSize(s,myFont);
          (*res)[0]=val.x;
          (*res)[1]=val.y;
          return res;
        }
      } 
        else {    // Array Input
        for (SizeT i = 0; i < nEl; i++) { //this is IDL behaviour.
          WidgetIDT widgetID = (*p0L)[i];
          GDLWidget *widget = GDLWidget::GetWidget(widgetID);
          if (widget == NULL) e->Throw("Invalid widget identifier:" + i2s(widgetID));
        }
        DLongGDL* res = new DLongGDL(dimension(2,p0L->N_Elements()), BaseGDL::NOZERO);
        for (SizeT i = 0, k=0; i < nEl; i++) {
          WidgetIDT widgetID = (*p0L)[i];
          GDLWidget *widget = GDLWidget::GetWidget(widgetID);
          wxFont myFont=widget->getFont();
          if (font.size() > 0) {
            bool ok = myFont.SetNativeFontInfoUserDesc(wxString(font.c_str(), wxConvLibc));
            if (!ok) myFont=widget->getFont(); //will not provide the same result as IDL as IDL return [0,0] when font is not known.
          }
          wxSize val=widget->calculateTextScreenSize(s,myFont);
          (*res)[k++]=val.x;
          (*res)[k++]=val.y;
        }
        return res;
      }
    }

  
  if ( allchildren) {
      // Scalar Input only
    if (!p0L->Scalar()) e->Throw("Expression must be an array in this context: " + e->GetString(allchildIx));
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) {
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } else {
        return widget->GetChildrenList();
      }
  }

  //SYSTEM_COLORS
  if (giveSystemColors){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL )
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      else return GDLWidget::getSystemColours();
  }
  // GEOMETRY keyword
  if ( geometry ) {
    wxRealPoint fact = wxRealPoint(1.,1.);
    if ( unitsGiven ) fact = GetRequestedUnitConversionFactor( e );
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL )
        e->Throw("Invalid widget identifier:"+i2s(widgetID));
      else return widget->GetGeometry( fact );
    } else {
      // Array Input
      DStructDesc* dWidgeomDesc = FindInStructList( structList, "WIDGET_GEOMETRY");
      DStructGDL* ex = new DStructGDL(dWidgeomDesc, p0L->Dim( ), BaseGDL::NOZERO );
      ex->Clear();
//      static unsigned tag1=ex->Desc()->TagIndex("XOFFSET");
//      static unsigned tag2=ex->Desc()->TagIndex("YOFFSET");
//      static unsigned tag3=ex->Desc()->TagIndex("XSIZE");
//      static unsigned tag4=ex->Desc()->TagIndex("YSIZE");
//      static unsigned tag5=ex->Desc()->TagIndex("SCR_XSIZE");
//      static unsigned tag6=ex->Desc()->TagIndex("SCR_YSIZE");
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );
        if ( widget != NULL ) {
          atLeastOneFound=true;
          DStructGDL* ret=widget->GetGeometry( fact );
          for (SizeT itag=0; itag<ret->Desc()->NTags(); ++itag) (*static_cast<DFloatGDL*>(ex->GetTag(itag, i)))[0]=(*static_cast<DFloatGDL*>(ret->GetTag(itag, 0)))[0];
        }
      }
      if (atLeastOneFound) return ex; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }  // End /GEOMETRY

  // MODAL keyword (stub)
  if ( modal ) {
    WidgetIDT widgetID = (*p0L)[0];
    GDLWidget *widget = GDLWidget::GetWidget( widgetID );
    if ( widget != NULL) return new DLongGDL(widget->IsModal());
    return new DLongGDL( 0 );
  }
  // End /MODAL

  // VALID , MANAGED, BUTTONSET etc keywords giving back 0 or 1
  if ( valid || managed || realized || buttonset || tlb_size_events ||
       tlb_iconify_events || tlb_kill_request_events || tlb_move_events || draw_button_events  
|| draw_expose_events  || draw_keyboard_events|| draw_motion_events  || draw_wheel_events  || draw_viewport_events
|| drop_events         ) {
    if ( rank == 0 ) {
      // Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );

      // Check if valid widgetID else exit with 0
      if ( widget == NULL) {
	      if (valid || managed) return new DLongGDL( 0 ); 
	      else e->Throw("Invalid widget identifier:"+i2s(widgetID));
      } //note: /display is not currently in gdl      
      bool result=false;
      if (valid) result=( widget != NULL );
      else if (managed) result=( widget->GetManaged( ) == true );
      else if (realized) result=( widget->IsRealized( ) == true );
      else if (buttonset) result=( widget->GetButtonSet() == true );
        else { //tlb only for base widget
          if (widget->IsBase()) {
                 if (tlb_size_events)    result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE);
            else if (tlb_iconify_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_ICONIFY) == GDLWidget::EV_ICONIFY);
            else if (tlb_kill_request_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KILL) == GDLWidget::EV_KILL);
            else if (tlb_move_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_MOVE) == GDLWidget::EV_MOVE);
          }
          if (widget->IsDraw()) {
            if (draw_button_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_BUTTON) == GDLWidget::EV_BUTTON);
            if (draw_expose_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_EXPOSE) == GDLWidget::EV_EXPOSE);
            if (draw_keyboard_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KEYBOARD) == GDLWidget::EV_KEYBOARD) ? 1 :
              ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KEYBOARD2) == GDLWidget::EV_KEYBOARD2) ? 2 : 0;
            if (draw_wheel_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_WHEEL) == GDLWidget::EV_WHEEL);
            if (draw_viewport_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_VIEWPORT) == GDLWidget::EV_VIEWPORT);
            if (drop_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_DROP) == GDLWidget::EV_DROP);
          }
        }
      return new DLongGDL( result); 
    } else {
      // Array Input
      DLongGDL* res = new DLongGDL( p0L->Dim( ), BaseGDL::NOZERO );
      bool atLeastOneFound=false;
      for ( SizeT i = 0; i < nEl; i++ ) {
        WidgetIDT widgetID = (*p0L)[i];
        GDLWidget *widget = GDLWidget::GetWidget( widgetID );

        if ( widget == NULL ) ( *res )[ i] = (DLong) 0;
        else {
          atLeastOneFound=true;
          bool result=false;
          if (valid) result=( widget != NULL );
          else if (managed) result=( widget->GetManaged( ) == true );
          else if (realized) result=( widget->IsRealized( ) == true );
          else if (buttonset) result=( widget->GetButtonSet( ) == true );
          else { //tlb only for base widget
            if (widget->IsBase()) {
              if (tlb_size_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE);
              else if (tlb_iconify_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_ICONIFY) == GDLWidget::EV_ICONIFY);
              else if (tlb_kill_request_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KILL) == GDLWidget::EV_KILL);
              else if (tlb_move_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_MOVE) == GDLWidget::EV_MOVE);
              }
              if (widget->IsDraw()) {
                if (draw_button_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_BUTTON) == GDLWidget::EV_BUTTON);
                if (draw_expose_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_EXPOSE) == GDLWidget::EV_EXPOSE);
                if (draw_keyboard_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KEYBOARD) == GDLWidget::EV_KEYBOARD) ? 1 :
                  ((widget->GetEventFlags() & (DULong) GDLWidget::EV_KEYBOARD2) == GDLWidget::EV_KEYBOARD2) ? 2 : 0;
                if (draw_wheel_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_WHEEL) == GDLWidget::EV_WHEEL);
                if (draw_viewport_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_VIEWPORT) == GDLWidget::EV_VIEWPORT);
                if (drop_events) result = ((widget->GetEventFlags() & (DULong) GDLWidget::EV_DROP) == GDLWidget::EV_DROP);
              }
            }
          ( *res )[ i] = result;
        }
      }
      if (atLeastOneFound) return res; else e->Throw("Invalid widget identifier:"+i2s((*p0L)[0]));
    }
  }
  // End VALID , MANAGED, BUTTONSET

  if (textselect) {
      // Always Scalar Input
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      if ( widget->IsText()) return static_cast<GDLWidgetText*>(widget)->GetTextSelection();
      if ( widget->IsTable()) std::cerr<<"sorry, TEXT_SELECT not ready for WIDGET_TABLE, Fixme!";
      //other cases return [0,0]
      DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
      return pos;
  }
  
  if (tableselect||tabledisjointselection||giveColumnWidths||giveRowHeights)
  {
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsTable() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetTable *table = (GDLWidgetTable *) widget;
      
      if (tableselect) return table->GetSelection();
      if (tabledisjointselection)  return new DLongGDL( table->GetDisjointSelection()?1:0 );
      
      DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);
      bool useATableSelection = table->GetValidTableSelection(tableSelectionToUse); //will throw if a syntax problem
      if (giveColumnWidths && unitsGiven ){
        wxRealPoint fact=GetRequestedUnitConversionFactor(e);
        DFloatGDL* ret;
        if (useATableSelection) ret=table->GetColumnWidth(tableSelectionToUse); else ret=table->GetColumnWidth();
        for (SizeT i=0; i< ret->N_Elements(); ++i) (*ret)[i]/=fact.x;
        return ret;
      } else if (giveColumnWidths ){
        if (useATableSelection) return table->GetColumnWidth(tableSelectionToUse); else return table->GetColumnWidth();
      }
      if (giveRowHeights && unitsGiven){
        wxRealPoint fact=GetRequestedUnitConversionFactor(e);
        DFloatGDL* ret;
        if (useATableSelection) ret=table->GetRowHeight(tableSelectionToUse); else ret=table->GetRowHeight();
        for (SizeT i=0; i< ret->N_Elements(); ++i) (*ret)[i]/=fact.y;
        return ret;
      }else if (giveRowHeights){
        if (useATableSelection) return table->GetRowHeight(tableSelectionToUse); else return table->GetRowHeight();
      }
  }
  
  if (tooltip) { // WIDGET_BUTTON, WIDGET_DRAW, WIDGET_TREE, WIDGET_WINDOW. TBD.
    std::cerr << "FIXME!" << std::endl;
      if (rank == 0) return new DStringGDL();
      else return new DStringGDL(p0L->Dim());
  }
    if (treeroot || treeselect || treedragselect || treefolder || treeexpanded || treeindex || treebitmap
       || treemask || draggable || dragnotify || dropevents || treecheckbox || treechecked) {
      if (rank == 0) {
        // Scalar Input
        WidgetIDT widgetID = (*p0L)[0];
        GDLWidget *widget = GDLWidget::GetWidget(widgetID);
        if (widget == NULL ) e->Throw("Invalid widget identifier:" + i2s(widgetID));
        if (!widget->IsTree()) return new DLongGDL(0);
        GDLWidgetTree *thisTreeItem = (GDLWidgetTree *) widget;
        GDLWidgetTree *thisTreeRoot = thisTreeItem->GetMyRootGDLWidgetTree();
        if (treeselect || treedragselect) {
          if (thisTreeRoot == thisTreeItem) {
            if (treeselect) return thisTreeItem->GetAllSelectedID();
            else if (treedragselect) return thisTreeItem->GetAllDragSelectedID();
          } else {
            if (treeselect) return new DLongGDL(thisTreeItem->IsSelectedID());
            else if (treedragselect) return new DLongGDL(thisTreeItem->IsDragSelectedID());
          }
        } 
        else if (treeindex) return new DLongGDL(thisTreeItem->GetTreeIndex());
        else if (treefolder) return new DLongGDL(thisTreeItem->IsFolder());
        else if (treeexpanded) return new DLongGDL(thisTreeItem->IsExpanded());
        else if (treeroot) return new DLongGDL(thisTreeItem->GetMyRootGDLWidgetTree()->GetWidgetID());
        else if (treebitmap) return thisTreeItem->ReturnBitmapAsBytes();
        else if (treecheckbox) return new DLongGDL(thisTreeItem->HasCheckBox());
        else if (treechecked) return new DLongGDL(thisTreeItem->IsChecked());
        else if (treemask) return new DLongGDL(thisTreeItem->HasMask());
        else if (draggable) return new DLongGDL(thisTreeItem->GetDraggableValue());
        else if (dropevents) return new DLongGDL(thisTreeItem->GetDroppableValue());
        else if (dragnotify) return new DStringGDL(thisTreeItem->GetDragNotifyValue()); //other not implemented!
      } else {
        if (treebitmap) e->Throw("Expression must be a scalar in this context.");
        BaseGDL* myres;
        bool atLeastOneFound = false;
        // Array Input
        if (dragnotify) {
          DStringGDL* res = new DStringGDL(p0L->Dim());
          for (SizeT i = 0; i < nEl; i++) {
            WidgetIDT widgetID = (*p0L)[i];
            GDLWidget *widget = GDLWidget::GetWidget(widgetID);
            if (widget != NULL ) {
              atLeastOneFound = true;
              if (widget->IsTree()) {
              GDLWidgetTree *thisTreeItem = (GDLWidgetTree *) widget;
              (*res)[ i] = thisTreeItem->GetDragNotifyValue();
              } else (*res)[ i] = "<default>";
            } else {
              GDLDelete(res);
              e->Throw("Invalid widget identifier:" + i2s(widgetID));
            }
          }
          myres=res; //pointers
        } else {
          DLongGDL* res = new DLongGDL(p0L->Dim());
          for (SizeT i = 0; i < nEl; i++) {
            WidgetIDT widgetID = (*p0L)[i];
            GDLWidget *widget = GDLWidget::GetWidget(widgetID);
            if (widget != NULL) {
              atLeastOneFound = true;
              if (widget->IsTree()) {
                GDLWidgetTree *thisTreeItem = (GDLWidgetTree *) widget;
                if (treeselect) (*res)[ i] = thisTreeItem->IsSelectedID(); //IDL does silly things when ID list contains the root 
                else if (treedragselect) (*res)[ i] = thisTreeItem->IsDragSelectedID(); //IDL does silly things when ID list contains the root 
                else if (treeindex) (*res)[ i] = thisTreeItem->GetTreeIndex();
                else if (treefolder) (*res)[ i] = thisTreeItem->IsFolder();
                else if (treeexpanded) (*res)[ i] = thisTreeItem->IsExpanded();
                else if (treeroot) (*res)[ i] = thisTreeItem->GetMyRootGDLWidgetTree()->GetWidgetID();
                else if (treecheckbox) (*res)[ i] = thisTreeItem->HasCheckBox();
                else if (treechecked) (*res)[ i] = thisTreeItem->IsChecked();
                else if (treemask) (*res)[ i] = thisTreeItem->HasMask();
                else if (draggable) (*res)[ i] = thisTreeItem->GetDraggableValue();
                else if (dropevents) (*res)[ i] = thisTreeItem->GetDroppableValue();
              } else (*res)[ i] = 0;
            } else {
              GDLDelete(res);
              e->Throw("Invalid widget identifier:" + i2s(widgetID));
            }
          }
          myres=res; //pointers
        }
        if (atLeastOneFound) return myres;
        else e->Throw("Invalid widget identifier:" + i2s((*p0L)[0]));
      }
    }
  
  if (listselect){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsList() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetList *list = (GDLWidgetList *) widget;
      
      if (listselect) return list->GetSelectedEntries();
  }
  
  if (droplistselect){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsDropList() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetDropList *droplist = (GDLWidgetDropList *) widget;
      
      if (droplistselect) return droplist->GetSelectedEntry();
  }
  
  if (comboboxgettext){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsComboBox() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetComboBox *combo = (GDLWidgetComboBox *) widget;
      
      if (comboboxgettext) return combo->GetSelectedEntry();
  } 
    
  if (tabnumber||tabcurrent||tabmultiline){
      WidgetIDT widgetID = (*p0L)[0];
      GDLWidget *widget = GDLWidget::GetWidget( widgetID );
      if ( widget == NULL || !widget->IsTab() ) e->Throw("Invalid widget identifier:"+i2s(widgetID));
      GDLWidgetTab *tab = (GDLWidgetTab *) widget;
      
      if (tabnumber) return tab->GetTabNumber();
      if (tabcurrent) return tab->GetTabCurrent();
      if (tabmultiline) return tab->GetTabMultiline();
  } 
  // if code pointer arrives here, give WIDGET_VERSION:
  // if you get here and should not, you forgot to return the value you got...
  //it is as if /version was set.
  

  DStructGDL* res = new DStructGDL( "WIDGET_VERSION" );
  res->InitTag("STYLE",DStringGDL(std::string(wxPlatformInfo::Get().GetPortIdName().mb_str(wxConvUTF8))));
  res->InitTag("TOOLKIT",DStringGDL("wxWidgets"));
  std::ostringstream stringStream;
  stringStream << wxPlatformInfo::Get().GetToolkitMajorVersion() <<"."<<wxPlatformInfo::Get().GetToolkitMinorVersion();
  std::string release=stringStream.str();
  res->InitTag("RELEASE",DStringGDL(release)); 
  return res;
#endif
}


// WIDGET_EVENT
// WIDGET_EVENT is either 1) called with option /XMANAGER_BLOCK from XMANAGER.pro, in the case XMANAGER is BLOCKING one or more widgets.
// Or, it is called by the user, in which case it returns the event WITHOUT processing it.

  BaseGDL* widget_event(EnvT* e) {
    // 1) for a specific event, start from the originating widget and go through the list of parents, 
    // and process the first event-related procedure associated.
    // 2) If the event handling found is a PROCEDURE, do it, and go back looking for another event.
    // 3) If the event handling found is FUNCTION, use it and examine return:
    //  3a- if the return is NOT A STRUCTURE, discard it, and (as above) go back looking for an event.
    //  3b- if the return IS A STRUCTURE, check this structure is OK (3 fields ID, TOP, HANDLER) else issue an error. 
    //  3c- Otherwise, the return value replaces the initial event, and the process of looking for another event handling continues.
    // 4) If the top of the hierarchy is attained without being swallowed by an event handler, it is returned as the value of WIDGET_EVENT.
    // 5) Empty events are returned in any other case.

#ifndef HAVE_LIBWXWIDGETS
    e->Throw("GDL was compiled without support for wxWidgets");
    return NULL;
#else
    DStructGDL* defaultRes = new DStructGDL("WIDGET_NOEVENT");
    static int savehourglassIx = e->KeywordIx("SAVE_HOURGLASS");
    bool savehourglass = e->KeywordSet(savehourglassIx);
    // it is said in the doc: 1) that WIDGET_CONTROL,/HOURGLASS busyCursor ends at the first WIDGET_EVENT processed. 
    // And 2) that /SAVE_HOURGLASS exist to prevent just that, ending.
    if (!savehourglass) if (wxIsBusy()) wxEndBusyCursor();
    //xmanager_block (Hidden IDL option) is used to block until TLB is killed
    static int xmanagerBlockIx = e->KeywordIx("XMANAGER_BLOCK");
    bool blockedByXmanager = e->KeywordSet(xmanagerBlockIx);
    static int nowaitIx = e->KeywordIx("NOWAIT");
    bool nowait = e->KeywordSet(nowaitIx);
    static int badidIx = e->KeywordIx("BAD_ID");
    bool dobadid = e->KeywordPresent(badidIx);
    if (dobadid) e->AssureGlobalKW(badidIx);

    SizeT nParam = e->NParam();
    std::vector<WidgetIDT> widgetIDList;
    std::vector<bool> has_children;
    DLongGDL* p0L = NULL;
    SizeT nEl = 0;

    bool all = true;
    if (nParam > 0) { //specific widget, or array of widgets. Note that ALL THE CHILDREN of the widget are searched.
      p0L = e->GetParAs<DLongGDL>(0);
      all = false;
      nEl = p0L->N_Elements();
      for (SizeT i = 0; i < nEl; i++) {
        GDLWidget *widget = GDLWidget::GetWidget((*p0L)[i]);
        if (widget == NULL) {
          if (dobadid) {//if id is OK, but BAD_ID was given, we must return 0 in BAD_ID.
            e->SetKW(badidIx, new DLongGDL((*p0L)[i]));
            return defaultRes; //important!!!
          } else {
            e->Throw("Invalid widget identifier:" + i2s((*p0L)[i]));
          }
        }
        widgetIDList.push_back((*p0L)[i]);
        if (widget->NChildren() > 0) has_children.push_back(true); //NChildren() is more general than IsContainer().
          //At some point, remove the discrepancy between Containers and Menus/Submenus.
          //The latter having a problem wrt the general structure of widgets in that they are on the stack and cannot be treated as "permanent" widgets,
          //hence the different treatment everywhere in the code. Painful.
        else has_children.push_back(false);
      }
      //loop on this list, and add recursively all children when widget is a container.
      SizeT currentVectorSize = widgetIDList.size();
      while (1) {
        for (SizeT i = 0; i < currentVectorSize; i++) {
          if (has_children.at(i)) {
            has_children.at(i) = false;
            GDLWidget *widget = GDLWidget::GetWidget(widgetIDList.at(i));
            DLongGDL* list = static_cast<GDLWidgetContainer*> (widget)->GetChildrenList();
            for (SizeT j = 0; j < list->N_Elements(); j++) {
              widgetIDList.push_back((*list)[j]);
              if (GDLWidget::GetWidget((*list)[j])->NChildren() > 0) has_children.push_back(true);
              else has_children.push_back(false);
            }
          }
        }
        if (widgetIDList.size() == currentVectorSize) break; //no changes
        currentVectorSize = widgetIDList.size();
      }
    } else { //return default zero struct if there is no MANAGED widget on screen
       DLongGDL* res = static_cast<DLongGDL*>( GDLWidget::GetWidgetsList( ) );
       Guard<BaseGDL> guard(res);
       bool oneIsManaged=false;
       for (SizeT i=0; i< res->N_Elements(); ++i) {
         GDLWidget* w=GDLWidget::GetWidget((*res)[i]);
         if (w->GetManaged()) {
           oneIsManaged=true;
           break;
         }
       }
       if (!oneIsManaged) return defaultRes;
    }
    if (dobadid) e->SetKW(badidIx, new DLongGDL(0)); //if id is OK, but BAD_ID was given, we must return 0 in BAD_ID.

    DLong id;
    int infinity = (nowait) ? 0 : 1;
    DStructGDL* ev;

    do { // outer while loop, will run once if NOWAIT
   while (1) { //inner loop, catch controlC, default return if no event trapped in nowait mode
   GDLWidget::CallWXEventLoop();
       if (!all) {
          //specific widget(s)
          // we cannot check only readlineEventQueue thinking our XMANAGER in blocking state looks to ALL widgets.
          // because XMANAGER may have been called AFTER events are created.
          while ((ev = GDLWidget::BlockingEventQueue.Pop()) != NULL) { // get event
            static int idIx = ev->Desc()->TagIndex("ID");
            id = (*static_cast<DLongGDL*> (ev->GetTag(idIx, 0)))[0]; // get its id
            for (SizeT i = 0; i < widgetIDList.size(); i++) { //is ID corresponding to any widget in list?
              if (widgetIDList.at(i) == id) { //if yes
                goto endwait;
              }
            }
          }
          while ((ev = GDLWidget::InteractiveEventQueue.Pop()) != NULL) { // get event
            static int idIx = ev->Desc()->TagIndex("ID");
            id = (*static_cast<DLongGDL*> (ev->GetTag(idIx, 0)))[0]; // get its id
            for (SizeT i = 0; i < widgetIDList.size(); i++) { //is ID corresponding to any widget in list?
              if (widgetIDList.at(i) == id) { //if yes
                goto endwait;
              }
            }
          }
        } else {
          //wait for ALL . This is the case of /XMANAGER_BLOCK for example. Both queues may be active, some widgets being managed other not. 
          if ((ev = GDLWidget::BlockingEventQueue.Pop()) != NULL) goto endwait;
          if ((ev = GDLWidget::InteractiveEventQueue.Pop()) != NULL) goto endwait;
        }
        if (nowait) return defaultRes;
        if (sigControlC) return defaultRes;
      } //end inner loop
      //here we got a real event, process it, walking back the hierachy (in CallEventHandler()) for modified ev in case of function handlers.
    endwait:
      if (blockedByXmanager && ev->Desc( )->Name( ) == "*TOPLEVEL_DESTROYED*" ) {
        // deleted widgets list are hopefully handled internally by xmanager 
        GDLDelete(ev);
        return defaultRes;
      }
      ev = CallEventHandler(ev); //process it recursively (going up hierarchy) in eventHandler. Should block waiting for xmanager.
      // examine return:
      if (ev == NULL) { //swallowed by a procedure or non-event-stucture returning function 
        if (nowait) return defaultRes; //else will loop again
      } else { // untreated or modified by a function
          return ev;
      }
    } while (infinity);
    return NULL; //pacifier.
#endif //HAVE_LIBWXWIDGETS
  }

void widget_control( EnvT* e ) {
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
#else
  
  static int deffontIx = e->KeywordIx( "DEFAULT_FONT" );
  bool doDefFont=e->KeywordPresent( deffontIx );
  if (doDefFont) {
    std::string inputfont = "";
    e->AssureStringScalarKWIfPresent(deffontIx, inputfont);
    if (inputfont.length() > 0) {
//#ifdef __WXMSW__
//      wxFont f=wxFont(wxFontInfo(8).FaceName(inputfont));GDLWidget::setDefaultFont(f);
//#else
      wxFont f=GDLWidget::defaultFont;
//      std::cerr <<"DEFAULT BEFORE: "<< f.GetNativeFontInfoUserDesc() << std::endl;
      bool ok = f.SetNativeFontInfoUserDesc(wxString(inputfont.c_str(), wxConvLibc));
//      std::cerr <<"DEFAULT AFTER: "<< f.GetNativeFontInfoUserDesc() << std::endl;
      if (ok) { //it seems to be always OK with wxWidgets, that gives back its defaultFont in bad cases.Thus: behaviuor not as IDL.
//        std::cerr <<"FINAL DESC: "<< f.GetNativeFontInfoDesc() << std::endl;
        GDLWidget::setDefaultFont(f);
      } else GDLWidget::setDefaultFont(GDLWidget::systemFont);  //defining a bad font goes back to the system font.
//#endif
    }
    return;
  }
  static int resetIx = e->KeywordIx( "RESET" );
  bool reset = e->KeywordSet( resetIx );
  if (reset) {
    GDLWidget::ResetWidgets();
    return;
  }
  if (e->NParam()==0) return; //quietly return when no widget is given.

  static int hourglassIx =  e->KeywordIx( "HOURGLASS" );
  bool sethourglass = e->KeywordPresent( hourglassIx );  
  
  if (sethourglass){ //Ignore it for the moment!
    if (e->KeywordSet( hourglassIx )) wxBeginBusyCursor();
    else  if (wxIsBusy()) wxEndBusyCursor();
  }

  static int showIx = e->KeywordIx( "SHOW" );
  bool show = e->KeywordPresent( showIx );
  
  static int realizeIx = e->KeywordIx( "REALIZE" );
  bool realize = e->KeywordSet( realizeIx );

  static int sensitiveControlIx = e->KeywordIx( "SENSITIVE" );
  bool sensitiveControl = e->KeywordPresent( sensitiveControlIx );
  
  static int inputfocusIx = e->KeywordIx( "INPUT_FOCUS" );
  bool inputfocus = e->KeywordSet( inputfocusIx );

  static int managedIx = e->KeywordIx( "MANAGED" );
  bool managed = e->KeywordSet( managedIx );

  static int mapIx = e->KeywordIx( "MAP" );
  bool do_map=e->KeywordPresent( mapIx );
  
  static int iconifyIx = e->KeywordIx( "ICONIFY" );
  bool do_iconify=e->KeywordPresent( iconifyIx );

  static int xmanActComIx = e->KeywordIx( "XMANAGER_ACTIVE_COMMAND" ); //set by xmanager.pro when /NO_BLOCK is given.
  bool xmanActCom = e->KeywordSet( xmanActComIx );

  static int destroyIx = e->KeywordIx( "DESTROY" );
  bool destroy = e->KeywordSet( destroyIx );
// Better signaling it is ignored for the moment.  
//  static int delay_destroyIx = e->KeywordIx( "DELAY_DESTROY" );
//  bool delay_destroy = e->KeywordSet( delay_destroyIx ); //TBD, this seems tricky.
  
  static int timerIx = e->KeywordIx( "TIMER" );
  bool doTimer = e->KeywordPresent( timerIx );

  DString eventPro = "";
  static int eventproIx = e->KeywordIx( "EVENT_PRO" );
  bool eventpro = e->KeywordPresent( eventproIx );

  DString eventFun = "";
  static int eventfunIx = e->KeywordIx( "EVENT_FUNC" );
  bool eventfun = e->KeywordPresent( eventfunIx );

  DString killNotifyFunName = "";
  static int killnotifyIx = e->KeywordIx( "KILL_NOTIFY" );
  bool killnotify = e->KeywordPresent( killnotifyIx );

  DString notifyRealizeFunName = "";
  static int notifyrealizeIx = e->KeywordIx( "NOTIFY_REALIZE" );
  bool notifyrealize = e->KeywordPresent(notifyrealizeIx);

  static int getuvalueIx = e->KeywordIx( "GET_UVALUE" );
  bool getuvalue = e->KeywordPresent( getuvalueIx );

  static int setuvalueIx = e->KeywordIx( "SET_UVALUE" );
  bool setuvalue = e->KeywordPresent( setuvalueIx );

  static int setvalueIx = e->KeywordIx( "SET_VALUE" );
  bool setvalue = e->KeywordPresent( setvalueIx );
  
  static int getvalueIx = e->KeywordIx( "GET_VALUE" );
  bool getvalue = e->KeywordPresent( getvalueIx );

  static int setunameIx = e->KeywordIx( "SET_UNAME" );
  bool setuname = e->KeywordPresent( setunameIx );
  DString uname = "";
  e->AssureStringScalarKWIfPresent( setunameIx, uname );

  static int prosetvalueIx = e->KeywordIx( "PRO_SET_VALUE" );
  bool prosetvalue = e->KeywordPresent( prosetvalueIx );
  DString setvaluepro = "";
  e->AssureStringScalarKWIfPresent( prosetvalueIx, setvaluepro );

  static int funcgetvalueIx = e->KeywordIx( "FUNC_GET_VALUE" );
  bool funcgetvalue = e->KeywordPresent( funcgetvalueIx );
  DString setvaluefunc = "";
  e->AssureStringScalarKWIfPresent( funcgetvalueIx, setvaluefunc );

  static int setbuttonIx = e->KeywordIx( "SET_BUTTON" );
  bool setbutton = e->KeywordPresent( setbuttonIx );

  static int bitmapIx = e->KeywordIx( "BITMAP" );
  bool isBitmap =  e->KeywordSet( bitmapIx );
  static int imageIx = e->KeywordIx( "IMAGE" );
  bool hasImage =  e->KeywordSet( imageIx );

  static int tlbgetsizeIx =  e->KeywordIx( "TLB_GET_SIZE" );
  bool givetlbsize = e->KeywordPresent( tlbgetsizeIx );
  static int tlbgetoffsetIx =  e->KeywordIx( "TLB_GET_OFFSET" );
  bool givetlboffset = e->KeywordPresent( tlbgetoffsetIx );
  static int tlbsettitleIx =  e->KeywordIx( "TLB_SET_TITLE" );
  bool settlbtitle = e->KeywordPresent( tlbsettitleIx );  
  static int tlbsetxoffsetIx =  e->KeywordIx( "TLB_SET_XOFFSET" );
  bool settlbxoffset = e->KeywordPresent( tlbsetxoffsetIx );  
  static int tlbsetyoffsetIx =  e->KeywordIx( "TLB_SET_YOFFSET" );
  bool settlbyoffset = e->KeywordPresent( tlbsetyoffsetIx );  
  static int setxoffsetIx =  e->KeywordIx( "XOFFSET" );
  bool setxoffset = e->KeywordPresent( setxoffsetIx );  
  static int setyoffsetIx =  e->KeywordIx( "YOFFSET" );
  bool setyoffset = e->KeywordPresent( setyoffsetIx );  
 
  static int SEND_EVENT = e->KeywordIx( "SEND_EVENT" );
  static int CLEAR_EVENTS = e->KeywordIx( "CLEAR_EVENTS" );
//  static int PUSHBUTTON_EVENTS = e->KeywordIx( "PUSHBUTTON_EVENTS" );
  static int TRACKING_EVENTS = e->KeywordIx( "TRACKING_EVENTS" );
  static int DRAW_BUTTON_EVENTS = e->KeywordIx( "DRAW_BUTTON_EVENTS" );
  static int DRAW_EXPOSE_EVENTS = e->KeywordIx( "DRAW_EXPOSE_EVENTS" );
  static int DRAW_KEYBOARD_EVENTS = e->KeywordIx( "DRAW_KEYBOARD_EVENTS" );
  static int DRAW_MOTION_EVENTS = e->KeywordIx( "DRAW_MOTION_EVENTS" );
  static int DRAW_VIEWPORT_EVENTS = e->KeywordIx( "DRAW_VIEWPORT_EVENTS" );
  static int DRAW_WHEEL_EVENTS = e->KeywordIx( "DRAW_WHEEL_EVENTS" );
  static int SET_DROP_EVENTS = e->KeywordIx( "SET_DROP_EVENTS" );
  static int TLB_KILL_REQUEST_EVENTS = e->KeywordIx( "TLB_KILL_REQUEST_EVENTS" );
  static int TLB_MOVE_EVENTS = e->KeywordIx( "TLB_MOVE_EVENTS" );
  static int TLB_SIZE_EVENTS = e->KeywordIx( "TLB_SIZE_EVENTS" );
  static int KBRD_FOCUS_EVENTS = e->KeywordIx( "KBRD_FOCUS_EVENTS" );
  static int CONTEXT_EVENTS = e->KeywordIx( "CONTEXT_EVENTS" );
  static int TLB_ICONIFY_EVENTS = e->KeywordIx( "TLB_ICONIFY_EVENTS" );
  static int ALL_TABLE_EVENTS = e->KeywordIx( "ALL_TABLE_EVENTS" );
  static int ALL_TEXT_EVENTS = e->KeywordIx( "ALL_TEXT_EVENTS" );

  static int EDITABLE = e->KeywordIx( "EDITABLE" ); 
  DByteGDL* editable = GetKeywordAs<DByteGDL>(e, EDITABLE );

    
  bool send_event = e->KeywordPresent( SEND_EVENT );
  bool clear_events = e->KeywordSet( CLEAR_EVENTS );
  bool tracking_events = e->KeywordPresent( TRACKING_EVENTS );

  bool draw_button_events = e->KeywordPresent( DRAW_BUTTON_EVENTS );
// not supported (spacebar handling of buttons...)
//  bool pushbutton_events = e->KeywordPresent( PUSHBUTTON_EVENTS );
  bool draw_expose_events = e->KeywordPresent( DRAW_EXPOSE_EVENTS );
  bool draw_keyboard_events = e->KeywordPresent( DRAW_KEYBOARD_EVENTS );
  bool draw_motion_events = e->KeywordPresent( DRAW_MOTION_EVENTS );
  bool draw_wheel_events = e->KeywordPresent( DRAW_WHEEL_EVENTS );
  bool draw_viewport_events = e->KeywordPresent( DRAW_VIEWPORT_EVENTS );
  bool drop_events = e->KeywordPresent( SET_DROP_EVENTS );
  bool tlb_kill_request_events = e->KeywordPresent( TLB_KILL_REQUEST_EVENTS );
  bool tlb_move_events = e->KeywordPresent( TLB_MOVE_EVENTS );
  bool tlb_size_events = e->KeywordPresent( TLB_SIZE_EVENTS );
  bool tlb_iconify_events = e->KeywordPresent( TLB_ICONIFY_EVENTS );
  bool kbrdfocusevents = e->KeywordPresent( KBRD_FOCUS_EVENTS );
  bool contextevents = e->KeywordPresent( CONTEXT_EVENTS );
  bool all_table_events = e->KeywordPresent( ALL_TABLE_EVENTS );
  bool all_text_events = e->KeywordPresent( ALL_TEXT_EVENTS );
  
  static int SCR_XSIZE = e->KeywordIx( "SCR_XSIZE" );
  static int SCR_YSIZE= e->KeywordIx( "SCR_YSIZE" );
  static int XSIZE = e->KeywordIx( "XSIZE" );
  static int YSIZE = e->KeywordIx( "YSIZE" );
  static int DRAW_XSIZE = e->KeywordIx( "DRAW_XSIZE" );
  static int DRAW_YSIZE = e->KeywordIx( "DRAW_YSIZE" );
  
  bool hasScr_xsize  = e->KeywordPresent( SCR_XSIZE );
  bool hasScr_ysize  = e->KeywordPresent( SCR_YSIZE );
  bool hasXsize      = e->KeywordPresent( XSIZE );
  bool hasYsize      = e->KeywordPresent( YSIZE );
  bool hasDraw_xsize = e->KeywordPresent( DRAW_XSIZE );
  bool hasDraw_ysize = e->KeywordPresent( DRAW_YSIZE );

  static int UPDATE = e->KeywordIx( "UPDATE" );
  bool update=e->KeywordPresent(UPDATE);
  
  static int badidIx = e->KeywordIx( "BAD_ID" );
  bool dobadid = e->KeywordPresent( badidIx );
  if (dobadid) e->AssureGlobalKW(badidIx);

  static int group_leaderIx = e->KeywordIx( "GROUP_LEADER" );

  static int unitsIx = e->KeywordIx( "UNITS" );
  bool unitsGiven = e->KeywordPresent ( unitsIx );

  static int dynamicResizeIx = e->KeywordIx( "DYNAMIC_RESIZE" );
  bool dynres = e->KeywordPresent( dynamicResizeIx );  

  static int base_set_titleIx = e->KeywordIx( "BASE_SET_TITLE" );
  bool set_base_title = e->KeywordSet( base_set_titleIx );

//TEXT and TABLE
  static int USE_TEXT_SELECT = e->KeywordIx("USE_TEXT_SELECT");
  static int settextselectIx = e->KeywordIx("SET_TEXT_SELECT");
  bool settextselect = e->KeywordPresent(settextselectIx); 
  
  //TABLE
  static int USE_TABLE_SELECT = e->KeywordIx("USE_TABLE_SELECT");
  static int FORMAT = e->KeywordIx("FORMAT");
	
//    static int IGNORE_ACCELERATORS = e->KeywordIx( "IGNORE_ACCELERATORS" );
	
//  static int DELAY_DESTROY = e->KeywordIx( "DELAY_DESTROY");
//  bool delay_destroy = e->KeywordPresent(DELAY_DESTROY);
  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);

  WidgetIDT widgetID = (*p0L)[0];
  GDLWidget *widget = GDLWidget::GetWidget( widgetID );
  if ( widget == NULL ) {
    if ( dobadid ) {
      e->AssureGlobalKW(badidIx );
      BaseGDL** badidKW = &e->GetTheKW( badidIx );
      if (badidKW) GDLDelete( (*badidKW) );
      *badidKW=new DLongGDL( widgetID );
      return;
    } else {
      e->Throw( "Invalid widget identifier: " + i2s( widgetID ) );
    }
    }

//  if (delay_destroy) {//this should be only used by XMANAGER 
//    widget->SetDelayDestroy(e->KeywordSet(DELAY_DESTROY));
//  }
  //special modifiers used in various parts below: define them globally here
  //special for TABLE 	  bool useATableSelection=false;
  bool useATableSelection=false;
  DLongGDL* tableSelectionToUse = NULL;  
  DStringGDL* format = NULL;
  if (widget->IsTable()) { //set and throw if table selection is bad.
	if (e->KeywordPresent( USE_TEXT_SELECT )) e->Throw("Sorry, USE_TEXT_SELECT is not ready for WIDGET_TABLE, Fixme!"); //need to process the text_select events in the wxGridGDLCellTextEditor object. Just DO it.
	if (settextselect) e->Throw("Sorry, SET_TEXT_SELECT is not ready for WIDGET_TABLE, Fixme!");  //need to process the text_select events in the wxGridGDLCellTextEditor object. Just DO it.
	GDLWidgetTable *table = (GDLWidgetTable *) widget;
	tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);
	useATableSelection=table->GetValidTableSelection(tableSelectionToUse); //will throw if a syntax problem
    format = GetKeywordAs<DStringGDL>(e, FORMAT);
  }
  if ( getvalue ) {
    e->AssureGlobalKW( getvalueIx );
    BaseGDL** valueKW = &e->GetTheKW( getvalueIx );

    DString getFuncName = widget->GetFuncValue( );
    if ( !(getFuncName.empty( )) ) {
      StackGuard<EnvStackT> guard( e->Interpreter( )->CallStack( ) );

      DString callF = StrUpCase( getFuncName );

      SizeT funIx = GDLInterpreter::GetFunIx( callF );
      EnvUDT* newEnv = new EnvUDT( e->CallingNode( ), funList[ funIx], (DObjGDL**) NULL );

      // add parameter
      newEnv->SetNextPar( new DLongGDL( widgetID ) ); // pass as local
      e->Interpreter( )->CallStack( ).push_back( newEnv );

      // make the call
      BaseGDL* res = e->Interpreter( )->call_fun( static_cast<DSubUD*> (newEnv->GetPro( ))->GetTree( ) );

      // set the keyword to the function's return value which can be anything!!!
      if (valueKW) GDLDelete( (*valueKW) );
      *valueKW = res;
    } else { 
        if ( widget->IsTable( ) ) { //TABLE
        GDLWidgetTable *table = (GDLWidgetTable *) widget;
		  if (valueKW) GDLDelete((*valueKW));
		  *valueKW = table->GetTableValues( tableSelectionToUse );
      } else if ( widget->IsSlider( ) ) {
        GDLWidgetSlider *s = (GDLWidgetSlider *) widget;
        if (valueKW) GDLDelete( (*valueKW) );
        *valueKW = new DLongGDL(s->GetValue());
      } else if ( widget->IsTree( ) ||  widget->IsLabel( ) || widget->IsDropList( ) || widget->IsComboBox( ) || widget->IsDraw() ) { 
        BaseGDL *widval = widget->GetVvalue( );
        if ( widval != NULL ) {
          if (valueKW) GDLDelete( (*valueKW) );
          *valueKW = widval->Dup( );
        }
      } else if ( widget->IsButton( ) ) { //bitmap buttons return an error on this call.
        GDLWidgetButton *s = (GDLWidgetButton *) widget;
        if ( s->IsBitmapButton() ) e->Throw("Unable to obtain non-string button value.");
        BaseGDL *widval = widget->GetVvalue( );
        if ( widval != NULL ) {
          if (valueKW) GDLDelete( (*valueKW) );
          *valueKW = widval->Dup( );
        }
      } else if ( widget->IsText( ) ) {
        bool usetextselect = e->KeywordPresent( USE_TEXT_SELECT );
        BaseGDL *v;
        if (usetextselect) {
          GDLWidgetText* txt= static_cast<GDLWidgetText*>(widget);
          v = txt->GetSelectedText();
        } else {
          GDLWidgetText* txt= static_cast<GDLWidgetText*>(widget);
//          v = widget->GetVvalue( );
          v = new DStringGDL(txt->GetLastValue());
          }
          if (v != NULL) {
            if (valueKW) GDLDelete((*valueKW));
            DStringGDL *s = static_cast<DStringGDL *> (v);
#ifdef _WIN32
            std::string crlf("\r\n");
            int crlfSize = 2;
#else
            std::string crlf("\n");
            int crlfSize = 1;
#endif
            int nlines = 1; //text in widgets has no ending return.
            size_t opos = 0;
            size_t pos = ((*s)[0]).find(crlf);
            while ((pos != std::string::npos)) {
              nlines++;
              opos = pos + crlfSize;
              pos = ((*s)[0]).find(crlf, opos);
            }

            if (nlines == 1) *valueKW = v;
            else {
              DStringGDL* res = new DStringGDL(dimension(nlines), BaseGDL::NOZERO);
              int index = 0;
              opos=0;
              pos = ((*s)[0]).find(crlf);
              while ((pos != std::string::npos)) {
                (*res)[index] = ((*s)[0]).substr(opos, pos - opos);
                index++;
                opos = pos + crlfSize;
                pos = ((*s)[0]).find(crlf, opos);
              }
              (*res)[index] = ((*s)[0]).substr(opos); //text in widgets has no ending return.
              GDLDelete(v);
              *valueKW = res;
            }
          }
      } else {
        e->Throw("Class of specified widget has no value: "+i2s(widget->GetWidgetType()));
      }
    }
  } //end getValue
  
  //at that point, invalid widgets will not respond to widget_control.
  if (!widget->IsValid()) return;


  DLong groupLeader = 0;
  if (e->KeywordPresent( group_leaderIx )){
    e->AssureLongScalarKWIfPresent( group_leaderIx, groupLeader );
    if (groupLeader != 0) {
      GDLWidget* leader=widget->GetWidget(groupLeader);
      if (leader) leader->AddToFollowers(widget->GetWidgetID());
    }
  } 
  
  
  if (hasScr_xsize || hasScr_ysize) { //simple: direct sizing in pixels or UNITS for ALL widgets
    DLong xsize=-1, ysize=-1;
    if (hasScr_xsize) {xsize= (*e->GetKWAs<DLongGDL>(SCR_XSIZE))[0]; if (xsize<0) e->Throw("Illegal keyword value for SCR_XSIZE.");}
    if (hasScr_ysize) {ysize= (*e->GetKWAs<DLongGDL>(SCR_YSIZE))[0]; if (ysize<0) e->Throw("Illegal keyword value for SCR_YSIZE.");}

    wxWindow* me=dynamic_cast<wxWindow*>(widget->GetWxWidget());
    if (me==NULL) e->Throw("Geometry request not allowed for menubar or pulldown menus.");

    wxRealPoint fact = wxRealPoint(1.,1.);
    if ( unitsGiven ) {
      fact = GetRequestedUnitConversionFactor( e );
      if (hasScr_xsize) xsize*=fact.x;
      if (hasScr_ysize) ysize*=fact.y;
    }    
    widget->SetWidgetScreenSize(xsize,ysize);
  }
  
  if ( (hasDraw_xsize || hasDraw_ysize ) && widget->IsDraw() ) {
    DLong xsize=-1, ysize=-1;
    if (hasDraw_xsize) {xsize= (*e->GetKWAs<DLongGDL>(DRAW_XSIZE))[0]; if (xsize<0) e->Throw("Illegal keyword value for DRAW_XSIZE.");}
    if (hasDraw_ysize) {ysize= (*e->GetKWAs<DLongGDL>(DRAW_YSIZE))[0]; if (ysize<0) e->Throw("Illegal keyword value for DRAW_YSIZE.");}
    wxRealPoint fact = wxRealPoint(1.,1.);
    if ( unitsGiven ) {
      fact = GetRequestedUnitConversionFactor( e );
      if (hasDraw_xsize) xsize*=fact.x;
      if (hasDraw_ysize) ysize*=fact.y;
    } 
    widget->SetWidgetVirtualSize(xsize,ysize);   
   }
  
  if ((hasXsize || hasYsize) && !widget->IsTable()) { //widgetTable tested separately
	
    if ( widget->IsButton()) {
      GDLWidgetButton* whatSortofBut=static_cast<GDLWidgetButton*>(widget);
      if (whatSortofBut->IsMenu() || whatSortofBut->IsEntry()) e->Throw("Geometry request not allowed for menubar or pulldown menus.");
    }
    DLong xsize=-1, ysize=-1;
    if (hasXsize) {xsize= (*e->GetKWAs<DLongGDL>(XSIZE))[0]; if (xsize<0) e->Throw("Illegal keyword value for XSIZE.");}
    if (hasYsize) {ysize= (*e->GetKWAs<DLongGDL>(YSIZE))[0]; if (ysize<0) e->Throw("Illegal keyword value for YSIZE.");}

    wxWindow* me=dynamic_cast<wxWindow*>(widget->GetWxWidget());
    if (!me) e->Throw("Geometry request not allowed for menubar or pulldown menus.");
    
    if (!(widget->IsList() || widget->IsTable() || widget->IsText())) {
    wxRealPoint fact = wxRealPoint(1.,1.);
      if ( unitsGiven ) {
        fact = GetRequestedUnitConversionFactor( e );
        if (hasXsize) xsize*=fact.x;
        if (hasYsize) ysize*=fact.y;
      }     
    } 
    widget->SetWidgetSize(xsize,ysize);
  }

    //set dynres BEFORE set_value (IDL logical behaviour)
    if (dynres) {
      if (!(widget->IsButton() ||widget->IsComboBox() || widget->IsDropList() || widget->IsLabel()) ) e->Throw("DYNAMIC_RESIZE keyword does not apply to specified widget type.");
      DLong allowDynRes = 0;
      e->AssureLongScalarKWIfPresent(dynamicResizeIx, allowDynRes);
      if (allowDynRes == 1) widget->SetDynamicResize();
      else widget->UnsetDynamicResize();
    }

    // start with set/ or get/value
    // this insures that all widgets, i.e. including invalid widgets (in the process of being deleted)
    // still respond to set or get values.
    if (setvalue) {
      DString wType = widget->GetWidgetName();
      BaseGDL* value = e->GetKW(setvalueIx);
      if (value != NULL) value = value->Dup();
      //process eventual PRO_SETVALUE call
      DString setProName = widget->GetProValue();
      if (setProName != "") CallEventPro(setProName, p0L->Dup(), value);
      if (wType == "TEXT") {
        static int noNewLineIx = e->KeywordIx("NO_NEWLINE");
        bool noNewLine = e->KeywordSet(noNewLineIx);
        static int usetextselectIx = e->KeywordIx("USE_TEXT_SELECT");
        bool usetextselect = e->KeywordPresent(usetextselectIx);
        static int appendvalueIx = e->KeywordIx("APPEND");
        bool appendvalue = e->KeywordPresent(appendvalueIx);

        BaseGDL* valueKW = e->GetKW(setvalueIx);
        DStringGDL* valueStr = NULL;
        if (valueKW != NULL) {
          if (valueKW->Type() != GDL_STRING)
            e->Throw("VALUE must be a STRING for WIDGET_TEXT.");
          valueStr = static_cast<DStringGDL*> (valueKW);
          bool success = e->StealLocalKW(setvalueIx);
          if (!success)
            valueStr = valueStr->Dup();

          GDLWidgetText *textWidget = (GDLWidgetText *) widget;
          if (appendvalue) textWidget->InsertText(valueStr, noNewLine, true); //aka insertAtEnd
          else if (usetextselect) textWidget->InsertText(valueStr, noNewLine);
          else textWidget->ChangeText(valueStr, noNewLine);
        }
      } else if (wType == "SLIDER") {
        DLong value = 0;
        e->AssureLongScalarKWIfPresent(setvalueIx, value);
        GDLWidgetSlider *s = (GDLWidgetSlider *) widget;
        s->ControlSetValue(value);
      } else if (wType == "LABEL") {
        DString value = "";
        e->AssureStringScalarKWIfPresent(setvalueIx, value);
        //	std::cout << "setlabelvalue: " << value.c_str() << std::endl;
        GDLWidgetLabel *labelWidget = (GDLWidgetLabel *) widget;
        labelWidget->SetLabelValue(value);
        //      if (labelWidget->IsDynamicResize()) labelWidget->RefreshDynamicWidget();
      } else if (wType == "COMBOBOX") {
        GDLWidgetComboBox *combo = static_cast<GDLWidgetComboBox*> (widget);
        combo->SetValue(value);
        //      if (combo->IsDynamicResize()) combo->RefreshDynamicWidget();
      } else if (wType == "LIST") {
        GDLWidgetList *list = static_cast<GDLWidgetList*> (widget);
        list->SetValue(value);
      } else if (wType == "DROPLIST") {
        GDLWidgetDropList *droplist = static_cast<GDLWidgetDropList*> (widget);
        droplist->SetValue(value);
        //      if (droplist->IsDynamicResize()) droplist->RefreshDynamicWidget();
      } else if (wType == "BUTTON") {
        DString strvalue = " "; //default value : a whitespace as some buttons do not like empty strings (wxWidgets assert)
        wxBitmap * bitmap = NULL;
        BaseGDL* invalue = e->GetKW(setvalueIx);
        if (invalue==NULL) goto endsetvalue; //happens
        //value=filename if /BITMAP present. Otherwise value must be string, although if array of correct size, is bitmap!
        //Note BITMAP and RadioButtons are not possible directly.
        DByteGDL* passedBytes = NULL;
        if (invalue->Type() == GDL_STRING) { //value is a string
          e->AssureStringScalarKW(setvalueIx, strvalue);
          // BITMAP is ignored when invalue is of the correct type. Otherwise if string, the bitmap must be present.
          if (isBitmap) {
            hasImage = false; //image will be ignored.
            WordExp(strvalue);
            bitmap = new wxBitmap(wxString(strvalue.c_str(), wxConvUTF8), wxBITMAP_TYPE_ANY);
            if (bitmap->IsOk()) {
              strvalue = " "; //not clear, hurts wxWidgets feelings.
              GDLWidgetButton *bb = (GDLWidgetButton *) widget;
#ifdef PREFERS_MENUBAR
              if (dynamic_cast<GDLWidgetMenuBarButton*> (bb) != NULL) e->Throw("Menu bars items cannot be images.");
#endif
              bb->SetButtonWidgetBitmap(bitmap);
              goto endsetvalue;
            } else {
              Warning("WIDGET_BUTTON: Can't open bitmap file: " + strvalue);
              delete bitmap;
              bitmap = NULL; //necessary!
              e->AssureStringScalarKWIfPresent(setvalueIx, strvalue);
              GDLWidgetButton *bb = (GDLWidgetButton *) widget;
              bb->SetButtonWidgetLabelText(strvalue);
              goto endsetvalue;
            }
          } else { //just a string, use it, unless "hasImage" where both the string strvalue AND the bimap will be passed
          }
        } else { //value is an image, no string
          passedBytes = e->GetKWAs<DByteGDL>(setvalueIx); //test it
          hasImage = false; //image will be ignored.
        }
        // if hasImage is still true, it is the image KW that contains an image:
        if (hasImage) passedBytes = e->GetKWAs<DByteGDL>(imageIx); //value must be a text and image is in IMAGE=xx KW

        //whatever the passedBytes it must be OK:
        if (passedBytes) {
          bitmap = GetBitmapFromPassedBytes(e, passedBytes);
          if (!hasImage) strvalue = " "; //not clear, hurts wxWidgets feelings.
        }
        GDLWidgetButton *bb = (GDLWidgetButton *) widget;
#ifdef PREFERS_MENUBAR
        if (bitmap && dynamic_cast<GDLWidgetMenuBarButton*> (bb) != NULL) e->Throw("Menu bars items cannot be images.");
#endif
        //passes the string before bitmap
        bb->SetButtonWidgetLabelText(strvalue);
        if (bitmap) bb->SetButtonWidgetBitmap(bitmap);
        
      } else if (widget->IsTable()) {
        GDLWidgetTable *table = (GDLWidgetTable *) widget;
		int majority = table->GetMajority();
		if (useATableSelection && format != NULL) e->Throw("Unable to set format for table widget."); //format not allowed if selection
		format=table->GetCurrentFormat(); //use stored format
		//convert 'value' to vValue type FIRST...
		BaseGDL* v=table->GetVvalue();
		if (v != NULL) {
		  DType type = v->Type();
		  if (table->GetMajority() == GDLWidgetTable::NONE_MAJOR) value = value->Convert2(type);
		}
		//... then create the String equivalent
		DStringGDL* newValueAsStrings=GetTableValueAsString(e, value, format, majority, true); //true as IDL accepts non-array in this case
		//set all values inside:
		table->SetTableValues( value, newValueAsStrings, tableSelectionToUse);
		//The above formatting was not in error, as it not throwed: if format is not null, replace format in widget for future reference:
		if (format != NULL) table->SetCurrentFormat(format);
      } else if (widget->IsTree()) {
        DString value = "";
        e->AssureStringScalarKWIfPresent(setvalueIx, value); //value is a string
        GDLWidgetTree *tree = (GDLWidgetTree *) widget;
        tree->SetValue(value);
      } else if (wType == "BASE" || wType == "TAB") {
        //no use to complain. this is accepted.
//#ifdef GDL_DEBUG_WIDGETS
//        wxMessageOutputStderr().Printf(_T("Class of specified widget has no value: %d (%s)\n"), widget->GetWidgetID(),widget->GetWidgetName());
//#endif
      }
    } //end SetValue

  endsetvalue:
  
//  static int FRAME = e->KeywordIx( "FRAME" );
//  if (e->KeywordPresentAndDefined( FRAME )) {
//    DLongGDL* val=e->GetKWAs<DLongGDL>(FRAME);
//    if ( (*val)[0] > 0) widget->AddFrame((*val)[0]); else widget->RemoveFrame();
//  }
//  static int SCROLL = e->KeywordIx( "SCROLL" );
//  bool scroll=e->KeywordPresent( SCROLL );
//  if (scroll) { if (e->KeywordSet( SCROLL)) widget->AddScrollBars(120,120); else widget->RemoveScrollBars();}

    if (send_event) {
      BaseGDL* event = e->GetKW(SEND_EVENT)->Dup();
      if (event && event->Type() != GDL_STRUCT) e->Throw("Expression must be a structure in this context: " + e->GetString(SEND_EVENT));
      DStructGDL* ev = static_cast<DStructGDL*> (event);
      SizeT id1 = ev->Desc()->TagIndex("ID");
      SizeT id2 = ev->Desc()->TagIndex("TOP");
      SizeT id3 = ev->Desc()->TagIndex("HANDLER");
      if (id1 == -1 || id2 == -1 || id3 == -1) e->Throw("Invalid SEND_EVENT value.");
      BaseGDL* val1 = ev->GetTag(id1);
      BaseGDL* val2 = ev->GetTag(id2);
      BaseGDL* val3 = ev->GetTag(id3);
      if ((val1->Type() != GDL_LONG) || (val2->Type() != GDL_LONG) || (val3->Type() != GDL_LONG)) {
        e->Throw("Invalid SEND_EVENT value.");
      }
      DLongGDL* lval1 = static_cast<DLongGDL*> (val1);
      DLongGDL* lval2 = static_cast<DLongGDL*> (val2);
      DLongGDL* lval3 = static_cast<DLongGDL*> (val3);
      WidgetIDT baseWidgetID = widget->GetBaseId(widgetID);
      if ((*lval1)[0] == 0) (*lval1)[0] = widgetID;
      if ((*lval2)[0] == 0) (*lval2)[0] = baseWidgetID;
      if ((*lval3)[0] == 0) (*lval3)[0] = baseWidgetID;
      GDLWidget::PushEvent(baseWidgetID, ev);
    }
    
  if (clear_events) widget->ClearEvents();
  
  
  if (tlb_kill_request_events && widget->IsBase() && widget->GetParentID() == GDLWidget::NullID ){ //silently ignore other cases.
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
     if (e->KeywordSet(TLB_KILL_REQUEST_EVENTS)) {
      if ( (widget->GetEventFlags() & GDLWidget::EV_KILL) == 0) { //bit was not set
        topFrame->Disconnect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnUnhandledCloseFrame));
        topFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnCloseFrame));
        widget->AddEventType(GDLWidget::EV_KILL);
      }
    }
    else
    {
      if ((widget->GetEventFlags() & GDLWidget::EV_KILL) == GDLWidget::EV_KILL ) { //if was enabled
        topFrame->Disconnect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnCloseFrame));
        topFrame->Connect(widgetID, wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnUnhandledCloseFrame));
        widget->RemoveEventType(GDLWidget::EV_KILL);
      }
    }
  }

  if (tlb_move_events && widget->IsBase() && widget->GetParentID() == GDLWidget::NullID ){//silently ignore other cases.
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
     if (e->KeywordSet(TLB_MOVE_EVENTS)){
      if ( (widget->GetEventFlags() & GDLWidget::EV_MOVE) == 0 ) { //was not set
        topFrame->Connect(widgetID, wxEVT_MOVE, wxMoveEventHandler(gdlwxFrame::OnMove));
        widget->AddEventType(GDLWidget::EV_MOVE);
      }      
    }
    else
    {
      if ( (widget->GetEventFlags() & GDLWidget::EV_MOVE) == GDLWidget::EV_MOVE) { //was set
        topFrame->Disconnect(widgetID, wxEVT_MOVE, wxMoveEventHandler(gdlwxFrame::OnMove));
        widget->RemoveEventType(GDLWidget::EV_MOVE);
      }      
    }
  }

  if (tlb_size_events && widget->IsBase() && widget->GetParentID() == GDLWidget::NullID ){//silently ignore other cases.
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
    if (e->KeywordSet(TLB_SIZE_EVENTS)) {
      if ((widget->GetEventFlags() & GDLWidget::EV_SIZE) == 0) { 
        topFrame->Connect(widgetID, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
        widget->AddEventType(GDLWidget::EV_SIZE);
      }
    }
    else
    {
      if ( (widget->GetEventFlags() & GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE) { 
       topFrame->Disconnect(widgetID, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
       widget->RemoveEventType(GDLWidget::EV_SIZE);
      }
    }
  }

  if (tlb_iconify_events && widget->IsBase() && widget->GetParentID() == GDLWidget::NullID ){//silently ignore other cases.
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
    if (e->KeywordSet(TLB_ICONIFY_EVENTS)){
      if ( (widget->GetEventFlags() & GDLWidget::EV_ICONIFY) == 0) {
        topFrame->Connect(widgetID, wxEVT_ICONIZE, wxIconizeEventHandler(gdlwxFrame::OnIconize)); 
        widget->AddEventType(GDLWidget::EV_ICONIFY);
      }
    }
    else
    { 
      if ( (widget->GetEventFlags() & GDLWidget::EV_ICONIFY) == GDLWidget::EV_ICONIFY) { //do it if not already done
        topFrame->Disconnect(widgetID, wxEVT_ICONIZE, wxIconizeEventHandler(gdlwxFrame::OnIconize)); 
        widget->RemoveEventType(GDLWidget::EV_ICONIFY);
      }
    }
  }

  if ( kbrdfocusevents && ( widget->IsBase() || widget->IsTable() || widget->IsText() ) ) { //globally handled, no need to be more specific
    if (e->KeywordSet(KBRD_FOCUS_EVENTS)) widget->AddEventType(GDLWidget::EV_KBRD_FOCUS);
    else widget->RemoveEventType(GDLWidget::EV_KBRD_FOCUS);
  }
  
  if ( contextevents && ( widget->IsBase() || widget->IsList() || widget->IsTable() || widget->IsText() || widget->IsTree() ) ) { //globally handled, no need to be more specific
    if (e->KeywordSet(CONTEXT_EVENTS)) widget->AddEventType(GDLWidget::EV_CONTEXT);
    else widget->RemoveEventType(GDLWidget::EV_CONTEXT);
  }

  if (draw_motion_events && widget->IsDraw()) {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(DRAW_MOTION_EVENTS)) draw->AddEventType(GDLWidget::EV_MOTION);
    else draw->RemoveEventType(GDLWidget::EV_MOTION);
  }
  if (draw_expose_events && widget->IsDraw()) {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(DRAW_EXPOSE_EVENTS)) draw->AddEventType(GDLWidget::EV_EXPOSE);
    else draw->RemoveEventType(GDLWidget::EV_EXPOSE);
  }
  if (tracking_events) {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(TRACKING_EVENTS))  draw->AddEventType(GDLWidget::EV_TRACKING);
    else draw->RemoveEventType(GDLWidget::EV_TRACKING);
  }
   if (draw_viewport_events && widget->IsDraw())  {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(DRAW_VIEWPORT_EVENTS))  draw->AddEventType(GDLWidget::EV_VIEWPORT);
    else draw->RemoveEventType(GDLWidget::EV_VIEWPORT);
  }
  if (draw_wheel_events && widget->IsDraw())  {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(DRAW_WHEEL_EVENTS))  draw->AddEventType(GDLWidget::EV_WHEEL);
    else draw->RemoveEventType(GDLWidget::EV_WHEEL);
  }
  if (draw_button_events && widget->IsDraw())  {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    if (e->KeywordSet(DRAW_BUTTON_EVENTS))   draw->AddEventType(GDLWidget::EV_BUTTON);
    else draw->RemoveEventType(GDLWidget::EV_BUTTON);
  }
  if (draw_keyboard_events && widget->IsDraw()) {
    GDLWidgetDraw* draw=static_cast<GDLWidgetDraw*>(widget);
    draw->RemoveEventType(GDLWidget::EV_KEYBOARD2);
    draw->RemoveEventType(GDLWidget::EV_KEYBOARD);
    DLong val= (*e->GetKWAs<DLongGDL>(DRAW_KEYBOARD_EVENTS))[0];
    if (val==2) { widget->SetFocus();  draw->AddEventType(GDLWidget::EV_KEYBOARD2);}
    else if (val==1)  { widget->SetFocus(); draw->AddEventType(GDLWidget::EV_KEYBOARD);}
  }
  
  if (all_table_events && widget->IsTable()) {
    if (e->KeywordSet(ALL_TABLE_EVENTS)) widget->AddEventType(GDLWidget::EV_ALL);
    else widget->RemoveEventType(GDLWidget::EV_ALL);
  }
   
  if (all_text_events && widget->IsText()) {
    if (e->KeywordSet(ALL_TEXT_EVENTS)) widget->AddEventType(GDLWidget::EV_ALL);
    else widget->RemoveEventType(GDLWidget::EV_ALL);
  }

//end events 
  if ( update ) {
    widget->EnableWidgetUpdate(e->KeywordSet(UPDATE));
  }

  if (realize && !widget->IsRealized()) { //prevent to act on a realized widget : caused problems.
    if (do_map) widget->Realize(e->KeywordSet(mapIx)); else widget->Realize(true,true); //specifically ask for whatever map value was given to the base widget
  } else if (do_map) { //alone... 
    GDLWidgetBase * w = widget->GetMyBaseWidget();
    if (w != NULL) w->mapBase(e->KeywordSet(mapIx));
  } 
  
  if (show) {
    if (e->KeywordSet(showIx)) widget->Raise(); else widget->Lower();
  }
  
  if (do_iconify) {
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
     widget->GetMyTopLevelBaseWidget()->GetTopFrame()->Iconize(e->KeywordSet(iconifyIx));
  }
  
  if ( managed ) {
    widget->SetManaged( true );
  }

  if ( xmanActCom ) {
    //       cout << "Set xmanager active command: " << widgetID << endl;
    widget->MakeInteractive( );
    }


  if ( sensitiveControl) {
    if (e->KeywordSet(sensitiveControlIx)) widget->SetSensitive( true );
    else widget->SetSensitive( false );
  }
  
  if ( inputfocus /*&& (widget->IsDraw()||widget->IsButton()||widget->IsText())*/) widget->SetFocus();

  if (doTimer) {
    DDouble seconds=0;
    e->AssureDoubleScalarKWIfPresent( timerIx, seconds );
    widget->SendWidgetTimerEvent(seconds);
  }
  
  if ( eventpro ) {
    e->AssureStringScalarKWIfPresent( eventproIx, eventPro );
    widget->SetEventPro( eventPro );
  }

  if ( eventfun ) {
    e->AssureStringScalarKWIfPresent( eventfunIx, eventFun );
    widget->SetEventFun( eventFun );
  }
  
  if ( killnotify ) {
    e->AssureStringScalarKWIfPresent( killnotifyIx, killNotifyFunName );
//    cout << killNotifyFunName << endl;
    widget->SetKillNotify( killNotifyFunName );
  }
   if ( notifyrealize ) {
    e->AssureStringScalarKWIfPresent( notifyrealizeIx, notifyRealizeFunName );
//    cout << killNotifyFunName << endl;
    widget->SetNotifyRealize( notifyRealizeFunName );
  }
 
  if (settlbtitle || settlbxoffset || settlbyoffset ) {
     GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
     gdlwxFrame* topFrame=tlb->GetTopFrame();
     if (settlbtitle) {
       DStringGDL* tlbTitle=e->GetKWAs<DStringGDL>( tlbsettitleIx );
       wxString tlbName = wxString( (*tlbTitle)[0].c_str( ), wxConvUTF8 );
       topFrame->SetTitle(tlbName);
     }
     if (settlbxoffset) {
       DLongGDL* xoffset=e->GetKWAs<DLongGDL>( tlbsetxoffsetIx );
       if (unitsGiven) {
         wxRealPoint fact=GetRequestedUnitConversionFactor(e);
         topFrame->Move( (*xoffset)[0]*fact.x, topFrame->GetPosition().y );
       } else   topFrame->Move( (*xoffset)[0], topFrame->GetPosition().y );
     }
     if (settlbyoffset) {
       DLongGDL* yoffset=e->GetKWAs<DLongGDL>( tlbsetyoffsetIx );
       if (unitsGiven) {
         wxRealPoint fact=GetRequestedUnitConversionFactor(e);
         topFrame->Move(topFrame->GetPosition().x, (*yoffset)[0]*fact.y  );
       } else  topFrame->Move(topFrame->GetPosition().x, (*yoffset)[0]  );
     }
  }

  if (givetlbsize) { 
    e->AssureGlobalKW( tlbgetsizeIx );
    BaseGDL** tlbsizeKW = &e->GetTheKW( tlbgetsizeIx );
    GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
    if (tlbsizeKW) GDLDelete((*tlbsizeKW));
    *tlbsizeKW = new DLongGDL(2,BaseGDL::ZERO);
    DLong *retsize=&(*static_cast<DLongGDL*>(*tlbsizeKW))[0];
    int i=0;
    int j=0;
    wxFrame* topFrame=tlb->GetTopFrame();
    if (topFrame) topFrame->GetClientSize(&i,&j);
    retsize[0]=i;
    retsize[1]=j;
    //size is in pixels, pass in requested units:
     if (unitsGiven) {
       wxRealPoint fact=GetRequestedUnitConversionFactor(e);
       retsize[0]/=fact.x;
       retsize[1]/=fact.y;
     }
  }
  
  if (givetlboffset) { 
    e->AssureGlobalKW( tlbgetoffsetIx );
    BaseGDL** tlboffsetKW = &e->GetTheKW( tlbgetoffsetIx );
    GDLWidgetTopBase* tlb = widget->GetMyTopLevelBaseWidget();
    if ( tlb == NULL ) e->Throw("Widget "+i2s( widgetID )+" has no top-level Base (please report!).");
    if (tlboffsetKW) GDLDelete((*tlboffsetKW));
    *tlboffsetKW = new DLongGDL(2,BaseGDL::ZERO);
    DLong *retoffset=&(*static_cast<DLongGDL*>(*tlboffsetKW))[0];
    int i=0;
    int j=0;
    wxFrame* topFrame=tlb->GetTopFrame();
    if (topFrame) topFrame->GetPosition(&i,&j);
    retoffset[0]=i;
    retoffset[1]=j;
    //size is in pixels, pass in requested units:
     if (unitsGiven) {
       wxRealPoint fact=GetRequestedUnitConversionFactor(e);
       retoffset[0]/=fact.x;
       retoffset[1]/=fact.y;
     }
  }
  
  if ( setxoffset || setyoffset ) {
    if (widget->GetMyParent() && widget->GetMyParent()->GetSizer()) e->Throw("Row/Column bases do not allow child offsets to be changed.");

    DLong xoff=-1; //not asked for
    DLong yoff=-1; //not asked for
      if (setxoffset) {
        DLongGDL* xoffset = e->GetKWAs<DLongGDL>(setxoffsetIx);
        if (unitsGiven) {
          wxRealPoint fact = GetRequestedUnitConversionFactor(e);
          (*xoffset)[0] *= fact.x;
        }
        xoff=(*xoffset)[0];
      }
      if (setyoffset) {
        DLongGDL* yoffset = e->GetKWAs<DLongGDL>(setyoffsetIx);
        if (unitsGiven) {
          wxRealPoint fact = GetRequestedUnitConversionFactor(e);
          (*yoffset)[0] *= fact.y;
        }
        yoff=(*yoffset)[0];
      }
      widget->SetWidgetPosition(xoff,yoff);
    }

  if ( getuvalue ) {
    e->AssureGlobalKW( getuvalueIx );
    BaseGDL** uvalueKW = &e->GetTheKW( getuvalueIx );
      BaseGDL *widval = widget->GetUvalue();
     if ( widval != NULL) {
       if (uvalueKW) GDLDelete((*uvalueKW));
       *uvalueKW = widval->Dup();
     }
  }

  if ( setuvalue ) {
    BaseGDL* uvalue = e->GetKW( setuvalueIx );
    if ( uvalue != NULL ) uvalue = uvalue->Dup( );
    widget->SetUvalue( uvalue );
  }

  if ( setuname ) {
    widget->SetUname( uname );
  }
  if ( set_base_title && widget->IsBase()) {
    DStringGDL* s=e->GetKWAs<DStringGDL>( base_set_titleIx );
    GDLWidgetBase* me=static_cast<GDLWidgetBase*>(widget);
    if (me->IsTabbedBase()) static_cast<GDLWidgetTabbedBase*>(me)->SetBaseTitle((*s)[0]);
    else if (me->IsTopBase()) {
      GDLWidgetTopBase* you=static_cast<GDLWidgetTopBase*>(widget);
      gdlwxFrame* topFrame=you->GetTopFrame();
      topFrame->SetTitle((*s)[0]);
    }
  }
  if ( prosetvalue ) {
    widget->SetProValue( setvaluepro );
  }

  if ( funcgetvalue ) {
    widget->SetFuncValue( setvaluefunc );
    }

  if (setbutton) {
//      if (!widget->IsButton() && !widget->IsBase()) {
//        e->Throw("Only WIDGET_BUTTON are allowed with keyword SET_BUTTON.");
//      }
      if (widget->IsButton() || widget->IsBase()) {
        DLong buttonVal;
        e->AssureLongScalarKWIfPresent(setbuttonIx, buttonVal);
        if (buttonVal == 0)
          widget->SetButtonWidget(false);
        else
          widget->SetButtonWidget(true);
      } 
    }

  if ( settextselect ) {
    DString wType = widget->GetWidgetName( );
    if ( wType == "TEXT" ) {
      DLongGDL* value=e->GetKWAs<DLongGDL>( settextselectIx );
      if (value->N_Elements() > 2) e->Throw( "Keyword array parameter SET_TEXT_SELECT must have from 1 to 2 elements." );
      GDLWidgetText *textWidget = (GDLWidgetText *) widget;
      textWidget->SetTextSelection( value );
    } else if ( wType == "TABLE" ) e->Throw( "SET_TEXT_SELECT not ready for Table Widgets, FIXME." ); //need to process the text_select events in the wxGridGDLCellTextEditor object. Just DO it.
  }

  if ( editable ) {
    DString wType = widget->GetWidgetName( );
    if ( wType == "TEXT" ) {
      GDLWidgetText *textWidget = (GDLWidgetText *) widget;
      textWidget->SetEditable( (*editable)[0]!=0 );
    } else if ( wType == "TABLE" ) e->Throw( "EDITABLE not ready for Table Widgets, FIXME." );
  }
  
  if (widget->IsDropList()){
    GDLWidgetDropList *droplist = (GDLWidgetDropList *) widget;
    
    static int SET_DROPLIST_SELECT = e->KeywordIx( "SET_DROPLIST_SELECT" );
    if (e->KeywordPresentAndDefined(SET_DROPLIST_SELECT)) {
      DLongGDL* droplistSelection =  e->GetKWAs<DLongGDL>(SET_DROPLIST_SELECT);
      if (droplistSelection->N_Elements() > 1) e->Throw( "Expression must be a scalar or 1 element array in this context:");
      droplist->SelectEntry((*droplistSelection)[0]);
    }    
  }

  if (widget->IsSlider()) {
      GDLWidgetSlider *s = (GDLWidgetSlider *) widget;

      static int setsliderminIx = e->KeywordIx("SET_SLIDER_MIN");
      bool setslidermin = e->KeywordPresent(setsliderminIx);
      static int setslidermaxIx = e->KeywordIx("SET_SLIDER_MAX");
      bool setslidermax = e->KeywordPresent(setslidermaxIx);
      if (setslidermin) {
        DLongGDL* value = e->GetKWAs<DLongGDL>(setsliderminIx);
        s->ControlSetMinValue((*value)[0]);
      }
      if (setslidermax) {
        DLongGDL* value = e->GetKWAs<DLongGDL>(setslidermaxIx);
        s->ControlSetMaxValue((*value)[0]);
      }
    }
  
  if (widget->IsList()){
    GDLWidgetList *list = (GDLWidgetList *) widget;
    
    static int SET_LIST_SELECT = e->KeywordIx( "SET_LIST_SELECT" );
    if (e->KeywordPresentAndDefined(SET_LIST_SELECT)) {
      DLongGDL* listSelection =  e->GetKWAs<DLongGDL>(SET_LIST_SELECT);
      for (int i=0; i<listSelection->N_Elements() ; ++i) list->SelectEntry((*listSelection)[i]); //most probably not the right thing to do.
    }
  }
  
  if (widget->IsComboBox()){
    GDLWidgetComboBox *combo = (GDLWidgetComboBox *) widget;
    
    static int SET_COMBOBOX_SELECT = e->KeywordIx( "SET_COMBOBOX_SELECT" );
    if (e->KeywordPresentAndDefined(SET_COMBOBOX_SELECT)) {
      DLongGDL* comboSelection =  e->GetKWAs<DLongGDL>(SET_COMBOBOX_SELECT);
      if (comboSelection->N_Elements() > 1) e->Throw( "Expression must be a scalar or 1 element array in this context:");
      combo->SelectEntry((*comboSelection)[0]);
    }
    static int COMBOBOX_ADDITEM = e->KeywordIx( "COMBOBOX_ADDITEM" );
    static int COMBOBOX_DELETEITEM = e->KeywordIx( "COMBOBOX_DELETEITEM" );
    static int COMBOBOX_INDEX = e->KeywordIx( "COMBOBOX_INDEX" );
    if (e->KeywordPresentAndDefined(COMBOBOX_ADDITEM)) {
      DLong pos=-1;
      DString value="";
      e->AssureStringScalarKWIfPresent(COMBOBOX_ADDITEM, value);
      e->AssureLongScalarKWIfPresent(COMBOBOX_INDEX, pos);
      combo->AddItem(value,pos);
    } 
    if (e->KeywordPresent(COMBOBOX_DELETEITEM)) {
      DLong pos=-1;
      e->AssureLongScalarKWIfPresent(COMBOBOX_DELETEITEM, pos);
      combo->DeleteItem(pos);
    } 
  }
  
  if (widget->IsDraw()){
    GDLWidgetDraw *draw = (GDLWidgetDraw *) widget;
    static int GET_DRAW_VIEW = e->KeywordIx( "GET_DRAW_VIEW" );
    if (e->KeywordPresent(GET_DRAW_VIEW)) {
      e->AssureGlobalKW( GET_DRAW_VIEW );
      BaseGDL** drwKW = &e->GetTheKW( GET_DRAW_VIEW );
       if (drwKW!=NULL) GDLDelete((*drwKW));
      DLongGDL* res= new DLongGDL(dimension(2));
      wxPoint pos=draw->GetPos();
       (*res)[0]=pos.x;
       (*res)[1]=pos.y;
       *drwKW=res->Dup();
     }
    static int SET_DRAW_VIEW = e->KeywordIx( "SET_DRAW_VIEW" );
    if (e->KeywordPresentAndDefined(SET_DRAW_VIEW)) {
      DLongGDL* pos=e->IfDefGetKWAs<DLongGDL>(SET_DRAW_VIEW);
      int x=(*pos)[0];
      int y=x; //yes.
      if (pos->N_Elements()>1) y=(*pos)[1];
      draw->SetPos(x,y);
    }
  }
  
  if (widget->IsTab()) {
    GDLWidgetTab *tab = (GDLWidgetTab *) widget;
    static int SET_TAB_CURRENT =e->KeywordIx( "SET_TAB_CURRENT" );
    DLong tabcurrent=-1;
    e->AssureLongScalarKWIfPresent(SET_TAB_CURRENT,tabcurrent);
    if (tabcurrent>-1) tab->SetTabCurrent(tabcurrent);
  }
  
  if (widget->IsTable()) {
    GDLWidgetTable *table = (GDLWidgetTable *) widget;
    static int ALIGNMENT = e->KeywordIx( "ALIGNMENT" );
    static int BACKGROUND_COLOR = e->KeywordIx( "BACKGROUND_COLOR" );
    static int COLUMN_LABELS = e->KeywordIx( "COLUMN_LABELS" );
    static int COLUMN_WIDTHS = e->KeywordIx( "COLUMN_WIDTHS" );
    static int DELETE_COLUMNS = e->KeywordIx( "DELETE_COLUMNS" );
    static int DELETE_ROWS = e->KeywordIx( "DELETE_ROWS" );
    static int FONT = e->KeywordIx( "FONT" );
    static int FOREGROUND_COLOR = e->KeywordIx( "FOREGROUND_COLOR" );
    static int INSERT_COLUMNS = e->KeywordIx( "INSERT_COLUMNS" );
    static int INSERT_ROWS = e->KeywordIx( "INSERT_ROWS" );
    static int ROW_LABELS = e->KeywordIx( "ROW_LABELS" );
    static int ROW_HEIGHTS = e->KeywordIx( "ROW_HEIGHTS" );
    static int SET_TABLE_SELECT = e->KeywordIx( "SET_TABLE_SELECT" );
    static int SET_TABLE_VIEW = e->KeywordIx( "SET_TABLE_VIEW" );
//    static int TAB_MODE = e->KeywordIx( "TAB_MODE" );
//    static int TABLE_BLANK = e->KeywordIx( "TABLE_BLANK" );
    static int TABLE_DISJOINT_SELECTION = e->KeywordIx( "TABLE_DISJOINT_SELECTION" );
    static int TABLE_XSIZE = e->KeywordIx( "TABLE_XSIZE" );
    static int TABLE_YSIZE = e->KeywordIx( "TABLE_YSIZE" );
    static int EDIT_CELL = e->KeywordIx("EDIT_CELL");

    static int AM_PM = e->KeywordIx( "AM_PM" );
    DStringGDL* amPm = GetKeywordAs<DStringGDL>(e, AM_PM);
    //check
    if (amPm) {if (amPm->N_Elements()!=2) e->Throw("% Keyword array parameter AM_PM must have 2 elements.");else table->SetAmPm(amPm);}
    static int DAYS_OF_WEEK = e->KeywordIx( "DAYS_OF_WEEK" );
    DStringGDL* daysOfWeek = GetKeywordAs<DStringGDL>(e, DAYS_OF_WEEK);
    //check
    if (daysOfWeek) {if (daysOfWeek->N_Elements()!=2) e->Throw("% Keyword array parameter DAYS_OF_WEEK must have 7 elements."); else table->SetDOW(daysOfWeek);}
    static int MONTHS = e->KeywordIx( "MONTHS" );
    DStringGDL* month = GetKeywordAs<DStringGDL>(e, MONTHS);
    //check
    if (month) {if (month->N_Elements()!=12) e->Throw("% Keyword array parameter MONTH must have 12 elements."); else table->SetMonth(month);}

    DByteGDL* alignment = GetKeywordAs<DByteGDL>(e, ALIGNMENT);
    DByteGDL* backgroundColor = GetKeywordAs<DByteGDL>(e, BACKGROUND_COLOR);
    DStringGDL* columnLabels = GetKeywordAs<DStringGDL>(e, COLUMN_LABELS);
    DLongGDL* columnWidth = GetKeywordAs<DLongGDL>(e, COLUMN_WIDTHS);
    bool hasColumnsToDelete = e->KeywordPresent(DELETE_COLUMNS);
    bool hasRowsToDelete = e->KeywordPresent(DELETE_ROWS);

    bool insertColumns = e->KeywordPresent(INSERT_COLUMNS);
    int columnsToInsert = 0;
    if (insertColumns) e->AssureLongScalarKWIfPresent(INSERT_COLUMNS,columnsToInsert);

    bool insertRows = e->KeywordPresent(INSERT_ROWS);
    int rowsToInsert = 0;
    if (insertRows) e->AssureLongScalarKWIfPresent(INSERT_ROWS,rowsToInsert);
    
    DByteGDL* foregroundColor = GetKeywordAs<DByteGDL>(e, FOREGROUND_COLOR);
    DLongGDL* rowHeights = GetKeywordAs<DLongGDL>(e, ROW_HEIGHTS);
    DStringGDL* rowLabels = GetKeywordAs<DStringGDL>(e, ROW_LABELS);

    bool setATableView = e->KeywordPresent(SET_TABLE_VIEW);
    DLongGDL* tableView = GetKeywordAs<DLongGDL>(e, SET_TABLE_VIEW);

    bool editcell = e->KeywordPresent(EDIT_CELL);
    DLongGDL* cellToEdit = GetKeywordAs<DLongGDL>(e, EDIT_CELL);
   
    bool setATableSelection = e->KeywordPresent(SET_TABLE_SELECT);
    DLongGDL* tableSelectionToSet = GetKeywordAs<DLongGDL>(e, SET_TABLE_SELECT);
    if (setATableSelection) { //check further a bit...
      if (table->GetDisjointSelection()) {
        if (tableSelectionToSet->Dim(0) != 2) e->Throw( "Array must have dimensions of (2, N): " + e->GetString( SET_TABLE_SELECT ) );
      } else {
        if (tableSelectionToSet->Rank() != 1 || tableSelectionToSet->Dim(0) != 4 ) e->Throw( "Array must have dimensions of (4): " + e->GetString( SET_TABLE_SELECT ) );
      }
      table->SetSelection(tableSelectionToSet);
    }
    
	DLongGDL* tableSelectionToUse = GetKeywordAs<DLongGDL>(e, USE_TABLE_SELECT);
	bool useATableSelection = table->GetValidTableSelection(tableSelectionToUse); //will throw if a syntax problem
    
    bool tablexsize=e->KeywordSet(TABLE_XSIZE);
    bool tableysize=e->KeywordSet(TABLE_YSIZE);
    bool justxsize=e->KeywordSet(XSIZE);
    bool justysize=e->KeywordSet(YSIZE);

    bool hasTableDisjointSelection = e->KeywordPresent(TABLE_DISJOINT_SELECTION);
    if (hasTableDisjointSelection) {
      bool oldDisjoint = table->GetDisjointSelection();
      bool newDisjoint = e->KeywordSet( TABLE_DISJOINT_SELECTION );
      if (oldDisjoint != newDisjoint) {
        table->ClearSelection();
        table->SetDisjointSelection(newDisjoint);
      }
    }
    
    bool hasAlignment=(alignment!=NULL);
    if (hasAlignment) {
      table->SetAlignment(alignment);
       if (useATableSelection) table->DoAlign(tableSelectionToUse); else table->DoAlign();
    }
    bool hasBackgroundColor=(backgroundColor!=NULL);
    if (hasBackgroundColor) {
      if (backgroundColor->N_Elements()%3 !=0 ) e->Throw("Table grid color attribute has the wrong dimensions.");
      table->SetBackgroundColor(backgroundColor);
       if (useATableSelection) table->DoBackgroundColor(tableSelectionToUse); else table->DoBackgroundColor();
    }
    bool hasForegroundColor=(foregroundColor!=NULL);
    if (hasForegroundColor) {
      if (foregroundColor->N_Elements()%3 !=0 ) e->Throw("Table grid color attribute has the wrong dimensions.");
      table->SetForegroundColor(foregroundColor);
      if (useATableSelection) table->DoForegroundColor(tableSelectionToUse); else table->DoForegroundColor(); 
    }
    bool hasColumnLabels=(columnLabels!=NULL);
    if (hasColumnLabels) {
      table->SetColumnLabels(columnLabels);
      table->DoColumnLabels();
    }
    bool hasRowLabels=(rowLabels!=NULL);
    if (hasRowLabels) {
      table->SetRowLabels(rowLabels);
      table->DoRowLabels();
    }
    bool hasColumnWidth=(columnWidth!=NULL);
    if (hasColumnWidth) {
      if (unitsGiven) widget->ChangeUnitConversionFactor(e); //pass new conversion factor
      else widget->SetCurrentUnitConversionFactor(wxRealPoint(1.,1.)); //force to use pixels
      table->SetColumnWidth(columnWidth);
      if (useATableSelection) table->DoColumnWidth(tableSelectionToUse); else  table->DoColumnWidth();
    }
    bool hasRowHeights=(rowHeights!=NULL);
    if (hasRowHeights) {
      if (unitsGiven) widget->ChangeUnitConversionFactor(e); //pass new conversion factor
      else widget->SetCurrentUnitConversionFactor(wxRealPoint(1.,1.)); //force to use pixels
      table->SetRowHeights(rowHeights);
      if (useATableSelection) table->DoRowHeights(tableSelectionToUse); else table->DoRowHeights();
    }
    if (hasColumnsToDelete) {
	  if (table->GetMajority()==GDLWidgetTable::ROW_MAJOR)  e->Throw("Unable to delete rows and/or columns in table widget.");
      if (useATableSelection) table->DeleteColumns(tableSelectionToUse); else table->DeleteColumns();
    }
    if (hasRowsToDelete) {
	  if (table->GetMajority()==GDLWidgetTable::COLUMN_MAJOR)  e->Throw("Unable to delete rows and/or columns in table widget.");  
      if (useATableSelection) table->DeleteRows(tableSelectionToUse); else table->DeleteRows();
    }
    if (insertRows) {
	  if (table->GetMajority()==GDLWidgetTable::COLUMN_MAJOR) e->Throw("Unable to insert rows and/or columns in table widget.");
      bool success;
      if (useATableSelection)  success=table->InsertRows(rowsToInsert,false,tableSelectionToUse); else  success=table->InsertRows(rowsToInsert,true);
      if (!success) e->Throw("Error adding Row(s).");
    } 
    if (insertColumns) {
	  if (table->GetMajority()==GDLWidgetTable::ROW_MAJOR) e->Throw("Unable to insert rows and/or columns in table widget.");
      bool success;
      if (useATableSelection) success=table->InsertColumns(columnsToInsert,false,tableSelectionToUse); else success=table->InsertColumns(columnsToInsert,true);
      if (!success) e->Throw("Error adding Column(s).");
    }
    if (setATableView) {if ( tableView->N_Elements() !=2 ) e->Throw("Table view attribute has the wrong dimensions.");
      table->SetTableView(tableView);
    }
    if (editcell) {if ( cellToEdit->N_Elements() !=2 ) e->Throw("Edit cell attribute has the wrong dimensions.");
      table->MakeCellEditable(cellToEdit);
    }
    if (tablexsize) {
      DLong xsize= (*e->GetKWAs<DLongGDL>(TABLE_XSIZE))[0];
      table->SetTableXsizeAsNumberOfColumns(xsize);
    } else if (justxsize) {
      DLong xsize= (*e->GetKWAs<DLongGDL>(XSIZE))[0];
      table->SetTableXsizeAsNumberOfColumns(xsize);
    }
    if (tableysize) {
      DLong ysize= (*e->GetKWAs<DLongGDL>(TABLE_YSIZE))[0];
      table->SetTableYsizeAsNumberOfRows(ysize);
    } else if (justysize) {
      DLong ysize= (*e->GetKWAs<DLongGDL>(YSIZE))[0];
      table->SetTableYsizeAsNumberOfRows(ysize);
    }
  }

  if (widget->IsTree()) {
	  static int SET_TREE_SELECT = e->KeywordIx("SET_TREE_SELECT");
	  bool setTreeselect = e->KeywordPresent(SET_TREE_SELECT);
	  static int SET_TREE_INDEX = e->KeywordIx("SET_TREE_INDEX");
	  bool setTreeindex = e->KeywordPresent(SET_TREE_INDEX);
	  static int SET_DRAG_NOTIFY = e->KeywordIx("SET_DRAG_NOTIFY");
	  bool setDragnotify = e->KeywordPresent(SET_DRAG_NOTIFY);
	  static int SET_DRAGGABLE = e->KeywordIx("SET_DRAGGABLE");
	  bool setDraggable = e->KeywordPresent(SET_DRAGGABLE);
	  static int SET_TREE_CHECKED = e->KeywordIx("SET_TREE_CHECKED");
	  bool setTreechecked = e->KeywordPresent(SET_TREE_CHECKED);
	  static int SET_TREE_BITMAP = e->KeywordIx("SET_TREE_BITMAP");
	  bool setTreebitmap = e->KeywordPresent(SET_TREE_BITMAP);
	  static int SET_MASK = e->KeywordIx("SET_MASK");
	  bool setTreemask = e->KeywordPresent(SET_MASK);

	  static int SET_TREE_EXPANDED = e->KeywordIx("SET_TREE_EXPANDED");
	  bool setTreeexpanded = e->KeywordPresent(SET_TREE_EXPANDED);
	  static int SET_TREE_VISIBLE = e->KeywordIx("SET_TREE_VISIBLE");
	  bool setTreevisible = e->KeywordSet(SET_TREE_VISIBLE);
      if (setTreeexpanded) static_cast<GDLWidgetTree *>(widget)->DoExpand(e->KeywordSet(SET_TREE_EXPANDED));
      else if (setDraggable) {
        DLong what=-1;
        e->AssureLongScalarKWIfPresent(SET_DRAGGABLE,what);
        if (what > 1 || what < -1) what=-1;
        static_cast<GDLWidgetTree *>(widget)->SetDragability(what);
      }
      else if (setTreechecked) static_cast<GDLWidgetTree *> (widget)->CheckItem(e->KeywordSet(SET_TREE_CHECKED));
      else if (setTreevisible) static_cast<GDLWidgetTree *> (widget)->SetVisible();
      if (drop_events) { //drop_events are INHERITED for TREE 
        DLong what=-1; //default value: inherit
        e->AssureLongScalarKWIfPresent(SET_DROP_EVENTS,what);
        if (what > 1 || what < -1) what=-1;
        static_cast<GDLWidgetTree *>(widget)->SetDropability(what);
      }
      else if (setTreeselect) {
        bool what=e->KeywordSet(SET_TREE_SELECT);
        static_cast<GDLWidgetTree *>(widget)->Select(what);
      } else if (setTreeindex) {
        DLong what=-1; e->AssureLongScalarKWIfPresent(SET_TREE_INDEX,what);
        static_cast<GDLWidgetTree *>(widget)->SetTreeIndex(what);
      } else if (setDragnotify) {
        DString dragNotify;
        e->AssureStringScalarKW( SET_DRAG_NOTIFY, dragNotify );
        dragNotify=StrUpCase(dragNotify); //UPPERCASE
        if (dragNotify.size() > 0) static_cast<GDLWidgetTree *>(widget)->SetDragNotify(dragNotify);
      }
      else if (setTreebitmap) {
        wxBitmap* bitmap;
          DByteGDL* testByte = e->GetKWAs<DByteGDL>(SET_TREE_BITMAP);
          if (testByte) {
            if (testByte->Rank() == 3 && testByte->Dim(2) == 3) {
              BaseGDL* transpose = testByte->Transpose(NULL);
              transpose->Reverse(2); //necessary 
              wxImage tryImage = wxImage(transpose->Dim(1), transpose->Dim(2), static_cast<unsigned char*> (transpose->DataAddr()), true); //STATIC DATA I BELIEVE.
              bitmap = new wxBitmap(tryImage);
              if (setTreemask) {
                unsigned char r = tryImage.GetRed(0, 0);
                unsigned char g = tryImage.GetGreen(0, 0);
                unsigned char b = tryImage.GetBlue(0, 0);
                wxMask* m = new wxMask(*bitmap, wxColour(r, g, b));
                bitmap->SetMask(m);
              }
              GDLDelete(transpose);
            } else {
              if (testByte->Rank() == 0 && (*testByte)[0] == 0) { //do nothing! yet another IDL trick: will use a default system bitmap.
              } else e->Throw("Invalid bitmap array. Bitmap array must have dimensions of [M,N,3] or [M,N,4].");
            }
          }
          GDLWidgetTree *t=static_cast<GDLWidgetTree *>(widget);
          t->SetBitmap(bitmap);
          t->SetMask(setTreemask); 
        }
      }
  
    // This is the sole programmatic entry where a widget can be destroyed, apart 2 other special cases:
    // - the destruction of a toplevel widget using a (trapped, managed) click on the close window.
    // - the destruction of a widget induced by its parent destruction or a group leader.
    if (destroy) {
      WidgetIDT id;
      gdlwxFrame* local_topFrame;
      bool reconnect = widget->DisableSizeEvents(local_topFrame, id);
      if (id == widgetID) reconnect = false; //no need reconnec a destroyed widget...
      // call KILL_NOTIFY procedures
      widget->OnKill();

      // widget may have been killed by above OnKill:
      widget = GDLWidget::GetWidget(widgetID);
      if (widget != NULL) {
        if (widget->IsDraw()) {
          GDLWidgetDraw* d = static_cast<GDLWidgetDraw*> (widget);
          gdlwxGraphicsPanel* draw = static_cast<gdlwxGraphicsPanel*> (d->GetWxWidget());
          draw->DeleteUsingWindowNumber(); //just emit quivalent to "wdelete,winNum".
        } else delete widget;

        if (reconnect) GDLWidget::EnableSizeEvents(local_topFrame, id);
      }
      return;
    }
  
#endif
}
#ifdef HAVE_WXWIDGETS_PROPERTYGRID

// WIDGET_PROPERTYSHEET
  BaseGDL* widget_property( EnvT* e)
  {
    e->Throw( "Widget_property unsupported, FIXME." );
  }
  // possible code:
//#ifndef HAVE_LIBWXWIDGETS
//  e->Throw( "GDL was compiled without support for wxWidgets" );
//  return NULL; // avoid warning
//#else
//  SizeT nParam = e->NParam( 1 );
//
//  DLongGDL* p0L = e->GetParAs<DLongGDL>(0);
//  WidgetIDT parentID = (*p0L)[0];
//  GDLWidget *widget = GDLWidget::GetWidget( parentID );
//  if (widget==NULL) e->Throw( "Invalid widget identifier: " + i2s( parentID ) );
//  
//  GDLWidgetPropertySheet* ps = new GDLWidgetPropertySheet( parentID, e);
//  if (ps->GetWidgetType()==GDLWidget::WIDGET_UNKNOWN )   ps->SetWidgetType( GDLWidget::WIDGET_PROPERTYSHEET );
//
//  return new DLongGDL( ps->WidgetID( ) );
//  }
//#endif
#endif

void widget_displaycontextmenu( EnvT* e ) { //Parent, X, Y, ContextBaseID 
#ifndef HAVE_LIBWXWIDGETS
  e->Throw( "GDL was compiled without support for wxWidgets" );
#else
  SizeT nParam = e->NParam();
  if (nParam != 4) e->Throw( "Incorrect number of arguments.");

  
  DLong  parent = 0;
  e->AssureLongScalarPar(0,parent); if ( parent == 0 ) e->Throw( "Invalid widget identifier: " + i2s( parent ) );
  GDLWidget *master = GDLWidget::GetWidget( parent );
  if ( master == NULL ) e->Throw( "Invalid widget identifier: " + i2s( parent ) );
  wxWindow* parentWindow=dynamic_cast<wxWindow*>(master->GetWxWidget());
  if (parentWindow) {
    DLong x=-1; e->AssureLongScalarPar(1,x); if ( x < 0 ) e->Throw( "X position for context menu not valid: " + i2s( x ) );
    DLong y=-1; e->AssureLongScalarPar(2,y); if ( y < 0 ) e->Throw( "Y position for context menu not valid: " + i2s( y ) );

    DLong  id = 0;
    e->AssureLongScalarPar(3, id); if ( id == 0 ) e->Throw( "Invalid widget identifier: " + i2s( id ) );

    GDLWidget *slave = GDLWidget::GetWidget( id );
    if ( slave == NULL ) e->Throw( "Invalid widget identifier: " + i2s( id ) );
    wxMenu* transient=dynamic_cast<wxMenu*>(slave->GetWxWidget());
    if (transient) parentWindow->PopupMenu(transient);
  } else cerr<<"widget_displaycontextmenu(): on non-existent widget!"<<endl;
#endif
}

} // namespace library
