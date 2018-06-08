/***************************************************************************
                          tiff_cl.cxx - GDL TIFF library functions
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

#include "tiff_cl.hxx"
#include "dstructfactory.hxx"
#include <type_traits>

template<typename E>
auto enumIntegralValue(E e)
{
    return static_cast<typename std::underlying_type<E>::type>(e);
}

uint8 popCount8(uint8 x)
{
    x = x - ((x >> 1) & 0x55);
    x = (x & 0x33) + ((x >> 2) & 0x33);
    return ((x + (x >> 4)) & 0x0F);
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
            case TIFF::Directory::SampleFormat::Untyped:
            case TIFF::Directory::SampleFormat::ComplexInteger:
            case TIFF::Directory::SampleFormat::ComplexFloatingPoint:
            default:;
            }

            return pixelType;
        }

        #ifdef USE_GEOTIFF
        GeoKey::~GeoKey()
        {
            if(value.ptr)
                free(value.ptr);
        }
        #endif

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

            if(defEH_) TIFFSetErrorHandler(defEH_);
            if(defWH_) TIFFSetWarningHandler(defWH_);
        }

        bool Handler::Open(const char* file, const char* mode)
        {
            TIFFHeaderCommon head;
            FILE* fptr;

            if(!(fptr = fopen(file, "r"))) {
                fprintf(stderr, "%s: could not open file for reading\n", file);
                return false;
            }

            if(!fread(&head, sizeof(head), 1, fptr)) {
                fprintf(stderr, "%s: could not read TIFF header\n", file);
                fclose(fptr);
                return false;
            }

            fclose(fptr);
            verNum_ = head.tiff_version;

            union { uint16_t w; uint8_t b[2]; } u = { 0xBEEF };
            static bool big = (u.b[0] == 0xBE && u.b[1] == 0xEF);

            if(head.tiff_magic == TIFF_BIGENDIAN && !big)
                TIFFSwabShort(&verNum_);

            #ifdef USE_GEOTIFF
            if(!(tiff_ = XTIFFOpen(file, mode)))
                return (Close(), false);

            if(!(gtif_ = GTIFNew(tiff_)))
                return (Close(), false);
            #else
            if(!(tiff_ = TIFFOpen(file, mode)))
                return (Close(), false);
            #endif

            while(TIFFReadDirectory(tiff_)) nDirs_++;
            TIFFSetDirectory(tiff_, 0);
            return true;
        }

        void Handler::Close()
        {
            #ifdef USE_GEOTIFF
            if(gtif_)
            {
                GTIFFree(gtif_);
                gtif_ = nullptr;
            }
            #endif

            if(tiff_)
            {
                TIFFClose(tiff_);
                tiff_ = nullptr;
            }

            nDirs_ = 1;
            verNum_ = 0;
        }

        bool Handler::GetDirectory(tdir_t index, Directory& dir) const
        {
            if(!tiff_ || !TIFFSetDirectory(tiff_, index))
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
                GetField(TIFFTAG_IMAGEDESCRIPTION,          dir.description);
                GetField(TIFFTAG_DOCUMENTNAME,              dir.name);
                GetField(TIFFTAG_DATETIME,                  dir.dateTime);

                // IDL/GDL specific values for non-tiled images
                if(!(dir.tileWidth && dir.tileHeight)) {
                    dir.tileWidth = dir.width;
                    dir.tileHeight = 1;
                }

                if(!(dir.resolution.x && dir.resolution.y))
                    dir.resolution.unit = Directory::Resolution::Unit::None;
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

        uint16 Handler::DirectoryCount() const
        {
            return (tiff_ ? nDirs_ : 0);
        }

        uint16 Handler::FileVersion() const
        {
            return (tiff_ ? verNum_ : 0);
        }

        typedef void (*ScanlineFn)(BaseGDL*, uint32, uint32, const void*, size_t);
        template<typename T>
        ScanlineFn createScanlineFn(BaseGDL*& var, T* val, const uint32 channels)
        {
            if(!(var = val))
                return nullptr;

            return [](BaseGDL* image, uint32 x, uint32 y, const void* buf, size_t bytes) {
                auto img = static_cast<T*>(image);
                auto ptr = reinterpret_cast<typename T::Ty*>(img->DataAddr());
                auto dim = img->Dim();
                memcpy(ptr + (y * dim[0] + x), buf, bytes);
            };
        }

        BaseGDL* Handler::ReadImage(const Directory& dir, const Rectangle& rect, const Interleaving interleave, const uint8 channelMask)
        {
            uint32 c = popCount8(channelMask);
            uint32 w = (rect.w ? rect.w : dir.width - rect.x);
            uint32 h = (rect.h ? rect.h : dir.height - rect.y);

            if(c > dir.samplesPerPixel)
                c = dir.samplesPerPixel;

            /* TODO: Add support for channel masking */
            if(c != dir.samplesPerPixel) {
                fprintf(stderr, "Channel masking currently not supported\n");
                return nullptr;
            }

            ScanlineFn addScanline;
            BaseGDL* image = nullptr;
            dimension dim(w, h);

            if(c > 1) {
                SizeT ndim[3];

                switch(interleave) {
                case Interleaving::Pixel:       ndim[0] = c; ndim[1] = w; ndim[2] = h; break;

                /* TODO: Add support for forced interleaving
                case Interleaving::Scanline:    ndim[0] = w; ndim[1] = c; ndim[2] = h; break;
                case Interleaving::Planar:      ndim[0] = w; ndim[1] = h; ndim[2] = c; break;
                */
                default:
                    fprintf(stderr, "Forced image interleaving currently not supported\n");
                    return nullptr;
                }

                if(dir.planarConfig != TIFF::Directory::PlanarConfig::Contiguous) {
                    fprintf(stderr, "Non-contiguous planar configurations currently not supported\n");
                    return nullptr;
                }

                dim = dimension(ndim, 3);
            }

            switch(dir.PixelType()) {
            case GDL_BYTE:      addScanline = createScanlineFn(image, new DByteGDL(dim),    c); break;
            case GDL_UINT:      addScanline = createScanlineFn(image, new DUIntGDL(dim),    c); break;
            case GDL_ULONG:     addScanline = createScanlineFn(image, new DULongGDL(dim),   c); break;
            case GDL_ULONG64:   addScanline = createScanlineFn(image, new DULong64GDL(dim), c); break;
            case GDL_INT:       addScanline = createScanlineFn(image, new DIntGDL(dim),     c); break;
            case GDL_LONG:      addScanline = createScanlineFn(image, new DLongGDL(dim),    c); break;
            case GDL_LONG64:    addScanline = createScanlineFn(image, new DLong64GDL(dim),  c); break;
            case GDL_FLOAT:     addScanline = createScanlineFn(image, new DFloatGDL(dim),   c); break;
            case GDL_DOUBLE:    addScanline = createScanlineFn(image, new DDoubleGDL(dim),  c); break;

            default:
                fprintf(stderr, "Unsupported PIXEL_TYPE: %d\n", dir.PixelType());
                return nullptr;
            }

            char *buffer = nullptr, *start;
            ptrdiff_t sampOff = (dir.samplesPerPixel * (dir.bitsPerSample >= 8 ? (dir.bitsPerSample / 8) : 1));

            // Scanline-based images
            if(!TIFFIsTiled(tiff_)) {
                auto scanSize = TIFFScanlineSize(tiff_);

                if(!(buffer = static_cast<char*>(_TIFFmalloc(scanSize)))) {
                    fprintf(stderr, "Could not allocate %lu bytes for TIFF scanline buffer\n", scanSize);
                    goto error;
                }

                for(uint32 y = 0; y < h; ++y) {
                    if(TIFFReadScanline(tiff_, buffer, rect.y + y, 0) == -1)
                        goto error;

                    start = buffer + (sampOff * rect.x);
                    addScanline(image, 0, y, start, sampOff * w);
                }
            }

            // Tile-based images
            else {
                auto tileSize = TIFFTileSize(tiff_);

                if(!(buffer = static_cast<char*>(_TIFFmalloc(tileSize)))) {
                    fprintf(stderr, "Could not allocate %lu bytes for TIFF tile buffer\n", tileSize);
                    goto error;
                }

                for(uint32 y = 0, yrem, yoff; y < h; y += yrem) {
                    yoff = (rect.y + y) % dir.tileHeight;

                    if(((yrem = dir.tileHeight - yoff) + y) > h)
                        yrem = h - y;

                    for(uint32 x = 0, xrem, xoff; x < w; x += xrem) {
                        if(TIFFReadTile(tiff_, buffer, rect.x + x, rect.y + y, 0, 0) == -1)
                            goto error;

                        xoff = (rect.x + x) % dir.tileWidth;
                        start = buffer + (sampOff * ((yoff * dir.tileWidth) + xoff));

                        if(((xrem = dir.tileWidth - xoff) + x) > w)
                            xrem = w - x;

                        for(uint32 ty = 0; ty < yrem; ++ty, start += (sampOff * dir.tileWidth))
                            addScanline(image, x, y + ty, start, sampOff * xrem);
                    }
                }
            }

            if(buffer)
                _TIFFfree(buffer);

            return image;

        error:
            if(buffer)
                _TIFFfree(buffer);

            if(image)
                delete image;

            return nullptr;
        }

        #ifdef USE_GEOTIFF
        DStructGDL* Handler::CreateGeoStruct(tdir_t index) const
        {
            if(!tiff_ || !TIFFSetDirectory(tiff_, index))
                return nullptr;

            DStructFactory gtif;
            TIFF::GeoKey gk;
            int16 nvals;
            double* val;

            // TIFF geo fields
            if(GetField(TIFFTAG_GEOPIXELSCALE, nvals, val))
                gtif.AddArr<DDoubleGDL>("MODELPIXELSCALETAG", nvals, val);
            if(GetField(TIFFTAG_GEOTRANSMATRIX, nvals, val))
                gtif.AddArr<DDoubleGDL>("MODELTRANSFORMATIONTAG", nvals, val);
            if(GetField(TIFFTAG_GEOTIEPOINTS, nvals, val))
                gtif.AddMat<DDoubleGDL>("MODELTIEPOINTTAG", 6, nvals / 6, val);

            // GeoTIFF keys
            if(GetGeoKey(GTModelTypeGeoKey, gk))
                gtif.Add<DIntGDL>("GTMODELTYPEGEOKEY", *gk.value.i);
            if(GetGeoKey(GTModelTypeGeoKey, gk))
                gtif.Add<DIntGDL>("GTRASTERTYPEGEOKEY", *gk.value.i);
            if(GetGeoKey(GTCitationGeoKey, gk))
                gtif.Add<DStringGDL>("GTCITATIONGEOKEY", gk.value.str);
            if(GetGeoKey(GeographicTypeGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGRAPHICTYPEGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogCitationGeoKey, gk))
                gtif.Add<DStringGDL>("GEOGCITATIONGEOKEY", gk.value.str);
            if(GetGeoKey(GeogGeodeticDatumGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGGEODETICDATUMGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogPrimeMeridianGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGPRIMEMERIDIANGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogLinearUnitsGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGLINEARUNITSGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogLinearUnitSizeGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGLINEARUNITSIZEGEOKEY", *gk.value.d);
            if(GetGeoKey(GeogAngularUnitsGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGANGULARUNITSGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogAngularUnitSizeGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGANGULARUNITSIZEGEOKEY", *gk.value.d);
            if(GetGeoKey(GeogEllipsoidGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGELLIPSOIDGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogSemiMajorAxisGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGSEMIMAJORAXISGEOKEY", *gk.value.d);
            if(GetGeoKey(GeogSemiMinorAxisGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGSEMIMINORAXISGEOKEY", *gk.value.d);
            if(GetGeoKey(GeogInvFlatteningGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGINVFLATTENINGGEOKEY", *gk.value.d);
            if(GetGeoKey(GeogAzimuthUnitsGeoKey, gk))
                gtif.Add<DIntGDL>("GEOGAZIMUTHUNITSGEOKEY", *gk.value.i);
            if(GetGeoKey(GeogPrimeMeridianLongGeoKey, gk))
                gtif.Add<DDoubleGDL>("GEOGPRIMEMERIDIANLONGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjectedCSTypeGeoKey, gk))
                gtif.Add<DIntGDL>("PROJECTEDCSTYPEGEOKEY", *gk.value.i);
            if(GetGeoKey(PCSCitationGeoKey, gk))
                gtif.Add<DStringGDL>("PCSCITATIONGEOKEY", gk.value.str);
            if(GetGeoKey(ProjectionGeoKey, gk))
                gtif.Add<DIntGDL>("PROJECTIONGEOKEY", *gk.value.i);
            if(GetGeoKey(ProjCoordTransGeoKey, gk))
                gtif.Add<DIntGDL>("PROJCOORDTRANSGEOKEY", *gk.value.i);
            if(GetGeoKey(ProjLinearUnitsGeoKey, gk))
                gtif.Add<DIntGDL>("PROJLINEARUNITSGEOKEY", *gk.value.i);
            if(GetGeoKey(ProjLinearUnitSizeGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJLINEARUNITSIZEGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjStdParallel1GeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJSTDPARALLEL1GEOKEY", *gk.value.d);
            if(GetGeoKey(ProjStdParallel2GeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJSTDPARALLEL2GEOKEY", *gk.value.d);
            if(GetGeoKey(ProjNatOriginLongGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJNATORIGINLONGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjNatOriginLatGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJNATORIGINLATGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseEastingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSEEASTINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseNorthingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSENORTHINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseOriginLongGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSEORIGINLONGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseOriginLatGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSEORIGINLATGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseOriginEastingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSEORIGINEASTINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjFalseOriginNorthingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJFALSEORIGINNORTHINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjCenterLongGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJCENTERLONGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjCenterEastingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJCENTEREASTINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjCenterNorthingGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJCENTERNORTHINGGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjScaleAtNatOriginGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJSCALEATNATORIGINGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjScaleAtCenterGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJSCALEATCENTERGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjAzimuthAngleGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJAZIMUTHANGLEGEOKEY", *gk.value.d);
            if(GetGeoKey(ProjStraightVertPoleLongGeoKey, gk))
                gtif.Add<DDoubleGDL>("PROJSTRAIGHTVERTPOLELONGGEOKEY", *gk.value.d);
            if(GetGeoKey(VerticalCSTypeGeoKey, gk))
                gtif.Add<DIntGDL>("VERTICALCSTYPEGEOKEY", *gk.value.i);
            if(GetGeoKey(VerticalCitationGeoKey, gk))
                gtif.Add<DStringGDL>("VERTICALCITATIONGEOKEY", gk.value.str);
            if(GetGeoKey(VerticalDatumGeoKey, gk))
                gtif.Add<DIntGDL>("VERTICALDATUMGEOKEY", *gk.value.i);
            if(GetGeoKey(VerticalUnitsGeoKey, gk))
                gtif.Add<DIntGDL>("VERTICALUNITSGEOKEY", *gk.value.i);

            return gtif.Create();
        }

        bool Handler::GetGeoKey(geokey_t key, GeoKey& res) const
        {
            int size;

            if((gtif_ && (res.count = GTIFKeyInfo(gtif_, key, &size, &res.type)))) {
                if(res.value.ptr)
                    free(res.value.ptr);

                if((res.value.ptr = malloc(size * res.count))) {
                    if(GTIFKeyGet(gtif_, key, res.value.ptr, 0, res.count))
                        return true;

                    free(res.value.ptr);
                }
            }

            res = { 0 };
            return false;
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
        e->AssureLongScalarKWIfPresent("IMAGE_INDEX", imageIndex);

        TIFF::Handler tiff;
        TIFF::Directory dir;

        if(tiff.Open(filename.c_str(), "r")) {
            if(!tiff.GetDirectory(imageIndex, dir)) {
                fprintf(stderr, "invalid IMAGE_INDEX: %d\n", imageIndex);
                return new DLongGDL(0);
            }

            DInt hasPalette = (dir.photometric == TIFF::Directory::Photometric::Palette);
            DType pixelType = dir.PixelType();

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

            #ifdef USE_GEOTIFF
            static int gtifIx = e->KeywordIx("GEOTIFF");
            if(e->KeywordPresent(gtifIx)) {
                e->SetKW(gtifIx, tiff.CreateGeoStruct(imageIndex));
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
        e->AssureLongScalarKWIfPresent("IMAGE_INDEX", imageIndex);
        bool verbose = e->KeywordPresent("VERBOSE");

        TIFF::Handler tiff;
        TIFF::Directory dir;
        BaseGDL* image = nullptr;

        if(tiff.Open(filename.c_str(), "r")) {
            if(!tiff.GetDirectory(imageIndex, dir))
                e->Throw("Invalid IMAGE_INDEX value");

            TIFF::Rectangle rect = { 0, 0, dir.width, dir.height };
            TIFF::Interleaving interleave = TIFF::Interleaving::Pixel;
            uint8 channels = 0; // Up to 8 channels supported

            // TODO: Handle three-channel images with color separated planar configuration
            if((dir.planarConfig == TIFF::Directory::PlanarConfig::Separate)
            && (dir.samplesPerPixel == 3)) {
                /* TODO: R, G, B arguments
                Named variables that will contain the Red, Green, and Blue color vectors of the color table from the file if one exists.
                If the TIFF file is written as a three-channel image, interleaved by plane, and the R, G, and B parameters are present,
                the three channels of the image are returned in the R, G, and B variables.

                As a special case, for three-channel TIFF image files that are stored in planar interleave format,
                and if four parameters are provided, READ_TIFF returns the integer value zero,
                and returns three separate images in the variables defined by the R, G, and B arguments.
                */

                return new DLongGDL(0);
            }

            // Include explicitly defined channels if available, otherwise all (up to 8)
            static int channelsIx = e->KeywordIx("CHANNELS");
            if(e->KeywordSet(channelsIx)) {
                auto chanArr = e->GetKW(channelsIx);
                int n = chanArr->N_Elements(), i;

                if(n > 8)
                    e->Throw("Invalid number of elements in CHANNELS array (maximum 8 supported)");

                for(i = 0; i < n; ++i)
                    channels |= (1 << i);
            }
            else channels = 0xFF;

            // Use explicitly defined interleaving if present, otherwise pixel interleaving
            static int interleaveIx = e->KeywordIx("INTERLEAVE");
            if(e->KeywordSet(interleaveIx)) {
                DLong interleaveVal;
                e->AssureLongScalarKW(interleaveIx, interleaveVal);

                if(interleaveVal < 0 || interleaveVal > 2)
                    e->Throw("Invalid INTERLEAVE value (expected 0, 1 or 2)");

                interleave = static_cast<TIFF::Interleaving>(interleaveVal);

                // TODO: Implement support for forced interleaving
                if(interleave != TIFF::Interleaving::Pixel)
                    e->Throw("Unsupported INTERLEAVE value (only 0 is currently implemented)");
            }

            // Use exclicitly defined sub rectangle if present
            static int subRectIx = e->KeywordIx("SUB_RECT");
            if(e->KeywordSet(subRectIx)) {
                auto subRect = static_cast<DULongGDL*>(e->GetKW(subRectIx)->Convert2(GDL_ULONG, BaseGDL::COPY));

                if(subRect->N_Elements() != 4)
                    e->Throw("Invalid number of elements in SUB_RECT array (4 expected)");

                rect.x = (*subRect)[0];
                rect.y = (*subRect)[1];
                rect.w = (*subRect)[2];
                rect.h = (*subRect)[3];

                if((rect.x + rect.w > dir.width) || (rect.y + rect.h > dir.height))
                    e->Throw("Invalid SUB_RECT value exeeds image dimensions");
            }

            if(!(image = tiff.ReadImage(dir, rect, interleave, channels)))
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
                e->SetKW(gtifIx, tiff.CreateGeoStruct(imageIndex));
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
       }

       return image;
   }
}

#endif

