/***************************************************************************
                          tiff_cl.cpp - GDL TIFF library functions
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

#ifdef HAVE_CONFIG_H
#    include "config.h"
#else
#    define USE_TIFF 1
#endif

#ifdef USE_TIFF

#include "includefirst.hpp"
#include "datatypes.hpp"
#include "envt.hpp"
#include "tiff_cl.hpp"
#include <map>

namespace lib
{
    namespace TIFF
    {
        Handler::Handler() {
            defEH_ = TIFFSetErrorHandler([](const char* mod, const char* fmt, va_list args) {
                // Ignore TIFFSetDirectory errors
                if(!strcmp(mod, "TIFFSetDirectory")) return;
                fprintf(stderr, "%s: Error, ", mod);
                vfprintf(stderr, fmt, args);
            });

            defWH_ = TIFFSetWarningHandler([](const char* mod, const char* fmt, va_list args) {
                // Ignore TIFFReadDirectory warnings
                if(!strcmp(mod, "TIFFReadDirectory")) return;
                fprintf(stderr, "%s: Warning, ", mod);
                vfprintf(stderr, fmt, args);
            });
        }

        Handler::~Handler() {
            if(defEH_) TIFFSetErrorHandler(defEH_);
            if(defWH_) TIFFSetWarningHandler(defWH_);
            if(tif_) TIFFClose(tif_);
        }

        bool Handler::Open(const char* file, const char* mode) {
            if(FILE* f = fopen(file, "r")) {
                if(!fread(&header_, sizeof(header_), 1, f))
                    fprintf(stderr, "%s: could not read TIFF header\n", file);
                fclose(f);
            }

            if((tif_ = TIFFOpen(file, mode))) {
                while(TIFFReadDirectory(tif_)) nDirs_++;
                TIFFSetDirectory(tif_, 0);
                return true;
            }

            return false;
        }

        bool Handler::GetDirectory(tdir_t index, Directory& dir) {
            if(!tif_ || !TIFFSetDirectory(tif_, index))
                return false;

            try {
                GetField(TIFFTAG_IMAGEWIDTH,        dir.width,              true);
                GetField(TIFFTAG_IMAGELENGTH,       dir.height,             true);
                GetField(TIFFTAG_TILEWIDTH,         dir.tileWidth,          false);
                GetField(TIFFTAG_TILELENGTH,        dir.tileHeight,         false);
                GetField(TIFFTAG_SAMPLESPERPIXEL,   dir.samplesPerPixel,    true);
                GetField(TIFFTAG_BITSPERSAMPLE,     dir.bitsPerSample,      true);
                GetField(TIFFTAG_XPOSITION,         dir.position.x,         false);
                GetField(TIFFTAG_YPOSITION,         dir.position.y,         false);
                GetField(TIFFTAG_RESOLUTIONUNIT,    dir.resolution.unit,    false);
                GetField(TIFFTAG_XRESOLUTION,       dir.resolution.x,       true);
                GetField(TIFFTAG_YRESOLUTION,       dir.resolution.y,       true);
                GetField(TIFFTAG_ORIENTATION,       dir.orientation,        false);
                GetField(TIFFTAG_SAMPLEFORMAT,      dir.sampleFormat,       true);
                GetField(TIFFTAG_PLANARCONFIG,      dir.planarConfig,       true);
                GetField(TIFFTAG_PHOTOMETRIC,       dir.photometric,        true);
                GetField(TIFFTAG_IMAGEDESCRIPTION,  dir.description,        false);
                GetField(TIFFTAG_DOCUMENTNAME,      dir.name,               false);
                GetField(TIFFTAG_DATETIME,          dir.dateTime,           false);
            }
            catch(const char* tag) {
                fprintf(stderr, "missing tag: %s\n", tag);
                return false;
            }
            catch(ttag_t tag) {
                fprintf(stderr, "missing unknown tag: %u\n", tag);
                return false;
            }

            return true;
        }

        size_t Handler::DirectoryCount() const {
            return (tif_ ? nDirs_ : 0);
        }

        uint16 Handler::FileVersion() const {
            // Byte-order dependent
            return (tif_ ? header_.tiff_version : 0);
        }
    }

    class DStructFactory
    {
    public:
        ~DStructFactory() {
            for(auto& pair : vals_)
                delete pair.second;
        }

        template<class T, typename... Vs>
        void Add(const char* name, const Vs&... vals) {
            constexpr auto N = sizeof...(Vs);
            auto proto = new typename T::Traits(dimension(N));
            typename T::Ty arr[] = { static_cast<typename T::Ty>(vals)... };
            desc_->AddTag(name, proto);
            vals_[name] = (N > 1 ? new T(arr, N) : new T(arr[0]));
            delete proto;
        }

        DStructGDL* Create() {
            auto res = new DStructGDL(desc_, dimension());
            for(auto& pair : vals_) {
                res->InitTag(pair.first, *pair.second);
                delete pair.second;
            }
            vals_.clear();
            return res;
        }

    private:
        DStructDesc* desc_ = new DStructDesc("$truct");
        std::map<const char*, BaseGDL*> vals_;
    };

    BaseGDL* tiff_query(EnvT* e) {
        // ref: https://www.harrisgeospatial.com/docs/query___routines.html
        // ref: https://www.harrisgeospatial.com/docs/QUERY_TIFF.html
        // ref: https://www.adobe.io/open/standards/TIFF.html

        SizeT nParam = e->NParam(1);

        DString filename;
        e->AssureScalarPar<DStringGDL>(0, filename);
        if(filename.length()) WordExp(filename);
        else return new DLongGDL(0);

        DLong imageIndex = 0;
        e->AssureLongScalarKWIfPresent("IMAGE_INDEX", imageIndex);

        TIFF::Handler tiff;
        TIFF::Directory dir;
        DInt pixelType = GDL_UNDEF;

        if(tiff.Open(filename.c_str(), "r")) {
            if(!tiff.GetDirectory(imageIndex, dir)) {
                fprintf(stderr, "invalid IMAGE_INDEX: %d\n", imageIndex);
                return new DLongGDL(0);
            }

            DInt pixelType = GDL_UNDEF;
            DInt hasPalette = (dir.photometric == TIFF::Directory::Photometric::Palette);
            static const char BPS_ERR_FMT[] = "unsupported value of BITSPERSAMPLE for SAMPLEFORMAT %s: %u\n";

            // NOTE: IDL seems to support 1 and 4 bits per sample values, however,
            // the PIXEL_TYPE value is not clearly stated in the documentation.
            // For now, just assume GDL_BYTE for these values.

            switch(dir.sampleFormat) {
            case TIFF::Directory::SampleFormat::UnsignedInteger:
                switch(dir.bitsPerSample) {
                case  1:
                case  4:
                case  8: pixelType = GDL_BYTE;  break;
                case 16: pixelType = GDL_UINT;  break;
                case 32: pixelType = GDL_ULONG; break;
                default: printf(BPS_ERR_FMT, "UINT", dir.bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::SignedInteger:
                switch(dir.bitsPerSample) {
                case 16: pixelType = GDL_INT;   break;
                case 32: pixelType = GDL_LONG;  break;
                default: printf(BPS_ERR_FMT, "INT", dir.bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::FloatingPoint:
                switch(dir.bitsPerSample) {
                case 32: pixelType = GDL_FLOAT; break;
                default: printf(BPS_ERR_FMT, "FLOAT", dir.bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::Untyped:
            case TIFF::Directory::SampleFormat::ComplexInteger:
            case TIFF::Directory::SampleFormat::ComplexFloatingPoint:
            default:;
            }

            // The TIFF standard support SAMPLEFORMAT of complex numbers (of both integers and floating points),
            // but according to the documentation, there is no explicit support for 64 and 128 bits per sample.
            // Though we could support such images in GDL, this would be an non-compliant extension of IDL.
            // In the future, we might add improved TIFF support using some custom input parameter, thus also
            // allowing us to support both half- and double-precision floating points, as well as 64-bit integers.

            if(pixelType == GDL_UNDEF)
                return new DLongGDL(0);

            static int infoIx = e->KeywordIx("INFO");
            if(e->KeywordPresent(infoIx)) {
                DStructFactory info;

                info.Add<DLongGDL  >("CHANNELS",        dir.samplesPerPixel);
                info.Add<DLongGDL  >("DIMENSIONS",      dir.width, dir.height);
                info.Add<DIntGDL   >("HAS_PALETTE",     hasPalette);
                info.Add<DLongGDL  >("NUM_IMAGES",      tiff.DirectoryCount());
                info.Add<DLongGDL  >("IMAGE_INDEX",     imageIndex);
                info.Add<DIntGDL   >("PIXEL_TYPE",      pixelType);
                // TODO: SBIT_VALUES
                info.Add<DStringGDL>("TYPE",            "TIFF");
                info.Add<DLongGDL  >("BITS_PER_SAMPLE", dir.bitsPerSample);
                info.Add<DStringGDL>("DESCRIPTION",     dir.description);
                info.Add<DStringGDL>("DOCUMENT_NAME",   dir.name);
                info.Add<DStringGDL>("DATE_TIME",       dir.dateTime);
                info.Add<DLongGDL  >("ORIENTATION",     dir.orientation);
                info.Add<DLongGDL  >("PLANAR_CONFIG",   dir.planarConfig);
                info.Add<DFloatGDL >("POSITION",        dir.position.x, dir.position.y);
                info.Add<DLongGDL  >("PHOTOMETRIC",     dir.photometric);
                info.Add<DFloatGDL >("RESOLUTION",      dir.resolution.x, dir.resolution.y);
                info.Add<DLongGDL  >("UNITS",           dir.resolution.unit);
                info.Add<DLongGDL  >("TILE_SIZE",       dir.tileWidth, dir.tileHeight);
                info.Add<DLongGDL  >("VERSION",         tiff.FileVersion());

                e->SetKW(infoIx, info.Create());
            }

            return new DLongGDL(1);
        }

        return new DLongGDL(0);
    }
}

#endif

