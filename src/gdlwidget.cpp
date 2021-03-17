/***************************************************************************
                          gdlwidget.cpp  -  GDL widget system implementation 
                             -------------------
    begin                : Fri May 7 2004
    copyright            : (C) 2004 by Marc Schellens
    email                : m_schellens@users.sourceforge.net
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

#ifdef HAVE_LIBWXWIDGETS

#include <memory> 

#include <wx/grid.h>
#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>
#include <wx/textctrl.h>
#include <wx/menuitem.h>

#include "basegdl.hpp"
#include "dstructgdl.hpp"

#include "dinterpreter.hpp"


#include "gdlwxstream.hpp"

#include "gdlwidget.hpp"

#include "widget.hpp"
#include "graphicsdevice.hpp"

//must arrive after "gdlwidget.hpp"
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
#include <random>
#include <chrono>
#endif

//abbrevs to avoid sending size events when changing sizes
#define START_CHANGESIZE_NOEVENT \
WidgetIDT id;\
gdlwxFrame* local_topFrame;\
bool reconnect=this->DisableSizeEvents(local_topFrame,id);

#define END_CHANGESIZE_NOEVENT if (reconnect) EnableSizeEvents(local_topFrame,id);

#define START_ADD_EVENTUAL_FRAME \
wxScrolled<wxPanel>* saveWidgetPanel = widgetPanel;\
wxSizer* frameSizer;\
if (frameWidth > 0) {\
  framePanel = new wxPanel(widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, gdlBORDER_EXT);\
  frameSizer = new wxBoxSizer(wxVERTICAL);\
  framePanel->SetSizer(frameSizer);\
  widgetPanel = static_cast<wxScrolled<wxPanel>*>(framePanel);\
}

#define END_ADD_EVENTUAL_FRAME \
  if (frameWidth > 0) {\
    theWxContainer = widgetPanel;\
    frameSizer->Add(static_cast<wxWindow*> (theWxWidget), ALLOWSTRETCH, wxALL|wxEXPAND , frameWidth);\
    frameSizer->Fit(static_cast<wxWindow*> (theWxWidget));\
    framePanel->Fit();\
    widgetPanel = saveWidgetPanel;\
    }

#define TIDY_WIDGET(xxx) \
  this->setFont();\
  if (widgetSizer) {\
    /*if we create a widget after the topwidget has been realized, insure the insertion is IDL-compliant for multicolumns, using the following; */\
    /* 1) recompute the base siser taking into account the additional widget to be inserted */\
    /* 2) recreate the adequate sizer */\
    /* 3) re-add all child windows (including this one) */\
    if (this->GetRealized()) {\
      GDLWidgetBase* b = static_cast<GDLWidgetBase*> (gdlParent);\
      b->ReorderForANewWidget(static_cast<wxWindow*> (theWxContainer), DONOTALLOWSTRETCH, widgetStyle | wxALL, xxx);\
    } else widgetSizer->Add(static_cast<wxWindow*> (theWxContainer), DONOTALLOWSTRETCH, widgetStyle | wxALL, xxx);\
  } else {\
    static_cast<wxWindow*> (theWxContainer)->SetPosition(wOffset);\
  }\
  widgetPanel->FitInside();

#define UPDATE_WINDOW { if (this->GetRealized()) UpdateGui(); }
#define REALIZE_IF_NEEDED { if (this->GetRealized()) this->OnRealize();  }

const WidgetIDT GDLWidget::NullID = 0;

// instantiation
WidgetListT GDLWidget::widgetList;

GDLEventQueue GDLWidget::eventQueue; // the event queue
GDLEventQueue GDLWidget::readlineEventQueue; // for process at command line level
bool GDLWidget::wxIsOn=false;
bool GDLWidget::handlersOk=false;
wxFont GDLWidget::defaultFont=wxNullFont; //the font defined by widget_control,default_font.
wxFont GDLWidget::systemFont=wxNullFont;  //the initial system font. This to behave as IDL


void GDLEventQueue::Purge()
{
  for ( SizeT i = 0; i < dq.size( ); ++i )
    delete dq[i];
  dq.clear( );
  //   isEmpty = true;
}

// removes all events for TLB 'topID'
void GDLEventQueue::Purge( WidgetIDT topID)
{
  for( long i=dq.size()-1; i>=0;--i)
  {
    DStructGDL* ev = dq[i];
    static int topIx = ev->Desc( )->TagIndex( "TOP" );
    assert( topIx == 1 );
    DLong top = (*static_cast<DLongGDL*> (ev->GetTag( topIx, 0 )))[0];
    if( top == topID)
    {
      delete ev;
      dq.erase( dq.begin( ) + i );
    }
  }
  //   isEmpty = true;
}
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
inline wxColour RandomWxColour() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  std::uniform_int_distribution<int> distribution(0,255);
  int r = distribution(generator);
  int g = distribution(generator);
  int b = distribution(generator);
  return wxColour(r,g,b);
}
#endif

//this seem to respect the order of preference if variuos align_xxx are passed to the widget.
inline int GDLWidget::labelTextAlignment()
{//this concerns only how the thext is written in the label. 
  // Top and bottom are not allowed in IDL.
      if ( alignment & gdlwALIGN_RIGHT ) return (wxALIGN_RIGHT|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
      if ( alignment & gdlwALIGN_CENTER ) return (wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE);
      if ( alignment & gdlwALIGN_LEFT ) return (wxALIGN_LEFT|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE); 
      return wxALIGN_CENTRE_HORIZONTAL|wxALIGN_CENTRE_VERTICAL|wxST_NO_AUTORESIZE;
}

inline int GDLWidget::buttonTextAlignment()
{ //this concerns only how the thext is written in the button. Apparently wxBU_RIGHT does not work, and center is absent.
  // Top and bottom are not allowed in IDL.
      if ( alignment & gdlwALIGN_RIGHT ) return wxBU_EXACTFIT|wxBU_RIGHT; //right does not work.
      if ( alignment & gdlwALIGN_CENTER ) return wxBU_EXACTFIT; //center does not exist
      if ( alignment & gdlwALIGN_LEFT ) return wxBU_EXACTFIT|wxBU_LEFT; //default, works
      return wxBU_EXACTFIT;
}

inline int GDLWidgetLabel::widgetAlignment()
{
  long myAlign = alignment;
  if (myAlign == gdlwALIGN_NOT) {
    myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
}
  //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
  if (myAlign == gdlwALIGN_NOT) return wxALIGN_CENTER;
  //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
  //ignore sets that do not concern the current layout (vetrtical or horizontal)
  if (this->GetMyParentBaseWidget()->IsVertical()) { //col=1 left, center, right
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  } else if (this->GetMyParentBaseWidget()->IsHorizontal()) { //row=1
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
  } else {
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  }
  return 0;
}

inline int GDLWidget::widgetAlignment()
{
  long myAlign = alignment;
  if (myAlign == gdlwALIGN_NOT) {
    myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
  }
  //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
  long expand=wxEXPAND;
  if (this->IsLabel()) expand=0; //labels are not expanded 
  if (this->IsDraw()) expand=0; //draw are not expanded 
  if (myAlign == gdlwALIGN_NOT) return expand|wxALIGN_LEFT|wxALIGN_TOP;
  //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
  //ignore sets that do not concern the current layout (vetrtical or horizontal)
  if (this->GetMyParentBaseWidget()->IsVertical()) { //col=1 left, center, right
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
  } else if (this->GetMyParentBaseWidget()->IsHorizontal()) { //row=1
    if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
    if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
    if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
    if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
  }
  return 0;
}

 void GDLWidget::setFont() {
   wxWindow* w = dynamic_cast<wxWindow*> (this->GetWxWidget());
   if (w != NULL) w->SetFont(font); //may be menu: this is ok
 }
 void GDLWidget::setFont(wxObject* o) {
   wxWindow* w = dynamic_cast<wxWindow*> (o);
   if (w != NULL) w->SetFont(font);
}

inline wxSizer* AddABaseSizer(DLong col, DLong row, bool grid, long space)
{
  if (row <= 0 && col <= 0) return NULL;
  //This to mimic the disposal of children in IDL's Bases, where the 'packing' is more clever
  //than in wxWidgets, we will ALWAYS use a ROW gridding, where the number of rows is
  //deduced from the number of cols and of children when this number is known...
  //..which is not the case for us until the wigdet is realized, and ReorderWidgets() is called.
  //So the col >1 case below is a bit irrelevant as it will be recomputed.
  //In this case (col >1) we do not create a base Sizer, it will be created in ReorderWidgets().
  //We also need to use a BoxSizer for col==1 or row==1 as only BoxSizer is able to strectch correctly.
  if (row==1 && space < 1) {
    wxBoxSizer* s=new wxBoxSizer(wxHORIZONTAL);
    return s;
}
  if (col==1 && space < 1) {
    wxBoxSizer* s=new wxBoxSizer(wxVERTICAL);
    return s;
  }
  // grid behaviour of Motif (IDL) is not as perfect as wxWidget's. We cannot use wxGridSizer here,
  // the trick is to set only the non-flexible grow mode

  if (row>0) {
    wxFlexGridSizer* s=new wxFlexGridSizer(row,0,space,space);
    if (grid) {
      s->SetFlexibleDirection(wxVERTICAL);
      s->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    }
    return s;
  }
//  if (col>0) { //will be treated only at realize time, since default order is not good.
//    return NULL;
//  }
   if (col>0) {
    wxFlexGridSizer* s=new wxFlexGridSizer(0,col,space,space);
    if (grid) {
      s->SetFlexibleDirection(wxHORIZONTAL);
      s->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
    }
    return s;
  }
  return NULL;
}

//returns the current (widget or default) fontSize, taking care a typical width is returned instead of 0 as it is the case for variable pitch fonts, at least under windows
inline wxSize GDLWidget::getFontSize() {
  wxSize fontSize = defaultFont.GetPixelSize();
  if (!font.IsSameAs(wxNullFont)) fontSize = font.GetPixelSize();
  // under Windows, as of today: problem getting sizes
  if (fontSize.x < 1 || fontSize.y < 1 ) { //do it ourselves
    wxScreenDC dc;
    dc.SetFont(font);
    fontSize=dc.GetTextExtent(wxString('M'));
  }
  return fontSize;
}

// return the size of text (pixels) as it will take if displayed with current or given font
wxSize GDLWidget::calculateTextScreenSize(std::string &s, wxFont testFont) {
  wxFont f=font; //current font
  if (testFont!=wxNullFont) f = testFont;
  wxScreenDC dc;
  dc.SetFont(f);
  return dc.GetTextExtent(wxString(s.c_str( ), wxConvUTF8));
}

inline wxSize GDLWidgetText::computeWidgetSize()
{
  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  int lineHeight=1.19*fontSize.y;
  if (wSize.x > 0) {
    widgetSize.x = (wSize.x) * fontSize.x;
  } else {
    //if (scrolled || noNewLine) widgetSize.x =  20 * fontSize.x; else
      widgetSize.x =  calculateTextScreenSize(lastValue).x+2*fontSize.x;//add 2 char wide for border.
  }
  
  if (nlines > 1 || scrolled ) widgetSize.x +=  gdlSCROLL_WIDTH_Y;
  
  //if xsize is not enough, wxWidget will add an X-direction scrollbar. Y must be higher
  
  if (wSize.y > 0) {
    widgetSize.y = wSize.y * lineHeight;
  } else {
    widgetSize.y = lineHeight;
  }

   if (scrolled)  widgetSize.y += gdlSCROLL_HEIGHT_X; 
   else if (nlines < 2 && !( wrapped || scrolled ) ) widgetSize.y+=2*gdlTEXT_YMARGIN;
   
  //but..
  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  int x = ceil(widgetSize.x);
  int y = ceil(widgetSize.y);
  return wxSize(x, y);
}

inline wxSize GDLWidgetList::computeWidgetSize()
{ 
  //default wxChoice sizing is 2 lines, lines are well-separated. default IDL is only 1 line. Thus we need to fix sizes 
  
  //widget text size is in LINES in Y and CHARACTERS in X. But overridden by scr_xsize et if present
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight=(fontSize.y<20)?fontSize.y*1.5:fontSize.y*1.2;
  if (wSize.x > 0) {
    widgetSize.x = (wSize.x+1) * fontSize.x;
  } else {
    widgetSize.x = ( maxlinelength+1 ) * fontSize.x;
  } 

  if (wSize.y > 0) {
    widgetSize.y = wSize.y * lineHeight;
  } else {
    widgetSize.y = lineHeight;
  }

  if (wSize.x > 0 && maxlinelength > wSize.x) widgetSize.y += gdlSCROLL_HEIGHT_X;
  if (nlines > wSize.y) widgetSize.x += gdlSCROLL_WIDTH_Y;
  widgetSize.y += 10;
  //but..
  if (wScreenSize.x > 0 ) widgetSize.x = wScreenSize.x;  //we need an integer value
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  int x=ceil(widgetSize.x); int y=ceil(widgetSize.y);
  return wxSize(x,y);
}

inline wxSize GDLWidgetLabel::computeWidgetSize()
{
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  //widget label size is in pixels.
  wxSize widgetSize = wSize; //start with wanted values.
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight = fontSize.y+2*gdlLABEL_SPACE ; //(fontSize.y < 20) ? fontSize.y * 1.2 : fontSize.y * 1.2;  
  
  if (wSize.x < 0) widgetSize.x =  calculateTextScreenSize(value).x+2*fontSize.x;//add 2 char wide for border. //fontSize.x*(value.size());
  if (wSize.y < 0) widgetSize.y = lineHeight;

  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  if (sunken) {widgetSize.y+=1; widgetSize.x+=1;}

  return widgetSize;
}

inline wxSize GDLWidgetMenuBarButton::computeWidgetSize()
{
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  //widget label size is in pixels.
  wxSize widgetSize = wSize; //start with wanted values.
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight = 2*fontSize.y; 
  std::string s=valueWxString.ToStdString();
  if (wSize.x < 0) widgetSize.x =  calculateTextScreenSize(s).x+2*fontSize.x;//add 2 char wide for border. //fontSize.x*(value.size());
  if (wSize.y < 0) widgetSize.y = lineHeight;

  if (wScreenSize.x > 0) widgetSize.x = wScreenSize.x;
  if (wScreenSize.y > 0) widgetSize.y = wScreenSize.y;

  return widgetSize;
}
inline wxSize GDLWidget::computeWidgetSize()
{
//here is a good place to make dynamic widgets static, since dynamic_resize is permitted only if size is not given.
  if (wSize.x > 0 || wSize.y > 0 || wScreenSize.x > 0 || wScreenSize.y > 0) dynamicResize=-1;
  wxSize widgetSize;
  if ( wSize.x > 0 ) widgetSize.x = wSize.x*unitConversionFactor.x;
  else widgetSize.x = wxDefaultSize.x;
//but..
  if (wScreenSize.x > 0) widgetSize.x=wScreenSize.x;
  
  if ( wSize.y > 0 )  widgetSize.y = wSize.y * unitConversionFactor.y; 
  else widgetSize.y = wxDefaultSize.y;
//but..
   if (wScreenSize.y > 0) widgetSize.y=wScreenSize.y;
  
  return widgetSize;
}

void GDLWidget::UpdateGui()
{
  START_CHANGESIZE_NOEVENT 
  
  WidgetIDT actID = parentID;
  while ( actID != GDLWidget::NullID ) {
    GDLWidget *widget = GetWidget( actID );
    wxPanel* p=widget->GetPanel();
    if (p) {
      wxSizer* s=p->GetSizer();
      if (s) s->Fit(p);  else p->Fit();
    }
    actID = widget->parentID;
  }
  this->GetMyTopLevelFrame()->Fit();
  END_CHANGESIZE_NOEVENT 
}

//Alternate version if there were sizing problems with the one above.
//void GDLWidget::UpdateGui()
//{
//  START_CHANGESIZE_NOEVENT 
//  
//  // go back to top parent widget while resizing each BASE containers since there must be some update needed.
//  WidgetIDT actID = widgetID;
//  while ( actID != GDLWidget::NullID ) {
//    GDLWidget *widget = GetWidget( actID ); //Not Necessarily a Base
//    if (widget->IsBase()) {
//      wxPanel* p=widget->GetPanel();
//      wxWindow* thisBaseOuterPanel=dynamic_cast<wxWindow*>(widget->GetWxContainer());
//      wxWindow* thisBaseInnerPanel=dynamic_cast<wxWindow*>(widget->GetWxWidget());
//      assert((p==thisBaseOuterPanel)||(p==thisBaseInnerPanel));
//      GDLWidgetBase* b=static_cast<GDLWidgetBase*>(widget);
//      if (b->IsScrolled()) thisBaseInnerPanel->FitInside(); else thisBaseInnerPanel->Fit();
//      while (thisBaseOuterPanel!=thisBaseInnerPanel) {
//        wxSizer* s=thisBaseInnerPanel->GetContainingSizer();
//        assert (s); //must exist as there are always sizers between theWxContainer ans theWxWidget
//        s->FitInside(thisBaseInnerPanel);
//        thisBaseInnerPanel=thisBaseInnerPanel->GetParent();
//      }
//    }
//    actID = widget->parentID;
//  }
//  this->GetMyTopLevelFrame()->Fit();
//  END_CHANGESIZE_NOEVENT 
//}

// widget from ID
GDLWidget* GDLWidget::GetWidget( WidgetIDT widID)
{
  if ( widID == GDLWidget::NullID ) return NULL;
  WidgetListT::iterator it = widgetList.find( widID );
  if ( it == widgetList.end( ) ) return NULL;
  return it->second;
}

// widget parent from ID
GDLWidget* GDLWidget::GetParent( WidgetIDT widID)
{
  GDLWidget *widget = GetWidget( widID );
  if ( widget == NULL )
    return NULL;
  WidgetIDT parentID = widget->parentID;
  GDLWidget *parent = GetWidget( parentID );
  return parent;
}
// widget parent
GDLWidget* GDLWidget::GetMyParent( ){return GetWidget( parentID ); }

void  GDLWidget::EnableWidgetUpdate(bool update){
   wxWindow * me = dynamic_cast<wxWindow*>(theWxWidget);
   if (me) {if (update) {if ( me->IsFrozen()) me->Thaw(); else me->Refresh(); } else me->Freeze();} else cerr<<"freezing unknown widget\n";
}
  
GDLWidgetBase* GDLWidget::GetMyBaseWidget()
{
  WidgetIDT actID = widgetID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return NULL;
    if ( widget->IsBase( ) ) 
      return static_cast<GDLWidgetBase*> (widget);
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

// this widget's first parent base widget
GDLWidgetBase* GDLWidget::GetMyParentBaseWidget( )
{
  WidgetIDT actID =  parentID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return NULL;
    if ( widget->IsBase( ) ) 
      return static_cast<GDLWidgetBase*> (widget);
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

WidgetIDT GDLWidget::GetBaseId( WidgetIDT widID)
{
  GDLWidget *widget;
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget* widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->IsBase( ) )
      return actID;
    assert( widget->parentID != GDLWidget::NullID );
    actID = widget->parentID;
  }
}

GDLWidgetTopBase* GDLWidget::GetTopLevelBaseWidget( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (GDLWidgetTopBase*)GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*> (widget);
    else
      actID = widget->parentID;
  }
}

GDLWidgetTopBase* GDLWidget::GetMyTopLevelBaseWidget()
{
  WidgetIDT actID = this->GetWidgetID();
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (GDLWidgetTopBase*)GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*> (widget);
    else
      actID = widget->parentID;
  }
}

gdlwxFrame* GDLWidget::GetMyTopLevelFrame()
{
  WidgetIDT actID = this->GetWidgetID();
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return (gdlwxFrame*) NULL;
    if ( widget->parentID == GDLWidget::NullID )
      return static_cast<GDLWidgetTopBase*>(widget)->GetTopFrame();
    else
      actID = widget->parentID;
  }
}
WidgetIDT GDLWidget::GetIdOfTopLevelBase( WidgetIDT widID)
{
  WidgetIDT actID = widID;
  while ( 1 ) {
    GDLWidget *widget = GetWidget( actID );
    if ( widget == NULL )
      return GDLWidget::NullID;
    if ( widget->parentID == GDLWidget::NullID )
      return actID;
    else
      actID = widget->parentID;
  }
}

/**
  @brief        content of theWxWidget has changed, make size changes appear if Dynamic property Set
 */
void GDLWidget::RefreshDynamicWidget() {
    if (this->IsDynamicResize()) {
      if (theWxContainer && theWxContainer!=theWxWidget) {
        wxPanel* p=static_cast<wxPanel*>(theWxContainer);
        wxWindow* w=static_cast<wxWindow*>(theWxWidget);
        if (p) {
          wxSizer* s=p->GetSizer();
          if (s) s->Fit(w);  else p->Fit();
        }
      }
      UpdateGui();
    }
}

void GDLWidget::HandleWidgetEvents()
{
  //make one loop for wxWidgets Events...
#if __WXMSW__ 
    wxTheApp->MainLoop(); //central loop for wxEvents!
#else
    wxTheApp->Yield();
#endif
  //treat our GDL events...
    DStructGDL* ev = NULL;
    while( (ev = GDLWidget::readlineEventQueue.Pop()) != NULL)
    {
      static int idIx = ev->Desc( )->TagIndex( "ID" ); // 0
      static int topIx = ev->Desc( )->TagIndex( "TOP" ); // 1
      static int handlerIx = ev->Desc( )->TagIndex( "HANDLER" ); // 2
      assert( idIx == 0 );
      assert( topIx == 1 );
      assert( handlerIx == 2 );

      ev = CallEventHandler( ev );

      if( ev != NULL)
      {
        WidgetIDT id = (*static_cast<DLongGDL*> (ev->GetTag( idIx, 0 )))[0];
        Warning( "Unhandled event. ID: " + i2s( id ) );
        GDLDelete( ev );
        ev = NULL;
      } 
    }
    if (wxIsBusy()) wxEndBusyCursor( );
//  }
}

void GDLWidget::PushEvent( WidgetIDT baseWidgetID, DStructGDL* ev) {
  // Get XmanagerActiveCommand status
  GDLWidget *baseWidget = GDLWidget::GetWidget( baseWidgetID );
  if ( baseWidget != NULL ) {
    bool xmanActCom = baseWidget->GetXmanagerActiveCommand( );
    if ( !xmanActCom ) { //blocking: events in eventQueue.
      //     wxMessageOutputStderr().Printf(_T("eventQueue.Push: %d\n"),baseWidgetID);
      eventQueue.Push( ev );
    } else { //non-Blocking: events in readlineeventQueue.
      //     wxMessageOutputStderr().Printf(_T("readLineEventQueue.Push: %d\n"),baseWidgetID);
      readlineEventQueue.Push( ev );
    }
  } else cerr << "NULL baseWidget (possibly Destroyed?) found in GDLWidget::PushEvent( WidgetIDT baseWidgetID=" << baseWidgetID << ", DStructGDL* ev=" << ev << "), please report!\n";
}

void GDLWidget::InformAuthorities(const std::string& message){
        // create GDL event struct
        DStructGDL* ev = new DStructGDL( "*WIDGET_RUNTIME_ERROR*" );
        ev->InitTag( "ID", DLongGDL( 0) );
        ev->InitTag( "TOP", DLongGDL( 0 ) );
        ev->InitTag( "HANDLER", DLongGDL( 0 ) );
        ev->InitTag( "MESSAGE", DStringGDL(message) );
          readlineEventQueue.PushFront( ev ); // push front (will be handled next)
}

bool GDLWidget::GetXmanagerBlock() 
{
  bool xmanBlock = false;
  WidgetListT::iterator it;
  // (*it).first is widgetID
  // (*it).second is pointer to widget

  bool managed;
  bool xmanActCom;

#ifdef GDL_DEBUG_WIDGETS
  std::cout << "+ GetXmanagerBlock: widgetList:" << std::endl;
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    std::cout << (*it).first << ": " << (*it).second->widgetID << "  parentID: " <<
    (*it).second->parentID << "  uname: " << (*it).second->uName << std::endl;
  }
  std::cout << "- GetXmanagerBlock: widgetList end" << std::endl;
#endif
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    // Only consider base widgets
    if ( (*it).second->parentID == GDLWidget::NullID ) {
      managed = (*it).second->GetManaged( );
      xmanActCom = (*it).second->GetXmanagerActiveCommand( );
    }
    if ( managed && !xmanActCom ) {
      xmanBlock = true;
      break;
    }
  }
  return xmanBlock;
}
DLong GDLWidget::GetNumberOfWidgets() {
  WidgetListT::iterator it;
  DLong result=0;
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) result++;
  return result;
}
BaseGDL* GDLWidget::GetWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index=0;
  DLongGDL* result=new DLongGDL(nw,BaseGDL::NOZERO);
  for ( it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    (*result)[index]=(*it).second->widgetID; index++;
  }
  return result;
}
BaseGDL* GDLWidget::GetManagedWidgetsList() {
  DLong nw=GetNumberOfWidgets();
  if (nw<=0) return new DLongGDL(0);
  WidgetListT::iterator it;
  SizeT index;
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) index++;
  }
  if (index<=0) return new DLongGDL(0);
  DLongGDL* result=new DLongGDL(index,BaseGDL::NOZERO);
  for (index=0, it = widgetList.begin( ); it != widgetList.end( ); ++it ) {
    if ((*it).second->GetManaged() == true) (*result)[index++]=(*it).second->widgetID;
  }
  return result;
}
// Init
void GDLWidget::Init()
{
  if (!wxInitialize()) {
    std::cerr << "WARNING: wxWidgets not initializing" << std::endl;
    return;
  }
 //set system font to something sensible now that wx is ON:
  if (forceWxWidgetsUglyFonts)
    systemFont = wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL) ;//  identical for me to GDLWidget::setDefaultFont(wxFont("Monospace 8"));
#if __WXMSW__ //update for windows:
    bool ok=systemFont.SetNativeFontInfoUserDesc(wxString("consolas 8"));  //consolas 8 is apparently the one most identical to linux courier 8 and IDL X11 default font.
    if (!ok) systemFont = wxFont(8, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL) ; 
#endif
    else systemFont = *wxSMALL_FONT; // close to IDL in size, but nicer (not fixed).

  //initially defaultFont and systemFont are THE SAME.
  defaultFont=systemFont;
  SetWxStarted();
  //use a phantom window to retrieve th exact size of scrollBars wxWidget give wrong values.
   gdlwxPhantomFrame* test = new gdlwxPhantomFrame();
   test->Hide();
   test->Realize();
   test->Destroy();
}
// UnInit
void GDLWidget::UnInit() {
  if (wxIsStarted()) {
    WidgetListT::iterator it;
    for (it = widgetList.begin(); it != widgetList.end(); ++it) {
      GDLWidget* w = (*it).second;
      if (w != NULL) delete w;
    }
    GDLWidget::HandleWidgetEvents();
    // the following cannot be done: once unitialized, the wxWidgets library cannot be safely initilized again.
    //    wxUninitialize( );
    UnsetWxStarted(); //reset handlersOk too.
  }
}

void GDLWidget::ConnectToDesiredEvents(){
  wxWindow* w=dynamic_cast<wxWindow*>(theWxWidget);
  assert (w!=NULL);
  //connect to general events
  w->Connect(widgetID, wxEVT_TIMER, wxTimerEventHandler(gdlwxFrame::OnWidgetTimer)); //always.
  if ( eventFlags & GDLWidget::EV_TRACKING ) {
    w->Connect(widgetID,wxEVT_ENTER_WINDOW, wxMouseEventHandler(gdlwxFrame::OnEnterWindow));
    w->Connect(widgetID,wxEVT_LEAVE_WINDOW, wxMouseEventHandler(gdlwxFrame::OnLeaveWindow));
  }
  if ( eventFlags & GDLWidget::EV_CONTEXT ) w->Connect(widgetID,wxEVT_CONTEXT_MENU, wxContextMenuEventHandler(gdlwxFrame::OnContextEvent));
  if ( eventFlags & GDLWidget::EV_KBRD_FOCUS ) {
      w->Connect(widgetID,wxEVT_SET_FOCUS, wxFocusEventHandler(gdlwxFrame::OnKBRDFocusChange));
      w->Connect(widgetID,wxEVT_KILL_FOCUS, wxFocusEventHandler(gdlwxFrame::OnKBRDFocusChange));
  }
  for (int i = 0; i < desiredEventsList.size(); ++i) {
      desiredEventsList[i]->w->Connect(widgetID, desiredEventsList[i]->t, desiredEventsList[i]->f);
  }
// useful to have no event yet produced at this stage?
//  w->DeletePendingEvents();
}

 bool GDLWidget::DisableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id) {
  GDLWidgetBase *tlb = this->GetMyTopLevelBaseWidget();
  tlbFrame = tlb->GetMyTopLevelFrame();
  id = tlb->GetWidgetID();
  bool disconnect = ((tlb->GetEventFlags() & GDLWidget::EV_SIZE) == GDLWidget::EV_SIZE);
  if (disconnect) tlbFrame->Disconnect(id, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
  return disconnect;
}

 void GDLWidget::EnableSizeEvents(gdlwxFrame* &tlbFrame,WidgetIDT &id) {
  if (tlbFrame) tlbFrame->Connect(id, wxEVT_SIZE, gdlSIZE_EVENT_HANDLER);
}
//initialize static member
int GDLWidget::gdl_lastControlId=0;

GDLWidget::GDLWidget( WidgetIDT p, EnvT* e, BaseGDL* vV, DULong eventFlags_)
: theWxWidget( NULL )
, theWxContainer(NULL)
, widgetID (0)
, parentID( p )
, uValue( NULL )
, vValue( vV )
, scrolled(false)
, sensitive(true)
, managed( false )
, eventFlags( eventFlags_ )
, exclusiveMode( 0 )
, wOffset(wxDefaultPosition)
, wSize(wxDefaultSize)
, wScreenSize(wxDefaultSize)
, widgetSizer( NULL )
, widgetPanel( NULL )
, framePanel( NULL )
, widgetType(GDLWidget::WIDGET_UNKNOWN)
, widgetName("")
, groupLeader(GDLWidget::NullID)
, unitConversionFactor(wxRealPoint(1.0,1.0)) //no conversion at start.
, frameWidth(0)
, font(defaultFont)
, valid(true)
, alignment(gdlwALIGN_NOT)
, widgetStyle(wxSTRETCH_NOT)
, dynamicResize(0) //unset
, eventFun("")
, eventPro("")
, killNotify("")
, notifyRealize("")
, proValue("")
, funcValue("")
, uName("")
{
  m_windowTimer = NULL;
  
  if ( e != NULL ) GetCommonKeywords( e ); else DefaultValuesInAbsenceofEnv();

// was   widgetID =  wxWindow::NewControlId( ); // but some scripts use the fact that widget ids are positive (graffer.pro)
  widgetID = GDLNewControlId( );
  
  if ( parentID != GDLWidget::NullID ) {

    GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    
    // a col or row sizer means no offset
    if (GetParentSizer()) wOffset=wxDefaultPosition;
    //there is a ParentID, so widgetPanel:
    widgetPanel=GetParentPanel();
      
    if ( gdlParent->IsContainer( ) ) { //only case, include some buttons
      GDLWidgetContainer* base = dynamic_cast<GDLWidgetContainer*> (gdlParent);
      base->AddChildID( widgetID );
    }
    else if ( gdlParent->IsMenu( ) )
    { 
      GDLWidgetMenu* menu = dynamic_cast<GDLWidgetMenu*> (gdlParent);
      if (menu) menu->AddChildID( widgetID );
    }
    else if ( gdlParent->IsMenuBar( ) )
    { 
       GDLWidgetMenuBar* mb = dynamic_cast<GDLWidgetMenuBar*> (gdlParent);
       mb->AddChildID( widgetID );
    }
//    else if ( gdlParent->IsTree( ) )
//    { 
//      // do nothing 
//    }//    else {    assert(false);} //other widgets are parent of popups, assert should be removed.
  }
  
  widgetList.insert( widgetList.end( ), std::pair<WidgetIDT, GDLWidget*>(widgetID, this) );
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "inserted: ID: %d  parentID: %d\n" ), widgetID, parentID );
#endif
  if (groupLeader != GDLWidget::NullID) {
    GDLWidget* leader=this->GetWidget(groupLeader);
    if (leader) leader->AddToFollowers(widgetID);
  }
}

void GDLWidget::SetSensitive(bool value)
{
    sensitive = value;
    wxWindow *me=dynamic_cast<wxWindow*>(theWxWidget); 
    if (me) {if (value) me->Enable(); else me->Disable();}
}

bool GDLWidget::GetSensitive()
{
  return sensitive;
}
void GDLWidget::SetFocus() //gives focus to the CHILD of the panel.
{
  wxWindow *me=dynamic_cast<wxWindow*>(this->GetWxWidget()); if (me!=NULL) me->SetFocus(); else cerr<<"Setting Focus for unknown widget!\n";
}
  void GDLWidget::SetWidgetPosition(DLong posx, DLong posy){
  // -1: not set ---> keep the same
    wxWindow* me=static_cast<wxWindow*>(theWxContainer);
    if (me) {
      wxPoint where=me->GetPosition();
      if (posx >= 0) where.x=posx;
      if (posy >= 0) where.y=posy;
      me->Move(where);
    } else cerr<<"set offset on non-existent widget!"<<endl;

    UPDATE_WINDOW;

  }

void GDLWidget::SetWidgetSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT
  
  //although 2 values are passed, one only may be set by the user.
  //the code is:
  // -1: not set --> keep the same
  // 0 : set, and means: do your best.
  // > 0 : imposed size.

  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me=dynamic_cast<wxWindow*>(theWxWidget);
  if (me==NULL) {
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize of non wxWindow %s\n"),this->GetWidgetName().c_str());
#endif
  return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize currentSize=%d,%d (%s)\n"),currentSize.x,currentSize.y,this->GetWidgetName().c_str());
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;
  
  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = currentBestSize.x;
    if (sizey == 0) sizey = currentBestSize.y;
  }
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize);
  //framed bases may have a complicated frame around, to be resized:
  if (theWxContainer != theWxWidget) {
    wxWindow* container = dynamic_cast<wxWindow*> (theWxContainer);
    wxSizer* s=me->GetSizer();
    if (s) s->Fit(me); else container->Fit();
  }
  UpdateGui();

  END_CHANGESIZE_NOEVENT
  

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}

void GDLWidget::SetWidgetScreenSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT

  //although 2 values are passed, one only may be set by the user.
  //the code is:
  // -1: not set --> keep the same
  // 0 : set, and means: do your best.
  // > 0 : imposed size.

  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me = dynamic_cast<wxWindow*> (theWxWidget);
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize = me->GetSize();
  wxSize currentBestSize = me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize currentSize=%d,%d (%s)\n"), currentSize.x, currentSize.y, this->GetWidgetName().c_str());
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;

  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = currentBestSize.x;
    if (sizey == 0) sizey = currentBestSize.y;
  }
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize); //includes scrollbars.
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize); //includes scrollbars.
  //framed bases may have a complicated frame around, to be resized:
  if (theWxContainer != theWxWidget) {
    wxWindow* container = dynamic_cast<wxWindow*> (theWxContainer);
    wxSizer* s = me->GetSizer();
    if (s) s->Fit(me);
    else container->Fit();
  }
  UpdateGui();

  END_CHANGESIZE_NOEVENT


#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidget::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

void GDLWidgetTabbedBase::SetBaseTitle(std::string &title_)
{
  GDLWidgetTab* parentTab=dynamic_cast<GDLWidgetTab*>(this->GetMyParent());
  if (parentTab) {
    wxNotebook* wxParent = dynamic_cast<wxNotebook*> (parentTab->GetWxWidget( ));
    wxParent->SetPageText(wxParent->FindPage(static_cast<wxWindow*>(theWxWidget)),wxString(title_.c_str(), wxConvUTF8));
  }
#ifdef GDL_DEBUG_WIDGETS
  else wxMessageOutputStderr( ).Printf( _T( "GDLWidgetTabbedBase::SetBaseTitle() no parent tab exists? %d\n"));
#endif
}

GDLWidgetTabbedBase::~GDLWidgetTabbedBase()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTabbedBase(" << widgetID << ")" << std::endl;
#endif
  GDLWidgetTab* parentTab=static_cast<GDLWidgetTab*>(this->GetMyParent());
  if (parentTab) { //may be already destroyed.
    wxNotebook* wxParent = static_cast<wxNotebook*> (parentTab->GetWxWidget( ));
    //whereAmI?
    myPage=wxParent->FindPage(static_cast<wxWindow*>(theWxContainer));
    if (wxParent) wxParent->RemovePage(myPage); //do not delete the page, GDL will delete the contents itself, widget per widget.
  }
#ifdef GDL_DEBUG_WIDGETS
  else wxMessageOutputStderr( ).Printf( _T( "~GDLWidgetTabbedBase() no parent tab exists? %d\n"));
#endif
}

void GDLWidget::Realize( bool map, bool use_default) {  GetMyTopLevelBaseWidget()->Realize(map, use_default); }

void GDLWidgetTopBase::Realize(bool map, bool use_default) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetTopBase:Realize: %d\n"), widgetID);
#endif
  if (use_default) map = GetMap();

  OnRealize();
  
 if (map) topFrame->Show() ; //endShowRequestEvent();
  else topFrame->Hide(); //SendHideRequestEvent();
  realized=true;
}

bool GDLWidget::GetRealized() {
    GDLWidgetTopBase *tlb = GetMyTopLevelBaseWidget();
    gdlwxFrame* topFrame = tlb->GetTopFrame();
    return (tlb->IsRealized());
  }
  void GDLWidgetContainer::OnRealize() {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetContainer::OnRealize: %d\n"), widgetID);
#endif
    //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
  this->setFont();
  this->SetSensitive(sensitive);

  for (std::deque<WidgetIDT>::reverse_iterator c = children.rbegin(); c != children.rend(); ++c) {
//  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidget* w = GetWidget(*c);
   if (w != NULL)
    w->OnRealize();
  }
  
  wxPanel* p=static_cast<wxPanel*> (theWxContainer);
  if (p) p->Fit(); 
#ifdef GDL_DEBUG_WIDGETS
    else wxMessageOutputStderr().Printf(_T("Unknown Container for (%s) widget ID %d\n"), widgetName, widgetID);
#endif

  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
  //define Events Last (TBC).
  ConnectToDesiredEvents();
 }
  
void GDLWidgetMenu::OnRealize() {
  //setFont() will set the font for this widget and children if FONT= is present in the WIDGET_XXX command (and is supported) 
  this->setFont();
  this->SetSensitive(sensitive);

  //Menu children are menus or entries, *NOT* any widget.
  for (std::deque<WidgetIDT>::reverse_iterator c = children.rbegin(); c != children.rend(); ++c) {
//  for (std::deque<WidgetIDT>::iterator c = children.begin(); c != children.end(); ++c) {
   GDLWidgetMenu* w = dynamic_cast<GDLWidgetMenu*>(GetWidget(*c));
   if (w != NULL)
    w->OnRealize();
  }

  if (notifyRealize != "") { //insure it is called once only for this.
   std::string note = notifyRealize;
   notifyRealize.clear();
   CallEventPro(note, new DLongGDL(widgetID));
  }
 }

#define GetSysC(x)   { col=wxSystemSettings::GetColour(x); r=col.Red();g=col.Green();b=col.Blue(); (*val)[0]=r;(*val)[1]=g;(*val)[2]=b; }
BaseGDL * GDLWidget::getSystemColours()
{
  DStructGDL* colo = new DStructGDL( "WIDGET_SYSTEM_COLORS");
  DIntGDL*  val=new DIntGDL( dimension(3), BaseGDL::NOZERO);
  int  r,g,b;
  wxColour col;
  GetSysC(wxSYS_COLOUR_3DDKSHADOW );
  colo->InitTag("DARK_SHADOW_3D",(*val));
  colo->InitTag("SHADOW_3D", (*val));
  GetSysC(wxSYS_COLOUR_3DLIGHT );
  colo->InitTag("FACE_3D", (*val));
  colo->InitTag("LIGHT_EDGE_3D", (*val));
  colo->InitTag("LIGHT_3D", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVEBORDER  );
  colo->InitTag("ACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_ACTIVECAPTION   );
  colo->InitTag("ACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_APPWORKSPACE    );
  colo->InitTag("APP_WORKSPACE", (*val));
  GetSysC(wxSYS_COLOUR_DESKTOP     );
  colo->InitTag("DESKTOP", (*val));
  GetSysC(wxSYS_COLOUR_BTNTEXT     );
  colo->InitTag("BUTTON_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_CAPTIONTEXT     );
  colo->InitTag("CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_GRAYTEXT     );
  colo->InitTag("GRAY_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHT     );
  colo->InitTag("HIGHLIGHT", (*val));
  GetSysC(wxSYS_COLOUR_HIGHLIGHTTEXT     );
  colo->InitTag("HIGHLIGHT_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVEBORDER     );
  colo->InitTag("INACTIVE_BORDER", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTION     );
  colo->InitTag("INACTIVE_CAPTION", (*val));
  GetSysC(wxSYS_COLOUR_INACTIVECAPTIONTEXT     );
  colo->InitTag("INACTIVE_CAPTION_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_INFOBK     );
  colo->InitTag("TOOLTIP_BK", (*val));
  GetSysC(wxSYS_COLOUR_INFOTEXT     );
  colo->InitTag("TOOLTIP_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_MENU     );
  colo->InitTag("MENU", (*val));
  GetSysC(wxSYS_COLOUR_MENUTEXT     );
  colo->InitTag("MENU_TEXT", (*val));
  GetSysC(wxSYS_COLOUR_SCROLLBAR     );
  colo->InitTag("SCROLLBAR", (*val));
  GetSysC(wxSYS_COLOUR_WINDOW     );
  colo->InitTag("WINDOW_BK", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWFRAME     );
  colo->InitTag("WINDOW_FRAME", (*val));
  GetSysC(wxSYS_COLOUR_WINDOWTEXT     );
  colo->InitTag("WINDOW_TEXT", (*val));
  return colo;
}

GDLWidget::~GDLWidget()
{
#ifdef GDL_DEBUG_WIDGETS
      std::string state=(this->IsValid())?" ":"(Already destroyed) "; 
      std::cout << "~GDLWidget "+state+"("<< widgetID <<")"<< std::endl;
#endif
        
  if (!this->IsValid()) return; //to avoid crashing if asserts are not honored
  //unvalidate widget to prevent some further actions
  this->SetUnValid();
  if (m_windowTimer) {if (m_windowTimer->IsRunning()) m_windowTimer->Stop();}

  // call KILL_NOTIFY procedures
  this->OnKill();

  // kill followers (here?)
  // delete all followers (in reverse order ?)
  while (!followers.empty()) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidget("<< widgetID <<"): destroy follower "<< followers.back()<< endl;
#endif
      GDLWidget* follower=GetWidget(followers.back()); followers.pop_back();
    if (follower) delete follower;
  }

  GDLWidget* gdlParent = GetMyParent();
  if (gdlParent) { //not the TLB
    gdlParent->RemoveIfFollower(widgetID);

//    UpdateGui();  //way too long! use a more subtle command!

  } else {
    static_cast<GDLWidgetTopBase*>(this)->GetTopFrame()->Hide();
    static_cast<GDLWidgetTopBase*>(this)->GetTopFrame()->Destroy(); //delete topBase
  }
  
  GDLDelete(uValue);
  GDLDelete(vValue);
  uName.clear();
  proValue.clear();
  funcValue.clear();
  eventPro.clear(); // event handler PRO
  eventFun.clear(); // event handler FUN
  notifyRealize.clear();
  killNotify.clear();
  valid=false; //if not, is in the process of being destroyed (prevent reentrance).

  widgetList.erase(widgetID);
  desiredEventsList.clear();
  followers.clear();
  widgetName.clear();
  
  theWxContainer=NULL;
  theWxWidget=NULL;
  widgetSizer=NULL;
  widgetPanel=NULL;
  framePanel=NULL;
}

void GDLWidget::Raise()
{
  this->GetMyTopLevelFrame()->Raise();
}

void GDLWidget::Lower()
{
  this->GetMyTopLevelFrame()->Lower();
}

DStructGDL* GDLWidget::GetGeometry(wxRealPoint fact)
{
  if (!this->GetRealized()) this->Realize(true,false);//necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  assert(container != NULL);
  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;

  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");

  wxWindow* test = dynamic_cast<wxWindow*> (theWxContainer);
  if (test==NULL) return ex;
  wxRect r=test->GetRect();
//  std::cerr<<"CONTAINER: x: "<<r.x<<", y: "<<r.y<<", width="<<r.width<<", height="<<r.height<<std::endl;
  xoffset = r.x ;
  yoffset = r.y ;
  wxWindow* w = dynamic_cast<wxWindow*> (theWxWidget);
  if (w==NULL) return ex;
  wxRect z=w->GetRect();
//  std::cerr<<"WIDGET: x: "<<z.x<<", y: "<<z.y<<", width="<<z.width<<", height="<<z.height<<std::endl;

  iscr_xsize = z.width;
  iscr_ysize = z.height;
  wxSize s=w->GetClientSize();
  ixsize = s.x; if (ixsize < 1) ixsize=iscr_xsize; //Patch for OSX where s.x is 0
  iysize = s.y; if (iysize < 1) iysize=iscr_ysize; //Patch for OSX where s.y is 0
  
  if (theWxWidget == theWxContainer) margin=0; else {
    margin=(r.width-z.width)/2; //Rect is about the container, which is the widgetPanel and not the frame depending on case.
  }
  
  if (this->IsInCharacters()) {
  wxSize fontSize = getFontSize();
  xsize = ixsize / fontSize.x;
  ysize = iysize / fontSize.y;
  } else {
  //size is in pixels, pass in requested units (1.0 default)
  xsize = ixsize / fact.x;
  ysize = iysize / fact.y;
  }
  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));
//special DRAW:
  if (this->IsDraw()) {
    wxSize d=w->GetVirtualSize();
    DFloat dxsize=d.x;
    DFloat dysize=d.y;
    ex->InitTag("DRAW_XSIZE", DFloatGDL(dxsize/fact.x));
    ex->InitTag("DRAW_YSIZE", DFloatGDL(dysize/fact.y));
  }

  return ex;
}

GDLWidgetContainer::GDLWidgetContainer( WidgetIDT parentID, EnvT* e, ULong eventFlags_)
: GDLWidget( parentID, e, NULL, eventFlags_)
{
}
  
  GDLWidgetContainer::~GDLWidgetContainer()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetContainer(" << widgetID <<")"<< std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child=GetWidget(children.back()); children.pop_back();

     if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetContainer, deleting child ID #" << child->GetWidgetID() << " of container  #" << widgetID << std::endl;
#endif
      delete child;
     }
  }
    
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy(); //which is the panel.
}

#ifdef PREFERS_MENUBAR  
   GDLWidgetMenuBar::~GDLWidgetMenuBar() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBar(" << widgetID << ")" << std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child=GetWidget(children.back()); children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenuBar, deleting child ID #" << child->GetWidgetID() << " of container  #" << widgetID << std::endl;
#endif
      delete child;
    }
  }
  //remove wxWidgets MBAR from wxFrame container otherwise wxFrame will attempt to destroy it again and segfault
  dynamic_cast<wxMenuBar*> (theWxWidget)->GetFrame()->SetMenuBar(NULL);
 }
#else
   GDLWidgetMenuBar::~GDLWidgetMenuBar() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBar(" << widgetID << ")" << std::endl;
#endif

  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child=GetWidget(children.back()); children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenuBar, deleting child ID #" << child->GetWidgetID() << " of container  #" << widgetID << std::endl;
#endif
      delete child;
    }
  }
  //toolbar can be removed
  dynamic_cast<wxToolBar*> (theWxWidget)->Destroy();
 }
#endif  
 /*********************************************************/
// for WIDGET_BASE
/*********************************************************/
GDLWidgetBase::GDLWidgetBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetContainer(parentID, e, eventFlags_)
, lastRadioSelection(NullID)
, nrows(row)
, ncols(col)
, grid(grid_layout)
, childrenAlignment(children_alignment)
, space(space_)
, xpad(xpad_)
, ypad(ypad_)
, doMap(mapWid) {
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

  wSize = computeWidgetSize();
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:

  if (x_scroll_size > 0) {scrolled=true;x_scroll_size*=unitConversionFactor.x;x_scroll_size+=gdlSCROLL_WIDTH_Y;} 
  if (y_scroll_size > 0) {scrolled=true;y_scroll_size*=unitConversionFactor.y;y_scroll_size+=gdlSCROLL_HEIGHT_X;}
  if (scrolled) {
    if (x_scroll_size < 1) x_scroll_size = gdlDEFAULT_XSIZE+gdlSCROLL_WIDTH_Y;
    if (y_scroll_size < 1) y_scroll_size = gdlDEFAULT_YSIZE+gdlSCROLL_HEIGHT_X;
  }
  wScrollSize = scrolled ? wxSize(x_scroll_size , y_scroll_size ) : wSize; //y_scroll_size + gdlSCROLL_HEIGHT_X);
  
  // Set exclusiveMode
  // If exclusive then set to -1 to signal first radiobutton
  if ( exclusiveMode_ == BGEXCLUSIVE )
  this->SetExclusiveMode( BGEXCLUSIVE1ST );
  else
    this->SetExclusiveMode( exclusiveMode_ );

  // COLUMN is default if col=0 and row=0 for exclusive etc bases. 
  if ( this->GetExclusiveMode() != BGNORMAL ) {
    if (ncols < 1 && nrows <1 ) ncols=1;
  }

}

// generic creation of a base in which widgets will be put.
//
// defines widgetPanel which is the (scrolled) panel in which children will be Added. 
// widgetPanel may be a panel inside a (scrolled) panel used to mimic padding if necessary. In ths case it is the padding panel that is scrolled.
// Further, all this casn be inside a frame.
//
// define the widgetSizer, the sizer that children will 'obey' (that will govern overall placement and size of children)
// the widgetSizer may be NULL, in which case the placement of children inside the panel will be governed by their respective size and position,
// but if widgetSizer exist, then everytime a new child is created, it must be invoked to resize/reposition this child.
//
void GDLWidgetBase::CreateBase(wxWindow* parent){
//the container is as ScrollPanel
  bool doFrame=true; //!(this->IsTopBase()); //IDL Prevents topBases to be framed (?).
  if (doFrame && frameWidth > 0) {
    wxScrolled<wxPanel>* frame = new wxScrolled<wxPanel>(parent, wxID_ANY, wOffset, wxDefaultSize, gdlBORDER_EXT); 
    theWxContainer=frame;
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
    frame->SetBackgroundColour(wxColour(0x60, 0xe0, 0x94)); //vert clair
#endif
    wxBoxSizer* panelsz = new wxBoxSizer(wxVERTICAL);
    frame->SetSizer(panelsz);
// Fancy variant:
//    int mode = wxBORDER_NONE;
//    int width = 2; //the size of wxBORDER_SUNKEN
//    if (frameWidth > 2) {
//      mode = wxBORDER_SIMPLE;
//      width = 3;
//    }
//    if (frameWidth > 3) {
//      mode = wxBORDER_RAISED;
//      width = 4;
//    }
//    wxPanel* frame_inside = new wxPanel(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, mode);
//    sz->Add(frame_inside, FRAME_ALLOWSTRETCH, wxALL, frameWidth - width);
//
//    wxBoxSizer* sz_inside = new wxBoxSizer(wxVERTICAL);
//    frame_inside->SetSizer(sz_inside);
//    widgetPanel = new wxScrolledWindow(frame_inside, wxID_ANY, wOffset, wxDefaultSize); 
    if (xpad > 0 || ypad > 0) {
      wxScrolled<wxPanel>* padxpady = new wxScrolled<wxPanel>(frame);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      padxpady->SetBackgroundColour(wxColour(0xa7, 0x3d, 0x0f)); //orange fonce
#endif
      wxGridBagSizer* sz = new wxGridBagSizer(ypad, xpad);
      padxpady->SetSizer(sz);
      sz->SetEmptyCellSize(wxSize(0, 0));
      if (xpad > 1) sz->Add(0, 0, wxGBPosition(1, 2));
      if (ypad > 1) sz->Add(0, 0, wxGBPosition(2, 1));

      widgetPanel = new wxScrolledWindow(padxpady, wxID_ANY, wOffset, wxDefaultSize); 
      sz->Add(widgetPanel, wxGBPosition(1, 1));
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      padxpady->SetSize(wScrollSize);
      padxpady->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        padxpady->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        padxpady->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
      panelsz->Add(padxpady, FRAME_ALLOWSTRETCH, wxALL | wxEXPAND, frameWidth);//gdlFRAME_MARGIN);
      panelsz->Fit(padxpady);
    } else {
      widgetPanel = new wxScrolledWindow(frame, wxID_ANY, wOffset, wxDefaultSize); 
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      widgetPanel->SetSize(wScrollSize);
      widgetPanel->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        widgetPanel->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        widgetPanel->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
      panelsz->Add(widgetPanel, FRAME_ALLOWSTRETCH, wxALL | wxEXPAND, frameWidth);//gdlFRAME_MARGIN);
      panelsz->Fit(widgetPanel);
    }
    theWxWidget = widgetPanel;
  } else {
    if (xpad > 0 || ypad > 0 ) {
      wxScrolled<wxPanel>* padxpady = new wxScrolled<wxPanel>(parent);
      theWxContainer = padxpady;
  #ifdef GDL_DEBUG_WIDGETS_COLORIZE
      padxpady->SetBackgroundColour(wxColour(0xa7, 0x3d, 0x0f)); //orange fonce
  #endif
      wxGridBagSizer* sz = new wxGridBagSizer(ypad, xpad);
      padxpady->SetSizer(sz);
      sz->SetEmptyCellSize(wxSize(0, 0));
      if (xpad > 1) sz->Add(0, 0, wxGBPosition(1, 2));
      if (ypad > 1) sz->Add(0, 0, wxGBPosition(2, 1));

      widgetPanel = new wxScrolledWindow(padxpady, wxID_ANY, wOffset, wxDefaultSize); 
      sz->Add(widgetPanel, wxGBPosition(1, 1));     
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
      //    widgetPanel->SetVirtualSize(wSize);
      padxpady->SetSize(wScrollSize);
      padxpady->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        padxpady->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        padxpady->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
    } else {
      widgetPanel = new wxScrolledWindow(parent, wxID_ANY, wOffset, wxDefaultSize); 
      theWxContainer = widgetPanel;
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
      widgetPanel->SetBackgroundColour(RandomWxColour());
#endif
    //    widgetPanel->SetVirtualSize(wSize);
      widgetPanel->SetSize(wScrollSize);
      widgetPanel->SetMinSize(wScrollSize);
      //Just Enable scrollBars if scrolling is necessary
      if (scrolled) {
        widgetPanel->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
        widgetPanel->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
      }
    }
    
    theWxWidget = widgetPanel;
  }


//define inside sizer.  
  widgetSizer = AddABaseSizer(ncols, nrows, grid, space);
  if (widgetSizer) widgetPanel->SetSizer(widgetSizer);

  wxSizer* parentSizer = parent->GetSizer();
  if (parentSizer) parentSizer->Add(static_cast<wxWindow*>(theWxContainer), DONOTALLOWSTRETCH, wxALL | widgetAlignment(), gdlSPACE);
}

 void GDLWidgetBase::SetWidgetSize(DLong sizex, DLong sizey) 
{
  START_CHANGESIZE_NOEVENT
  
  wxWindow* me=dynamic_cast<wxWindow*>(theWxWidget);
  if (me==NULL) {
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize of non wxWindow %s\n"),this->GetWidgetName().c_str());
#endif
  return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentVirtualSize=me->GetVirtualSize();
  wxSize currentSize=me->GetSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetBase::SetSize currentSize=%d,%d\n"),currentSize.x,currentSize.y);
#endif
  
  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = scrolled?currentSize.x:currentVirtualSize.x;
    if (sizey == 0) sizey = scrolled?currentSize.y:currentVirtualSize.y;
  }
  wxSize theSize=wxSize(sizex,sizey);
  if (scrolled) theSize += wxSize(gdlSCROLL_HEIGHT_X, gdlSCROLL_WIDTH_Y);
  me->SetSize( theSize );
  
 //framed bases may have a complicated frame around, to be resized:
  if (theWxContainer != theWxWidget) { //framed -> has a sizer
    wxWindow* container=dynamic_cast<wxWindow*>(theWxContainer);
    wxWindow* contained=dynamic_cast<wxWindow*>(theWxWidget);
    container->GetSizer()->Fit(contained);
  }
  
  UPDATE_WINDOW;

  END_CHANGESIZE_NOEVENT

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetBase::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}
/*********************************************************/
// special TOP WIDGET_BASE
/*********************************************************/

GDLWidgetTopBase::GDLWidgetTopBase( EnvT* e, ULong eventFlags_,
bool mapWid,
WidgetIDT& mBarIDInOut, bool modal_, DLong frame_attr,
DLong col, DLong row,
int exclusiveMode_,
bool floating,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
int xpad_, int ypad_,
DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase( GDLWidget::NullID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_)
, mbarID(mBarIDInOut)
, xmanActCom(false)
, modal(modal_)
, realized(false)
{
  
  // All bases can receive events: EV_CONTEXT, EV_KBRD_FOCUS, EV_TRACKING

  long style=wxDEFAULT_FRAME_STYLE|wxFRAME_TOOL_WINDOW; //wxFRAME_TOOL_WINDOW to NOT get focus. See behaviour of 'P' (photometry) while using ATV (atv.pro). 
  if (frame_attr) {
    style=0;
    if (!(frame_attr & 1)) style |= (wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER| wxCAPTION);
    if (!(frame_attr & 2)) style |=wxSYSTEM_MENU;
    if (!(frame_attr & 4)) style |=wxCAPTION;
    if (!(frame_attr & 8)) style |=wxCLOSE_BOX;
    //style 16 not supported.
  }
  if (floating) {
    if (this->GetWidget(groupLeader)==NULL) e->Throw("FLOATING top level bases must have a group leader specified.");
    style |= wxFRAME_TOOL_WINDOW|wxSTAY_ON_TOP ; //wxFRAME_FLOAT_ON_PARENT will destroy the parent widget!!
  }

  // Top Level Base Widget: can receive special events: tlb_size, tlb_move, tlb_icon and tlb_kill. cannot be framed.
  wxString titleWxString;
  if (title_.size() < 1) titleWxString = wxString("GDL");
  else titleWxString = wxString(title_.c_str(), wxConvUTF8);
  if (floating) {
    wxWindow* parent=static_cast<wxWindow*>(this->GetWidget(groupLeader)->GetWxWidget());
    //make it really above parent:
    wxRect x=parent->GetScreenRect();
    if (wOffset.x < 0) wOffset.x =x.x+x.width/2;
    if (wOffset.y < 0) wOffset.y =x.y+x.height/2;
  }
  topFrame = new gdlwxFrame(NULL, this, widgetID, titleWxString, wOffset, wxDefaultSize, style);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
  topFrame->SetBackgroundColour(wxColour(0x81, 0x46, 0xf1)); //violet
#endif

  //Base Frame inherits default font -- then each widget will possibly get its own font when /FONT is possible    
  topFrame->SetFont(defaultFont);

  if (mbarID != 0) {
#if PREFERS_MENUBAR
    GDLWidgetMenuBar* mBar = new GDLWidgetMenuBar(widgetID, e);
    mbarID = mBar->GetWidgetID();
    mBarIDInOut = mbarID;
    wxMenuBar* me = dynamic_cast<wxMenuBar*> (mBar->GetWxWidget());
    if (me) topFrame->SetMenuBar(me);
    else cerr << "Warning: GDLWidgetBase::GDLWidgetBase: Non-existent menubar widget!\n";
#else    
    GDLWidgetMenuBar* mBar = new GDLWidgetMenuBar(topFrame, widgetID, e);
    mbarID = mBar->GetWidgetID();
    mBarIDInOut = mbarID;
#endif
  }
  wxSizer* tfSizer=new wxBoxSizer(wxVERTICAL);
  topFrame->SetSizer(tfSizer);
  CreateBase(topFrame); //define widgetPanel, widgetSizer, theWxWidget and theWxContainer.
  //it is the FRAME that manage all events. Here we dedicate particularly the tlb_* events:
  // note that we have the choice for Size Event Handler for Frames, but need to change also is widgets.cpp
  if (eventFlags & GDLWidget::EV_SIZE) this->AddToDesiredEvents(wxEVT_SIZE, gdlSIZE_EVENT_HANDLER, topFrame); //this->AddToDesiredEvents(wxEVT_SIZE, wxSizeEventHandler(GDLFrame::OnSizeWithTimer),topFrame); 
  if (eventFlags & GDLWidget::EV_MOVE) this->AddToDesiredEvents(wxEVT_MOVE, wxMoveEventHandler(gdlwxFrame::OnMove), topFrame);
  if (eventFlags & GDLWidget::EV_ICONIFY) this->AddToDesiredEvents(wxEVT_ICONIZE, wxIconizeEventHandler(gdlwxFrame::OnIconize), topFrame);
  if (eventFlags & GDLWidget::EV_KILL) {
    this->AddToDesiredEvents(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnCloseFrame), topFrame);
  } else {
    this->AddToDesiredEvents(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(gdlwxFrame::OnUnhandledCloseFrame), topFrame);
  }
 this->AddToDesiredEvents(wxEVT_SHOW_REQUEST, wxCommandEventHandler(gdlwxFrame::OnShowRequest), topFrame);
 this->AddToDesiredEvents(wxEVT_HIDE_REQUEST, wxCommandEventHandler(gdlwxFrame::OnShowRequest), topFrame);
 //UPDATE_WINDOW; REALIZE_IF_NEEDED //no use here
 //this->Realize(false);
}

inline int GDLWidgetTopBase::widgetAlignment() {
    long myAlign = alignment;
    //there is no parent for topBase...
//    if (myAlign == gdlwALIGN_NOT) {
//      myAlign = this->GetMyParentBaseWidget()->getChildrenAlignment();
//    }
    //wxALIGN_LEFT and TOP is in fact wxALIGN_NOT as this is the default alignment
    if (myAlign == gdlwALIGN_NOT) return wxALIGN_CENTER;
    //left is top by default and right is bottom. So define left as top and remove top if bottom, etc.
    //ignore sets that do not concern the current layout (vetrtical or horizontal)
    if (this->IsVertical()) { //col=1 left, center, right
      if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_HORIZONTAL;
      if (myAlign & gdlwALIGN_LEFT) return wxALIGN_LEFT;
      if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_RIGHT;
      if (myAlign & gdlwALIGN_TOP) return wxALIGN_LEFT;
      if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_RIGHT;
    } else if (this->IsHorizontal()) { //row=1
      if (myAlign & gdlwALIGN_CENTER) return wxALIGN_CENTER_VERTICAL;
      if (myAlign & gdlwALIGN_TOP) return wxALIGN_TOP;
      if (myAlign & gdlwALIGN_BOTTOM) return wxALIGN_BOTTOM;
      if (myAlign & gdlwALIGN_LEFT) return wxALIGN_TOP;
      if (myAlign & gdlwALIGN_RIGHT) return wxALIGN_BOTTOM;
    }
    return 0;
  }

GDLWidgetTopBase::~GDLWidgetTopBase() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTopBase(" << widgetID << ")" << std::endl;
#endif
  topFrame->NullGDLOwner();

  //IMPORTANT: unxregister TLB if was managed 
  if (this->GetManaged()) CallEventPro("UNXREGISTER", new DLongGDL(widgetID)); //UNXREGISTER defined in XMANAGER.PRO

  //send RIP 
  // create GDL event struct
  DStructGDL* ev = new DStructGDL("*TOPLEVEL_DESTROYED*");
  ev->InitTag("ID", DLongGDL(widgetID));
  ev->InitTag("TOP", DLongGDL(widgetID));
  ev->InitTag("HANDLER", DLongGDL(0));
  if (this->GetXmanagerActiveCommand() || !this->GetManaged()) {
    readlineEventQueue.PushFront(ev); // push front (will be handled next)
  } else {
    eventQueue.PushFront(ev); // push front (will be handled next)
  }
}
/*********************************************************/
// Context Menu pseudo-base
/*********************************************************/
GDLWidgetContextBase::GDLWidgetContextBase( WidgetIDT parentID, EnvT* e, ULong eventFlags_,
bool mapWid,
DLong col, DLong row,
int exclusiveMode_,
const DString& resource_name, const DString& rname_mbar,
const DString& title_,
const DString& display_name,
int xpad_, int ypad_,
DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetMenu(parentID, e, new DStringGDL(title_), eventFlags_, NULL)
{
GDLWidget* gdlParent = GetWidget( parentID );
    assert( gdlParent != NULL);
    gdlParent->AddToFollowers(widgetID); //very important, the transient must be deleted if the associated widget is deleted!
    wxWindow* parentWindow=dynamic_cast<wxWindow*>(gdlParent->GetWxWidget());
    assert( parentWindow != NULL);
  //get default value: a menu. May be NULL here
    wxMenu *menu = new wxMenu();
  theWxContainer = menu; //is not a window
  theWxWidget = menu;
  buttonType = MENU;

//  UPDATE_WINDOW; REALIZE_IF_NEEDED; //no use (TBC)
}

GDLWidgetContextBase::~GDLWidgetContextBase() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetContextBase(" << widgetID << ")" << std::endl;
#endif
}
/*********************************************************/
// Normal WIDGET_BASE but Tab hosted
/*********************************************************/
GDLWidgetTabbedBase::GDLWidgetTabbedBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase(parentID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_)
, myPage(-1)
{
  //for historical logic reasons, there must be a container panel child of the tab, that will contain the actual widget, a panel.
  //the container is a scrolledwindow OR just a panel.
  //it would have been simpler to use just a wxScrolled<> for each container, and have it scroll or not depending on GDLWidget::scrolled.
  //this would accessorily eliminate the use of 'scrollSizer' and 'scrollPanel'.
  //but this has consequences in all the creation/deletion chain, to be done in time.
  GDLWidgetTab* parent = static_cast<GDLWidgetTab*> (GetWidget(parentID));
  assert(parent != NULL);

  wxNotebook* parentTab = dynamic_cast<wxNotebook*> (parent->GetWxWidget());
  assert(parentTab != NULL);

  wxString titleWxString = wxString(title_.c_str(), wxConvUTF8);
  wxSizer* sz=new wxBoxSizer(wxVERTICAL);
//  wxPanel* p=new wxPanel(parentTab, wxID_ANY, wxDefaultPosition, wxDefaultSize);
//  p->SetSize(wScrollSize);
  CreateBase(parentTab);
//  sz->Add(w,DONOTALLOWSTRETCH, wxALL | wxEXPAND, 0);
//  p->SetSizer(sz);
//  theWxContainer=p;
//
//  theWxWidget = p;
  wxWindow* w=static_cast<wxWindow*>(theWxContainer); //defined in CreateBase.
  myPage=parentTab->GetPageCount();
//  parentTab->AddPage(w, titleWxString);
  parentTab->InsertPage(myPage, w, titleWxString);
//  myPage=parentTab->FindPage(w);
  
  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}
/*********************************************************/
// Normal WIDGET_BASE

/*********************************************************/
GDLWidgetNormalBase::GDLWidgetNormalBase(WidgetIDT parentID, EnvT* e, ULong eventFlags_,
  bool mapWid,
  DLong col, DLong row,
  int exclusiveMode_,
  const DString& resource_name, const DString& rname_mbar,
  const DString& title_,
  const DString& display_name,
  int xpad_, int ypad_,
  DLong x_scroll_size, DLong y_scroll_size, bool grid_layout, long children_alignment, int space_)
: GDLWidgetBase(parentID, e, eventFlags_, mapWid, col, row, exclusiveMode_, resource_name, rname_mbar, title_, display_name, xpad_, ypad_, x_scroll_size, y_scroll_size, grid_layout, children_alignment, space_) {
  // If parent base widget exists ....
  GDLWidget* gdlParent = GetWidget(parentID);
  assert(gdlParent != NULL);
  wxWindow* wxParent = dynamic_cast<wxWindow*> (GetParentPanel());
  assert(wxParent != NULL);
  CreateBase(wxParent);

  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}

DStructGDL* GDLWidgetBase::GetGeometry(wxRealPoint fact) {
  if (!this->GetRealized()) this->Realize(true, false); //necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
  int panel_xoff = 0;
  int panel_yoff = 0;

  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;
  //reported size includes xpad and ypad.
  int ixpad=this->getXPad();
  int iypad=this->getYPad();
  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");
 
  wxWindow* w = dynamic_cast<wxWindow*> (theWxContainer);
  if (w == NULL) return ex;
  if (frameWidth > 0 && (ixpad>0 || iypad >0)) w=w->GetChildren()[0]; //padxpady is the first child of frame
  wxRect z = w->GetRect();

  margin=frameWidth;
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  if (container == NULL) { //TLB
//    w->GetScreenPosition(&panel_xoff, &panel_yoff);
    xoffset = panel_xoff+margin;
    yoffset = panel_yoff+margin;
  } else {
    xoffset = z.x+margin;
    yoffset = z.y+margin;
  }
  iscr_xsize = z.width-2*margin;
  iscr_ysize = z.height-2*margin;
  wxSize s = w->GetClientSize();
  ixsize = s.x-2*margin;
  iysize = s.y-2*margin;
  //size is in pixels, pass in requested units (1.0 default)
  xsize = ixsize / fact.x;
  ysize = iysize / fact.y;
  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));

  //reported size includes xpad and ypad.
  DFloat xpad=ixpad/ fact.x;
  DFloat ypad=iypad/ fact.y;
  DFloat space=this->getSpace()/ fact.x;
  ex->InitTag( "XPAD", DFloatGDL( xpad ) );
  ex->InitTag( "YPAD", DFloatGDL( ypad ) );
  ex->InitTag( "SPACE", DFloatGDL( space) );
  return ex;
}

//DStructGDL* GDLWidgetBase::GetGeometry(wxRealPoint fact)
//{
//  if (!this->GetRealized()) this->Realize(true,false);//necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
//  //the only way to get accurate positions seems to get the screen position of the Panel
//  //in which the window is (the Panel is the useful part of the Base) and substract them from the
//  //screen position of the siwget itself. And get the margin (=frame) size if any.
//  //for Bases, one has also to retrieve xpad,ypad and space;
//  int panel_xoff = 0;
//  int panel_yoff = 0;
//
//  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
//  if (container != NULL) { //not TLB
//    wxPanel* p = dynamic_cast<wxPanel*> (container->GetPanel());
//    p->GetScreenPosition(&panel_xoff, &panel_yoff);
//  }
//  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
//  int widget_xoff = 0;
//  int widget_yoff = 0;
//  DFloat xsize = 0;
//  DFloat ysize = 0;
//  DFloat scr_xsize = 0;
//  DFloat scr_ysize = 0;
//  DFloat xoffset = 0;
//  DFloat yoffset = 0;
//  DFloat margin = frameWidth;
//  
//  wxWindow* test = dynamic_cast<wxWindow*> (theWxWidget);
//
//  if (test != NULL) {
//    //offsets. easy.
//    test->GetScreenPosition(&widget_xoff, &widget_yoff);
//    xoffset = widget_xoff - panel_xoff;
//    yoffset = widget_yoff - panel_yoff;
//
//    test->GetSize(&iscr_xsize, &iscr_ysize); //contains frame size if framed
//
//    ixsize = iscr_xsize; //with scrollbars, if any, and frames
//    iysize = iscr_ysize;
//
//    //reported size includes xpad and ypad.
//    DFloat xpad=this->getXPad()/ fact.x;
//    DFloat ypad=this->getYPad()/ fact.y;
//    DFloat space=this->getSpace()/ fact.x;
//    //offset is including xpad,ypad
//    xoffset -= xpad;
//    yoffset -= ypad;
//    //size is in pixels, pass in requested units (1.0 default)
//    xsize = ixsize / fact.x + 2*margin;
//    ysize = iysize / fact.y + 2*margin;
//    scr_xsize = iscr_xsize / fact.x;
//    scr_ysize = iscr_ysize / fact.y;
//    xoffset /= fact.x;
//    yoffset /= fact.y;
//    margin /= fact.x;
//  }
//
//  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");
//  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
//  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
//  ex->InitTag("XSIZE", DFloatGDL(xsize));
//  ex->InitTag("YSIZE", DFloatGDL(ysize));
//  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
//  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
//  ex->InitTag("MARGIN", DFloatGDL(margin));
//  ex->InitTag( "XPAD", DFloatGDL( xpad ) );
//  ex->InitTag( "YPAD", DFloatGDL( ypad ) );
//  ex->InitTag( "SPACE", DFloatGDL( space) );
//  return ex;
//}

//// called from event handling thread
//// sends a destroy event for itself
//void GDLWidgetTopBase::SelfDestroy()
//{
//  assert( parentID == NullID );
//  // create GDL event struct
//  DStructGDL* ev = new DStructGDL( "*WIDGET_DESTROY*" );
//  ev->InitTag( "ID", DLongGDL( widgetID ) );
//  ev->InitTag( "TOP", DLongGDL( widgetID ) );
//  ev->InitTag( "HANDLER", DLongGDL( 0 ) );
//  if ( this->GetXmanagerActiveCommand( ) || !this->GetManaged() ){
//    readlineEventQueue.PushFront( ev ); // push front (will be handled next)
//  } else {
//    eventQueue.PushFront( ev ); // push front (will be handled next)
//  }
//}

  void GDLWidgetBase::mapBase(bool val){
    wxWindow* me=dynamic_cast<wxWindow*>(theWxContainer);
    if (me) me->Show(val); else {cerr<<"Warning: GDLWidgetBase::mapBase(): Non-existent widget!\n"; return;}
  }
  void GDLWidgetTopBase::mapBase(bool val){
    topFrame->Show(val);
  }  
  void GDLWidgetTabbedBase::mapBase(bool val){
    //Note: it is impossible to reproduce the IDL behaviour (tab is present but void) with wxWidgets unless doing ridiculously complicated things.
    wxWindow* me=dynamic_cast<wxWindow*>(theWxContainer);
    if (me) me->Show(val); else {cerr<<"Warning: GDLWidgetTabbedBase::mapBase(): Non-existent widget!\n"; return;}
  }
  // Order in which widgets are plotted is different btw wxWindow and IDL if one does not do the following tricks:
  void GDLWidgetBase::DoReorderColWidgets(int code,int style, int border) {
  int nchild = this->NChildren();
  //get children list...
  DLongGDL* gdlwList= GetChildrenList();
  // set up window list, get existing code, style and border for each of the children
  std::vector<int> proportionlist;
  std::vector<wxWindow*> windowlist;
  std::vector<int> flaglist;
  std::vector<int> borderlist;
  for (int i = 0; i < nchild; ++i) {
    GDLWidget* g=GetWidget((*gdlwList)[i]);
    wxWindow* w=static_cast<wxWindow*>(g->GetWxContainer());
    windowlist.push_back(w);
    wxSizerItem* s=widgetSizer->GetItem(w);
    if (s==NULL) { //this is a new window that has not yet been added (the widget is Realized) : use passed values
      proportionlist.push_back(code);
      flaglist.push_back(style);
      borderlist.push_back(border);
    } else { //use existing values
      proportionlist.push_back(s->GetProportion());
      flaglist.push_back(s->GetFlag());
      borderlist.push_back(s->GetBorder());
    }
  }
  GDLDelete(gdlwList);
  
  //compute new positions (align items along column 1, then 2 etc... when wxWidget will insert them col1,col2,col1,col2, etc..)
  int local_nchild=nchild;
  // method for COL=n , row is growing
  //This should be called only for /COL => nrows is 0 
  int local_ncols = ncols; 
  int local_nrows = local_nchild / local_ncols;
  if (local_nrows * local_ncols < local_nchild) local_nrows++;
  //how many filled cols?
  int reste = (local_ncols * local_nrows) - local_nchild;
  int filledcols = local_ncols - reste;

  //we create the corresponding flexigridsizer, destroy the previous one, assign back to widgetSizer
  wxFlexGridSizer* newWidgetSizer = new wxFlexGridSizer(local_nrows, local_ncols, border, border);
  widgetPanel->SetSizer(newWidgetSizer, true);
  widgetPanel->SetAutoLayout(true);
  if (grid) {
    newWidgetSizer->SetFlexibleDirection(wxHORIZONTAL);
    newWidgetSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);
  }
  widgetSizer = newWidgetSizer;

  //so the first 'filledcols' will be filled, the rest will have (apparently) one row less.
  //we have to insert a place holder in the list of childrens at each index corresponding to the last row, starting at the first incomplete column. 
  //widgets indexes are k=icol*local_nrows+jrow, compute a new indexlist where empty indexes are those jumped


  int list[local_ncols * local_nrows];
  int i = 0;
  int k = 0;
  int avoidrow = (reste == 0) ? local_nrows : local_nrows - 1;
  int lastfilledcol = (reste == 0) ? local_ncols : filledcols - 1;
  //  list of indexes as displayed in colums (idl) with -1 when there should be a void.
  for (int icol = 0; icol < local_ncols; icol++) {
    for (int jrow = 0; jrow < local_nrows; jrow++) {
      if ((icol > lastfilledcol) && (jrow == avoidrow)) {
        list[k++] = -1;
      } else {
        list[k++] = i++;
      }
    }
  }

  //find transposed values: read list in transposed order, use a local_nrows stride.
  int stride = local_nrows;
  int outer_stride = local_ncols*local_nrows;
  for (k = 0, i = 0; i < nchild; ++i) {
    widgetSizer->Add(windowlist[list[k]], proportionlist[list[k]],flaglist[list[k]],borderlist[list[k]]);
    k += stride;
    if (k >= outer_stride) {
      k -= outer_stride;
      k++;
    }
  }
}

void GDLWidgetBase::ReorderWidgets()
{
  // NULL widget Sizer means 1) no row no col was asked for (so, no sizer) or, if col>1, we have to create the sizer here and add children in specific order.
  // do *not* forget to give back the sizer pointer instead of the previous NULL to the base widget!
  if (widgetSizer == NULL) return;
  if (ncols > 1) DoReorderColWidgets(); //need to reorder widget for /COL only
}

void GDLWidgetBase::ReorderForANewWidget(wxWindow* w, int code,int style, int border)
{
  if (widgetSizer == NULL) return;
  if (ncols > 1 ) DoReorderColWidgets(code, style, border); //need to reorder widget for /COL only
  else widgetSizer->Add(w, code, style, border ); // just add widget in sizer for /ROW
}  
/*********************************************************/
// for WIDGET_TAB
/*********************************************************/
GDLWidgetTab::GDLWidgetTab( WidgetIDT p, EnvT* e, ULong eventFlags_, DLong location, DLong multiline )
: GDLWidgetContainer( p, e, eventFlags_ ) {

  scrolled=false; //TAB has no Scrolled.

//  GDLWidget* gdlParent = GetWidget( parentID );
//  widgetPanel =  GetParentPanel( );

  wxSizer* parentSizer = GetParentSizer( );

  long style = wxNB_TOP;
  if ( location == 1 ) style = wxNB_BOTTOM;
  if ( location == 2 ) style = wxNB_LEFT;
  if ( location == 3 ) style = wxNB_RIGHT;

  if ( multiline != 0 )
    style |= wxNB_MULTILINE; //works only for WINDOWS.
  
  wxPanel* panel_if_framed;
  wxPanel* container=widgetPanel; //default container if no intervening panels
  wxSizer* framesizer=NULL;
  wxNotebook * notebook;
  if (frameWidth>0) { //frame 
    panel_if_framed=new wxPanel(container, wxID_ANY, wOffset, wxDefaultSize, gdlBORDER_EXT);
    theWxContainer =  panel_if_framed;
    framesizer=new wxBoxSizer(wxVERTICAL);
    panel_if_framed->SetSizer(framesizer);
    container=panel_if_framed; //becomes container for further widgets 
    if (parentSizer) parentSizer->Add(container, DONOTALLOWSTRETCH, widgetAlignment(),gdlFRAME_MARGIN); //add to parent sizer
    notebook = new wxNotebook( container, widgetID, wxDefaultPosition, computeWidgetSize( ), style );
    theWxWidget = notebook;
    framesizer->Add(notebook,DONOTALLOWSTRETCH,wxALL,frameWidth);
  } else {
    notebook = new wxNotebook( container, widgetID, wOffset, computeWidgetSize( ), style );
    theWxContainer = theWxWidget = notebook;
    if (parentSizer) parentSizer->Add(notebook,DONOTALLOWSTRETCH,widgetAlignment()|wxALL, gdlSPACE);
  }
  //wxNotebook DOES NOT USE a sizer.
  this->AddToDesiredEvents(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,wxNotebookEventHandler(gdlwxFrame::OnPageChanged),notebook);
}
GDLWidgetTab::~GDLWidgetTab() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTab(" << widgetID << ")" << std::endl;
#endif
}

BaseGDL* GDLWidgetTab::GetTabNumber(){
  wxNotebook * notebook=dynamic_cast<wxNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetPageCount());
}

BaseGDL* GDLWidgetTab::GetTabCurrent(){
  wxNotebook * notebook=dynamic_cast<wxNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetSelection());
}

void GDLWidgetTab::SetTabCurrent(int val){
  wxNotebook * notebook=dynamic_cast<wxNotebook*>(theWxWidget);
  assert( notebook != NULL);
  if (val<notebook->GetPageCount()){
//   notebook->GetPage(val)->Raise();    
   notebook->ChangeSelection(val);
  }
}

BaseGDL* GDLWidgetTab::GetTabMultiline(){
  wxNotebook * notebook=dynamic_cast<wxNotebook*>(theWxWidget);
  assert( notebook != NULL);
  return new DIntGDL(notebook->GetExtraStyle()&wxNB_MULTILINE);
}


/*********************************************************/
// for WIDGET_TABLE
/*********************************************************/
//overrides method to label the columns & lines
  wxString wxGridTableBase::GetRowLabelValue( int row )
{
    wxString s;

    // RD: Starting the rows at zero confuses users,
    // no matter how much it makes sense to us geeks.
    // GD: So IDL and GDL are for geeks.
    s << row ;

    return s;
}
  wxString wxGridTableBase::GetColLabelValue( int col )
{
    wxString s;
    s << col ;

    return s;
}

GDLWidgetTable::GDLWidgetTable( WidgetIDT p, EnvT* e,
DByteGDL* alignment_,
DStringGDL* amPm_,
DByteGDL* backgroundColor_,
DByteGDL* foregroundColor_,
DStringGDL* columnLabels_,
int majority_,
DLongGDL* columnWidth_,
DStringGDL* daysOfWeek_,
bool disjointSelection_,
DByteGDL* editable_,
DStringGDL* format_,
//bool ignoreAccelerators_,
DStringGDL* month_,
bool noColumnHeaders_,
bool noRowHeaders_,
bool resizeableColumns_,
bool resizeableRows_,
DLongGDL* rowHeights_,
DStringGDL* rowLabels_,
//DLong tabMode_,
BaseGDL* value_,
DLong xScrollSize_,
DLong yScrollSize_,
DStringGDL* valueAsStrings_,
DULong eventFlags_
)
: GDLWidget( p, e, value_, eventFlags_ )
, table_alignment( alignment_ )
, amPm( amPm_ )
, backgroundColor( backgroundColor_ )
, foregroundColor( foregroundColor_ )
, columnLabels( columnLabels_ )
, majority ( majority_ )
, columnWidth( columnWidth_ )
, daysOfWeek( daysOfWeek_ )
, disjointSelection( disjointSelection_ )
, editable( editable_)
, format( format_ )
//, ignoreAccelerators( ignoreAccelerators_ )
, month( month_ )
, noColumnHeaders( noColumnHeaders_ )
, noRowHeaders( noRowHeaders_ )
, resizeableColumns( resizeableColumns_ )
, resizeableRows( resizeableRows_ )
, rowHeights( rowHeights_ )
, rowLabels( rowLabels_ )
//, tabMode( tabMode_ )
, x_scroll_size( xScrollSize_ )
, y_scroll_size( yScrollSize_)
, valueAsStrings( valueAsStrings_ )
, updating(false)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );

  //due to subtle problems in the absence of a frame around, a 0 or negative frame becomes a frame=1
//  if (frameWidth <1) frameWidth=1;
  
  START_ADD_EVENTUAL_FRAME

  widgetStyle=widgetAlignment();

//at this stage, valueAsStrings is OK dim 1 or 2 BUT vVALUE MAY BE NULL!
SizeT numRows,numCols;
if (valueAsStrings->Rank()==1) {
  numRows=1;
  numCols=valueAsStrings->Dim(0); //lines
} else {
  numRows=valueAsStrings->Dim(1);
  numCols=valueAsStrings->Dim(0);
}
SizeT grid_nrows=(wSize.y<=0)?numRows:wSize.y;
SizeT grid_ncols=(wSize.x<=0)?numCols:wSize.x;
  
  wxGridGDL *grid = new wxGridGDL( widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize);
//important:set wxWidget here. (fonts)
  theWxContainer = theWxWidget = grid;
// important: use adapted font for further sizes & shapes. Define font for labels AND  cells.
  this->setFont();
 //Column Width Before creating
bool hasColumnWidth=(columnWidth!=NULL);
if (hasColumnWidth) { //one value set for all?
  if (columnWidth->N_Elements()==1) {
    grid->SetDefaultColSize((*columnWidth)[0]*unitConversionFactor.x) ;
    hasColumnWidth=false;
  }
}
//RowHeight
bool hasRowHeights=(rowHeights!=NULL);
if (hasRowHeights) { //one value set for all?
  if (rowHeights->N_Elements()==1) {
    grid->SetDefaultRowSize((*rowHeights)[0]*unitConversionFactor.y) ;
    hasRowHeights=false;
  }
}
//Alignment
bool hasAlignment=(table_alignment!=NULL);
if (hasAlignment) {
  if (table_alignment->N_Elements()==1) { //singleton case
    switch( (*table_alignment)[0] ){
      case 0:
        grid->SetDefaultCellAlignment(wxALIGN_LEFT,wxALIGN_CENTRE); break;
      case 1:
        grid->SetDefaultCellAlignment(wxALIGN_CENTRE,wxALIGN_CENTRE); break;
      case 2:
        grid->SetDefaultCellAlignment(wxALIGN_RIGHT,wxALIGN_CENTRE);
    }
    hasAlignment=false; //otherwise will be treated cell by cell below!
  }
} 
//General Editability
bool isEditable=(editable!=NULL);
if (isEditable) {
  if (editable->N_Elements()==1) { //singleton case
    if ((*editable)[0]==0) isEditable=false;
    else {grid->EnableEditing(true); isEditable=false;}
  }
} else grid->EnableEditing(false); 
if (isEditable) grid->EnableEditing(true); //since now isEditable means "individually editable", which needs global editing set.
//Single Background Colour
bool isBackgroundColored=(backgroundColor!=NULL);
if (isBackgroundColored) { //one value set for all?
  if (backgroundColor->N_Elements()==3) { 
    grid->SetDefaultCellBackgroundColour(wxColour((*backgroundColor)[0],(*backgroundColor)[1],(*backgroundColor)[2])) ;  
    isBackgroundColored=false;
  }
}
//Single Text Colour
bool isForegroundColored=(foregroundColor!=NULL);
if (isForegroundColored) { //one value set for all?
  if (foregroundColor->N_Elements()==3) {
    grid->SetDefaultCellTextColour(wxColour((*foregroundColor)[0],(*foregroundColor)[1],(*foregroundColor)[2])) ;
    isForegroundColored=false;
  }
}
//No column Headers
if (noColumnHeaders) grid->SetColLabelSize(0);
//No row Headers
if (noRowHeaders) grid->SetRowLabelSize(0);
//end General Setup
int selmode = wxGrid::wxGridSelectCells; //wxWidgets's modes do not reflect IDL's. We trick the selections according to modes in eventhandler.
if (!resizeableColumns) grid->DisableDragColSize();
if (!resizeableRows) grid->DisableDragRowSize();
grid->CreateGrid( grid_nrows, grid_ncols, static_cast<wxGrid::wxGridSelectionModes>(selmode));
// Set grid cell contents as strings. Note that there may be less or more cells than valueAsStrings, due to possibly different wSize.x,wSize.y :

      for ( int ival=0, i=0; i<grid_nrows; ++i, ++ival) for (int jval=0, j=0; j<grid_ncols; ++j, ++jval)
      {
        if (ival < numRows && jval < numCols ) {
          StrTrim((*valueAsStrings)[jval*numRows+ival]);
          grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[jval*numRows+ival]).c_str(), wxConvUTF8 ) ); 
        }
      }

//Editability
//take too long as soon as table has a more than a few elements. Fixme!
//if (isEditable) {SizeT k=0; for (SizeT irow=0; irow< grid_nrows; ++irow) for (SizeT icol=0; icol< grid_ncols; ++icol) {grid->SetReadOnly( irow, icol, ((*editable)[k%editable->N_Elements()]==0));++k;}}
//colors per element
if (isBackgroundColored) this->DoBackgroundColor();
if (isForegroundColored) this->DoForegroundColor();
if (hasColumnWidth) this->DoColumnWidth();
if (hasRowHeights) this->DoRowHeights();
//treat other alignment cases.
if (hasAlignment) this->DoAlign();

if (columnLabels!=NULL)this->DoColumnLabels();
if (rowLabels!=NULL) this->DoRowLabels();

//get back on sizes. Do we enforce some size or scroll_size, in columns/rows:
int currentColLabelHeight = grid->GetColLabelSize();
int currentRowLabelWidth = grid->GetRowLabelSize();

int fullsizex=currentRowLabelWidth;
int fullsizey=currentColLabelHeight; 
for (SizeT i=0; i< numCols ; ++i) fullsizex+=(i<grid_ncols)?grid->GetColSize(i):grid->GetDefaultColSize(); 
for (SizeT j=0; j< numRows ; ++j) fullsizey+=(j<grid_nrows)?grid->GetRowHeight(j):grid->GetDefaultRowSize(); 

int visiblesizex=currentRowLabelWidth;
int visiblesizey=currentColLabelHeight; 
for (SizeT i=0; i< grid_ncols ; ++i) visiblesizex+=grid->GetColSize(i); 
for (SizeT j=0; j< grid_nrows ; ++j) visiblesizey+=grid->GetRowHeight(j); 

int sizex=-1;
int sizey=-1;
int scr_sizex=-1;
int scr_sizey=-1;
  if ( wSize.x > 0 ) { sizex = visiblesizex; //size in columns given
  } else {sizex=fullsizex;}
  if ( wSize.y > 0 ) { sizey = visiblesizey; //size in rows given
  } else {sizey=fullsizey;}
  if ( x_scroll_size > 0 ) { //scroll size is in columns
    scrolled=true;
    scr_sizex=currentRowLabelWidth+gdlSCROLL_HEIGHT_X;
    for (SizeT i=0; i< x_scroll_size ; ++i) scr_sizex+=grid->GetColSize(i);
    scr_sizex=min(scr_sizex,fullsizex);
    if (y_scroll_size <=0) y_scroll_size=x_scroll_size;
  }
  if ( y_scroll_size > 0 ) { //rows
    scrolled=true;
    scr_sizey=currentColLabelHeight+gdlSCROLL_WIDTH_Y;
    for (SizeT j=0; j< y_scroll_size ; ++j) scr_sizey+=grid->GetRowHeight(j);
    scr_sizey=min(scr_sizey,fullsizey);
  }
//fix size if relevant
if (scrolled && scr_sizex == -1) scr_sizex = (sizex>0)?sizex:fullsizex;
if (scrolled && scr_sizey == -1) scr_sizey = (sizey>0)?sizey:fullsizey;
//wScrXSize etc to be considered since sizes are not in pixels:
if (wScreenSize.x>0) {scr_sizex=wScreenSize.x; scrolled=true;}  
if (wScreenSize.y>0) {scr_sizey=wScreenSize.y; scrolled=true;}  
//wxGrid IS a scrolled window
if (scrolled) {
  grid->ShowScrollbars(wxSHOW_SB_ALWAYS,wxSHOW_SB_ALWAYS);
  grid->SetInitialSize(wxSize(scr_sizex, scr_sizey)); 
} else {
  if (wSize.x>0||wSize.y>0) {
    grid->SetInitialSize(wxSize(sizex,sizey)); 
}
}
grid->SetScrollLineX(grid->GetColSize(0));
grid->SetScrollLineY(grid->GetRowHeight(0));

END_ADD_EVENTUAL_FRAME
TIDY_WIDGET(gdlBORDER_SPACE);
  
      this->AddToDesiredEvents( wxEVT_GRID_COL_SIZE,wxGridSizeEventHandler(wxGridGDL::OnTableColResizing),grid);
      this->AddToDesiredEvents( wxEVT_GRID_ROW_SIZE,wxGridSizeEventHandler(wxGridGDL::OnTableRowResizing),grid);
      this->AddToDesiredEvents( wxEVT_GRID_RANGE_SELECT,wxGridRangeSelectEventHandler(wxGridGDL::OnTableRangeSelection),grid);
      this->AddToDesiredEvents( wxEVT_GRID_SELECT_CELL,wxGridEventHandler(wxGridGDL::OnTableCellSelection),grid);
//      this->AddToDesiredEvents( wxEVT_GRID_CELL_LEFT_CLICK,wxGridEventHandler(wxGridGDL::OnTableCellSelection),grid);

 UPDATE_WINDOW
 REALIZE_IF_NEEDED
}

bool GDLWidgetTable::IsSomethingSelected(){
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  return grid->IsSomethingSelected();
}

DLongGDL* GDLWidgetTable::GetSelection( ) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  SizeT k = 0;
  DLongGDL * sel;
  std::vector<wxPoint> list = grid->GetSelectedDisjointCellsList( );
  if ( disjointSelection ) { //pairs lists
    if ( list.size( ) < 1 ) {sel = new DLongGDL( 2, BaseGDL::ZERO ); sel->Dec(); return sel;} //returns [-1,-1] if nothing selected
    SizeT dims[2];
    dims[0] = 2;
    dims[1] = list.size( );
    dimension dim( dims, 2 );
    sel = new DLongGDL( dim );
    for ( std::vector<wxPoint>::iterator it = list.begin( ); it != list.end( ); ++it ) {
      (*sel)[k++] = (*it).y;
      (*sel)[k++] = (*it).x;
    }
  } else { //4 values
    wxGridCellCoordsArray selectionTL = grid->GetSelectionBlockTopLeft( );
    wxGridCellCoordsArray selectionBR = grid->GetSelectionBlockBottomRight( );
    sel = new DLongGDL( 4, BaseGDL::ZERO ); sel->Dec(); //will return [-1,-1,-1,-1] if nothing selected
    if (!selectionTL.IsEmpty() && !selectionBR.IsEmpty()){ //ok with a block...
      //LEFT TOP BOTTOM RIGHT
      (*sel)[0] = selectionTL[0].GetCol( );
      (*sel)[1] = selectionTL[0].GetRow( );
      (*sel)[2] = selectionBR[0].GetCol( );
      (*sel)[3] = selectionBR[0].GetRow( );   
    } else {
      //try columns, rows, and singletons
      wxArrayInt selectionRow=grid->GetSelectedRows();
      wxArrayInt selectionCol=grid->GetSelectedCols();
      if ( selectionRow.GetCount() >0 ) {
        (*sel)[0] = 0;
        (*sel)[1] = selectionRow[0];
        (*sel)[2] = grid->GetNumberCols()-1;
        (*sel)[3] = selectionRow[selectionRow.GetCount()-1];
      } else if ( selectionCol.GetCount() >0 ) {
        (*sel)[0] = selectionCol[0];
        (*sel)[1] = 0;
        (*sel)[2] = selectionCol[selectionCol.GetCount()-1];
        (*sel)[3] = grid->GetNumberRows()-1;
      } else {
        wxGridCellCoordsArray cellSelection = grid->GetSelectedCells( );
        if (cellSelection.size()>0) {
         int row = cellSelection[0].GetRow();
         int col = cellSelection[0].GetCol();
        (*sel)[0] = col;
        (*sel)[1] = row;
        (*sel)[2] = col;
        (*sel)[3] = row;
        }
      }
      return sel;
    }

  }
  return sel;
}

void GDLWidgetTable::ClearSelection()
{
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  updating=true;
  grid->ClearSelection();
  updating=false;
}

void GDLWidgetTable::DoAlign() {
  if (table_alignment->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k = 0;
  grid->BeginBatch();
  for ( SizeT i = 0; i < nRows; ++i ) {
    for ( SizeT j = 0; j < nCols; ++j ) {
      switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
        case 0:
          grid->SetCellAlignment( i, j, wxALIGN_LEFT, wxALIGN_CENTRE );
          break;
        case 1:
          grid->SetCellAlignment( i, j, wxALIGN_CENTRE, wxALIGN_CENTRE );
          break;
        case 2:
          grid->SetCellAlignment( i, j, wxALIGN_RIGHT, wxALIGN_CENTRE );
      }
      k++;
      if ( table_alignment->N_Elements( ) > 1 ) if ( k == table_alignment->N_Elements( ) ) break;
    }
    if ( table_alignment->N_Elements( ) > 1 ) if ( k == table_alignment->N_Elements( ) ) break;
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoAlign(DLongGDL* selection) {
  if (table_alignment->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  SizeT k = 0;
  grid->BeginBatch();
  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
     int ali;
      switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
        case 0:
          ali = wxALIGN_LEFT;
          break;
        case 1:
          ali = wxALIGN_CENTRE;
          break;
        case 2:
          ali = wxALIGN_RIGHT;
      }
      grid->SetCellAlignment( (*it).x, (*it).y, ali, wxALIGN_CENTRE );
      k++;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        int ali;
        switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
          case 0:
            ali = wxALIGN_LEFT;
            break;
          case 1:
            ali = wxALIGN_CENTRE;
            break;
          case 2:
            ali = wxALIGN_RIGHT;
        }
        grid->SetCellAlignment( row, col, ali, wxALIGN_CENTRE );
        k++;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
        int ali;
        switch ( (*table_alignment)[k % table_alignment->N_Elements( )] ) {
          case 0:
            ali = wxALIGN_LEFT;
            break;
          case 1:
            ali = wxALIGN_CENTRE;
            break;
          case 2:
            ali = wxALIGN_RIGHT;
        }
        grid->SetCellAlignment( i, j, ali, wxALIGN_CENTRE );
        k++;
     }
    }
  }
  grid->EndBatch();

}

void GDLWidgetTable::DoBackgroundColor() {
  SizeT nbColors=backgroundColor->N_Elements( );
  if (nbColors==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k=0; 
  grid->BeginBatch();
  for (SizeT i=0; i< nRows; ++i) for (SizeT j=0; j< nCols; ++j) //Row by Row, from top.
  {
   grid->SetCellBackgroundColour( i, j, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
   k+=3;
  }
  grid->EndBatch();

}
void GDLWidgetTable::DoBackgroundColor(DLongGDL* selection) {
  SizeT nbColors=backgroundColor->N_Elements( );
  if (nbColors==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch();

  SizeT k=0;

  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
    grid->SetCellBackgroundColour( (*it).x, (*it).y, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
    k+=3;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        grid->SetCellBackgroundColour( row, col, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
        k+=3;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
       grid->SetCellBackgroundColour( i, j, wxColour((*backgroundColor)[k%nbColors],(*backgroundColor)[k%nbColors+1],(*backgroundColor)[k%nbColors+2]));
       k+=3;
     }
    }
  }
  
  grid->EndBatch();

}

void GDLWidgetTable::DoForegroundColor() {
  SizeT nbColors=foregroundColor->N_Elements( );
  if (nbColors==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  int nCols = grid->GetNumberCols( );
  SizeT k=0; 
  grid->BeginBatch();
  for (SizeT i=0; i< nRows; ++i) for (SizeT j=0; j< nCols; ++j) //Row by Row, from top.
  {
    grid->SetCellTextColour( i, j, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
    k+=3;
  }
  grid->EndBatch();

}


void GDLWidgetTable::DoForegroundColor(DLongGDL* selection) {
  SizeT nbColors=foregroundColor->N_Elements( );
  if (nbColors==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch();

  SizeT k=0;

  if (selection->Rank()==0) { //use current wxWidgets selection
   std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
   for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
    grid->SetCellTextColour( (*it).x, (*it).y, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
    k+=3;
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      for (SizeT n=0,l=0; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];
        int row = (*selection)[l++];
        grid->SetCellTextColour( row, col, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
        k+=3;
      }
    }else{ //4 values
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
     {
       grid->SetCellTextColour( i, j, wxColour((*foregroundColor)[k%nbColors],(*foregroundColor)[k%nbColors+1],(*foregroundColor)[k%nbColors+2]));
       k+=3;
     }
    }
  }
  
  grid->EndBatch();

}
  
void GDLWidgetTable::DoColumnLabels( ) {
  if (columnLabels->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nCols = grid->GetNumberCols( );
  grid->BeginBatch();
  if ( columnLabels->N_Elements( ) == 1 ) { //singleton case
    if ( static_cast<DString> ((*columnLabels)[0]).length( ) == 0 ) {
      for ( SizeT j = 0; j < nCols; ++j ) grid->SetColLabelValue( j, wxEmptyString );
    } else {
      for ( SizeT j = 0; j < nCols; ++j ) {
        if ( j > (columnLabels->N_Elements( ) - 1) ) break;
        grid->SetColLabelValue( j, wxString( static_cast<DString> ((*columnLabels)[j]).c_str( ), wxConvUTF8 ) );
      }
    }
  } else {
    for ( SizeT j = 0; j < nCols; ++j ) {
      if ( j > (columnLabels->N_Elements( ) - 1) ) break;
      grid->SetColLabelValue( j, wxString( static_cast<DString> ((*columnLabels)[j]).c_str( ), wxConvUTF8 ) );
    }
  }
  grid->EndBatch();
  
}

void GDLWidgetTable::DoColumnWidth( ) {
  if (columnWidth->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nCols = grid->GetNumberCols( );
  grid->BeginBatch();
  if ( columnWidth->N_Elements( ) == 1 ) for ( SizeT j = 0; j < nCols; ++j ) grid->SetColSize(j,(*columnWidth)[0]*unitConversionFactor.x); 
  else {
      for ( SizeT j = 0; j < nCols; ++j ) {
        if ( j > (columnWidth->N_Elements( ) - 1) ) break;
        grid->SetColSize(j,(*columnWidth)[j]*unitConversionFactor.x);
      }
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoColumnWidth( DLongGDL* selection ) {
  SizeT nbCols = columnWidth->N_Elements( );
  if ( nbCols == 0 ) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
   for ( int it = 0; it <list.GetCount(); ++it) {
       grid->SetColSize( list[it], (*columnWidth)[it % nbCols]*unitConversionFactor.x);
    }
  } else { //use the passed selection, mode-dependent:
     if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::iterator iter;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( iter = allCols.begin(); iter !=allCols.end(); ++iter) {
        if ((*iter)!=theCol) {
          theCol=(*iter);
                  //if index is -1, we want to size the label column instead of the grid columns.
          if (theCol==-1)  grid->SetRowLabelSize((*columnWidth)[k % nbCols]*unitConversionFactor.x);
          else if (theCol>=0 && theCol<grid->GetNumberCols())  grid->SetColSize( theCol, (*columnWidth)[k % nbCols]*unitConversionFactor.x );
          k++;
        }
      }
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     for (int j=colTL; j<=colBR; ++j)
     {
       //if index is -1, we want to size the label column instead of the grid columns.
       if (j==-1)  grid->SetRowLabelSize((*columnWidth)[k % nbCols]*unitConversionFactor.x);
       else if (j>=0 && j<grid->GetNumberCols())  grid->SetColSize( j, (*columnWidth)[k % nbCols]*unitConversionFactor.x );
       k++;
     }
    }
  }
  
  grid->EndBatch( );
}

DFloatGDL* GDLWidgetTable::GetColumnWidth(DLongGDL* selection){
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  SizeT k=0;
  int nCols = grid->GetNumberCols( );
  
  if ( selection == NULL) {
    DFloatGDL* res=new DFloatGDL(dimension(nCols));
    for ( SizeT j = 0; j < nCols; ++j ) (*res)[j]=grid->GetColSize(j);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedColsList();
   //find concerned cols
    if (list.GetCount()==0) return NULL;
   DFloatGDL* res=new DFloatGDL(dimension(list.GetCount()));
   for ( int it = 0; it <list.GetCount(); ++it) {
       (*res)[it]=grid->GetColSize( list[it] );
    }
   return res;
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::iterator iter;
     std::vector<int> theCols;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( iter = allCols.begin(); iter !=allCols.end(); ++iter) {
        if ((*iter)!=theCol) {
          theCol=(*iter);
          k++;
          theCols.push_back(theCol);
        }
      }
     //final list:
     if (theCols.size()==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(theCols.size()));
     for ( iter = theCols.begin(); iter !=theCols.end(); ++iter) {
       (*res)[k++]=grid->GetColSize( (*iter));
      }     
     return res;
    } else { //4 values
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     int count = colBR-colTL+1;
     if (count==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(count));
     for (int j=colTL; j<=colBR; ++j)
     {
       (*res)[k++]=grid->GetColSize(j); 
     }
    }
  }
  return new DFloatGDL(0); //to keep compiler happy
}
DFloatGDL* GDLWidgetTable::GetRowHeight(DLongGDL* selection){
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  SizeT k=0;
  int nRows = grid->GetNumberRows( );
  
  if ( selection == NULL) {
    DFloatGDL* res=new DFloatGDL(dimension(nRows));
    for ( SizeT i = 0; i < nRows; ++i ) (*res)[i]=grid->GetRowSize(i);
    return res;
  } else if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
    wxArrayInt list=grid->GetSortedSelectedRowsList();
   //find concerned rows
    if (list.GetCount()==0) return NULL;
   DFloatGDL* res=new DFloatGDL(dimension(list.GetCount()));
   for ( int it = 0; it <list.GetCount(); ++it) {
       (*res)[it]=grid->GetRowSize( list[it] );
    }
   return res;
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::iterator iter;
     std::vector<int> theRows;
     //find concerned rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( iter = allRows.begin(); iter !=allRows.end(); ++iter) {
        if ((*iter)!=theRow) {
          theRow=(*iter);
          k++;
          theRows.push_back(theRow);
        }
      }
     //final list:
     if (theRows.size()==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(theRows.size()));
     for ( iter = theRows.begin(); iter !=theRows.end(); ++iter) {
       (*res)[k++]=grid->GetRowSize( (*iter));
      }     
     return res;
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     int count = rowBR-rowTL+1;
     if (count==0) return NULL;
     DFloatGDL* res=new DFloatGDL(dimension(count));
     for (int j=rowTL; j<=rowBR; ++j)
     {
       (*res)[k++]=grid->GetRowSize(j); 
     }
    }
  }
  return new DFloatGDL(0); //to keep compiler happy
}

void GDLWidgetTable::DoRowHeights( ) {
  if (rowHeights->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  grid->BeginBatch();
  if ( rowHeights->N_Elements( ) == 1 ) for ( SizeT i = 0; i < nRows; ++i ) grid->SetRowSize(i,(*rowHeights)[0]*unitConversionFactor.y); 
  else {
      for ( SizeT i = 0; i < nRows; ++i ) {
        if ( i > (rowHeights->N_Elements( ) - 1) ) break;
        grid->SetRowSize(i,(*rowHeights)[i]*unitConversionFactor.y);
      }
  }
  grid->EndBatch();
}

void GDLWidgetTable::DoRowHeights( DLongGDL* selection ) {
  SizeT nbRows = rowHeights->N_Elements( );
  if ( nbRows == 0 ) { return; }
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  SizeT k=0;

  if ( selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   for ( int it = 0; it <list.GetCount(); ++it) {
       if (list[it]<grid->GetNumberRows())  grid->SetRowSize( list[it], (*rowHeights)[it % nbRows]*unitConversionFactor.y );
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::iterator iter;
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
       l++;
       int row = (*selection)[l++];
       allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( iter = allRows.begin(); iter !=allRows.end(); ++iter) {
        if ((*iter)!=theRow) {
          theRow=(*iter);
         //if index is -1, we want to size the label column instead of the grid columns.
          if (theRow==-1)  grid->SetColLabelSize((*columnWidth)[k % nbRows]*unitConversionFactor.x);
          else if (theRow>=0 && theRow<grid->GetNumberRows()) grid->SetRowSize( theRow, (*rowHeights)[k % nbRows] *unitConversionFactor.y);
          k++;
        }
      }
    } else { //4 values
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     for (int i=rowTL; i<=rowBR; ++i)
     {
       //if index is -1, we want to size the label column instead of the grid columns.
       if (i==-1)  grid->SetColLabelSize((*columnWidth)[k % nbRows]*unitConversionFactor.x);
       else if (i>=0 && i<grid->GetNumberRows()) grid->SetRowSize( i, (*rowHeights)[k % nbRows]*unitConversionFactor.y );
       k++;
     }
    }
  }
  
  grid->EndBatch( );
}

void GDLWidgetTable::DoRowLabels( ) {
  if (rowLabels->N_Elements( )==0) {return;}
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  int nRows = grid->GetNumberRows( );
  grid->BeginBatch();
  if ( rowLabels->N_Elements( ) == 1 ) { //singleton case
    if ( static_cast<DString> ((*rowLabels)[0]).length( ) == 0 ) {
      for ( SizeT i = 0; i < nRows; ++i ) grid->SetRowLabelValue( i, wxEmptyString );
    } else {
      for ( SizeT i = 0; i < nRows; ++i ) {
        if ( i > (rowLabels->N_Elements( ) - 1) ) break;
        grid->SetRowLabelValue( i, wxString( static_cast<DString> ((*rowLabels)[i]).c_str( ), wxConvUTF8 ) );
      }
    }
  } else {
    for ( SizeT i = 0; i < nRows; ++i ) {
      if ( i > (rowLabels->N_Elements( ) - 1) ) break;
      grid->SetRowLabelValue( i, wxString( static_cast<DString> ((*rowLabels)[i]).c_str( ), wxConvUTF8 ) );
    }
  }
  grid->EndBatch();
}

void GDLWidgetTable::DeleteColumns(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  if ( selection==NULL || selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //delete in reverse order to avoid column-numbering problems
   for ( int it = list.GetCount()-1; it >-1 ; --it) {
       grid->DeleteCols( list[it], 1, true);
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     std::vector<int>::reverse_iterator riter;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     int theCol=-1;
     for ( riter = allCols.rbegin(); riter !=allCols.rend(); ++riter) {
        if ((*riter)!=theCol) {
          theCol=(*riter);
          grid->DeleteCols( theCol, 1, true);
        }
      }
    } else { //4 values, cols are contiguous, easy.
     int colTL = (*selection)[0];
     int colBR = (*selection)[2];
     int count=colBR-colTL+1;
     grid->DeleteCols( colTL , count, true );
    }
  }
  
  grid->EndBatch( );
}
bool GDLWidgetTable::InsertColumns(DLong count, DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  bool success;
  grid->BeginBatch( );

  if ( selection==NULL ){ //add count to rightmost position
    int pos=grid->GetNumberCols();
    success=grid->InsertCols(pos,count,true);
   // Set new grid cell contents TBD FIXME!
   {SizeT k=0; for (SizeT i=0; i< grid->GetNumberRows(); ++i) for (SizeT j=pos; j<grid->GetNumberCols() ; ++j) {grid->SetCellValue( i, j, wxString( "0" , wxConvUTF8 ) );++k;}}
  }
  else if (selection->Rank( ) == 0 ) { //add left of current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedColsList();
   //insert to left of first one
   success=grid->InsertCols( list[0], count, true);
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allCols;
     //find concerned cols
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int col = (*selection)[l++];l++;
        allCols.push_back(col);
      }
     std::sort (allCols.begin(), allCols.end());
     success=grid->InsertCols( *(allCols.begin()), 1, true);
    } else { //4 values, cols are contiguous, easy.
     int colTL = (*selection)[0];
     success=grid->InsertCols( colTL , count, true );
    }
  }
  
  grid->EndBatch( );
  return success;
}

void GDLWidgetTable::DeleteRows(DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );

  if ( selection==NULL || selection->Rank( ) == 0 ) { //use current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   //delete in reverse order to avoid column-numbering problems
   for ( int it = list.GetCount()-1; it >-1 ; --it) {
       grid->DeleteRows( list[it], 1, true);
    }
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     std::vector<int>::reverse_iterator riter;
     //find concerned Rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     int theRow=-1;
     for ( riter = allRows.rbegin(); riter !=allRows.rend(); ++riter) {
        if ((*riter)!=theRow) {
          theRow=(*riter);
          grid->DeleteRows( theRow, 1, true);
        }
      }
    } else { //4 values, Rows are contiguous, easy.
     int rowTL = (*selection)[1];
     int rowBR = (*selection)[3];
     int count=rowBR-rowTL+1;
     grid->DeleteRows( rowTL , count, true );
    }
  }
  
  grid->EndBatch( );
}

bool GDLWidgetTable::InsertRows(DLong count, DLongGDL* selection) {
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  bool success;
  grid->BeginBatch( );

  if ( selection==NULL ){ //add count to rightmost position
    int pos=grid->GetNumberRows();
    success=grid->InsertRows(pos,count,true);
   // Set new grid cell contents TBD FIXME!
   {SizeT k=0; for (SizeT i=pos; i<grid->GetNumberRows(); ++i) for (SizeT j=0; j<grid->GetNumberCols() ; ++j) {grid->SetCellValue( i, j, wxString( "0" , wxConvUTF8 ) );++k;}}
  }
  else if (selection->Rank( ) == 0 ) { //add left of current wxWidgets selection
   wxArrayInt list=grid->GetSortedSelectedRowsList();
   //insert to left of first one
   success=grid->InsertRows( list[0], count, true);
  } else { //use the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
     std::vector<int> allRows;
     //find concerned rows
     for ( SizeT n=0, l=0 ; n<selection->Dim(1); ++n) {
        int row = (*selection)[l++];l++;
        allRows.push_back(row);
      }
     std::sort (allRows.begin(), allRows.end());
     success=grid->InsertRows( *(allRows.begin()), 1, true);
    } else { //4 values, cols are contiguous, easy.
     int rowTL = (*selection)[1];
     success=grid->InsertRows( rowTL , count, true );
    }
  }
  
  grid->EndBatch( );
  return success;
}

void GDLWidgetTable::SetTableValues(DStringGDL* val, DLongGDL* selection)
{
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);

  grid->BeginBatch();
  
  if ( selection==NULL ){ //reset table to everything. val replaces valueAsStrings.
    GDLDelete(valueAsStrings);
    valueAsStrings=val->Dup();
    SizeT numRows,numCols;
    if (valueAsStrings->Rank()==1) {
      numRows=1;
      numCols=valueAsStrings->Dim(0); //lines
    } else {
      numRows=valueAsStrings->Dim(1);
      numCols=valueAsStrings->Dim(0);
    }
    grid->ClearGrid();
    int curr_rows=grid->GetNumberRows();
    int curr_cols=grid->GetNumberCols();
    //adjust rows and cols:
    if (numRows > curr_rows) grid->AppendRows(numRows-curr_rows);
    if (numCols > curr_cols) grid->AppendCols(numCols-curr_cols);
    // Set grid cell contents as strings
    {SizeT k=0; for (SizeT i=0; i<numRows ; ++i) for (SizeT j=0; j< numCols; ++j) {grid->SetCellValue( i, j, wxString(((*valueAsStrings)[k]).c_str(), wxConvUTF8 ) );++k;}}
    
  } else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      if (selection->Rank()==0) { 
        std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
        SizeT k=0;
        for (std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
          grid->SetCellValue( (*it).x, (*it).y ,wxString(((*val)[k++]).c_str(), wxConvUTF8 ) ); 
          if (k==val->N_Elements()) break;
        }
       } else {
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          grid->SetCellValue( row, col ,wxString(((*val)[k++]).c_str(), wxConvUTF8 ) ); 
          if (k==val->N_Elements()) break;
        }
      }
    } else { //IDL maintains the 2D-structure of val!
      SizeT numRows,numCols;
      if (val->Rank()==1) {
        numRows=1;
        numCols=val->Dim(0); //lines
      } else {
        numRows=val->Dim(1);
        numCols=val->Dim(0);
      }
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      for ( int ival=0, i=rowTL; i<=rowBR; ++i, ++ival) for (int jval=0, j=colTL; j<=colBR; ++j, ++jval)
      {
        if (ival < numRows && jval < numCols ) grid->SetCellValue( i, j ,wxString(((*val)[jval*numRows+ival]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  grid->EndBatch( );
}
BaseGDL* GDLWidgetTable::GetTableValuesAsStruct(DLongGDL* selection)
{
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  BaseGDL* res;
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  DStringGDL* stringres=this->GetTableValues(selection);
  if (stringres==NULL) return NULL; //pass error back.

  if ( selection==NULL ){ //just convert
    res=vValue->Dup();
    stringstream is;
//    if (majority == GDLWidgetTable::ROW_MAJOR ) {
//      BaseGDL* tmp=static_cast<BaseGDL*>(stringres)->Transpose(NULL);
//      GDLDelete(stringres);
//      stringres=static_cast<DStringGDL*>(tmp);
//      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
//    } else {
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
//    }
    res->FromStream( is);
  } 
  else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      std::vector<wxPoint> list;
      if (selection->Rank()==0) { //use current wxWidgets selection. Result is a STRUCT
        list=grid->GetSelectedDisjointCellsList();
      } else {                   //make equivalent vector.
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          list.push_back(wxPoint(row,col));
        }
      }
      SizeT k=0;
      DStructGDL*  typecodes = new DStructGDL( "GDL_TYPECODES_AS_STRUCT"); 
      // creating the output anonymous structure
      DStructDesc* res_desc = new DStructDesc("$truct");
      for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it, ++k) {
        //get tag values:
        BaseGDL* tested;
        if (majority == GDLWidgetTable::ROW_MAJOR )
          tested=static_cast<DStructGDL*>(vValue)->GetTag((*it).y); //table columns are tags
        else
          tested=static_cast<DStructGDL*>(vValue)->GetTag((*it).x); //table rows are tags
        stringstream os;
        os << std::setfill ('_') << std::setw (12) << k ; //as IDL does
        std::string tagName;
        os >> tagName; 
        res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
      }
      stringstream is;
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
      res = new DStructGDL(res_desc, dimension());
      res->FromStream( is);
    } else { //IDL maintains the 2D-structure of val!
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      //complication: if only one row (col) is selected, result is an array of <type>.
      //else result is a structure with correct tag names. Very clever!
      if ((majority == GDLWidgetTable::ROW_MAJOR && colTL==colBR)||(majority == GDLWidgetTable::COLUMN_MAJOR && rowTL==rowBR)) {
        DType what=GDL_BYTE;
        SizeT size;
        if (majority == GDLWidgetTable::ROW_MAJOR && colTL==colBR) {
          what=static_cast<DStructGDL*>(vValue)->GetTag(colTL)->Type();
          size=rowBR-rowTL+1;
        } else if (rowTL==rowBR) {
          what=static_cast<DStructGDL*>(vValue)->GetTag(rowTL)->Type();
          size=colBR-colTL+1;
        } 
        switch(what) {
          case GDL_STRING:
            res=new DStringGDL(dimension(size));
            break;
          case GDL_BYTE:
            res=new DByteGDL(dimension(size));
            break;
          case GDL_INT: 
            res=new DIntGDL(dimension(size));
            break;
          case GDL_LONG:
            res=new DLongGDL(dimension(size));
            break;
          case GDL_FLOAT:
            res=new DFloatGDL(dimension(size));
            break;
          case GDL_DOUBLE:
            res=new DDoubleGDL(dimension(size));
            break;
          case GDL_COMPLEX:
            res=new DComplexGDL(dimension(dimension(size)));
            break;
          case GDL_COMPLEXDBL:
            res=new DComplexDblGDL(dimension(size));
            break;
          case GDL_UINT:
            res=new DUIntGDL(dimension(size));
            break;
          case GDL_ULONG:
            res=new DULongGDL(dimension(size));
            break;
          case GDL_LONG64:
            res=new DLong64GDL(dimension(size));
            break;
          case GDL_ULONG64:
            res=new DULong64GDL(dimension(size));
            break;
          default:
            cerr<<"Unhandled Table Type, please report!"<<endl;
            return NULL; //signal error
        }
      } else { //create dedicated struct
        DStructGDL*  typecodes = new DStructGDL( "GDL_TYPECODES_AS_STRUCT"); 
        // creating the output anonymous structure
        DStructDesc* res_desc = new DStructDesc("$truct");
        SizeT size;
        if (majority == GDLWidgetTable::ROW_MAJOR) { //data is in rows of structures. Columns are tags
          size=rowBR-rowTL+1;
          for (SizeT j=colTL; j<=colBR; ++j) {
          //get tag values:
            BaseGDL* tested;
            std::string tagName;
            tested=static_cast<DStructGDL*>(vValue)->GetTag(j);
            tagName=static_cast<DStructGDL*>(vValue)->Desc()->TagName(j); //preserve tag names
            res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
          }          
        } else {
          size=colBR-colTL+1;
          for (SizeT i=rowTL; i<=rowBR; ++i) { 
          //get tag values:
            BaseGDL* tested;
            std::string tagName;
            tested=static_cast<DStructGDL*>(vValue)->GetTag(i);
            tagName=static_cast<DStructGDL*>(vValue)->Desc()->TagName(i); //preserve tag names
            res_desc->AddTag(tagName, typecodes->GetTag(tested->Type()));
          }
        }
        //create res with correct dim:
        res = new DStructGDL(res_desc, dimension(size));
      }
      //populate res:
      stringstream is;
      for( SizeT i = 0; i < stringres->N_Elements(); i++)  is << (*stringres)[ i] << '\n';
      res->FromStream( is);        
    }
  }
  return res;
}

DStringGDL* GDLWidgetTable::GetTableValues(DLongGDL* selection)
{
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);

  DStringGDL * stringres;
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);

  if ( selection==NULL ){
    int ncols=grid->GetNumberCols();
    int nrows=grid->GetNumberCols();

    SizeT dims[2];
    dims[1]=(nrows>numRows)?numRows:nrows;
    dims[0]=(ncols>numCols)?numCols:ncols;
    dimension dim(dims,2);
    stringres=new DStringGDL(dim);
    
    for ( int ival=0, i=0; i<nrows; ++i, ++ival) for (int jval=0, j=0; j<ncols; ++j, ++jval)
    {
      if (ival < numRows && jval < numCols ) (*stringres)[jval*numRows+ival]= grid->GetCellValue( i, j).mb_str(wxConvUTF8); 
    }
  } else { //use the wxWidget selection or the passed selection, mode-dependent:
    if (disjointSelection) { //pairs lists
      if (selection->Rank()==0) { //use current wxWidgets selection
        std::vector<wxPoint> list=grid->GetSelectedDisjointCellsList();
        stringres=new DStringGDL(list.size()); 
        SizeT k=0;
        for ( std::vector<wxPoint>::iterator it = list.begin(); it !=list.end(); ++it) {
          if ((*it).x >= numRows || (*it).y >= numCols) return static_cast<DStringGDL*>(NULL); 
          (*stringres)[k++]=grid->GetCellValue( (*it).x, (*it).y ).mb_str(wxConvUTF8);
        }
      } else {
        stringres=new DStringGDL(selection->Dim(1));
        for (SizeT k=0,n=0,l=0; n<selection->Dim(1); ++n) {
          int col = (*selection)[l++];
          int row = (*selection)[l++];
          if ( row >= numRows || col >= numCols) return static_cast<DStringGDL*>(NULL); 
          (*stringres)[k++]=grid->GetCellValue( row, col).mb_str(wxConvUTF8);
        }
      }
    } else { //IDL maintains the 2D-structure of val!
      int colTL,colBR,rowTL,rowBR;
      if (selection->Rank()==0) { 
        wxArrayInt block=grid->GetSelectedBlockOfCells();
        //normally only ONE block is available.
        colTL = block[0];
        rowTL = block[1];
        colBR = block[2];
        rowBR = block[3];
      } else {
        colTL = (*selection)[0];
        rowTL = (*selection)[1];
        colBR = (*selection)[2];
        rowBR = (*selection)[3];
      }
      SizeT dims[2];
      dims[1]=(rowBR-rowTL+1);
      dims[0]=(colBR-colTL+1);
      dimension dim(dims,2);
      stringres=new DStringGDL(dim);
      for (SizeT k=0,i=rowTL; i<=rowBR; ++i) for (SizeT j=colTL; j<=colBR; ++j)
      {
        if ( i >= numRows || j >= numCols) return static_cast<DStringGDL*>(NULL); 
        (*stringres)[k++]=grid->GetCellValue(i, j).mb_str(wxConvUTF8);
      }
    }
  }
  //convention: if value is of type struct, string array will always be row_major. thus if we are column major, transpose return string array
  if (vValue->Type()==GDL_STRUCT && majority==GDLWidgetTable::COLUMN_MAJOR) return static_cast<DStringGDL*>(stringres->Transpose(NULL))->Dup();  
  else return stringres;
}

void GDLWidgetTable::SetSelection(DLongGDL* selection)
{
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  updating=true; //prevent sending unwanted events
  grid->ClearSelection();
  wxPoint firstVisible=wxPoint(0,0);
  if (disjointSelection) { //pairs lists
    SizeT k=0;
    for (SizeT i=0; i< selection->Dim(1); ++i) {
      int col=(*selection)[k++];
      int row=(*selection)[k++];
      grid->SelectBlock(row,col,row,col,true);
      if (k==2) {firstVisible.x=row;firstVisible.y=col;} 
    }
  } else {
     int colTL = (*selection)[0];
     int rowTL = (*selection)[1];
     int colBR = (*selection)[2];
     int rowBR = (*selection)[3];
     grid->SelectBlock(rowTL,colTL,rowBR,colBR,false);
     firstVisible.x=rowTL;firstVisible.y=colTL; 
  }
  grid->EndBatch( );
  grid->MakeCellVisible(firstVisible.x,firstVisible.y);
  updating=false; //allow events
}
void GDLWidgetTable::SetTableView(DLongGDL* pos)
{  
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->MakeCellVisible((*pos)[1],(*pos)[0]);
}
void GDLWidgetTable::EditCell(DLongGDL* pos)
{  
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->SetReadOnly((*pos)[0],(*pos)[1],false);
}
void GDLWidgetTable::SetTableNumberOfColumns( DLong ncols){
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  int old_ncols=grid->GetNumberCols();
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  if (ncols > old_ncols) {
    grid->AppendCols(ncols-old_ncols);
    if (numCols > old_ncols) {
      int colTL,colBR,rowTL,rowBR;
      colTL=old_ncols-1;
      colBR=ncols-1;
      rowTL=0;
      rowBR=grid->GetNumberRows()-1;
      for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
      {
        if (i < numRows && j < numCols ) grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[j*numRows+i]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  else grid->DeleteCols(ncols,old_ncols-ncols);
  grid->EndBatch( );
}
void GDLWidgetTable::SetTableNumberOfRows( DLong nrows){
  wxGridGDL * grid = dynamic_cast<wxGridGDL*> (theWxWidget);
  assert( grid != NULL);
  grid->BeginBatch( );
  SizeT old_nrows=grid->GetNumberRows();
  int numRows=valueAsStrings->Dim(0);
  int numCols=valueAsStrings->Dim(1);
  if (nrows > old_nrows) {
    grid->AppendRows(nrows-old_nrows);
    if (numRows > old_nrows) {
      int colTL,colBR,rowTL,rowBR;
      colTL=0;
      colBR=grid->GetNumberCols()-1;
      rowTL=old_nrows-1;
      rowBR=nrows-1;
      for ( int i=rowTL; i<=rowBR; ++i) for (int j=colTL; j<=colBR; ++j)
      {
        if (i < numRows && j < numCols ) grid->SetCellValue( i, j ,wxString(((*valueAsStrings)[j*numRows+i]).c_str(), wxConvUTF8 ) ); 
      }
    }
  }
  else grid->DeleteRows(nrows,old_nrows-nrows);
  grid->EndBatch( );
}
DStructGDL* GDLWidgetTable::GetGeometry(wxRealPoint fact)
{
  if (!this->GetRealized()) this->Realize(true,false);//necessary if a geometry request is done previous to the command widget_control,xxx,,/Realize !
  GDLWidgetBase* container = static_cast<GDLWidgetBase*> (this->GetMyParent());
  assert(container != NULL);
  int ixsize = 0, iysize = 0, iscr_xsize = 0, iscr_ysize = 0;
  DFloat xsize = 0;
  DFloat ysize = 0;
  DFloat scr_xsize = 0;
  DFloat scr_ysize = 0;
  DFloat xoffset = 0;
  DFloat yoffset = 0;
  DFloat margin = 0;

  DStructGDL* ex = new DStructGDL("WIDGET_GEOMETRY");

  wxWindow* test = dynamic_cast<wxWindow*> (theWxContainer);
  if (test==NULL) return ex;
  wxRect r=test->GetRect();
  xoffset = r.x ;
  yoffset = r.y ;
  wxGrid* w = dynamic_cast<wxGrid*> (theWxWidget);
  if (w==NULL) return ex;
  wxRect z=w->GetRect();

  iscr_xsize = z.width;
  iscr_ysize = z.height;
  wxSize s=w->GetClientSize();
  ixsize = s.x;
  iysize = s.y;
  
  if (theWxWidget == theWxContainer) margin=0; else {
    margin=(r.width-z.width)/2; //Rect is about the container, which is the widgetPanel and not the frame depending on case.
  }
  //size is in columns
  int rowsize=w->GetRowSize(0);
  int rowlabelsize=w->GetRowLabelSize();
  int colsize=w->GetColSize(0);
  int collabelsize=w->GetColLabelSize();
  xsize =  (ixsize-rowlabelsize) / colsize  ;
  ysize =  (iysize-collabelsize) / rowsize  ;

  scr_xsize = iscr_xsize / fact.x;
  scr_ysize = iscr_ysize / fact.y;
  xoffset /= fact.x;
  yoffset /= fact.y;
  margin /= fact.x;

  ex->InitTag("XOFFSET", DFloatGDL(xoffset));
  ex->InitTag("YOFFSET", DFloatGDL(yoffset));
  ex->InitTag("XSIZE", DFloatGDL(xsize));
  ex->InitTag("YSIZE", DFloatGDL(ysize));
  ex->InitTag("SCR_XSIZE", DFloatGDL(scr_xsize));
  ex->InitTag("SCR_YSIZE", DFloatGDL(scr_ysize));
  ex->InitTag("MARGIN", DFloatGDL(margin));

  return ex;
}

void GDLWidgetTable::setFont() {
   wxGrid* g = dynamic_cast<wxGrid*> (this->GetWxWidget());
   if (g != NULL) {
    if (!font.IsSameAs(wxNullFont)) {
     g->SetLabelFont(font);
     g->SetDefaultCellFont(font);
    } else {
     g->SetLabelFont(defaultFont);
     g->SetDefaultCellFont(defaultFont);
    }
   }
  }
GDLWidgetTable::~GDLWidgetTable()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTable(" << widgetID << ")" << std::endl;
#endif  
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
  
  GDLDelete( table_alignment );
  GDLDelete( editable );
  GDLDelete( amPm );
  GDLDelete( backgroundColor );
  GDLDelete( foregroundColor );
  GDLDelete( columnLabels );
  GDLDelete( columnWidth );
  GDLDelete( daysOfWeek );
  GDLDelete( format );
  GDLDelete( month );
  GDLDelete( rowHeights );
  GDLDelete( rowLabels );
  GDLDelete( valueAsStrings );
}

/*********************************************************/
// for WIDGET_TREE
/*********************************************************/

GDLWidgetTree::GDLWidgetTree( WidgetIDT p, EnvT* e, BaseGDL* value_, DULong eventFlags_
,wxBitmap* bitmap
,DLong dragability
,DLong dropability
,bool expanded_
,bool folder_
,DLong treeindex
)
: GDLWidget( p, e, value_, eventFlags_ )
,droppable( false )
,draggable( false )
,expanded(expanded_)
,folder(folder_)
,rootID(0L)
,buttonImageId(0L)
,imageId(0L)
,treeItemData(NULL)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  DStringGDL* value=static_cast<DStringGDL*>(vValue);

  if ( gdlParent->IsBase( ) ) {
  
    START_ADD_EVENTUAL_FRAME
  
    if ( wSize.x <= 0 ) wSize.x = 200; //yes, has a default value!
    if ( wSize.y <= 0 ) wSize.y = 200;
    wSize=computeWidgetSize( );
    long style = wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT; //wxTR_HAS_BUTTONS|wxTR_TWIST_BUTTONS|wxTR_HIDE_ROOT|wxTR_HAS_VARIABLE_ROW_HEIGHT; //(wxTR_HIDE_ROOT|wxTR_DEFAULT_STYLE| wxTR_HAS_BUTTONS | wxSUNKEN_BORDER | wxTR_TWIST_BUTTONS)    ;
    // should be as of 2.9.0:  wxDataViewTreeCtrl* tree = new gdlTreeCtrl( widgetPanel, widgetID,
    wxTreeCtrlGDL* tree = new wxTreeCtrlGDL( widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize, style );
    theWxContainer = theWxWidget = tree;

    //our widget will ALWAYS have an image list...
    wxImageList* images=new wxImageList();
    images->Add(wxArtProvider::GetBitmap(wxART_FOLDER)); //0
    images->Add(wxArtProvider::GetBitmap(wxART_FOLDER_OPEN)); //1
    images->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE)); //2
////    images->Add(wxArtProvider::GetBitmap(wxART_FILE_OPEN)); //3 //no use: a selected entry is highlighted and there is no specific wxArt pixmap to do more (visually).

    tree->AssignImageList(images);
    folder=true;
    rootID=widgetID;
    treeItemData=new wxTreeItemDataGDL(widgetID);
    if (bitmap) {
      int index=images->Add(*bitmap);
      treeItemID = tree->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  index ,-1, treeItemData);
    } else { //use open and closed folder icons
      treeItemID = tree->AddRoot(wxString( (*value)[0].c_str( ), wxConvUTF8 ),  0 ,1, treeItemData);
    }    
    widgetStyle=widgetAlignment( );
    draggable=(dragability == 1);
    droppable=(dropability == 1);
//    tree->Expand(treeItemID); //do not expand root if hidden  
    tree->SetSize(wSize);
    tree->SetMinSize(wSize);
    END_ADD_EVENTUAL_FRAME
    TIDY_WIDGET(gdlBORDER_SPACE)
      
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemActivated),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnBeginDrag),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_END_DRAG,wxTreeEventHandler(wxTreeCtrlGDL::OnItemDropped),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemCollapsed),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemExpanded),tree);
    this->AddToDesiredEvents(wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEventHandler(wxTreeCtrlGDL::OnItemSelected),tree);
    UPDATE_WINDOW
    REALIZE_IF_NEEDED
      
  } else {
    GDLWidgetTree* parentTree = static_cast<GDLWidgetTree*> (gdlParent);
    assert( parentTree != NULL);
    theWxWidget = parentTree->GetWxWidget( );
    rootID =  parentTree->GetRootID();
    treeItemData=new wxTreeItemDataGDL(widgetID);
    wxTreeCtrlGDL * tree = dynamic_cast<wxTreeCtrlGDL*> (theWxWidget);
    assert( tree != NULL);
    theWxContainer=NULL; //this is not a widget

    wxImageList* images=tree->GetImageList();
    //if image is provided use it otherwise (since no image is a bit disappointing) use an internal wxWigdets icon
    if (bitmap) {
      int imindex=images->Add(*bitmap);
      if (treeindex > -1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) , imindex ,-1, treeItemData);
      else treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) , imindex ,-1, treeItemData);
    } else { //use open and closed folder icons
      if (folder) {
        if (treeindex>-1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,0,1, treeItemData);
        else treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,0,1, treeItemData);
      }
      else if (treeindex>-1) treeItemID = tree->InsertItem( parentTree->treeItemID, treeindex, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,2,2, treeItemData);
      else  treeItemID = tree->AppendItem( parentTree->treeItemID, wxString( (*value)[0].c_str( ), wxConvUTF8 ) ,2,2, treeItemData);
    }
    if ( parentTree->IsFolder() && parentTree->IsExpanded())  parentTree->DoExpand();
    //dragability inheritance.
    if (dragability == -1) draggable=parentTree->IsDraggable(); else draggable=(dragability == 1);
   //dropability inheritance.
    if (dropability == -1) droppable=parentTree->IsDroppable(); else droppable=(dropability == 1);
  }
}
DInt GDLWidgetTree::GetTreeIndex()
{
  DInt count=0;
  wxTreeCtrlGDL* tree=dynamic_cast<wxTreeCtrlGDL*>(theWxWidget);
  assert( tree != NULL);
  wxTreeItemId  id=this->treeItemID;
  wxTreeItemId prev_id=tree->GetPrevSibling(id);
  while (prev_id.IsOk()){
    count++; 
    id=prev_id; 
    prev_id=tree->GetPrevSibling(id);
  }
  return count+1; //to give compatible results with idl -- wxwidgets does not behave as idl!
}

GDLWidgetTree::~GDLWidgetTree()
{
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetTree: (" << widgetID <<")"<< std::endl;
#endif  
  //the wxWidget points to  the parent branch. A leaf has wxContainer=NULL. //If we are on a leaf, set thewxWidget to NULL at the end, as it would be doubly destroyed in ~GDLWidget otherwise.
  
  wxTreeCtrlGDL* tree = dynamic_cast<wxTreeCtrlGDL*> (theWxWidget);
  if (tree) { // container-type behaviuor: kill gdl childrens 
    wxTreeItemId id = this->treeItemID;
    if (id.IsOk()) {
      wxTreeItemIdValue cookie;
      wxTreeItemId child = tree->GetFirstChild(id, cookie);
      while (child.IsOk()) {
        WidgetIDT childID = dynamic_cast<wxTreeItemDataGDL*> (tree->GetItemData(child))->widgetID;
        GDLWidgetTree* GDLchild = static_cast<GDLWidgetTree*> (GDLWidget::GetWidget(childID));
        child = tree->GetNextSibling(child); //nextChild did not find the last (?) nextSibling does the job.
#ifdef GDL_DEBUG_WIDGETS
        std::cout << "~GDLWidgetTree, deleting item ID #" << childID << " of tree widget #" << widgetID << std::endl;
#endif
        delete GDLchild;
      }
      treeItemID.Unset();
      tree->Delete(id);
    }
    tree->Refresh();
  }
  //after all children destroyed, destroy my wx counterpart
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetTree::SetValue(DString val)
{
  GDLDelete(vValue); 
  vValue=new DStringGDL(val);
  wxTreeCtrlGDL* tree=dynamic_cast<wxTreeCtrlGDL*>(theWxWidget);
  assert( tree != NULL);
  tree->SetItemText(treeItemID, wxString( val.c_str( ), wxConvUTF8 ));
}

/*********************************************************/
// for WIDGET_SLIDER
/*********************************************************/
GDLWidgetSlider::GDLWidgetSlider( WidgetIDT p, EnvT* e, DLong value_
, DULong eventFlags_
, DLong minimum_
, DLong maximum_
, bool vertical
, bool suppressValue
, DString &title_ )
: GDLWidget( p, e, NULL, eventFlags_ )
, value( value_ ) //should disappear , duplicates vValue!
, minimum( minimum_ )
, maximum( maximum_ )
, title( title_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );

  START_ADD_EVENTUAL_FRAME
    
  if (( wSize.x <= 0 ) && !vertical) wSize.x=100; //yes, has a default value!
  if (( wSize.y <= 0 ) && vertical) wSize.y=100;

  long style = wxSL_HORIZONTAL;
  if ( vertical ) style = wxSL_VERTICAL;
  if ( !suppressValue ) {
#if wxCHECK_VERSION(2,9,1)
    style |= wxSL_VALUE_LABEL;
#else
    style |= wxSL_LABELS;
#endif
  }

  widgetStyle=widgetAlignment();
  const wxString s=wxString( title.c_str( ), wxConvUTF8 );
  wxSlider* slider;

//get defined sizes if any
  wSize=computeWidgetSize( );
// this is the size in absence of text:  
  wxSize slidersize=(vertical)?wxSize(-1,wSize.y):wxSize(wSize.x,-1);
  bool hastitle=(s.size()>0);
//// A gdlSlider is a comboPanel containing a wxStaticText (eventually) plus a slider. 
  //if text or frame, we need a panel containing the wxVERTICAL association. frame is sunken if /FRAME, or NONE
  if (hastitle) {
    wxPanel* comboPanel=new wxPanel(widgetPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    wxSizer* combosz = new wxBoxSizer(wxVERTICAL);
    comboPanel->SetSizer(combosz);
    theWxContainer = comboPanel; //else stays as panel_if_framed.
    comboPanel->SetFont(font); //enable fancy font
    //little difficulty: insure that combo widget size is correct in presence of (fancy) text.
    int w=0;
    int h=0;
    int d=0;
    int lead=0;
    comboPanel->GetTextExtent(s,&w,&h,&d,&lead,&font);
    slidersize=(vertical)?wxSize(-1,wSize.y-h-3*gdlSPACE):wxSize(max(wSize.x,w),-1);
    slider= new wxSlider( comboPanel, widgetID, value, minimum, maximum, wxDefaultPosition, wxDefaultSize, style);
    slider->SetSize(slidersize);
    slider->SetMinSize(slidersize);
    combosz->Add(slider,DONOTALLOWSTRETCH,wxEXPAND) ; //wxALL|(vertical)?wxALIGN_CENTRE_HORIZONTAL:widgetStyle,gdlSPACE);
    wxStaticText *theTitle=new wxStaticText(comboPanel,wxID_ANY,s);
    combosz->Add(theTitle,DONOTALLOWSTRETCH,widgetStyle|wxALL,gdlSPACE);
    combosz->AddSpacer(5);
    comboPanel->Fit();
    theWxWidget=comboPanel; //to trick ADD EVENTUAL FRAME 
  } else {
    slider= new wxSlider( widgetPanel, widgetID, value, minimum, maximum, wxDefaultPosition, wxDefaultSize, style);
    slider->SetSize(slidersize);
    slider->SetMinSize(slidersize);
    theWxContainer = theWxWidget=slider;
    widgetPanel->Fit();
  }

  END_ADD_EVENTUAL_FRAME
  theWxWidget=slider; //no trick anymore!
  TIDY_WIDGET(gdlBORDER_SPACE)
  UPDATE_WINDOW
  REALIZE_IF_NEEDED 
  this->AddToDesiredEvents( wxEVT_SCROLL_CHANGED,wxScrollEventHandler(gdlwxFrame::OnThumbRelease),slider);
  //dynamically select drag, saves resources! (note: there is no widget_control,/drag for sliders)
  if ( eventFlags & GDLWidget::EV_DRAG ) this->AddToDesiredEvents( wxEVT_SCROLL_THUMBTRACK,wxScrollEventHandler(gdlwxFrame::OnThumbTrack),slider);
}
void GDLWidgetSlider::ControlSetValue(DLong v){
  value=v;
  wxSlider* s=dynamic_cast<wxSlider*>(theWxWidget);
  assert( s != NULL);
  s->SetValue(v);
}

void GDLWidgetSlider::ControlSetMinValue(DLong v) {
  value = v;
  wxSlider* s = dynamic_cast<wxSlider*> (theWxWidget);
  assert(s != NULL);
  s->SetRange(v, s->GetMax());
}

void GDLWidgetSlider::ControlSetMaxValue(DLong v) {
  value = v;
  wxSlider* s = dynamic_cast<wxSlider*> (theWxWidget);
  assert(s != NULL);
  s->SetRange(s->GetMin(),v);
}

GDLWidgetSlider::~GDLWidgetSlider(){
  title.clear();
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSlider(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetButton::GDLWidgetButton( WidgetIDT p, EnvT* e,
DStringGDL* value , DULong eventflags, wxBitmap* bitmap_)
: GDLWidget( p, e, value, eventflags )
, buttonType( UNDEFINED )
, buttonBitmap(bitmap_)
, buttonState(false)
, menuItem(NULL)
, valueWxString(wxString((*value)[0]))
{
//  valueWxString = wxString((*value)[0]);
  if (valueWxString.Length() < 1) valueWxString=wxT(" ");
}


GDLWidgetButton::~GDLWidgetButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetButton(" << widgetID << ")" << std::endl;
#endif
  if (buttonBitmap) delete buttonBitmap;
}

//a normal button.
GDLWidgetNormalButton::GDLWidgetNormalButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags,  wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
{
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();
  
  START_ADD_EVENTUAL_FRAME

  widgetStyle = widgetAlignment();
  wSize = computeWidgetSize();
  long style=wxBORDER_NONE;
  //we deliberately prevent exclusive buttons when bitmap are present (exclusive buttons w/ pixmap do not exist in wxWidgets.
  if (gdlParent->GetExclusiveMode() == BGNORMAL || bitmap_) {
    if (bitmap_) {
      wxBitmapButton *button = new wxBitmapButton(widgetPanel, widgetID, *bitmap_, wxDefaultPosition, wxDefaultSize, style);
      theWxContainer = theWxWidget = button;
      buttonType = BITMAP;
      this->AddToDesiredEvents(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gdlwxFrame::OnButton), button);
    } else {
      wxButton *button = new wxButton(widgetPanel, widgetID, valueWxString,wxDefaultPosition, wxDefaultSize, buttonTextAlignment()); 
      theWxContainer = theWxWidget = button;
      buttonType = NORMAL;
      this->AddToDesiredEvents(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(gdlwxFrame::OnButton), button);
    }
  } else if (gdlParent->GetExclusiveMode() == BGEXCLUSIVE1ST) {
    wxRadioButton *radioButton = new wxRadioButton(widgetPanel, widgetID, valueWxString,  wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
    gdlParent->SetExclusiveMode(1);
    GDLWidgetBase* b = static_cast<GDLWidgetBase*> (gdlParent);
    if (b) b->SetLastRadioSelection(widgetID);
    else cerr << "Warning GDLWidgetNormalButton::GDLWidgetNormalButton(): widget type confusion(1)\n";

    theWxContainer = theWxWidget = radioButton;
    this->AddToDesiredEvents(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(gdlwxFrame::OnRadioButton), radioButton);
    buttonType = RADIO;
  } else if (gdlParent->GetExclusiveMode() == BGEXCLUSIVE) {
    wxRadioButton *radioButton = new wxRadioButton(widgetPanel, widgetID, valueWxString, wxDefaultPosition, wxDefaultSize);
    theWxContainer = theWxWidget = radioButton;
    this->AddToDesiredEvents(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(gdlwxFrame::OnRadioButton), radioButton);
    buttonType = RADIO;
  } else if (gdlParent->GetExclusiveMode() == BGNONEXCLUSIVE) {
    wxCheckBox *checkBox = new wxCheckBox(widgetPanel, widgetID, valueWxString,  wxDefaultPosition, wxDefaultSize);
    theWxContainer = theWxWidget = checkBox;
    this->AddToDesiredEvents(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(gdlwxFrame::OnCheckBox), checkBox);
    buttonType = CHECKBOX;
  } else cerr << "Warning GDLWidgetNormalButton::GDLWidgetNormalButton(): widget type confusion(2)\n";
  wxWindow *win = static_cast<wxWindow*> (theWxWidget);
  if (win && buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));

  END_ADD_EVENTUAL_FRAME
    
  win->SetSize(wSize);
  win->SetMinSize(wSize);

  TIDY_WIDGET(gdlBORDER_SPACE)
  UPDATE_WINDOW
  REALIZE_IF_NEEDED


}

void GDLWidgetNormalButton::SetButtonWidgetLabelText(const DString& value_) {
  if (theWxWidget != NULL) {
    wxAnyButton *b = dynamic_cast<wxAnyButton*> (theWxWidget);
    b->SetLabelText(wxString(value_.c_str(), wxConvUTF8));
    // Should switch to version > 2.9 now!
#if wxCHECK_VERSION(2,9,1)
    b->SetBitmap(wxBitmap(1, 1));
#endif 
    if (this->IsDynamicResize()) {
      this->SetWidgetSize(0,0);
//      this->RefreshDynamicWidget();
    }
  }
}

GDLWidgetNormalButton::~GDLWidgetNormalButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetNormalButton(" << widgetID << ")" << std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

//this type of buttons use a container
GDLWidgetMenu::GDLWidgetMenu(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
{
}

GDLWidgetMenu::~GDLWidgetMenu() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenu(" << widgetID << ")" << std::endl;
#endif
  // delete all children (in reverse order ?)
  while (!children.empty()) {
    GDLWidget* child = GetWidget(children.back());
    children.pop_back();
    if (child) {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenu(" << widgetID << "): removing child: ID=" << child->GetWidgetID() << std::endl;
#endif
      delete child;
    } else {
#ifdef GDL_DEBUG_WIDGETS
      std::cout << "~GDLWidgetMenu(" << widgetID << "): ignoring unexisting child: ID=" << child->GetWidgetID() << std::endl;
#endif
    }
  }
}

//this type of buttons use a container
GDLWidgetSubMenu::GDLWidgetSubMenu(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, bool hasSeparatorAbove, wxBitmap* bitmap_)
: GDLWidgetMenu(p, e, value, eventflags, bitmap_)
, addSeparatorAbove( hasSeparatorAbove)
, the_sep(NULL)
{
  GDLWidget* gdlParent = GetWidget(parentID);

  //get default value: a menu. May be NULL here
  wxMenu *menu = dynamic_cast<wxMenu*> (gdlParent->GetWxWidget());
  wxMenu* submenu = new wxMenu();
  if (addSeparatorAbove) the_sep=menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, widgetID, valueWxString, wxEmptyString, wxITEM_NORMAL, submenu );
  if (bitmap_) menuItem->SetBitmap(*bitmap_);
  menu->Append(menuItem);
  menu->Enable(menuItem->GetId(), sensitive);
  theWxContainer = menu; //is not a window: special treatment
  theWxWidget = submenu; 
  buttonType = MENU;
  //No UPDATE_WINDOW see menu entry.

}

void GDLWidgetSubMenu::SetSensitive(bool value) {
  sensitive = value;
  if (menuItem) menuItem->Enable(value);
}

void GDLWidgetSubMenu::SetButtonWidgetLabelText( const DString& value_ ) {
  if (menuItem && value_.length() > 0 ) menuItem->SetItemLabel( wxString(value_.c_str( ), wxConvUTF8 ) ); //avoid null strings asserts!
}

void GDLWidgetSubMenu::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
    if ( menuItem ) {
      menuItem->SetBitmap( *bitmap_ ); //probably only MsW
    }
}

GDLWidgetSubMenu::~GDLWidgetSubMenu() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetSubMenu(" << widgetID << ")" << std::endl;
#endif
  GDLWidgetMenu* gdlParent = dynamic_cast<GDLWidgetMenu*> (GetWidget(parentID));
  if (gdlParent) gdlParent->RemoveChild(widgetID);
  
  if (menuItem) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetSubMenu(" << widgetID << "): attempting to remove submenu #" << menuItem->GetId() << " from menu " << parentID << std::endl;
#endif
    wxMenu* menu = dynamic_cast<wxMenu*> (theWxContainer);
    menu->Remove(menuItem);
    if (addSeparatorAbove) menu->Remove(the_sep);
  }
}

//this type of buttons use a container
GDLWidgetMenuEntry::GDLWidgetMenuEntry(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, bool hasSeparatorAbove, wxBitmap* bitmap_)
: GDLWidgetButton(p, e, value, eventflags, bitmap_)
, addSeparatorAbove( hasSeparatorAbove)
, the_sep(NULL)
{
  GDLWidget* gdlParent = GetWidget(parentID);

  //get default value: a menu. May be NULL here
  wxMenu *menu = dynamic_cast<wxMenu*> (gdlParent->GetWxWidget());
//  //special treatment for popups menus: the menu is retrieved differently
//  GDLWidgetButton* whatSortofBut = static_cast<GDLWidgetButton*> (gdlParent);
//  if (whatSortofBut->buttonType == POPUP_NORMAL) {
//    menu = dynamic_cast<wxButtonGDL*> (whatSortofBut->GetWxWidget())->GetPopupMenu();
//  } else if (whatSortofBut->buttonType == POPUP_BITMAP) {
//    menu = dynamic_cast<wxBitmapButtonGDL*> (whatSortofBut->GetWxWidget())->GetPopupMenu();
//  }
  if (addSeparatorAbove) the_sep=menu->AppendSeparator();
  menuItem = new wxMenuItem(menu, widgetID, valueWxString);
  if (bitmap_) menuItem->SetBitmap(*bitmap_);
  menu->Append(menuItem);
  menu->Enable(menuItem->GetId(), sensitive);
  theWxContainer = menu; //is not a window
  theWxWidget = menuItem;
  buttonType = ENTRY;
  //NO UPDATE WINDOW as menu are dynamical on stack.
}

void GDLWidgetMenuEntry::SetSensitive(bool value) {
  sensitive = value;
  wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item) item->Enable(value);
}

void GDLWidgetMenuEntry::SetButtonWidgetLabelText( const DString& value_ ) {
  wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item)  item->SetItemLabel(wxString( value_.c_str( ), wxConvUTF8 ));
}
void GDLWidgetMenuEntry::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
wxMenuItem* item = dynamic_cast<wxMenuItem*> (theWxWidget);
  if (item) {
    item->SetBitmap( *bitmap_); //probably only on MSW
  }
}

GDLWidgetMenuEntry::~GDLWidgetMenuEntry() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuEntry(" << widgetID << ")" << std::endl;
#endif
   GDLWidgetMenu* gdlParent = dynamic_cast<GDLWidgetMenu*>(GetWidget(parentID));
   if (gdlParent) gdlParent->RemoveChild(widgetID);
  if (menuItem) {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetMenuEntry(" << widgetID << "): attempting to remove menuitem #" << menuItem->GetId() << " of menu " << parentID << std::endl;
#endif
    wxMenu* menu = dynamic_cast<wxMenu*> (theWxContainer);
    menu->Remove(menuItem);
    if (addSeparatorAbove) menu->Remove(the_sep);
  }
}

//Mbar buttons are pulldown MENUS, so container

#ifdef PREFERS_MENUBAR
GDLWidgetMenuBarButton::GDLWidgetMenuBarButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, NULL)
, entry(-1) {
  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  if (gdlParent) {
    wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (gdlParent->GetWxWidget());
    assert(menuBar != NULL);
    wxMenu* menu = new wxMenu();
    theWxContainer = menuBar; //not a WwxWindow
    theWxWidget = menu;
    entry = menuBar->GetMenuCount();
    menuBar->Insert(entry, menu, valueWxString);
    menuBar->EnableTop(entry, sensitive);
    buttonType = MENU;

    //MBAR menus cannot have a tooltip due to 

//    UPDATE_WINDOW; REALIZE_IF_NEEDED; //made on the fly, non need.
  }
}

#else
GDLWidgetMenuBarButton::GDLWidgetMenuBarButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_,  DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, NULL)
, entry(NULL) {


  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  assert(gdlParent->IsMenuBar());
  
  wxToolBar *toolBar = dynamic_cast<wxToolBar*> (gdlParent->GetWxWidget());
    assert(toolBar != NULL);
    if (bitmap_ == NULL) {
    wSize=computeWidgetSize();
    wxSize tbSize=toolBar->GetSize();
    wxButtonGDL *button = new wxButtonGDL(font, toolBar, widgetID, valueWxString,
      wOffset, wSize,  wxBORDER_NONE);
    buttonType = POPUP_NORMAL; //gdlMenuButton is a wxButton --> normal. Bitmaps will be supported starting from 2.9.1 
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
    entry=toolBar->AddControl(button);
    if (tbSize.y < wSize.y) toolBar->SetSize(wxSize(-1,wSize.y));
    toolBar->Realize();
    } else {
    wxBitmapButtonGDL *button = new wxBitmapButtonGDL(toolBar, widgetID, *bitmap_,
      wOffset, wxDefaultSize,  wxBU_EXACTFIT|wxBORDER_NONE);
    buttonType = POPUP_BITMAP; //
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
    entry=toolBar->AddControl(button);
    toolBar->Realize();
    }
    
  wxWindow *win = dynamic_cast<wxWindow*> (theWxContainer);
  if (win) {
    if (buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));
    if (widgetSizer) widgetSizer->Add(win, DONOTALLOWSTRETCH, widgetStyle|wxALL, gdlSPACE); //|wxALL, gdlSPACE_BUTTON);
  } else cerr << "Warning GDLWidgetMenuButton::GDLWidgetMenuButton(): widget type confusion.\n";

//    UPDATE_WINDOW; REALIZE_IF_NEEDED;
}
#endif

void GDLWidgetMenuBarButton::SetSensitive(bool value) {
#ifdef PREFERS_MENUBAR
  sensitive = value;
  wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
  menuBar->EnableTop(entry, value);
#else
  GDLWidget::SetSensitive(value)  ;
#endif
}

void GDLWidgetMenuBarButton::SetButtonWidgetLabelText( const DString& value_ ) {
#ifdef PREFERS_MENUBAR
  wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
  menuBar->SetMenuLabel(entry, wxString( value_.c_str( ), wxConvUTF8 ));
#else
  wxButton* m=dynamic_cast<wxButton*>(theWxWidget);
  if (m) m->SetLabelText(wxString( value_.c_str( ), wxConvUTF8 ));
  this->RefreshDynamicWidget();
#endif
}

GDLWidgetMenuBarButton::~GDLWidgetMenuBarButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuBarButton(" << widgetID << ")" << std::endl;
#endif
#ifdef PREFERS_MENUBAR
  GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*>(GetWidget(parentID));
  //which pos?
  if (gdlParent) {
    int pos=gdlParent->GetChildrenPos(widgetID);
    gdlParent->RemoveChild(widgetID);
    if (pos >= 0) {
      wxMenuBar *menuBar = dynamic_cast<wxMenuBar*> (theWxContainer);
      menuBar->Remove(pos);
    }
  }
#else
 GDLWidgetMenuBar* gdlParent = dynamic_cast<GDLWidgetMenuBar*> (GetWidget(parentID));
  assert(gdlParent->IsMenuBar());
  wxToolBar *toolBar = dynamic_cast<wxToolBar*> (gdlParent->GetWxWidget());
  assert(toolBar != NULL);
  toolBar->RemoveTool(widgetID);
#endif
}

//a MenuButton Widget is an otherwise normal button in a Base that activates a pulldown menu and is a container
GDLWidgetMenuButton::GDLWidgetMenuButton(WidgetIDT p, EnvT* e,
  DStringGDL* value, DULong eventflags, wxBitmap* bitmap_, DStringGDL* buttonToolTip)
: GDLWidgetMenu(p, e, value, eventflags, bitmap_) {
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();
  widgetStyle = widgetAlignment();
  wSize = computeWidgetSize();

  assert(gdlParent->IsBase());

  //A menu button in a base is a button starting a popup menu. it is a container also.
  //2 different buttons, if bitmap or not, waiting for version > 2.9.1 to have bitmaps handled for ALL buttontypes:
  if (bitmap_ == NULL) {
    wxButtonGDL *button = new wxButtonGDL(font, widgetPanel, widgetID, valueWxString,
      wOffset, wxDefaultSize, buttonTextAlignment());
    button->SetSize(wSize);
    button->SetMinSize(wSize);
    buttonType = POPUP_NORMAL; //gdlMenuButton is a wxButton --> normal. Bitmaps will be supported starting from 2.9.1 
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
  } else {
    wxBitmapButtonGDL *button = new wxBitmapButtonGDL(widgetPanel, widgetID, *bitmap_,
      wOffset, wxDefaultSize, buttonTextAlignment());
    button->SetSize(wSize);
    button->SetMinSize(wSize);
    buttonType = POPUP_BITMAP; //
    theWxContainer = button;
    theWxWidget = button->GetPopupMenu(); //a menu
    button->Enable(sensitive);
  }
  wxWindow *win = dynamic_cast<wxWindow*> (theWxContainer);
  if (win) {
    if (buttonToolTip) win->SetToolTip(wxString((*buttonToolTip)[0].c_str(), wxConvUTF8));
    if (widgetSizer) widgetSizer->Add(win, DONOTALLOWSTRETCH, widgetStyle|wxALL, gdlSPACE); //|wxALL, gdlSPACE_BUTTON);
  } else cerr << "Warning GDLWidgetMenuButton::GDLWidgetMenuButton(): widget type confusion.\n";

  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}

void GDLWidgetMenuButton::SetSensitive(bool value) {
  sensitive = value;
  wxButton* m = dynamic_cast<wxButton*> (theWxContainer);
  if (m) m->Enable(value);
}

void GDLWidgetMenuButton::SetButtonWidgetLabelText( const DString& value_ ) {
  wxButton* m=dynamic_cast<wxButton*>(theWxContainer);
  if (m) m->SetLabelText(wxString( value_.c_str( ), wxConvUTF8 ));
  this->RefreshDynamicWidget();
}

GDLWidgetMenuButton::~GDLWidgetMenuButton() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetMenuButton(" << widgetID << ")" << std::endl;
#endif
    GDLWidgetBase* b=dynamic_cast<GDLWidgetBase*>(GetWidget(parentID));
    if (b && !b->IsContextBase()) if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
  }

void GDLWidgetButton::SetButtonWidgetBitmap( wxBitmap* bitmap_ ) {
  if ( buttonType == BITMAP || buttonType == POPUP_BITMAP ) {
    wxBitmapButton *b = dynamic_cast<wxBitmapButton*> (theWxContainer); //not the wxWidget since the widget is the popup menu itself.
    if ( b ) {
      b->SetBitmapLabel( *bitmap_ );
      b->SetLabelText(wxEmptyString);
      }
  }  else if ( buttonType == MENU || buttonType ==ENTRY) {
    if ( menuItem != NULL ) {
      menuItem->SetBitmap( *bitmap_ );
      menuItem->SetItemLabel(wxT(" ")); //setting null strings to menus is not permitted !!!!
    }
  }
// Should switch to version > 2.9 now!
#if wxCHECK_VERSION(2,9,1)
  else if ( buttonType == NORMAL || buttonType == POPUP_NORMAL) {
      wxButton *b = dynamic_cast<wxButton*> (theWxWidget);
      if ( b ) {
        b->SetBitmapLabel( *bitmap_ );
        b->SetLabelText(wxEmptyString);
      }
     }
#endif 
  this->RefreshDynamicWidget();
}
  
GDLWidgetList::GDLWidgetList( WidgetIDT p, EnvT* e, BaseGDL *value, DLong style, DULong eventflags )
    : GDLWidget( p, e, value, eventflags)
, maxlinelength(0)
, nlines(0)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  START_ADD_EVENTUAL_FRAME
    
  widgetStyle = widgetAlignment();  
  if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  nlines = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < nlines; ++i ){
    int length=(*val)[i].length();
    if (length>maxlinelength){
      maxlinelength=length;
    }
    choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  }
  wxListBox * list=new wxListBox();
  theWxContainer = theWxWidget = list;
  //ok now size can be computed
  list->Create(widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize , choices, style|wxLB_NEEDED_SB|wxLB_SINGLE); //|wxLB_MULTIPLE );
  this->setFont(); //set fancy font before computing sizes!
  wSize=computeWidgetSize();
  list->SetClientSize(wSize);
  list->SetMinClientSize(wSize);
  list->SetSelection(wxNOT_FOUND);//necessary to avoid spurious event on first click.
  this->AddToDesiredEvents( wxEVT_COMMAND_LISTBOX_DOUBLECLICKED,wxCommandEventHandler(gdlwxFrame::OnListBoxDoubleClicked),list);
  this->AddToDesiredEvents( wxEVT_COMMAND_LISTBOX_SELECTED,wxCommandEventHandler(gdlwxFrame::OnListBox),list);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
  
  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}
void GDLWidgetList::SetWidgetSize(DLong sizex, DLong sizey)
{ 

  START_CHANGESIZE_NOEVENT
  
  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetList::SetSize currentSize=%d,%d \n"),currentSize.x,currentSize.y);
#endif

  //widget text size is in LINES in Y and CHARACTERS in X.
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight=(fontSize.y<20)?fontSize.y*1.5:fontSize.y*1.2;
  if (sizex > 0) {
    widgetSize.x = (sizex+0.5) * fontSize.x;
  } else {
    widgetSize.x = currentSize.x;
  } 

  if (sizey > 0) {
    widgetSize.y =sizey * lineHeight;
  } else {
    widgetSize.y = currentSize.y;
  }

  if (sizex > 0 && maxlinelength > sizex) widgetSize.y += gdlSCROLL_HEIGHT_X;
  if (nlines > sizey) widgetSize.x += gdlSCROLL_WIDTH_Y;
  widgetSize.y += 10;

  sizex=ceil(widgetSize.x);
  sizey=ceil(widgetSize.y);
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  me->SetMinSize(wSize);
  
  if (!widgetSizer) { if (framePanel) framePanel->Fit();}
  
  UPDATE_WINDOW

  END_CHANGESIZE_NOEVENT

#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

void GDLWidgetList::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);

  wxArrayString newchoices; // = new wxString[n];
  nlines = val->N_Elements( );
  for ( SizeT i = 0; i < nlines; ++i ){
    int length=(*val)[i].length();
    if (length>maxlinelength){
      maxlinelength=length;
    }
   newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );
  }

  wxListBox * list = dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
  
  list->Clear();
  list->InsertItems(newchoices,0);
  list->SetSelection(wxNOT_FOUND);
}

void GDLWidgetList::SelectEntry(DLong entry_number){
  if (entry_number < 0 ) return;
 wxListBox * list=dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
  if (entry_number > list->GetCount()-1) return;
 list->Select(entry_number); 
}

BaseGDL* GDLWidgetList::GetSelectedEntries(){
 wxListBox * list=dynamic_cast<wxListBox*>(theWxWidget);
  assert( list != NULL);
 wxArrayInt selections;
 list->GetSelections(selections);
 DIntGDL* liste;
 if  (selections.Count()<1) {
   liste=new DIntGDL(-1);
   return liste;
 }
 liste=new DIntGDL(dimension(selections.Count()));
 for (SizeT i=0; i< selections.Count(); ++i) (*liste)[i]=selections[i];
 return liste;
}

GDLWidgetList::~GDLWidgetList(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetList(" << widgetID << ")" << std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetDropList::GDLWidgetDropList( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags ,
const DString& title_, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )), eventflags)
, title( title_ )
, style( style_)
{
  GDLWidget* gdlParent = GetWidget(parentID);
  widgetPanel = GetParentPanel();
  widgetSizer = GetParentSizer();

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements();
  wxArrayString choices; // = new wxString[n];
  for (SizeT i = 0; i < n; ++i) {
    choices.Add(wxString((*val)[i].c_str(), wxConvUTF8));
  }
   
///  static int flatIx=e->KeywordIx("FLAT"); ignored.
   widgetStyle=widgetAlignment();
  bool hastitle=(title.size()>0);

  //get defined sizes if any
  wSize=computeWidgetSize( );
  
  wxChoice * droplist;
  theWxContainer=NULL;
  wxSizer* theSizer=NULL;
  wxWindow* theWindow = widgetPanel;
  if (frameWidth > 0) {
    framePanel=new wxPanel(theWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, gdlBORDER_EXT);
 #ifdef GDL_DEBUG_WIDGETS_COLORIZE
    framePanel->SetBackgroundColour(wxColour(0x3a, 0x82, 0x56)); //vert fonce
#endif
    wxSizer* frameSizer=new wxBoxSizer(wxVERTICAL);
    framePanel->SetSizer(frameSizer);
    theWxContainer=framePanel;
    theWindow=framePanel;
    wxPanel* intFramePanel=new wxPanel(framePanel);
    theWindow=intFramePanel;
    frameSizer->Add(intFramePanel, DONOTALLOWSTRETCH, wxALL, frameWidth);
  }
  if (hastitle) {
    wxPanel* comboPanel=new wxPanel(theWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    wxSizer* combosz = new wxBoxSizer(wxHORIZONTAL);
    comboPanel->SetSizer(combosz);
    if (!theWxContainer) theWxContainer = comboPanel; //else stays as panel_if_framed.
    this->setFont(comboPanel); //enable fancy font
    wxStaticText *theTitle=new wxStaticText(comboPanel,wxID_ANY,wxString(title.c_str(), wxConvUTF8));
    combosz->Add(theTitle,DONOTALLOWSTRETCH, wxALIGN_CENTER|wxALL,gdlSPACE);
    combosz->AddSpacer(5);
    droplist = new wxChoice( comboPanel, widgetID, wxDefaultPosition, wxDefaultSize, choices, style );
    droplist->SetSize(wSize);
    droplist->SetMinSize(wSize);
    theWxWidget = droplist;
    combosz->Add(droplist,DONOTALLOWSTRETCH,wxALIGN_CENTER|wxALL,gdlSPACE);
    combosz->Fit(droplist);
    comboPanel->Fit();
    } else {
    droplist = new wxChoice( theWindow, widgetID, wxDefaultPosition, wxDefaultSize, choices, style );
    droplist->SetSize(wSize);
    droplist->SetMinSize(wSize);
    theWxWidget = droplist;
    if (!theWxContainer) theWxContainer = droplist;
  }
  if (framePanel) framePanel->Fit();
  if (widgetSizer) {
    widgetSizer->Add(dynamic_cast<wxWindow*>(theWxContainer), DONOTALLOWSTRETCH, widgetStyle|wxALL, gdlTEXT_SPACE); 
    widgetSizer->Fit(dynamic_cast<wxWindow*>(theWxContainer));
  } else {
    widgetPanel->Fit();
    dynamic_cast<wxWindow*>(theWxContainer)->SetPosition(wOffset);
  }
  

  droplist->SetSelection(0);
  this->AddToDesiredEvents(  wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(gdlwxFrame::OnDropList),droplist);

  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}
  
void GDLWidgetDropList::SetValue(BaseGDL *value){
   GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxChoice* droplist=dynamic_cast<wxChoice*>(theWxWidget);
  assert( droplist != NULL);
  droplist->Clear();
  droplist->Append(newchoices);
  droplist->SetSelection(0);
  //IDL behaviour  with DynamicResize is identical
  if (this->IsDynamicResize()) {
    wSize=droplist->GetSizeFromTextSize(droplist->GetTextExtent(newchoices[0]).GetWidth());
    droplist->SetMinSize(wSize);
    droplist->SetSize(wSize);
   }
  RefreshDynamicWidget();
}

void GDLWidgetDropList::SelectEntry(DLong entry_number){
 wxChoice * droplist=dynamic_cast<wxChoice*>(theWxWidget);
 assert( droplist != NULL);
 droplist->Select(entry_number); 
}

BaseGDL* GDLWidgetDropList::GetSelectedEntry(){
  wxChoice * droplist=dynamic_cast<wxChoice*>(theWxWidget);
  assert( droplist != NULL);
  return new DIntGDL(droplist->GetSelection());
}

GDLWidgetDropList::~GDLWidgetDropList(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetDropList(" << widgetID << ")" << std::endl;
#endif
    if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}
  
GDLWidgetComboBox::GDLWidgetComboBox( WidgetIDT p, EnvT* e, BaseGDL *value, DULong eventflags, DLong style_ )
: GDLWidget( p, e, static_cast<DStringGDL*> (value->Convert2( GDL_STRING, BaseGDL::CONVERT )) , eventflags)
, style( style_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
 START_ADD_EVENTUAL_FRAME
    
  widgetStyle=widgetAlignment();
//  wxSize size = computeWidgetSize();

  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  DLong n = val->N_Elements( );
  wxArrayString choices; // = new wxString[n];
  for ( SizeT i = 0; i < n; ++i )
  choices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxString val0WxString = wxString( (*val)[0].c_str( ), wxConvUTF8 );

  wxComboBox * combobox = new wxComboBox();
  combobox->Create(widgetPanel, widgetID, val0WxString, wxDefaultPosition, wxDefaultSize, choices, style );  

  
  theWxContainer = theWxWidget = combobox;
  this->AddToDesiredEvents( wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEventHandler(gdlwxFrame::OnComboBox),combobox);
  if (style & wxTE_PROCESS_ENTER) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_ENTER,wxCommandEventHandler(gdlwxFrame::OnComboBoxTextEnter),combobox);
  wSize=computeWidgetSize();
  if (widgetSizer==NULL) {
    if (wSize.x < 1) wSize.x=gdlDEFAULT_XSIZE+gdlCOMBOBOX_ARROW_WIDTH;
  }
  combobox->SetSize(wSize);
  combobox->SetMinSize(wSize);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}

void GDLWidgetComboBox::SetValue(BaseGDL *value){
  GDLDelete(vValue);
  vValue=value;
 if( vValue->Type() != GDL_STRING)
  {
    vValue = static_cast<DStringGDL*> (vValue->Convert2( GDL_STRING, BaseGDL::CONVERT ));
  }
  DStringGDL* val = static_cast<DStringGDL*> (vValue);
  wxArrayString newchoices; // = new wxString[n];
  for ( SizeT i = 0; i < val->N_Elements( ); ++i )  newchoices.Add( wxString( (*val)[i].c_str( ), wxConvUTF8 ) );

  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  combo->Clear();
  combo->Append(newchoices);
  combo->SetSelection(0,0);
  combo->SetValue(newchoices[0]);
  //IDL behaviour  with DynamicResize is different as Combobox's size never decreases with this method, it cannot be smaller than the initial size.
  //this would plead to have a new wInitialSize for which wSize cannot be smaller her, but is reset to new values with SetSize. Oh well.
  if (this->IsDynamicResize()) {
    wSize=combo->GetSizeFromTextSize(combo->GetTextExtent(newchoices[0]).GetWidth());
    combo->SetMinSize(wSize);
    combo->SetSize(wSize);
  }
  RefreshDynamicWidget();
  }

void GDLWidgetComboBox::AddItem(DString value, DLong pos) {
  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  if (pos == -1) combo->Append( wxString( value.c_str( ), wxConvUTF8 ) );
  else if (pos > -1  && pos < nvalues) combo->Insert( wxString( value.c_str( ), wxConvUTF8 ), pos);
}

void GDLWidgetComboBox::DeleteItem(DLong pos) {
  wxComboBox * combo = dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  int nvalues=combo->GetCount();
  int selected=combo->GetSelection();
  if (pos > -1  && pos < nvalues) combo->Delete( pos );
  if (pos==selected && (nvalues-1) > 0 ) combo->Select((pos+1)%(nvalues-1)); else if (selected==-1 && pos==0) combo->Select(0);
}

void GDLWidgetComboBox::SelectEntry(DLong entry_number){
 wxComboBox * combo=dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
 combo->Select(entry_number); 
}

BaseGDL* GDLWidgetComboBox::GetSelectedEntry(){
  wxComboBox * combo=dynamic_cast<wxComboBox*>(theWxWidget);
  assert( combo != NULL);
  DStringGDL* stringres=new DStringGDL(dimension(1));
  (*stringres)[0]=combo->GetStringSelection().mb_str(wxConvUTF8); //return null string if no selection
  return stringres;
}

GDLWidgetComboBox::~GDLWidgetComboBox(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetComboBox(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

GDLWidgetText::GDLWidgetText( WidgetIDT p, EnvT* e, DStringGDL* valueStr, DULong eventflags, bool noNewLine_,
bool editable_ )
: GDLWidget( p, e, valueStr, eventflags )
, noNewLine( noNewLine_ )
, editable(editable_)
{
  static int wrapIx=e->KeywordIx("WRAP");
  wrapped=(e->KeywordSet(wrapIx));
  DString value = "";
  maxlinelength = 0;
  nlines=1;
  
  //reform entries into one string, with eventual \n . If noNewLines, do not insert a \n . If ysize=1 and no scroll, idem.
  bool doNotAddNl=(noNewLine || (!scrolled && wSize.y==-1) );
  
  if( vValue != NULL)
  {
      for( int i=0; i<vValue->N_Elements(); ++i)
      {
        int length=((*static_cast<DStringGDL*> (vValue))[i]).length();
        value += (*static_cast<DStringGDL*> (vValue))[i]; 
        if ( !doNotAddNl) maxlinelength=(length>maxlinelength)?length:maxlinelength; else maxlinelength+=length;
        if ( !doNotAddNl && (i + 1) != vValue->N_Elements( ) )
#ifdef _WIN32
        {value += "\r\n"; nlines++;}
#else
        {value += '\n'; nlines++;}
#endif
      }
  }
  //now the string is formatted as the widget will see it. If the string contains \n  the widget will be multiline anyway.
  //recompute number of \n as some could have been embedded in each of the strings
  nlines=1;
  char* s=&value[0];
  for (int i=0; i<value.length(); ++i)
  {
    if (s[i]==10) nlines++;
  }
  //however:
  if (noNewLine) nlines=1;
  
  lastValue = value;

  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  START_ADD_EVENTUAL_FRAME
  
  widgetStyle = widgetAlignment();
  bool report=((eventFlags & GDLWidget::EV_ALL)==1);
  
 //for text, apparently, if   wxTE_MULTILINE is in effect, the font handler is probably RichText 
 //and the text SIZE is OK only if imposed by a wxTextAttr. (go figure).
 //note that computeWidgetSize() above returns sizes "compatible" with the font size, without any trouble.
  wxTextAttr attr= wxTextAttr ();
  attr.SetFont(font, 	wxTEXT_ATTR_FONT);
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  long textStyle = wxTE_RICH2|wxTE_NOHIDESEL;

  if (nlines>1) textStyle |= wxTE_MULTILINE;
  if (wSize.y >1) textStyle |= wxTE_MULTILINE;
  if (!scrolled) textStyle |= wxTE_NO_VSCROLL;
  if (wrapped) textStyle |= wxTE_WORDWRAP; else textStyle |= wxTE_DONTWRAP;
  if (!editable && !report ) textStyle |= wxTE_READONLY;

  wxTextCtrlGDL* text = new wxTextCtrlGDL(widgetPanel, widgetID, wxEmptyString/*valueWxString*/, wxDefaultPosition, wxDefaultSize, textStyle);
  // above, permits to a procedure (cw_field.pro for example) to filter the editing of text fields.
  // so wxTextCtrl::OnChar is overwritten in gdlwidgeteventhandler.cpp just for this reason.
  theWxContainer = theWxWidget = text;
  this->setFont();
  if ((textStyle & wxTE_MULTILINE)==0) text->SetMargins(wxPoint(gdlTEXT_XMARGIN,gdlTEXT_YMARGIN)); //without the if clause, provokes: GLib-GObject-WARNING **: 23:04:21.080: invalid cast from 'GtkTextView' to 'GtkEntry' (and does not work)
  text->SetDefaultStyle(attr);
  text->SetValue(valueWxString);

  wSize = computeWidgetSize(); 
  text->SetClientSize(wSize);
  text->SetMinClientSize(wSize);
  
  text->SetSelection(0,0);
  text->SetInsertionPoint(0);
//  text->ShowPosition(0);
  
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)

//  this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(GDLFrame::OnTextEnter),text); //NOT USED
  if( editable || report) this->AddToDesiredEvents(wxEVT_CHAR, wxKeyEventHandler(wxTextCtrlGDL::OnChar),text);
  //add scrolling even if text is not scrollable since scroll is not permitted (IDL widgets are not at all the same as GTK...)
  if (!scrolled) this->AddToDesiredEvents(wxEVT_MOUSEWHEEL, wxMouseEventHandler(wxTextCtrlGDL::OnMouseEvents),text);
  this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(gdlwxFrame::OnText),text);
  if (report) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_PASTE, wxClipboardTextEventHandler(gdlwxFrame::OnTextPaste),text);
  if (report) this->AddToDesiredEvents( wxEVT_COMMAND_TEXT_CUT, wxClipboardTextEventHandler(gdlwxFrame::OnTextCut),text);
  if (report) this->AddToDesiredEvents( wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxFrame::OnTextMouseEvents),text); 
  if (report) this->AddToDesiredEvents( wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxFrame::OnTextMouseEvents),text); 

  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}
void GDLWidgetText::SetWidgetSize(DLong sizex, DLong sizey)
{ 

  START_CHANGESIZE_NOEVENT
  
  wxWindow* me = dynamic_cast<wxWindow*> (this->GetWxWidget());
  if (me == NULL) {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetText::SetSize of non wxWindow %s\n"), this->GetWidgetName().c_str());
#endif
    return;
  }
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetClientSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetText::SetSize currentSize=%d,%d\n"),currentSize.x,currentSize.y);
#endif

  //widget text size is in LINES in Y and CHARACTERS in X.
  wxRealPoint widgetSize = wxRealPoint(-1,-1);
  wxSize fontSize = getFontSize();
  //based on experience, actual line height is 1.2 times font y size for fonts > 20 but 1.5 for smaller fonts
  int lineHeight=(fontSize.y<20)?fontSize.y*1.5:fontSize.y*1.2;
  if (sizex > 0) {
    widgetSize.x = (sizex+0.5) * fontSize.x;
  } else {
    widgetSize.x = currentSize.x;
  } 

  if (sizey > 0) {
    widgetSize.y =sizey * lineHeight;
  } else {
    widgetSize.y = currentSize.y;
  }

  if (sizex > 0 && maxlinelength > sizex) widgetSize.y += gdlSCROLL_HEIGHT_X;
  if (nlines > sizey) widgetSize.x += gdlSCROLL_WIDTH_Y;
  widgetSize.y += 10;

  sizex=ceil(widgetSize.x);
  sizey=ceil(widgetSize.y);

  wSize.x = sizex;
  wSize.y = sizey;
  me->SetClientSize(wSize);
  me->SetMinClientSize(wSize);
  if (!widgetSizer) if (framePanel) framePanel->Fit();
  
  UPDATE_WINDOW

  END_CHANGESIZE_NOEVENT
  
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetList::SetSize %d,%d (%s)\n"), sizex, sizey, this->GetWidgetName().c_str());
#endif
}

GDLWidgetText::~GDLWidgetText(){
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetText(" << widgetID <<")"<< std::endl;
#endif
  if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetText::ChangeText( DStringGDL* valueStr, bool noNewLine_)
{
  delete vValue;
  vValue = valueStr;
  DString value = "";

  bool doNotAddNl=(noNewLine_ || (!scrolled && nlines<2) );

  nlines=0; 
    for( int i=0; i<valueStr->N_Elements(); ++i)
    {
    value += (*valueStr)[ i];
    if ( !doNotAddNl )
#ifdef _WIN32
    {value += "\r\n"; nlines++;}
#else
    {value += '\n'; nlines++;}
#endif
    }
  lastValue = value;
  //recompute nlines, maxlinelength from start to be sure
  nlines=1;
  maxlinelength=0;
  char* s=&lastValue[0];
  int length=0;
  for (int i=0; i<lastValue.length(); ++i, ++length)
  {
    if (s[i]==10) {
      maxlinelength=(length>maxlinelength)?length:maxlinelength;
      nlines++;
      length=0;
    }
  }
  if (maxlinelength==0) maxlinelength=lastValue.length();
  
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( theWxWidget != NULL ) {
    wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
    assert( txt != NULL);
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT -- IDL does not either.    
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::InsertText( DStringGDL* valueStr, bool noNewLine_, bool append)
{
  long from,to;
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert( txt != NULL);
  txt->GetSelection(&from,&to);
  if (append) {from=txt->GetLastPosition(); to=from;}

  bool doNotAddNl=(noNewLine_ || (!scrolled && nlines<2) );

  DString value = (doNotAddNl)?"":(!append)?"":"\n";
  for ( int i = 0; i < valueStr->N_Elements( ); ++i ) {
    int length=(*valueStr)[ i].length();
    value += (*valueStr)[ i];
    if ( !doNotAddNl ) {
#ifdef _WIN32
      value += "\r\n"; nlines++;
#else
      value += '\n'; nlines++;
#endif
    }
  }
  lastValue.replace(from,to-from,value);
  //recompute nlines, maxlinelength from start to be sure
  nlines=1;
  maxlinelength=0;
  char* s=&lastValue[0];
  int length=0;
  for (int i=0; i<lastValue.length(); ++i, ++length)
  {
    if (s[i]==10) {
      maxlinelength=(length>maxlinelength)?length:maxlinelength;
      nlines++;
      length=0;
    }
  }
  if (maxlinelength==0) maxlinelength=lastValue.length();
  delete vValue;
  vValue = new DStringGDL(lastValue);
//  cout<<from<<"->"<<to<<":"<<lastValue.c_str( )<<endl;
  wxString valueWxString = wxString( lastValue.c_str( ), wxConvUTF8 );
  if ( theWxWidget != NULL ) {
    txt->ChangeValue( valueWxString ); //by contrast with SetValue, does not generate an EVENT (neither does *DL).    
//    txt->Refresh();
//    txt->Update();
    txt->SetSelection(from,from);
    if (append) txt->ShowPosition(lastValue.length());
  }  else std::cerr << "Null widget in GDLWidgetText::SetTextValue(), please report!" << std::endl;
}

void GDLWidgetText::SetTextSelection(DLongGDL* pos)
{ //*DL selection are [start,length] [3,20] -> pos (3,23)
  //numerous cases to handle with invalid values (negative, too large).
  //Hope it works for all cases:
  long from,to,len;
  long oldSelStart, oldSelEnd;
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&oldSelStart,&oldSelEnd);
  len = txt->GetLastPosition()+1;

  if ((*pos)[0] < 0 ) from=0;
  else if ((*pos)[0] > len ) from=len;
  else from=(*pos)[0];

  
  if (pos->N_Elements()<2) {
    //selection = insertion
    txt->SetSelection(from,from);
    return;
  }
  //2 elements, define selection
  to=(*pos)[1];

  if ( from+to >= len ) {
    //set insertion position at end of last selection (?)
    txt->SetSelection(oldSelEnd,oldSelEnd);
    return;
  }
  if ( to < 0 ) {
    //set insertion position at from
    txt->SetSelection(from,from);
    return;
  }
  txt->SetSelection(from,from+to);
}

DLongGDL* GDLWidgetText::GetTextSelection()
{
  DLongGDL* pos=new DLongGDL(dimension(2),BaseGDL::ZERO);
  long from,to;
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*> (theWxWidget);
  assert( txt != NULL);
  
  txt->GetSelection(&from,&to);
  (*pos)[0]=from; (*pos)[1]=to-from;
  return pos;
}

DStringGDL* GDLWidgetText::GetSelectedText()
{
  wxTextCtrl* txt=dynamic_cast<wxTextCtrl*>(theWxWidget);
  assert( txt != NULL);
  
return new DStringGDL(txt->GetStringSelection().mb_str(wxConvUTF8).data());
}

GDLWidgetLabel::GDLWidgetLabel( WidgetIDT p, EnvT* e, const DString& value_ , DULong eventflags, bool sunken_)
: GDLWidget( p, e , NULL, eventflags )
, value(value_)
, sunken(sunken_)
{
  GDLWidget* gdlParent = GetWidget( parentID );
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );

  if (sunken) frameWidth=0;

  widgetStyle=widgetAlignment();
  const wxString valueWxString = wxString( value.c_str( ), wxConvUTF8 );

  bool simplelabel=(wSize.x < 0 && frameWidth <1 && !sunken); //before recomputing wSize!
  wSize=computeWidgetSize();
  //to comply with the positioning observed with IDL, a label with a defined size is in fact a label inside a (possibly invisible) frame of such size.
  //the centering of the label is 'center' by default and does not inherit from the parent base default centering (it is the frame that inherits), but if
  //a centering is required, it is done.
  //labels with no particular size and frames are made idem.
  //label with no particular size and no frame are aligned normally, i.e., they inherit if the alignment is not precised.
  // the simple case is
  if (simplelabel) {
    wxStaticText* label = new wxStaticText( widgetPanel, widgetID, wxT(""), wOffset, wxDefaultSize, wxST_NO_AUTORESIZE|widgetStyle);
    label->SetLabelText(valueWxString);
    label->SetSize(wSize);
    label->SetMinSize(wSize);
    label->Wrap(-1);
    theWxContainer = theWxWidget = label;
    if (widgetSizer) widgetSizer->Add(label, DONOTALLOWSTRETCH, widgetStyle|wxALL, gdlSPACE);
    if (widgetSizer) widgetSizer->Fit(label); else widgetPanel->Fit();
    UPDATE_WINDOW
    REALIZE_IF_NEEDED
    return;
  } 
  //Now more complicated. Framed is not Sunken. Framed implies adding 2 panels, one 'extruded'.
  //the inside panel is of (eventually fixed) size wSize. It contains the text, aligned.
  //sizers should link all those sizes
  framePanel = new wxPanel(widgetPanel, wxID_ANY, wOffset, wxDefaultSize, sunken?wxBORDER_SUNKEN:(frameWidth >0 )?gdlBORDER_EXT:wxBORDER_NONE);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
  framePanel->SetBackgroundColour(wxColour(0x3a, 0x82, 0x56)); //vert fonce
#endif
  wxSizer* frameSizer = new wxBoxSizer(wxVERTICAL);
  framePanel->SetSizer(frameSizer);
  wxStaticText* label;
  if (frameWidth >0) {
    //the inside panel, framed
    wxPanel* hiddenPanel = new wxPanel(framePanel);
    frameSizer->Add(hiddenPanel, DONOTALLOWSTRETCH, wxALL | wxEXPAND, frameWidth);
    hiddenPanel->SetSize(wSize);
    hiddenPanel->SetMinSize(wSize);
    wxSizer* sz = new wxBoxSizer(wxVERTICAL);
    hiddenPanel->SetSizer(sz);
    //the label, with special style inside
    int style = labelTextAlignment();
    //create and position label in panel
    label = new wxStaticText(hiddenPanel, widgetID, valueWxString);
    label->Wrap(-1);
    sz->Add(label, DONOTALLOWSTRETCH, style | wxALL, 0);
    sz->Fit(label); 
    frameSizer->Fit(hiddenPanel);
    framePanel->Fit();
  } else {
    framePanel->SetSize(wSize);
    framePanel->SetMinSize(wSize);
    //the label, with special style inside
    int style= labelTextAlignment();
  //create and position label in panel
    label = new wxStaticText( framePanel, widgetID, valueWxString);
    label->Wrap(-1);
    frameSizer->Add(label,DONOTALLOWSTRETCH,style|wxALL,0);
    frameSizer->Fit(label); //sz exist always
  }
  theWxContainer=framePanel;
  theWxWidget=label;
  if (widgetSizer) widgetSizer->Add(framePanel, DONOTALLOWSTRETCH, widgetStyle|wxALL, 0);
  if (widgetSizer) widgetSizer->Fit(framePanel); else widgetPanel->Fit();
  UPDATE_WINDOW
  REALIZE_IF_NEEDED
}

GDLWidgetLabel::~GDLWidgetLabel(){
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLWidgetLabel(" << widgetID << ")" << std::endl;
#endif 
    if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}
void GDLWidgetLabel::SetWidgetSize(DLong sizex, DLong sizey) //in pixels. Always.
{
  START_CHANGESIZE_NOEVENT
  //Sizes are in pixels. Units must be converted before calling this function.
  wxWindow* me=static_cast<wxWindow*>(theWxContainer);
  //a graphic window cannot be larger than the base widget it is in, if the base widget size has been fixed
  wxSize currentSize=me->GetSize();
  wxSize currentBestSize=me->GetBestSize();
#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidgetLabel::SetSize currentSize=%d,%d\n"),currentSize.x,currentSize.y);
#endif
  //unprecised sizes do not change
  if (sizex == -1) sizex = currentSize.x;
  if (sizey == -1) sizey = currentSize.y;
  
  if (widgetSizer) {
    //passed zeroes = mr sizer, do your best..
    if (sizex == 0) sizex = -1;
    if (sizey == 0) sizey = -1;
  } else {
    //passed zeroes = your best is to keep best size (in cache, value defined at creation)
    if (sizex == 0) sizex = currentBestSize.x;
    if (sizey == 0) sizey = currentBestSize.y;
  }
  wSize.x = sizex;
  wSize.y = sizey;
  me->SetSize(wSize);
  //the sizer takes care of the eventual frame
  me->SetMinSize(wSize);

  UPDATE_WINDOW

  END_CHANGESIZE_NOEVENT
  

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr( ).Printf( _T( "GDLWidget::SetSize %d,%d (%s)\n"),sizex,sizey,this->GetWidgetName().c_str());
#endif
}

void GDLWidgetLabel::SetLabelValue(const DString& value_) {
  value = value_;
  //update vValue
  delete(vValue);
  vValue = new DStringGDL(value);

  wxString valueWxString = wxString(value_.c_str(), wxConvUTF8);
  if (theWxWidget != NULL) {
    wxStaticText *label = dynamic_cast<wxStaticText*> (theWxWidget);
    if (label) {  
      label->SetLabelText(valueWxString);
      if (this->IsDynamicResize()) {
        wSize = wxSize(-1, -1);
        wSize = computeWidgetSize();
        label->SetMinSize(wSize);
        label->SetSize(wSize);
        //special for labels:
        if (theWxContainer && theWxContainer != theWxWidget) {
          wxPanel* p = static_cast<wxPanel*> (theWxContainer);
          if (p) {
            // this looks complicated but how to do otherwise?
            p->SetMinSize(label->GetBestSize()+wxSize(2*frameWidth,2*frameWidth));
          }
        }
      }
    }
    else cerr << "Warning GDLWidgetLabel::SetLabelValue(): widget type confusion\n";
    //SetLabel, unless SetLabelText, interprets markup (<b></b> etc)
  } else std::cerr << "Null widget in GDLWidgetLabel::SetLabelValue(), please report!" << std::endl;
}

//propertysheet. Unsupported as need to be completely rewritten as the other widgets are (new code) and not able to test.
#ifdef HAVE_WXWIDGETS_PROPERTYGRID
//
// GDLWidgetPropertySheet::GDLWidgetPropertySheet( WidgetIDT parentID, EnvT* e)
// : GDLWidget( p, e , value, eventflag)
//{
//  GDLWidget* gdlParent = GetWidget( parentID );
//  widgetPanel = GetParentPanel( );
//  widgetSizer = GetParentSizer( );
//  topWidgetSizer = this->GetTopLevelBaseWidget(parentID)->GetSizer();
// // Construct wxPropertyGrid control
//  wxPropertyGrid* pg = new wxPropertyGrid(gdlParent,wxID_ANY,wxDefaultPosition,wxDefaultSize,
//  // Here are just some of the supported window styles
//  wxPG_AUTO_SORT | // Automatic sorting after items added
//  wxPG_SPLITTER_AUTO_CENTER | // Automatically center splitter until user manually adjusts it
//  // Default style
//  wxPG_DEFAULT_STYLE );
//// Window style flags are at premium, so some less often needed ones are
//// available as extra window styles (wxPG_EX_xxx) which must be set using
//// SetExtraStyle member function. wxPG_EX_HELP_AS_TOOLTIPS, for instance,
//// allows displaying help strings as tool tips.
//  pg->SetExtraStyle( wxPG_EX_HELP_AS_TOOLTIPS );
//  
//  theWxWidget=pg;
//  
//  // Add int property
//pg->Append( new wxIntProperty("IntProperty", wxPG_LABEL, 12345678) );
//// Add float property (value type is actually double)
//pg->Append( new wxFloatProperty("FloatProperty", wxPG_LABEL, 12345.678) );
//// Add a bool property
//pg->Append( new wxBoolProperty("BoolProperty", wxPG_LABEL, false) );
//// A string property that can be edited in a separate editor dialog.
//pg->Append( new wxLongStringProperty("LongStringProperty",
//wxPG_LABEL,
//"This is much longer string than the "
//"first one. Edit it by clicking the button."));
//// String editor with dir selector button.
//pg->Append( new wxDirProperty("DirProperty", wxPG_LABEL, ::wxGetUserHome()) );
//// wxArrayStringProperty embeds a wxArrayString.
//pg->Append( new wxArrayStringProperty("Label of ArrayStringProperty",
//"NameOfArrayStringProp"));
//// A file selector property.
//pg->Append( new wxFileProperty("FileProperty", wxPG_LABEL, wxEmptyString) );
//
//UPDATE_WINDOW; REALIZE_IF_NEEDED;
//}
#endif
// GDL widgets =====================================================
// GDLFrame ========================================================
gdlwxFrame::gdlwxFrame( wxWindow* parent, GDLWidgetTopBase* gdlOwner_, wxWindowID id, const wxString& title , const wxPoint& pos, const wxSize& size, long style)
: wxFrame()
, mapped( false )
, frameSize(size)
, gdlOwner( gdlOwner_)
{
  Create ( parent, id, title, pos, size, style );
  m_resizeTimer = new wxTimer(this,RESIZE_TIMER);
}

gdlwxFrame::~gdlwxFrame()
{ 
    if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop(); 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: " << this << std::endl;
#endif  
 //frame is part of a TLB. if frame is destroyed, destroy TLB if still existing. 
 if( gdlOwner != NULL)
  {
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~GDLFrame: Destroying Base Container" << gdlOwner->GetWidgetID() << std::endl;
#endif
//    gdlOwner->NullWxWidget( ); //remove one's reference from container
//    gdlOwner->SelfDestroy( ); // send delete request to GDL owner = container.
  }
 }
// Phantom frame 
gdlwxPhantomFrame::gdlwxPhantomFrame()
: wxFrame(NULL,wxID_ANY,wxString("phantom"),wxDefaultPosition,wxDefaultSize,wxFRAME_TOOL_WINDOW)
{
  wxScrolled<wxPanel>* test = new wxScrolled<wxPanel>(this); //to measure scrollbars
  test->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
}

void gdlwxPhantomFrame::Realize() {
  wxWindow* c=this->GetChildren()[0];
  //retrieve toolkit's width and height of scrollbars (useful to calculate sizes for widgets)
  wxSize s=c->GetSize();
  wxSize e=c->GetClientSize();
  wxSize w=s-e;
//  std::cerr<<w.x<<","<<w.y<<std::endl;
  if (w.x <= 0) w.x=gdlABSENT_SIZE_VALUE;
  if (w.y <= 0) w.y=gdlABSENT_SIZE_VALUE;
  sysScrollHeight=w.y;
  sysScrollWidth=w.x;
}

// Frame for Plots ========================================================
gdlwxPlotFrame::gdlwxPlotFrame( const wxString& title , const wxPoint& pos, const wxSize& size, long style, bool scrolled_)
: wxFrame( NULL, wxID_ANY, title, pos, size, style )
, frameSize(size)
, scrolled(scrolled_)
{
  m_resizeTimer = new wxTimer(this,RESIZE_PLOT_TIMER);
}

gdlwxPlotFrame::~gdlwxPlotFrame() {
  //We are here outside gdl widget hierarchy. If we Destroy the frame, everything goes.
  // However, we MUST insure this is equivalent to "WDELETE" because the complicated interplay of multidevices, plplot streams, and widgets.
  // So it is better to pass and wait for the deletion of the contained window id (by marking it unValid) and let TidyWindowsList clear everything.
  // this widget will be actually destroyed when the inside GraphicsPanel will be destroyed.
  if (m_resizeTimer->IsRunning()) m_resizeTimer->Stop();
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~gdlwxPlotFrame: " << this << std::endl;
#endif
}

void gdlwxPlotFrame::Realize() {
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("gdlwxPlotFrame:Realize\n"));
#endif
}

//version using wxBG_STYLE_PAINT and blit to an AutoBufferedPaintDC, will this improve speed?
gdlwxGraphicsPanel::gdlwxGraphicsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxScrolled<wxPanel>() // Use default ctor here!
, pstreamIx( -1 )
, pstreamP( NULL )
, m_dc( NULL)
, drawSize(size)
{ 
        // Do this first:
        SetBackgroundStyle(wxBG_STYLE_PAINT);
  Create( parent, id, pos, size, style, name );
}
//gdlwxGraphicsPanel::gdlwxGraphicsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
//: wxScrolled<wxPanel>( parent, id, pos, size, style, name )
//, pstreamIx( -1 )
//, pstreamP( NULL )
//, m_dc( NULL)
//, drawSize(size)
//{ }
GDLWXStream* gdlwxGraphicsPanel::GetStream(){return pstreamP;};
void gdlwxGraphicsPanel::DeleteUsingWindowNumber(){
  pstreamP->SetValid(false);
  GraphicsDevice::GetGUIDevice()->TidyWindowsList(); //tidy Window List will delete widget by itself
}
void gdlwxGraphicsPanel::SetStream(GDLWXStream* s) {
  pstreamP = s;
  m_dc = pstreamP->GetDC();
}
gdlwxPlotPanel::gdlwxPlotPanel( gdlwxPlotFrame* parent) //, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: gdlwxGraphicsPanel::gdlwxGraphicsPanel(parent)
, myFrame(parent)
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "gdlwxPlotPanel::gdlwxPlotPanel(" << this << ") called."<< std::endl;
#endif
    this->SetBackgroundColour(*wxBLACK);
}
gdlwxDrawPanel::gdlwxDrawPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: gdlwxGraphicsPanel::gdlwxGraphicsPanel(parent, id, pos, size, style, name )
, myWidgetDraw(static_cast<GDLWidgetDraw*>(GDLWidget::GetWidget(id)))
{
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "gdlwxDrawPanel::gdlwxDrawPanel(" << this << ") called."<< std::endl;
#endif
}

void gdlwxDrawPanel::InitStream(int wIx)
{
  if (wIx < 0)  pstreamIx = GraphicsDevice::GetGUIDevice( )->WAddFree( );
  else  pstreamIx = wIx;
  if ( pstreamIx == -1 ) throw GDLException( "Failed to allocate GUI stream." );
  pstreamP = static_cast<GDLWXStream*>(GraphicsDevice::GetGUIDevice( )->GUIOpen( pstreamIx, drawSize.x, drawSize.y, this ));
  if( pstreamP == NULL)
  {
    throw GDLException( "Failed to open GUI stream: " + i2s( pstreamIx ) );
  }
  m_dc = pstreamP->GetDC( );
}

void gdlwxGraphicsPanel::ResizeDrawArea(wxSize s)
{
  bool doClear=false; 
  if (drawSize.x > s.x || drawSize.y > s.y ) doClear=true; 
  drawSize=s;
  this->SetVirtualSize(drawSize);
  
  if (pstreamP != NULL)
  {
// comment out if it is better to recreate a wxstream!    
   pstreamP->SetSize(drawSize);
////The following should not be necessary . It is a bad idea to create a new stream, but font size handling
////with plplot's wx implementation that is too awkward to do anything else now.
//
//    //get a new stream with good dimensions. Only possibility (better than resize) to have correct size of fonts and symbols.
//    GDLWXStream * newStream =  new GDLWXStream(s.x,s.y);
//  // replace old by new, called function destroys old:
//    GraphicsDevice::GetGUIDevice( )->ChangeStreamAt( pstreamIx, newStream );   //deletes old stream!
//    pstreamP = static_cast<GDLWXStream*> (GraphicsDevice::GetGUIDevice( )->GetStreamAt( pstreamIx ));
//    pstreamP->SetGdlwxGraphicsPanel( this );
//    m_dc = pstreamP->GetDC( );
//    pstreamP->Clear();
  }
  RepaintGraphics(doClear);
} 

gdlwxPlotPanel::~gdlwxPlotPanel()
{ 
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~gdlwxPlotPanel() :" << this << "pstreamP="<<GetStream()<< " is now invalid."<<std::endl;
#endif
    this->GetMyFrame()->Show(false);
    this->GetMyFrame()->Destroy(); //will be really destroyed after next call to wxApp (remember, wxApp is not running, only GDL is running).
}

gdlwxDrawPanel::~gdlwxDrawPanel() 
{ 
 //widget_control,this,/dest actually calls wdelete. So the only case where we are not 'deleted' with a wdelete or widget_control command is during the detetion of a container.
 // in which case, we avoid reentrance because on the first pass the strem has been invalidated.
#ifdef GDL_DEBUG_WIDGETS
    std::cout << "~gdlwxDrawPanel() :" << this << "pstreamP="<<GetStream()<< " is now invalid."<<std::endl;
#endif
  // if associated stream is invalid, do nothing : 'this' has already been called.
  if (pstreamP->GetValid()) return;
  //invalidate stream in any case, but do not have TidyWindowsList() re-deleting it, as whatever call us, gdlwxDrawPanel is going to die.
  pstreamP->SetValid(false);
  GraphicsDevice::GetGUIDevice()->TidyWindowsList(false);
  // get WIDGET_DRAW
  GDLWidgetDraw* d = GetMyWidget();
  // at exit from here, widget_draw is either destroyed because I ask for it or because it was already being destroyed. To prevent reentrance, NULL the reference to it.
  myWidgetDraw = NULL;
  //either widget_draw is valid and we must destroy it properly. Else it is not anymore (see ~GDLWidgetDraw() ) so we have nothing to do.
  if (d && d->IsValid()) { //we come from wdelete
    //this=thewxWidget will be destroyed no matter what, since this is part of its destruction process. To destroy the gdlwidget, it would need to remove 'this' from the
    //gdlWidget structure and the underlying wxwidgets parent-child lists. Difficult. Simpler at the moment:
    // unref the container (so destruction call of the wxcontainer will not be effected at widget destruction)
    // delete the wigdet to remove it from widget hierarchy.
    wxWindow* delayed_destroy_frame = static_cast<wxWindow*>(d->GetWxContainer()); //container can be theWxWidget=this
    d->UnrefTheWxContainer(); //in any cases
    //remove the container from its parent (panel)
    delayed_destroy_frame->GetParent()->RemoveChild(delayed_destroy_frame); //should hide the wxContainer.
    delayed_destroy_frame->Hide();
    //no link anymore to 'this' in gdlwidget destruction
    delete d;
  } 
}
GDLWidgetDraw::GDLWidgetDraw( WidgetIDT p, EnvT* e, int windowIndex,
                  DLong x_scroll_size, DLong y_scroll_size, bool app_scroll, DULong eventFlags_, DStringGDL* drawToolTip)
  : GDLWidget( p, e, NULL, eventFlags_)
  , pstreamIx(windowIndex)
{
  // xsize,ysize (or, scr_xsize, scr_ysize) define the plot size.
  assert( parentID != GDLWidget::NullID);
  wSize = computeWidgetSize();
  if (widgetSizer==NULL) {
    if (wSize.x < 1) wSize.x=gdlDEFAULT_XSIZE;
    if (wSize.y < 1) wSize.y=gdlDEFAULT_YSIZE;
  }
  //get immediately rid of scroll sizes in case of scroll or not... Here is the logic:
  // Note that IDL reports wrong dimensions when size and scr_xsize are used simultaneously for at least widget_draw. 

  if (x_scroll_size > 0) {scrolled=true;x_scroll_size*=unitConversionFactor.x;x_scroll_size+=gdlSCROLL_WIDTH_Y;} 
  if (y_scroll_size > 0) {scrolled=true;y_scroll_size*=unitConversionFactor.y;y_scroll_size+=gdlSCROLL_HEIGHT_X;}
  if (scrolled) {
    if (x_scroll_size < 1) x_scroll_size = gdlDEFAULT_XSIZE+gdlSCROLL_WIDTH_Y;
    if (y_scroll_size < 1) y_scroll_size = gdlDEFAULT_YSIZE+gdlSCROLL_HEIGHT_X;
  }  
  wScrollSize = scrolled ? wxSize(x_scroll_size , y_scroll_size ) : wSize; 
 
  GDLWidget* gdlParent = GetWidget( parentID);
  widgetPanel = GetParentPanel( );
  widgetSizer = GetParentSizer( );
  
  START_ADD_EVENTUAL_FRAME
    
  widgetStyle = widgetAlignment();
  long style = 0;

  gdlwxDrawPanel* draw = new gdlwxDrawPanel( widgetPanel, widgetID, wxDefaultPosition, wxDefaultSize, style);
#ifdef GDL_DEBUG_WIDGETS_COLORIZE
  draw->SetBackgroundColour(wxColour(0,255,255));
#endif
  draw->SetMinClientSize(wScrollSize);
  draw->SetClientSize(wScrollSize);
  draw->SetVirtualSize(wSize);
  draw->InitDrawSize(wSize);
  
  if (scrolled) {
    draw->SetScrollbars(gdlSCROLL_RATE, gdlSCROLL_RATE, wSize.x / gdlSCROLL_RATE, wSize.y / gdlSCROLL_RATE);
    draw->ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
  }

 
  draw->SetCursor(wxCURSOR_CROSS);
  if (drawToolTip) dynamic_cast<wxWindow*>(draw)->SetToolTip( wxString((*drawToolTip)[0].c_str(),wxConvUTF8));
  theWxContainer = theWxWidget = draw;
 
  END_ADD_EVENTUAL_FRAME
  TIDY_WIDGET(gdlBORDER_SPACE)
    
  draw->InitStream(windowIndex);
  
  pstreamIx = draw->PStreamIx();
  GDLDelete( vValue);
  vValue = new DLongGDL(pstreamIx);  

  //these widget specific events are always set:
   this->AddToDesiredEvents( wxEVT_PAINT, wxPaintEventHandler(gdlwxDrawPanel::OnPaint),draw);
//   this->AddToDesiredEvents( wxEVT_SIZE,  wxSizeEventHandler(gdlwxDrawPanel::OnSize),draw);
//   this->AddToDesiredEvents( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gdlwxDrawPanel::OnErase),draw);

  //other set event handling according to flags
  if (eventFlags & GDLWidget::EV_MOTION) this->AddToDesiredEvents( wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove),draw);
//  if ( eventFlags & GDLWidget::EV_DROP) nothing to do yet, fixme!;
  if ( eventFlags & GDLWidget::EV_EXPOSE) this->AddToDesiredEvents(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler(gdlwxDrawPanel::OnErase),draw);
//  if ( eventFlags &  GDLWidget::EV_VIEWPORT) idem;
  if ( eventFlags &  GDLWidget::EV_WHEEL) this->AddToDesiredEvents( wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel),draw); 
  if ( eventFlags &  GDLWidget::EV_BUTTON) {
      this->AddToDesiredEvents(wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
      this->AddToDesiredEvents(wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown),draw); 
      this->AddToDesiredEvents(wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp),draw); 
  }
  if (eventFlags &  GDLWidget::EV_KEYBOARD2 || eventFlags & GDLWidget::EV_KEYBOARD){
       this->AddToDesiredEvents(wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey),draw); 
       this->AddToDesiredEvents(wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey),draw); 
  }

   UPDATE_WINDOW; REALIZE_IF_NEEDED; 
}

GDLWidgetDraw::~GDLWidgetDraw() {
#ifdef GDL_DEBUG_WIDGETS
  std::cout << "~GDLWidgetDraw(" << widgetID << ")" << std::endl;
#endif
 if (theWxContainer) static_cast<wxWindow*>(theWxContainer)->Destroy();
}

void GDLWidgetDraw::AddEventType( DULong evType){
  eventFlags |= evType;
  gdlwxDrawPanel* draw=(gdlwxDrawPanel*)theWxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Connect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Connect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Connect(widgetID, wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Connect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Connect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Connect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Connect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
       draw->Connect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
  } 
}

void GDLWidgetDraw::RemoveEventType( DULong evType){
  gdlwxDrawPanel* draw=(gdlwxDrawPanel*)theWxWidget;
//this one is for the moment defined globally:
//   if ( evType == GDLWidget::EV_TRACKING) { 
//    draw->Disconnect(widgetID, wxEVT_ENTER_WINDOW, wxMouseEventHandler(GDLFrame::OnEnterWindow));
//    draw->Disconnect(widgetID, wxEVT_LEAVE_WINDOW, wxMouseEventHandler(GDLFrame::OnLeaveWindow));
//  } else
  if ( evType == GDLWidget::EV_MOTION) draw->Disconnect(widgetID, wxEVT_MOTION, wxMouseEventHandler(gdlwxDrawPanel::OnMouseMove));
//else  if ( evType == GDLWidget::EV_DROP) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_EXPOSE) nothing to do yet, fixme!;
//else  if ( evType == GDLWidget::EV_VIEWPORT) idem;
  else if ( evType == GDLWidget::EV_WHEEL) draw->Disconnect(widgetID, wxEVT_MOUSEWHEEL, wxMouseEventHandler(gdlwxDrawPanel::OnMouseWheel)); 
  else if ( evType == GDLWidget::EV_BUTTON) {
      draw->Disconnect(widgetID, wxEVT_LEFT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_LEFT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_MIDDLE_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DOWN, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_DCLICK, wxMouseEventHandler(gdlwxDrawPanel::OnMouseDown)); 
      draw->Disconnect(widgetID, wxEVT_RIGHT_UP, wxMouseEventHandler(gdlwxDrawPanel::OnMouseUp)); 
  }
  else if (evType == GDLWidget::EV_KEYBOARD2 || evType == GDLWidget::EV_KEYBOARD){
       draw->Disconnect(widgetID, wxEVT_KEY_DOWN, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
       draw->Disconnect(widgetID, wxEVT_KEY_UP, wxKeyEventHandler(gdlwxDrawPanel::OnKey)); 
  }
  eventFlags &= ~evType;
}

 void GDLWidgetDraw::SetWidgetVirtualSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
    gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
    wxSize drawSize=me->GetDrawSize();
    if (sizex < 1) sizex=drawSize.x;
    if (sizey < 1) sizey=drawSize.y;
#ifdef GDL_DEBUG_WIDGETS
    wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetVirtualSize %d,%d\n"), sizex, sizey);
#endif
     me->ResizeDrawArea(wxSize(sizex, sizey)); //changes draw area
    if (!scrolled) {
      wSize=wxSize(sizex, sizey);
      me->SetMinClientSize(wSize);
      me->SetClientSize(wSize);
    }
     UpdateGui();
  END_CHANGESIZE_NOEVENT
}

 //Draw widgets as Base Widgets have a Size that can be SIZE(without scrollbars) or SCR_SIZE(with scrollbars)
void GDLWidgetDraw::SetWidgetSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
  gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
  if (!scrolled) { //size is same as clientSize
    if (sizex < 1) sizex = wSize.x;
    if (sizey < 1) sizey = wSize.y;
    wSize=wxSize(sizex,sizey);
    me->SetSize(wSize);
    me->SetMinSize(wSize);
    me->ResizeDrawArea(wSize); //changes draw area
  } else { //value asked is for CLIENT size
    if (sizex < 1) sizex = wScrollSize.x;
    if (sizey < 1) sizey = wScrollSize.y;
    wScrollSize=wxSize(sizex,sizey);
    me->SetClientSize(wScrollSize);
    me->SetMinClientSize(wScrollSize);
  }

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetSize %d,%d\n"), sizex, sizey);
#endif
  UpdateGui();
  END_CHANGESIZE_NOEVENT
}

//Draw widgets as Base Widgets have a Size that can be SIZE(without scrollbars) or SCR_SIZE(with scrollbars)
void GDLWidgetDraw::SetWidgetScreenSize(DLong sizex, DLong sizey) {
  START_CHANGESIZE_NOEVENT
  gdlwxDrawPanel* me = dynamic_cast<gdlwxDrawPanel*> (theWxWidget);
  if (!scrolled) { //size is same as clientSize
    if (sizex < 1) sizex = wSize.x;
    if (sizey < 1) sizey = wSize.y;
    wSize=wxSize(sizex,sizey);
    me->SetClientSize(wSize);
    me->SetMinClientSize(wSize);
    me->ResizeDrawArea(wSize); //changes draw area
  } else { //value asked is for SIZE
    if (sizex < 1) sizex = wScrollSize.x;
    if (sizey < 1) sizey = wScrollSize.y;
    wScrollSize=wxSize(sizex,sizey);
    me->SetSize(wScrollSize);
    me->SetMinSize(wScrollSize);
  }

#ifdef GDL_DEBUG_WIDGETS
  wxMessageOutputStderr().Printf(_T("GDLWidgetDraw::SetWidgetSize %d,%d\n"), sizex, sizey);
#endif
  UpdateGui();
  END_CHANGESIZE_NOEVENT
}

//for windows, it seems necessary to define our own wxApp and run it manually
// for linux, it is NOT necessary, but thos works OK
// for MacOS /COCOA port, the following code does not work and the widgets are not created.
// (tied_scoped_ptr problem?)
#if __WXMSW__ 

#include "wx/evtloop.h"
#include "wx/ptr_scpd.h"
wxDEFINE_TIED_SCOPED_PTR_TYPE(wxEventLoop);

bool wxAppGDL::OnInit()
{ 
    return true;
}
   
  int wxAppGDL::MainLoop() {
    wxEventLoopTiedPtr mainLoop((wxEventLoop **)&m_mainLoop, new wxEventLoop);
    m_mainLoop->SetActive(m_mainLoop);
    loop = this->GetMainLoop();
    if (loop) {
    if (loop->IsRunning()) {
      while (loop->Pending()) // Unprocessed events in queue
      {
        loop->Dispatch(); // Dispatch next event in queue
      }
    }
  }
  return 0;
}

int wxAppGDL::OnExit()
{
  std::cout << " In GDLApp::OnExit()" << std::endl;
  // Defined in guiThread::OnExit() in gdlwidget.cpp
  //  std::cout << "Exiting thread (GDLApp::OnExit): " << thread << std::endl;
  return 0;
}
#endif

#endif
