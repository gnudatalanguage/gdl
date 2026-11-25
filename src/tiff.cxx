/***************************************************************************
                          tiff.cxx - GDL TIFF library functions
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

#include "tiff.hxx"
#include "dstructfactory.hxx"
#include <fstream>
#include <new>
#include <type_traits>
#include <vector>

template<typename E>
typename std::underlying_type<E>::type enumIntegralValue(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

namespace lib
{
    namespace TIFF
    {
        DType Directory::PixelType() const
        {
            static const char BPS_ERR_FMT[] = "unsupported value of BITSPERSAMPLE for SAMPLEFORMAT %s: %u\n";
            DType pixelType = GDL_UNDEF;

            // NOTE: IDL seems to support 1 and 4 bits per sample values, however,
            // the PIXEL_TYPE value is not clearly stated in the documentation.
            // For now, just assume GDL_BYTE for these values.

            switch(sampleFormat) {
            case TIFF::Directory::SampleFormat::UnsignedInteger:
                switch(bitsPerSample) {
                case  1:
                case  4:
                case  8: pixelType = GDL_BYTE;      break;
                case 16: pixelType = GDL_UINT;      break;
                case 32: pixelType = GDL_ULONG;     break;
                case 64: pixelType = GDL_ULONG64;   break;
                default: printf(BPS_ERR_FMT, "UINT", bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::SignedInteger:
                switch(bitsPerSample) {
                case 16: pixelType = GDL_INT;       break;
                case 32: pixelType = GDL_LONG;      break;
                case 64: pixelType = GDL_LONG64;    break;
                default: printf(BPS_ERR_FMT, "INT", bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::FloatingPoint:
                switch(bitsPerSample) {
                case 32: pixelType = GDL_FLOAT;     break;
                case 64: pixelType = GDL_DOUBLE;    break;
                default: printf(BPS_ERR_FMT, "FLOAT", bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::ComplexInteger:
                switch(bitsPerSample) {
                case 16: pixelType = GDL_INT;       break;
                case 32: pixelType = GDL_LONG;      break;
                case 64: pixelType = GDL_LONG64;    break;
                default: printf(BPS_ERR_FMT, "CINT", bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::ComplexFloatingPoint:
                switch(bitsPerSample) {
                case 64: pixelType = GDL_COMPLEX;      break;
                case 128: pixelType = GDL_COMPLEXDBL;    break;
                default: printf(BPS_ERR_FMT, "CINT", bitsPerSample);
                } break;
            case TIFF::Directory::SampleFormat::Untyped:
            default:;
            }

            return pixelType;
        }

        Handler::Handler()
        {
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

        Handler::~Handler()
        {
            Close();

            TIFFSetErrorHandler(defEH_);
            TIFFSetWarningHandler(defWH_);
        }

        bool Handler::Open(const char* file, const char* mode)
        {
            TIFFHeaderCommon head;
            {
                std::ifstream input(file, std::ios::binary);
                if (!input) {
                    fprintf(stderr, "%s: could not open file for reading\n", file);
                    return false;
                }
                if (!input.read(reinterpret_cast<char*>(&head), sizeof(head))) {
                    fprintf(stderr, "%s: could not read TIFF header\n", file);
                    return false;
                }
            }

            Close();
            verNum_ = head.tiff_version;

            union { uint16_t w; uint8_t b[2]; } u = { 0xBEEF };
            static bool big = (u.b[0] == 0xBE && u.b[1] == 0xEF);

            if(head.tiff_magic == TIFF_BIGENDIAN && !big)
                TIFFSwabShort(&verNum_);

            #ifdef USE_GEOTIFF
            tiff_.reset(XTIFFOpen(file, mode));
            if (!tiff_) return false;
            gtif_.reset(GTIFNew(tiff_.get()));
            if (!gtif_) {
                Close();
                return false;
            }
            #else
            tiff_.reset(TIFFOpen(file, mode));
            if (!tiff_) return false;
            #endif

            while(TIFFReadDirectory(tiff_.get())) nDirs_++;
            TIFFSetDirectory(tiff_.get(), 0);
            return true;
        }

        void Handler::Close()
        {
            #ifdef USE_GEOTIFF
            gtif_.reset();
            #endif
            tiff_.reset();

            nDirs_ = 1;
            verNum_ = 0;
        }

        bool Handler::GetDirectory(tdir_t index, Directory& dir) const
        {
            if(!tiff_ || !TIFFSetDirectory(tiff_.get(), index))
                return false;

            try {
                GetRequiredField(TIFFTAG_IMAGEWIDTH,        dir.width);
                GetRequiredField(TIFFTAG_IMAGELENGTH,       dir.height);
                GetRequiredField(TIFFTAG_PHOTOMETRIC,       dir.photometric);

                GetField(TIFFTAG_SAMPLESPERPIXEL,           dir.samplesPerPixel);
                GetField(TIFFTAG_BITSPERSAMPLE,             dir.bitsPerSample);
                GetField(TIFFTAG_SAMPLEFORMAT,              dir.sampleFormat);
                GetField(TIFFTAG_TILEWIDTH,                 dir.tileWidth);
                GetField(TIFFTAG_TILELENGTH,                dir.tileHeight);
                GetField(TIFFTAG_XPOSITION,                 dir.position.x);
                GetField(TIFFTAG_YPOSITION,                 dir.position.y);
                GetField(TIFFTAG_XRESOLUTION,               dir.resolution.x);
                GetField(TIFFTAG_YRESOLUTION,               dir.resolution.y);
                GetField(TIFFTAG_RESOLUTIONUNIT,            dir.resolution.unit);
                GetField(TIFFTAG_PLANARCONFIG,              dir.planarConfig);
                GetField(TIFFTAG_ORIENTATION,               dir.orientation);
                GetField(TIFFTAG_COLORMAP,                  dir.colorMap.red, dir.colorMap.green, dir.colorMap.blue);
                GetField(TIFFTAG_IMAGEDESCRIPTION,          dir.description);
                GetField(TIFFTAG_DOCUMENTNAME,              dir.name);
                GetField(TIFFTAG_DATETIME,                  dir.dateTime);

                // IDL/GDL specific values for non-tiled images
                if(!(dir.tileWidth && dir.tileHeight)) {
                    dir.tileWidth = dir.width;
                    dir.tileHeight = 1;
                }
            }
            catch(const char* tag) {
                fprintf(stderr, "missing tag: %s\n", tag);
                return false;
            }
            catch(ttag_t& tag) {
                fprintf(stderr, "missing unknown tag: %u\n", tag);
                return false;
            }

            dir.index = index;
            return true;
        }

        uint16_t Handler::DirectoryCount() const
        {
            return (tiff_ ? nDirs_ : 0);
        }

        uint16_t Handler::FileVersion() const
        {
            return (tiff_ ? verNum_ : 0);
        }

        std::unique_ptr<BaseGDL> Handler::ReadImage(const Directory& dir, const Rectangle& rect)
        {
            uint32_t c = dir.samplesPerPixel;
            uint32_t w = (rect.w ? rect.w : dir.width - rect.x);
            uint32_t h = (rect.h ? rect.h : dir.height - rect.y);
            if (!c || !w || !h) return nullptr;

            std::unique_ptr<BaseGDL> image;
            dimension dim(w, h);

            if(c > 1) {
                SizeT ndim[3] = { c, w, h };
                dim = dimension(ndim, 3);
            }

            switch(dir.PixelType()) {
            case GDL_BYTE:      image = std::make_unique<DByteGDL>(dim); break;
            case GDL_UINT:      image = std::make_unique<DUIntGDL>(dim); break;
            case GDL_ULONG:     image = std::make_unique<DULongGDL>(dim); break;
            case GDL_ULONG64:   image = std::make_unique<DULong64GDL>(dim); break;
            case GDL_INT:       image = std::make_unique<DIntGDL>(dim); break;
            case GDL_LONG:      image = std::make_unique<DLongGDL>(dim); break;
            case GDL_LONG64:    image = std::make_unique<DLong64GDL>(dim); break;
            case GDL_FLOAT:     image = std::make_unique<DFloatGDL>(dim); break;
            case GDL_DOUBLE:    image = std::make_unique<DDoubleGDL>(dim); break;
            case GDL_COMPLEX:   image = std::make_unique<DComplexGDL>(dim); break;
            case GDL_COMPLEXDBL: image = std::make_unique<DComplexDblGDL>(dim); break;

            default:
                fprintf(stderr, "Unsupported PIXEL_TYPE: %d\n", dir.PixelType());
                return nullptr;
            }

            const bool tiled = TIFFIsTiled(tiff_.get());
            const auto size = tiled ? TIFFTileSize(tiff_.get()) : TIFFScanlineSize(tiff_.get());
            const auto rowBytes = tiled ? TIFFTileRowSize(tiff_.get()) : size;
            const uint32_t rowWidth = tiled ? dir.tileWidth : dir.width;
            const uint32_t rows = tiled ? dir.tileHeight : 1;
            const uint64_t rowBits = static_cast<uint64_t>(rowWidth) * c * dir.bitsPerSample;
            if (!rowWidth || !rows || size <= 0 || rowBytes <= 0 || rowBytes > size / rows ||
                static_cast<uint64_t>(rowBytes) < (rowBits + 7) / 8)
                return nullptr;
            std::unique_ptr<char[]> scratch(new (std::nothrow) char[size]);
            char* buffer = scratch.get();
            if (!buffer) {
                fprintf(stderr, "Could not allocate %ld bytes for TIFF image buffer\n", (long)size);
                return nullptr;
            }
            const size_t pixelBytes = c * image->Sizeof();
            const auto bits = dir.bitsPerSample;
            const bool msbFirst = TIFFIsMSB2LSB(tiff_.get());
            auto* pixels = static_cast<char*>(image->DataAddr());
            auto copyRow = [&](uint32_t x, uint32_t y, const char* src, uint32_t srcX, size_t count) {
                auto* dst = pixels + (static_cast<size_t>(y) * w + x) * pixelBytes;
                if (bits >= 8) {
                    memcpy(dst, src + srcX * pixelBytes, count * pixelBytes);
                } else {
                    // TIFF packs 1- and 4-bit samples. GDL exposes each sample as a byte.
                    size_t bit = static_cast<size_t>(srcX) * c * bits;
                    for (size_t i = 0; i < count * c; ++i, bit += bits) {
                        const unsigned shift = msbFirst ? 8 - bits - bit % 8 : bit % 8;
                        dst[i] = (static_cast<unsigned char>(src[bit / 8]) >> shift) & ((1u << bits) - 1);
                    }
                }
            };

            if (!tiled) {
                for(uint32_t y = 0; y < h; ++y) {
                    if(TIFFReadScanline(tiff_.get(), buffer, rect.y + y, 0) == -1)
                        return nullptr;

                    copyRow(0, y, buffer, rect.x, w);
                }
            }

            // Tile-based images
            else {
                for(uint32_t y = 0, yrem, yoff; y < h; y += yrem) {
                    yoff = (rect.y + y) % dir.tileHeight;

                    if(((yrem = dir.tileHeight - yoff) + y) > h)
                        yrem = h - y;

                    for(uint32_t x = 0, xrem, xoff; x < w; x += xrem) {
                        if(TIFFReadTile(tiff_.get(), buffer, rect.x + x, rect.y + y, 0, 0) == -1)
                            return nullptr;

                        xoff = (rect.x + x) % dir.tileWidth;
                        if(((xrem = dir.tileWidth - xoff) + x) > w)
                            xrem = w - x;

                        for (uint32_t ty = 0; ty < yrem; ++ty)
                            copyRow(x, y + ty, buffer + (yoff + ty) * rowBytes, xoff, xrem);
                    }
                }
            }

            return image;
        }

        #ifdef USE_GEOTIFF
        BaseGDL* Handler::CreateGeoStructOrZero(tdir_t index) const
        {
            if(!tiff_ || !TIFFSetDirectory(tiff_.get(), index))
                return nullptr;

            DStructFactory gtif;
            uint16_t nvals;
            double* val;

            // TIFF geo fields
            if(GetField(TIFFTAG_GEOPIXELSCALE, nvals, val))
                gtif.AddArr<DDoubleGDL>("MODELPIXELSCALETAG", nvals, val);
            if(GetField(TIFFTAG_GEOTRANSMATRIX, nvals, val))
                gtif.AddArr<DDoubleGDL>("MODELTRANSFORMATIONTAG", nvals, val);
            if(GetField(TIFFTAG_GEOTIEPOINTS, nvals, val))
                gtif.AddMat<DDoubleGDL>("MODELTIEPOINTTAG", 6, nvals / 6, val);

            // GeoTIFF keys
            // Read scalar numeric keys into their native type. ASCII keys have a
            // variable count and need room for a trailing nul.
            auto getNumber = [&](geokey_t key, tagtype_t expected, auto& value) {
                int size;
                tagtype_t type;
                return GTIFKeyInfo(gtif_.get(), key, &size, &type) > 0 && type == expected &&
                       size == static_cast<int>(sizeof(value)) && GTIFKeyGet(gtif_.get(), key, &value, 0, 1);
            };
            auto addShort = [&](geokey_t key, const char* name) {
                unsigned short value;
                if (getNumber(key, TYPE_SHORT, value)) gtif.Add<DIntGDL>(name, value);
            };
            auto addDouble = [&](geokey_t key, const char* name) {
                double value;
                if (getNumber(key, TYPE_DOUBLE, value)) gtif.Add<DDoubleGDL>(name, value);
            };
            auto addString = [&](geokey_t key, const char* name) {
                int size;
                tagtype_t type;
                int count = GTIFKeyInfo(gtif_.get(), key, &size, &type);
                if (count > 0 && type == TYPE_ASCII && size == 1) {
                    std::vector<char> value(static_cast<size_t>(count) + 1);
                    if (GTIFKeyGet(gtif_.get(), key, value.data(), 0, count))
                        gtif.Add<DStringGDL>(name, value.data());
                }
            };

            addShort(GTModelTypeGeoKey, "GTMODELTYPEGEOKEY");
            addShort(GTRasterTypeGeoKey, "GTRASTERTYPEGEOKEY");
            addString(GTCitationGeoKey, "GTCITATIONGEOKEY");
            addShort(GeographicTypeGeoKey, "GEOGRAPHICTYPEGEOKEY");
            addString(GeogCitationGeoKey, "GEOGCITATIONGEOKEY");
            addShort(GeogGeodeticDatumGeoKey, "GEOGGEODETICDATUMGEOKEY");
            addShort(GeogPrimeMeridianGeoKey, "GEOGPRIMEMERIDIANGEOKEY");
            addShort(GeogLinearUnitsGeoKey, "GEOGLINEARUNITSGEOKEY");
            addDouble(GeogLinearUnitSizeGeoKey, "GEOGLINEARUNITSIZEGEOKEY");
            addShort(GeogAngularUnitsGeoKey, "GEOGANGULARUNITSGEOKEY");
            addDouble(GeogAngularUnitSizeGeoKey, "GEOGANGULARUNITSIZEGEOKEY");
            addShort(GeogEllipsoidGeoKey, "GEOGELLIPSOIDGEOKEY");
            addDouble(GeogSemiMajorAxisGeoKey, "GEOGSEMIMAJORAXISGEOKEY");
            addDouble(GeogSemiMinorAxisGeoKey, "GEOGSEMIMINORAXISGEOKEY");
            addDouble(GeogInvFlatteningGeoKey, "GEOGINVFLATTENINGGEOKEY");
            addShort(GeogAzimuthUnitsGeoKey, "GEOGAZIMUTHUNITSGEOKEY");
            addDouble(GeogPrimeMeridianLongGeoKey, "GEOGPRIMEMERIDIANLONGGEOKEY");
            addShort(ProjectedCSTypeGeoKey, "PROJECTEDCSTYPEGEOKEY");
            addString(PCSCitationGeoKey, "PCSCITATIONGEOKEY");
            addShort(ProjectionGeoKey, "PROJECTIONGEOKEY");
            addShort(ProjCoordTransGeoKey, "PROJCOORDTRANSGEOKEY");
            addShort(ProjLinearUnitsGeoKey, "PROJLINEARUNITSGEOKEY");
            addDouble(ProjLinearUnitSizeGeoKey, "PROJLINEARUNITSIZEGEOKEY");
            addDouble(ProjStdParallel1GeoKey, "PROJSTDPARALLEL1GEOKEY");
            addDouble(ProjStdParallel2GeoKey, "PROJSTDPARALLEL2GEOKEY");
            addDouble(ProjNatOriginLongGeoKey, "PROJNATORIGINLONGGEOKEY");
            addDouble(ProjNatOriginLatGeoKey, "PROJNATORIGINLATGEOKEY");
            addDouble(ProjFalseEastingGeoKey, "PROJFALSEEASTINGGEOKEY");
            addDouble(ProjFalseNorthingGeoKey, "PROJFALSENORTHINGGEOKEY");
            addDouble(ProjFalseOriginLongGeoKey, "PROJFALSEORIGINLONGGEOKEY");
            addDouble(ProjFalseOriginLatGeoKey, "PROJFALSEORIGINLATGEOKEY");
            addDouble(ProjFalseOriginEastingGeoKey, "PROJFALSEORIGINEASTINGGEOKEY");
            addDouble(ProjFalseOriginNorthingGeoKey, "PROJFALSEORIGINNORTHINGGEOKEY");
            addDouble(ProjCenterLongGeoKey, "PROJCENTERLONGGEOKEY");
            addDouble(ProjCenterEastingGeoKey, "PROJCENTEREASTINGGEOKEY");
            addDouble(ProjCenterNorthingGeoKey, "PROJCENTERNORTHINGGEOKEY");
            addDouble(ProjScaleAtNatOriginGeoKey, "PROJSCALEATNATORIGINGEOKEY");
            addDouble(ProjScaleAtCenterGeoKey, "PROJSCALEATCENTERGEOKEY");
            addDouble(ProjAzimuthAngleGeoKey, "PROJAZIMUTHANGLEGEOKEY");
            addDouble(ProjStraightVertPoleLongGeoKey, "PROJSTRAIGHTVERTPOLELONGGEOKEY");
            addShort(VerticalCSTypeGeoKey, "VERTICALCSTYPEGEOKEY");
            addString(VerticalCitationGeoKey, "VERTICALCITATIONGEOKEY");
            addShort(VerticalDatumGeoKey, "VERTICALDATUMGEOKEY");
            addShort(VerticalUnitsGeoKey, "VERTICALUNITSGEOKEY");

            if(auto geoStruct = gtif.Create())
	        return geoStruct;

            return new DLongGDL(0);
        }

        #endif
    }

    BaseGDL* tiff_query(EnvT* e)
    {
        // ref: https://www.harrisgeospatial.com/docs/query___routines.html
        // ref: https://www.harrisgeospatial.com/docs/QUERY_TIFF.html
        // ref: https://www.adobe.io/open/standards/TIFF.html

        SizeT nParam = e->NParam(1);

        DString filename;
        e->AssureScalarPar<DStringGDL>(0, filename);
        if(filename.length()) WordExp(filename);
        else return new DLongGDL(0);

        DLong imageIndex = 0;
        static int IMAGE_INDEX=e->KeywordIx("IMAGE_INDEX");
        e->AssureLongScalarKWIfPresent(IMAGE_INDEX, imageIndex);

        TIFF::Handler tiff;
        TIFF::Directory dir;

        if(tiff.Open(filename.c_str(), "r")) {
            if(!tiff.GetDirectory(imageIndex, dir)) {
                fprintf(stderr, "invalid IMAGE_INDEX: %d\n", imageIndex);
                return new DLongGDL(0);
            }

            DInt hasPalette = (dir.photometric == TIFF::Directory::Photometric::Palette);
            DType pixelType = dir.PixelType();
// see #1911 -- if the file is not a TIFF file, Open would have already returned with 0.
// Unfortunately, PixelType() does return a zero = GDL_UNDEF value for complex data.
// We cannot return 0 on the following test, as it would be false for bona fide complex TIFF files.
//            if(pixelType == GDL_UNDEF)
//               return new DLongGDL(0);

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

            #ifdef USE_GEOTIFF
            static int gtifIx = e->KeywordIx("GEOTIFF");
            if(e->KeywordPresent(gtifIx)) {
                e->SetKW(gtifIx, tiff.CreateGeoStructOrZero(imageIndex));
            }
            #endif

            return new DLongGDL(1);
        }

        return new DLongGDL(0);
    }

    BaseGDL* tiff_read(EnvT* e)
    {
        // ref: https://www.harrisgeospatial.com/docs/READ_TIFF.html
        // https://www.adobe.io/open/standards/TIFF.html

        SizeT nParam = e->NParam(1);

        DString filename;
        e->AssureScalarPar<DStringGDL>(0, filename);
        if(filename.length()) WordExp(filename);
        else e->Throw("Zero-length filename");

        DLong imageIndex = 0;
        static int IMAGE_INDEX=e->KeywordIx("IMAGE_INDEX");
        e->AssureLongScalarKWIfPresent(IMAGE_INDEX, imageIndex);
        static int VERBOSE=e->KeywordIx("VERBOSE");
        bool verbose = e->KeywordPresent(VERBOSE);

        TIFF::Handler tiff;
        TIFF::Directory dir;
        std::unique_ptr<BaseGDL> image;

        if(tiff.Open(filename.c_str(), "r")) {
            if(!tiff.GetDirectory(imageIndex, dir))
                e->Throw("Invalid IMAGE_INDEX value");

            TIFF::Rectangle rect = { 0, 0, dir.width, dir.height };

            // TODO: Plane interleaved images
            if(dir.planarConfig == TIFF::Directory::PlanarConfig::Separate) {
                e->Throw("Plane interleaved images not yet supported");

                /*
                If the TIFF file is written as a three-channel image, interleaved by plane, and the R, G, and B parameters are present,
                the three channels of the image are returned in the R, G, and B variables.

                As a special case, for three-channel TIFF image files that are stored in planar interleave format,
                and if four parameters are provided, READ_TIFF returns the integer value zero,
                and returns three separate images in the variables defined by the R, G, and B arguments.
                */

                return new DLongGDL(0);
            }

            // Palette coloured images
            else if(dir.photometric == TIFF::Directory::Photometric::Palette) {
                if(!(dir.bitsPerSample == 16 && dir.samplesPerPixel == 3))
                    e->Throw("Palettes only supported for 3 channel images with 16 bits per pixel");

                size_t count = (2 << (dir.bitsPerSample - 1));

                static int rIx = e->KeywordIx("RED");
                if(e->KeywordSet(rIx)) {
                    if(dir.colorMap.red) e->SetKW(rIx, new DUIntGDL(dir.colorMap.red, count));
                    else e->Throw("RED colormap missing in TIFF file");
                }

                static int gIx = e->KeywordIx("GREEN");
                if(e->KeywordSet(gIx)) {
                    if(dir.colorMap.green) e->SetKW(gIx, new DUIntGDL(dir.colorMap.green, count));
                    else e->Throw("GREEN colormap missing in TIFF file");
                }

                static int bIx = e->KeywordIx("BLUE");
                if(e->KeywordSet(bIx)) {
                    if(dir.colorMap.blue) e->SetKW(bIx, new DUIntGDL(dir.colorMap.blue, count));
                    else e->Throw("BLUE colormap missing in TIFF file");
                }
            }

            // Use exclicitly defined sub rectangle if present
            static int subRectIx = e->KeywordIx("SUB_RECT");
            if(e->KeywordSet(subRectIx)) {
                auto* subRect = e->GetKWAs<DULongGDL>(subRectIx);

                if(subRect->N_Elements() != 4)
                    e->Throw("Invalid number of elements in SUB_RECT array (4 expected)");

                rect.x = (*subRect)[0];
                rect.y = (*subRect)[1];
                rect.w = (*subRect)[2];
                rect.h = (*subRect)[3];

                if (rect.x >= dir.width || rect.y >= dir.height ||
                    rect.w > dir.width - rect.x || rect.h > dir.height - rect.y)
                    e->Throw("Invalid SUB_RECT value exeeds image dimensions");
            }

            if(!(image = tiff.ReadImage(dir, rect)))
                e->Throw("Failed to read TIFF image with defined parameters");

            static int dotRangeIx = e->KeywordIx("DOT_RANGE");
            if(e->KeywordPresent(dotRangeIx)) {
                /* TODO: DOT_RANGE keyword
                Set this keyword to a named variable in which to return a two-element integer array containing the TIFF DotRange tag value.
                If DotRange is not defined in the TIFF file, then a scalar zero will be returned instead.
                The DotRange tag is typically only present in CMYK TIFF files. DOT_RANGE[0] gives the image value that corresponds to a 0% dot,
                while DOT_RANGE[1] gives the image value that corresponds to a 100% dot.
                */

                e->SetKW(dotRangeIx, new DLongGDL(0));
            }

            #ifdef USE_GEOTIFF
            static int gtifIx = e->KeywordIx("GEOTIFF");
            if(e->KeywordPresent(gtifIx)) {
                e->SetKW(gtifIx, tiff.CreateGeoStructOrZero(imageIndex));
            }
            #endif

            static int iccProfileIx = e->KeywordIx("ICC_PROFILE");
            if(e->KeywordPresent(iccProfileIx)) {
                /* TODO: ICC_PROFILE keyword
                Set this keyword to a named variable in which to return a byte array containing the TIFF ICC_PROFILE tag value.
                If the ICC_PROFILE tag is not contained in the TIFF file then a scalar zero will be returned instead.
                The ICC_PROFILE array is returned as an opaque array of byte values which the user can then pass to the WRITE_TIFF routine.

                Note: The ICC_PROFILE tag is returned as an opaque array of byte values.
                It is assumed that the tag will be read from an existing file, and written out to a new file without modification.
                */

                e->SetKW(iccProfileIx, new DLongGDL(0));
            }

            static int orientIx = e->KeywordIx("ORIENTATION");
            if(e->KeywordPresent(orientIx)) {
                e->SetKW(orientIx, new DLongGDL(enumIntegralValue(dir.orientation)));
            }

            static int photoshopIx = e->KeywordIx("PHOTOSHOP");
            if(e->KeywordPresent(photoshopIx)) {
                /* TODO: PHOTOSHOP keyword
                Set this keyword to a named variable in which to return a byte array containing the TIFF PHOTOSHOP tag value.
                If the PHOTOSHOP tag is not contained in the TIFF file, then a scalar zero will be returned instead.
                The PHOTOSHOP array is returned as an opaque array of byte values which the user can then pass to the WRITE_TIFF routine.

                Note: The PHOTOSHOP tag is returned as an opaque array of byte values.
                It is assumed that the tag will be read from an existing file, and written out to a new file without modification.
                */

                e->SetKW(photoshopIx, new DLongGDL(0));
            }

            static int planarConfigIx = e->KeywordIx("PLANARCONFIG");
            if(e->KeywordPresent(planarConfigIx)) {
                e->SetKW(planarConfigIx, new DLongGDL(enumIntegralValue(dir.planarConfig)));
            }
       } else {
           e->Throw("Unable to open TIFF file: " + filename);
       }

       return image.release();
   }
}

#endif

