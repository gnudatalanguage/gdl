/***************************************************************************
                          tiff_cl.hxx - GDL TIFF library functions
                             -------------------
    begin                : May 22 2018
    copyright            : (C) 2018 by Remi A. Solås
    email                : remi.solaas (at) edinsights.no
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TIFF_HXX_CL
#define TIFF_HXX_CL

#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"

#ifdef USE_GEOTIFF
#   include <xtiffio.h>
#   include <geotiff.h>
#else
#   include <tiffio.h>
#endif

namespace lib
{
    namespace TIFF
    {
        struct Directory
        {
            tdir_t index                = 0;
            uint32 width                = 0;
            uint32 height               = 0;
            uint32 tileWidth            = 0;
            uint32 tileHeight           = 0;
            uint16 samplesPerPixel      = 1;
            uint16 bitsPerSample        = 1;

            struct Position
            {
                float x                 = 0.f;
                float y                 = 0.f;
            } position;

            struct Resolution
            {
                float x                 = 0.f;
                float y                 = 0.f;

                enum class Unit : uint16
                {
                    None                = 1,
                    Inches              = 2,
                    Centimeters         = 3,
                } unit                  = Unit::Inches;
            } resolution;

            enum class Orientation : uint16
            {
                LeftToRightTopToBottom  = 1,
                RightToLeftTopToBottom  = 2,
                RightToLeftBottomToTop  = 3,
                LeftToRigthBottomToTop  = 4,
                TopToBottomLeftToRight  = 5,
                TopToBottomRigthToLeft  = 6,
                BottomToTopRightToLeft  = 7,
                BottomToTopLeftToRight  = 8,
            } orientation               = Orientation::LeftToRightTopToBottom;

            enum class SampleFormat : uint16
            {
                UnsignedInteger         = 1,
                SignedInteger           = 2,
                FloatingPoint           = 3,
                Untyped                 = 4,
                ComplexInteger          = 5,
                ComplexFloatingPoint    = 6,
            } sampleFormat              = SampleFormat::UnsignedInteger;

            enum class PlanarConfig : uint16
            {
                Contiguous              = 1,
                Separate                = 2,
            } planarConfig              = PlanarConfig::Contiguous;

            enum class Photometric : uint16
            {
                MinIsWhite              = 0,
                MinIsBlack              = 1,
                RGB                     = 2,
                Palette                 = 3,
                TransparancyMask        = 4,
                Separated               = 5,
                YCBCR                   = 6,
                CIELab                  = 8,
                ICCLab                  = 9,
                ITULab                  = 10,
                ColorFilterArray        = 32803,
                CIELog2L                = 32844,
                CIELog2Luv              = 32845,
            } photometric               = Photometric::MinIsWhite;

            const char* description     = "";
            const char* name            = "";
            const char* dateTime        = "";

            DType PixelType() const;
       };

        #ifdef USE_GEOTIFF
        struct GeoKey
        {
            ~GeoKey();

            union
            {
                int8_t*     b;
                int16_t*    i;
                int32_t*    l;
                uint8_t*    ub;
                uint16_t*   ui;
                uint32_t*   ul;
                float*      f;
                double*     d;
                char*       str;
                void*       ptr;
            } value             = { 0 };

            tagtype_t   type    = TYPE_UNKNOWN;
            size_t      count   = 0;
        };
        #endif

        struct Rectangle
        {
            uint32 x, y;
            uint32 w, h;
        };

        class Handler
        {
        public:
             Handler();
            ~Handler();

            bool        Open(const char* file, const char* mode);
            void        Close();
            bool        GetDirectory(tdir_t, Directory&) const;
            uint16      DirectoryCount() const;
            uint16      FileVersion() const;
            BaseGDL*    ReadImage(const Directory&, const Rectangle& = { 0 }, const uint8 channelMask = 0xFF);

            template<typename... Ts>
            bool GetField(ttag_t tag, Ts&... vars) const
            {
                return (tiff_ && TIFFGetField(tiff_, tag, &vars...));
            }

            template<typename T>
            void GetRequiredField(ttag_t tag, T& var) const
            {
                if(tiff_ && !TIFFGetField(tiff_, tag, &var)) {
                    if(auto field = TIFFFieldWithTag(tiff_, tag))
                    throw TIFFFieldName(field); else throw tag;
                }
            }

            #ifdef USE_GEOTIFF
            DStructGDL* CreateGeoStruct(tdir_t) const;
            bool        GetGeoKey(geokey_t, GeoKey&) const;
            #endif

        private:
            ::TIFF*             tiff_   = nullptr;
            #ifdef USE_GEOTIFF
            GTIF*               gtif_   = nullptr;
            #endif
            TIFFErrorHandler    defEH_  = nullptr;
            TIFFErrorHandler    defWH_  = nullptr;
            uint16              nDirs_  = 1;
            uint16              verNum_ = 0;
        };
    }

    BaseGDL* tiff_query(EnvT*);
    BaseGDL* tiff_read(EnvT*);
}

#endif

