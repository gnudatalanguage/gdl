/***************************************************************************
                          tiff_cl.hpp - GDL TIFF library functions
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

#ifndef TIFF_HPP_CL
#define TIFF_HPP_CL

#include <tiffio.h>

namespace lib
{
    namespace TIFF
    {
        // Maximum number of tags combined into a single GDL key
        static constexpr size_t N = 2;

        struct Directory
        {
            uint32 width                = 0;
            uint32 height               = 0;
            uint32 tileWidth            = 0;
            uint32 tileHeight           = 0;
            uint16 samplesPerPixel      = 1;
            uint16 bitsPerSample        = 1;

            struct Position
            {
                float                   x = 0.f;
                float                   y = 0.f;
            } position;

            struct Resolution
            {
                enum class Unit : uint16
                {
                    None                = 1,
                    Inches              = 2,
                    Centimeters         = 3,
                };

                float x                 = 0.f;
                float y                 = 0.f;
                Unit unit               = Unit::Inches;
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
            } sampleFormat              = SampleFormat::Untyped;

            enum class PlanarConfig : uint16
            {
                Contiguous              = 1,
                Separate                = 2,
            } planarConfig;

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
            } photometric;

            const char* description     = "";
            const char* name            = "";
            const char* dateTime        = "";
        };

        class Handler
        {
        public:
             Handler();
            ~Handler();

            bool      Open(const char* file, const char* mode);
            bool      GetDirectory(tdir_t, Directory&);
            size_t    DirectoryCount() const;
            uint16    FileVersion() const;

            template<typename T>
            bool GetField(ttag_t tag, T& var, bool req = false) {
                if(tif_ && !TIFFGetField(tif_, tag, &var)) {
                    if(!req) return false;
                    if(auto field = TIFFFieldWithTag(tif_, tag))
                    throw TIFFFieldName(field); else throw tag;
                } else return (tif_ != nullptr);
            }

        private:
            ::TIFF*             tif_    = nullptr;
            TIFFErrorHandler    defEH_  = nullptr;
            TIFFErrorHandler    defWH_  = nullptr;
            TIFFHeaderCommon    header_ = { 0 };
            size_t              nDirs_  = 1;
        };
    }

    BaseGDL* tiff_query(EnvT*);
}

#endif

