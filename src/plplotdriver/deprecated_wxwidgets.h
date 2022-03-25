// Copyright (C) 2008  Werner Smekal
//
// This file is part of PLplot.
//
// PLplot is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// PLplot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with PLplot; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
//

#ifndef __WXWIDGETS_H__
#define __WXWIDGETS_H__

// some special wxWidgets headers
#include <wx/spinctrl.h>

// freetype headers and macros
#ifdef PL_HAVE_FREETYPE
  #include "plfreetype.h"
#endif

#ifndef max_number_of_grey_levels_used_in_text_smoothing
  #define max_number_of_grey_levels_used_in_text_smoothing    64
#endif

#define NDEV                                                  100 // Max number of output device types in menu

//--------------------------------------------------------------------------
// Physical dimension constants used by the driver

// Virtual coordinate scaling parameter, used to do calculations at
// higher resolution.  Chosen so that maximum plP_setphy coordinate
// is 32000 which is close to but still less than the maximum possible
// which is 32767.
//
// The trick here is to do everything in device coordinates on the driver
// side, but report/receive everything in virtual coordinates to/from the
// PLplot core.
//
#define VSCALE                   ( 40. )

// pixels per inch
#define DEVICE_PIXELS_PER_IN     ( 80. )
#define VIRTUAL_PIXELS_PER_IN    ( DEVICE_PIXELS_PER_IN * VSCALE )


// mm per inch
#define MM_PER_IN                    ( 25.4 )

// pixels per mm
#define DEVICE_PIXELS_PER_MM         ( DEVICE_PIXELS_PER_IN / MM_PER_IN )
#define VIRTUAL_PIXELS_PER_MM        ( VIRTUAL_PIXELS_PER_IN / MM_PER_IN )

// Default dimensions of the canvas (in inches)
#define CANVAS_WIDTH                 ( 10.0 )
#define CANVAS_HEIGHT                ( 7.5 )

// size of plot in pixels on screen if not given
#define PLOT_WIDTH                   ( 800 )
#define PLOT_HEIGHT                  ( 600 )

// These need to be distinguished since the handling is slightly different.
#define LOCATE_INVOKED_VIA_API       1
#define LOCATE_INVOKED_VIA_DRIVER    2

// Available backends
#define wxBACKEND_DC                 0
#define wxBACKEND_AGG                1
#define wxBACKEND_GC                 2

class wxPLplotFrame;

// base device class
class wxPLDevBase
{
public: // methods
    wxPLDevBase( int bcknd );
    virtual ~wxPLDevBase( void );

    // virtual functions which need to implemented
    virtual void DrawLine( short x1a, short y1a, short x2a, short y2a ) = 0;
    virtual void DrawPolyline( short *xa, short *ya, PLINT npts )       = 0;
    virtual void ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1 = -1, PLINT y1 = -1, PLINT x2 = -1, PLINT y2 = -1 ) = 0;
    virtual void FillPolygon( PLStream *pls ) = 0;
    virtual void BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH ) = 0;
    void AddtoClipRegion( int x1, int y1, int x2, int y2 );
    virtual void CreateCanvas()                    = 0;
    virtual void SetWidth( PLStream *pls )         = 0;
    virtual void SetColor0( PLStream *pls )        = 0;
    virtual void SetColor1( PLStream *pls )        = 0;
    virtual void SetExternalBuffer( void* buffer ) = 0;
#ifdef PL_HAVE_FREETYPE
    virtual void PutPixel( short x, short y, PLINT color ) = 0;
    virtual void PutPixel( short x, short y )  = 0;
    virtual PLINT GetPixel( short x, short y ) = 0;
#endif // PL_HAVE_FREETYPE
    virtual void ProcessString( PLStream* pls, EscText* args ) = 0;
    virtual void PSDrawText( PLUNICODE* ucs4, int ucs4Len, bool drawText );
    virtual void PSDrawTextToDC( char* utf8_string, bool drawText ) = 0;
    virtual void PSSetFont( PLUNICODE fci ) = 0;

public: // variables
    const int    backend;
    bool         ready;
    bool         ownGUI;
    bool         showGUI;
    bool         waiting;
    bool         resizing;
    bool         exit;
    int          comcount;

    wxPLplotFrame* m_frame;
    PLINT        xpos;
    PLINT        ypos;
    PLINT        width;
    PLINT        height;
    PLINT        bm_width;
    PLINT        bm_height;

    PLINT        xmin;
    PLINT        xmax;
    PLINT        ymin;
    PLINT        ymax;

    PLFLT        scalex;
    PLFLT        scaley;

    bool         plstate_width;         // Flags indicating change of state before
    bool         plstate_color0;        // device is fully initialized
    bool         plstate_color1;        //

    PLGraphicsIn gin;                   // Graphics input structure
    int          locate_mode;           // Set while in locate mode
    bool         draw_xhair;            // Set during xhair draws

    // clipping region
    int  clipminx, clipmaxx;
    int  clipminy, clipmaxy;
    bool newclipregion;

    // variables for antializing
    int          freetype;
    int          smooth_text;

    const char   ** devDesc;    // Descriptive names for file-oriented devices.  Malloc'ed.
    const char   ** devName;    // Keyword names of file-oriented devices. Malloc'ed.
    int          ndev;
    wxBitmapType bitmapType;

    // font variables
    static const int max_string_length = 500;
    wxFont           * m_font;
    bool             underlined;
    double           fontSize;
    double           fontScale;
    wxCoord          textWidth, textHeight, textDescent, textLeading;
    PLUNICODE        fci;
    //the distance between the superscript top and subscript base from the baseline
    wxCoord          superscriptHeight, subscriptDepth;
    double           lineSpacing;
    double           yOffset;
    PLINT            posX, posY;
    PLFLT            rotation, cos_rot, sin_rot;
    PLFLT            shear, cos_shear, sin_shear;
    PLFLT            stride;
};


class wxPLDevDC : public wxPLDevBase
{
public: // methods
    wxPLDevDC( void );
    ~wxPLDevDC( void );

    void DrawLine( short x1a, short y1a, short x2a, short y2a );
    void DrawPolyline( short *xa, short *ya, PLINT npts );
    void ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1 = -1, PLINT y1 = -1, PLINT x2 = -1, PLINT y2 = -1 );
    void FillPolygon( PLStream *pls );
    void BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH );
    void CreateCanvas();
    void SetWidth( PLStream *pls );
    void SetColor0( PLStream *pls );
    void SetColor1( PLStream *pls );
    void SetExternalBuffer( void* buffer );
#ifdef PL_HAVE_FREETYPE
    void PutPixel( short x, short y, PLINT color );
    void PutPixel( short x, short y );
    PLINT GetPixel( short x, short y );
#endif // PL_HAVE_FREETYPE
    void ProcessString( PLStream* pls, EscText* args );
    void PSDrawTextToDC( char* utf8_string, bool drawText );
    void PSSetFont( PLUNICODE fci );

private: // variables
    wxBitmap* m_bitmap;
    wxDC    * m_dc;
};


#ifdef HAVE_AGG
// antigrain headers
#include "agg2/agg_basics.h"
#include "agg2/agg_rendering_buffer.h"
#include "agg2/agg_rasterizer_scanline_aa.h"
#include "agg2/agg_scanline_u.h"
#include "agg2/agg_scanline_bin.h"
#include "agg2/agg_conv_stroke.h"
#include "agg2/agg_pixfmt_rgb.h"
#include "agg2/agg_renderer_base.h"
#include "agg2/agg_renderer_scanline.h"
#include "agg2/agg_renderer_primitives.h"
#include "agg2/agg_path_storage.h"
#include "agg2/agg_conv_curve.h"
#include "agg2/agg_conv_contour.h"
#include "agg2/agg_font_freetype.h"

class wxPLDevAGG : public wxPLDevBase
{
    // type definitions and enums
    typedef agg::pixfmt_rgb24                                              pixFormat;
    typedef agg::renderer_base<pixFormat>                                  rendererBase;
    typedef agg::renderer_scanline_aa_solid<rendererBase>                  rendererSolid;
    typedef agg::font_engine_freetype_int32                                fontEngineType;
    typedef agg::font_cache_manager<fontEngineType>                        fontManagerType;
    typedef agg::font_cache_manager<fontEngineType>::gray8_adaptor_type    fontRasterizer;
    typedef agg::font_cache_manager<fontEngineType>::gray8_scanline_type   fontScanline;
    typedef agg::conv_curve<agg::path_storage>                             convCurve;
    typedef agg::conv_stroke<convCurve>                                    convStroke;
    typedef agg::conv_transform<convCurve>                                 pathTransform;
    typedef agg::conv_transform<convStroke>                                strokeTransform;

    enum drawPathFlag { Fill, Stroke, FillAndStroke };

public: // methods
    wxPLDevAGG( void );
    ~wxPLDevAGG( void );

    void DrawLine( short x1a, short y1a, short x2a, short y2a );
    void DrawPolyline( short *xa, short *ya, PLINT npts );
    void ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1 = -1, PLINT y1 = -1, PLINT x2 = -1, PLINT y2 = -1 );
    void FillPolygon( PLStream *pls );
    void BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH );
    void CreateCanvas();
    void SetWidth( PLStream *pls );
    void SetColor0( PLStream *pls );
    void SetColor1( PLStream *pls );
    void SetExternalBuffer( void* buffer );
#ifdef PL_HAVE_FREETYPE
    void PutPixel( short x, short y, PLINT color );
    void PutPixel( short x, short y );
    PLINT GetPixel( short x, short y );
#endif // PL_HAVE_FREETYPE
    void ProcessString( PLStream* pls, EscText* args );
    void PSDrawTextToDC( char* utf8_string, bool drawText );
    void PSSetFont( PLUNICODE fci );
    void drawPath( drawPathFlag flag );
    void AGGAddtoClipRegion( short x1, short y1, short x2, short y2 );

private: // variables
    wxDC * mDC;
    wxImage               * mBuffer;
    agg::rendering_buffer mRenderingBuffer;
    fontEngineType        mFontEngine;
    fontManagerType       mFontManager;
    agg::conv_curve<fontManagerType::path_adaptor_type> mCurves;
    agg::conv_contour<agg::conv_curve<fontManagerType::path_adaptor_type> > mContour;

    pixFormat         mPixFormat;
    rendererBase      mRendererBase;
    rendererSolid     mRendererSolid;

    agg::scanline_u8  mScanLine;
    agg::rasterizer_scanline_aa<>                                           mRasterizer;
    agg::path_storage mPath;
    agg::trans_affine mTransform;

    convCurve         mConvCurve;
    convStroke        mConvStroke;
    pathTransform     mPathTransform;
    strokeTransform   mStrokeTransform;

    double            mStrokeWidth;
    wxUint8           mStrokeOpacity;
    unsigned char     mColorRedStroke;
    unsigned char     mColorGreenStroke;
    unsigned char     mColorBlueStroke;
    unsigned char     mColorRedFill;
    unsigned char     mColorGreenFill;
    unsigned char     mColorBlueFill;

    double            textWidth, textHeight;
    wxString          fontdir;
};
#endif


#if wxUSE_GRAPHICS_CONTEXT
#include <wx/graphics.h>

class wxPLDevGC : public wxPLDevBase
{
public: // methods
    wxPLDevGC( void );
    ~wxPLDevGC( void );

    void DrawLine( short x1a, short y1a, short x2a, short y2a );
    void DrawPolyline( short *xa, short *ya, PLINT npts );
    void ClearBackground( PLINT bgr, PLINT bgg, PLINT bgb, PLINT x1 = -1, PLINT y1 = -1, PLINT x2 = -1, PLINT y2 = -1 );
    void FillPolygon( PLStream *pls );
    void BlitRectangle( wxDC* dc, int vX, int vY, int vW, int vH );
    void CreateCanvas();
    void SetWidth( PLStream *pls );
    void SetColor0( PLStream *pls );
    void SetColor1( PLStream *pls );
    void SetExternalBuffer( void* buffer );
#ifdef PL_HAVE_FREETYPE
    void PutPixel( short x, short y, PLINT color );
    void PutPixel( short x, short y );
    PLINT GetPixel( short x, short y );
#endif // PL_HAVE_FREETYPE
    void ProcessString( PLStream* pls, EscText* args );
    void PSDrawTextToDC( char* utf8_string, bool drawText );
    void PSSetFont( PLUNICODE fci );

private: // variables
    wxBitmap         * m_bitmap;
    wxDC             * m_dc;
    wxGraphicsContext* m_context;

    // text colors
    unsigned char textRed, textGreen, textBlue;

    unsigned char mStrokeOpacity;
    unsigned char mColorRedStroke;
    unsigned char mColorGreenStroke;
    unsigned char mColorBlueStroke;
    unsigned char mColorRedFill;
    unsigned char mColorGreenFill;
    unsigned char mColorBlueFill;
};
#endif


struct dev_entry
{
    wxString dev_name;
    wxString dev_menu_short;
    wxString dev_menu_long;
    wxString dev_file_app;
    bool     pixelDevice;
};



// after how many commands the window should be refreshed
#define MAX_COMCOUNT    10000

// wxwidgets application definition (if needed)
WX_DEFINE_ARRAY( wxPLplotFrame *, wxArrayOfwxPLplotFrame );
class wxPLplotApp : public wxApp
{
public:
    virtual bool OnInit();
    // virtual int OnExit();
    void SetExitFlag( bool flag = true ) { exit = flag; };
    bool GetExitFlag( void ) { return exit; };
    void SetAdvanceFlag( bool flag = true ) { advance = flag; };
    bool GetAdvanceFlag( void ) { return advance; };
    void SetRefreshFlag( bool flag = true );
    void AddFrame( wxPLplotFrame* frame ) { FrameArray.Add( frame ); };
    void RemoveFrame( wxPLplotFrame* frame ) { FrameArray.Remove( frame ); };
    size_t FrameCount( void ) { return FrameArray.GetCount(); }
    void OnIdle( wxIdleEvent& event );

private:
    bool exit;
    bool advance;
    wxArrayOfwxPLplotFrame FrameArray;

    DECLARE_EVENT_TABLE()
};

// definition of the actual window/frame shown
class wxPLplotWindow : public wxWindow
{
public:
    wxPLplotWindow( wxWindow* parent, PLStream *pls );

    void SetRefreshFlag( bool flag = true ) { refresh = flag; };
    bool GetRefreshFlag( void ) { return refresh; };
    void SetOrientation( int rot );

private:
    void OnPaint( wxPaintEvent& event );
    void OnChar( wxKeyEvent& event );
    void OnIdle( wxIdleEvent& event );
    void OnErase( wxEraseEvent & WXUNUSED( event ) );
    void OnSize( wxSizeEvent & WXUNUSED( event ) );
    void OnMaximize( wxMaximizeEvent & WXUNUSED( event ) );
    void OnMouse( wxMouseEvent& event );
    void Locate( void );
    void DrawCrosshair();

    PLStream   * m_pls;
    wxPLDevBase* m_dev; // windows needs to know this structure
    bool       refresh;
    bool       xhair_drawn;
    int        mouse_x, mouse_y, old_mouse_x, old_mouse_y;

    DECLARE_EVENT_TABLE()
};


// declaration of the actual window/frame shown
class wxPLplotFrame : public wxFrame
{
public:
    wxPLplotFrame( const wxString& title, PLStream* pls );
    void OnMenu( wxCommandEvent& event );
    void OnClose( wxCloseEvent& event );
    bool SavePlot( const char* filename, const char* devname, int width, int height );
    void SetRefreshFlag( bool flag = true ) { m_window->SetRefreshFlag( flag ); };
    bool GetRefreshFlag( void ) { return m_window->GetRefreshFlag(); };

private:
    wxPanel       * m_panel;
    wxPLplotWindow* m_window;
    wxPLDevBase   * m_dev; // frame needs to know this structure

    DECLARE_EVENT_TABLE()
};

// menu ids
enum { wxPL_Save = 10000, wxPL_Next = 10100, wxPL_Locate, wxPL_Orientation_0, wxPL_Orientation_90,
       wxPL_Orientation_180, wxPL_Orientation_270 };


// Pixel size dialog
class wxGetSizeDialog : public wxDialog
{
public:
    // constructors and destructors
    wxGetSizeDialog( wxWindow *parent, wxWindowID id, const wxString &title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_DIALOG_STYLE,
                     int width = 800, int height = 600 );

    int getWidth()  { return spinControlWidth->GetValue(); }
    int getHeight()  { return spinControlHeight->GetValue(); }

private:
    wxSpinCtrl* spinControlWidth;
    wxSpinCtrl* spinControlHeight;

private:
    DECLARE_EVENT_TABLE()
};


// Use this macro if you want to define your own main() or WinMain() function
// and call wxEntry() from there.
#define IMPLEMENT_PLAPP_NO_MAIN( appname )                                                                                                        \
    wxAppConsole * wxPLCreateApp()                                                                                                                \
    {                                                                                                                                             \
        wxAppConsole::CheckBuildOptions( WX_BUILD_OPTIONS_SIGNATURE,                                                                              \
            "your program" );                                                                                                                     \
        return new appname;                                                                                                                       \
    }                                                                                                                                             \
    wxAppInitializer                                                                                                                              \
    wxAppInitializer( (wxAppInitializerFunction) ( wxApp::GetInitializerFunction() == NULL ? wxPLCreateApp : wxApp::GetInitializerFunction() ) ); \
    DECLARE_PLAPP( appname )                                                                                                                      \
    appname & wxPLGetApp() { return *wx_static_cast( appname *, wxApp::GetInstance() ); }

#define DECLARE_PLAPP( appname )    extern appname &wxPLGetApp();

// workaround against warnings for unused variables
static inline void Use( void * )
{
}
#define WX_SUPPRESS_UNUSED_WARN( x )    Use( &x )


//--------------------------------------------------------------------------
//  Declarations for the device.
//--------------------------------------------------------------------------

void plD_init_wxwidgets( PLStream * );
void plD_init_wxpng( PLStream * );
void plD_line_wxwidgets( PLStream *, short, short, short, short );
void plD_polyline_wxwidgets( PLStream *, short *, short *, PLINT );
void plD_eop_wxwidgets( PLStream * );
void plD_bop_wxwidgets( PLStream * );
void plD_tidy_wxwidgets( PLStream * );
void plD_state_wxwidgets( PLStream *, PLINT );
void plD_esc_wxwidgets( PLStream *, PLINT, void * );

void wx_set_dc( PLStream* pls, wxDC* dc );
void wx_set_buffer( PLStream* pls, wxImage* buffer );
void wx_set_size( PLStream* pls, int width, int height );
int plD_errorexithandler_wxwidgets( char *errormessage );
void plD_erroraborthandler_wxwidgets( char *errormessage );

//--------------------------------------------------------------------------
//  Debug functions
//--------------------------------------------------------------------------

// define if you want debug output
// #define _DEBUG //
// #define _DEBUG_VERBOSE //
void Log_Verbose( const char *fmt, ... );
void Log_Debug( const char *fmt, ... );


//--------------------------------------------------------------------------
// Font style and weight lookup tables
//--------------------------------------------------------------------------
const wxFontFamily fontFamilyLookup[5] = {
    wxFONTFAMILY_SWISS,      // sans-serif
    wxFONTFAMILY_ROMAN,      // serif
    wxFONTFAMILY_TELETYPE,   // monospace
    wxFONTFAMILY_SCRIPT,     // script
    wxFONTFAMILY_SWISS       // symbol
};

const int          fontStyleLookup[3] = {
    wxFONTFLAG_DEFAULT,      // upright
    wxFONTFLAG_ITALIC,       // italic
    wxFONTFLAG_SLANT         // oblique
};

const int          fontWeightLookup[2] = {
    wxFONTFLAG_DEFAULT,     // medium
    wxFONTFLAG_BOLD         // bold
};

#endif // __WXWIDGETS_H__
