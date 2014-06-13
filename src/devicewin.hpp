/* *************************************************************************
                          devicewin.hpp  -  M$ windows device
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

#ifndef DEVICEWIN_HPP_
#define DEVICEWIN_HPP_

#include <algorithm>
#include <vector>
#include <cstring>

#include "gdlwinstream.hpp"
#include "initsysvar.hpp"

#ifdef HAVE_OLDPLPLOT
#define SETOPT SetOpt
#else
#define SETOPT setopt
#endif

const int maxWin = 32;

class DeviceWIN : public GraphicsDevice
{
	std::vector<GDLGStream*>	winList;
	std::vector<long>			oList;
	long oIx;
	int  actWin;
	int decomposed;

	void SetActWin(int wIx)
	{
		// update !D
		if (wIx >= 0 && wIx < winList.size())
		{
			// window size and pos
			PLFLT xp; PLFLT yp;
			PLINT xleng; PLINT yleng;
			PLINT xoff; PLINT yoff;
			winList[wIx]->gpage(xp, yp, xleng, yleng, xoff, yoff);
			(*static_cast<DLongGDL*>(dStruct->GetTag(xSTag)))[0] = xleng;
			(*static_cast<DLongGDL*>(dStruct->GetTag(ySTag)))[0] = yleng;
			(*static_cast<DLongGDL*>(dStruct->GetTag(xVSTag)))[0] = xleng;
			(*static_cast<DLongGDL*>(dStruct->GetTag(yVSTag)))[0] = yleng;
		}

		// window number
		(*static_cast<DLongGDL*>(dStruct->GetTag(wTag)))[0] = wIx;

		actWin = wIx;
	}

	// process user deleted windows
	// should be done in a thread
	void ProcessDeleted()
	{
		int wLSize = winList.size();

		//     bool redo;
		//     do { // it seems that the event queue is only searched a few events deep
		//       redo = false;
		for (int i = 0; i<wLSize; i++)
		{
			if (winList[i] != NULL && !winList[i]->GetValid())
			{
				delete winList[i];
				winList[i] = NULL;
				oList[i] = 0;
				// 	    redo = true;
			}
			//     } while( redo);
		}

		// set to most recently created
		std::vector< long>::iterator mEl =
			std::max_element(oList.begin(), oList.end());

		// no window open
		if (*mEl == 0)
		{
			SetActWin(-1);
			oIx = 1;
		}
		else
			SetActWin(std::distance(oList.begin(), mEl));
	}

public:
	DeviceWIN() : GraphicsDevice(), oIx(1), actWin(-1), decomposed(-1)
	{
		name = "WIN";

		DLongGDL origin(dimension(2));
		DLongGDL zoom(dimension(2));
		zoom[0] = 1;
		zoom[1] = 1;

		dStruct = new DStructGDL("!DEVICE");
		dStruct->InitTag("NAME", DStringGDL(name));
		dStruct->InitTag("X_SIZE", DLongGDL(640));
		dStruct->InitTag("Y_SIZE", DLongGDL(512));
		dStruct->InitTag("X_VSIZE", DLongGDL(640));
		dStruct->InitTag("Y_VSIZE", DLongGDL(512));
		dStruct->InitTag("X_CH_SIZE", DLongGDL(9));
		dStruct->InitTag("Y_CH_SIZE", DLongGDL(12));
		dStruct->InitTag("X_PX_CM", DFloatGDL(40.0));
		dStruct->InitTag("Y_PX_CM", DFloatGDL(40.0));
		dStruct->InitTag("N_COLORS", DLongGDL(256));
		dStruct->InitTag("TABLE_SIZE", DLongGDL(ctSize));
		dStruct->InitTag("FILL_DIST", DLongGDL(0));
		dStruct->InitTag("WINDOW", DLongGDL(-1));
		dStruct->InitTag("UNIT", DLongGDL(0));
		dStruct->InitTag("FLAGS", DLongGDL(328124));
		dStruct->InitTag("ORIGIN", origin);
		dStruct->InitTag("ZOOM", zoom);

		winList.resize(maxWin);
		for (int i = 0; i < maxWin; i++) winList[i] = NULL;
		oList.resize(maxWin);
		for (int i = 0; i < maxWin; i++) oList[i] = 0;

		//GDLGStream::SetErrorHandlers();
	}

	~DeviceWIN()
	{
		std::vector<GDLGStream*>::iterator i;
		for (i = winList.begin(); i != winList.end(); ++i)
		{
			delete *i; *i = NULL;
		}
	}

	void EventHandler()
	{
		int wLSize = winList.size();
		for (int i = 0; i<wLSize; i++)
		if (winList[i] != NULL)
			winList[i]->EventHandler();

		ProcessDeleted();
	}

	bool WDelete(int wIx)
	{
		ProcessDeleted();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
			return false;

		delete winList[wIx];
		winList[wIx] = NULL;
		oList[wIx] = 0;

		// set to most recently created
		std::vector< long>::iterator mEl =
			std::max_element(oList.begin(), oList.end());

		// no window open
		if (*mEl == 0)
		{
			SetActWin(-1);
			oIx = 1;
		}
		else
			SetActWin(std::distance(oList.begin(), mEl));

		return true;
	}

	bool WOpen(int wIx, const std::string& title,
		int xSize, int ySize, int xPos, int yPos)
	{
		ProcessDeleted();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0)
			return false;

		if (winList[wIx] != NULL)
			delete winList[wIx];

		DLongGDL* pMulti = SysVar::GetPMulti();
		DLong nx = (*pMulti)[1];
		DLong ny = (*pMulti)[2];

		if (nx <= 0) nx = 1;
		if (ny <= 0) ny = 1;

		winList[wIx] = new GDLWINStream(nx, ny);
		oList[wIx] = oIx++;

		// set initial window size
		PLFLT xp; PLFLT yp;
		PLINT xleng; PLINT yleng;
		PLINT xoff; PLINT yoff;
		winList[wIx]->gpage(xp, yp, xleng, yleng, xoff, yoff);

		xleng = xSize;
		yleng = ySize;
		xoff = xPos;
		yoff = yPos;

		winList[wIx]->spage(xp, yp, xleng, yleng, xoff, yoff);

		// no pause on win destruction
		winList[wIx]->spause(false);

		// extended fonts
		winList[wIx]->fontld(1);

		// we want color
		winList[wIx]->scolor(1);

		// window title
		static char buf[256];
		strncpy(buf, title.c_str(), 255);
		buf[255] = 0;
		winList[wIx]->SETOPT("plwindow", buf);

		// we want color (and the driver options need to be overwritten)
		// winList[ wIx]->SETOPT( "drvopt","color=1");

		// set color map
		PLINT r[ctSize], g[ctSize], b[ctSize];
		actCT.Get(r, g, b);
		//    winList[ wIx]->scmap0( r, g, b, ctSize); 
		winList[wIx]->scmap1(r, g, b, ctSize);

		winList[wIx]->Init();
		// need to be called initially. permit to fix things
		winList[wIx]->ssub(1, 1);
		winList[wIx]->adv(0);
		// load font
		winList[wIx]->font(1);
		winList[wIx]->vpor(0, 1, 0, 1);
		winList[wIx]->wind(0, 1, 0, 1);
		//    winList[ wIx]->DefaultCharSize();
		//in case these are not initalized, here is a good place to do it.
		if (winList[wIx]->updatePageInfo() == true)
		{
			winList[wIx]->GetPlplotDefaultCharSize(); //initializes everything in fact..

		}
		// sets actWin and updates !D
		SetActWin(wIx);

		return true; //winList[ wIx]->Valid(); // Valid() need to called once
	}

	bool WSet(int wIx)
	{
		ProcessDeleted();

		int wLSize = winList.size();
		if (wIx >= wLSize || wIx < 0 || winList[wIx] == NULL)
			return false;

		SetActWin(wIx);
		return true;
	}

	int WAdd()
	{
		ProcessDeleted();

		int wLSize = winList.size();
		for (int i = maxWin; i<wLSize; i++)
		if (winList[i] == NULL) return i;

		// plplot allows only 101 windows
		if (wLSize == 101) return -1;

		winList.push_back(NULL);
		oList.push_back(0);
		return wLSize;
	}

	// should check for valid streams
	GDLGStream* GetStream(bool open = true)
	{
		ProcessDeleted();
		if (actWin == -1)
		{
			if (!open) return NULL;

			DString title = "GDL 0";
			DLong xSize, ySize;
			//DefaultXYSize(&xSize, &ySize);
			xSize = 640; ySize = 480;
			bool success = WOpen(0, title, xSize, ySize, 0, 0);
			if (!success)
				return NULL;
			if (actWin == -1)
			{
				std::cerr << "Internal error: plstream not set." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		return winList[actWin];
	}

	void plimage_gdl(PLStream* pls, unsigned char *idata, PLINT nx, PLINT ny, DLong tru, DLong chan)
	{
		PLINT ix, iy;
		wingcc_Dev *dev = (wingcc_Dev *)pls->dev;

		int x, y;

		//the following 2 tests cannot happen i think. I keep them for safety.
		if (pls->level < 3) {
			std::cerr << "plimage: window must be set up first" << std::endl; //plabort() not available anymore!
			return;
		}

		if (nx <= 0 || ny <= 0) {
			std::cerr << "plimage: nx and ny must be positive" << std::endl;
			return;
		}

		//oldErrorHandler = XSetErrorHandler(GetImageErrorHandler);
		HDC hdc = dev->hdc;

		PLINT xoff = (PLINT)(pls->wpxoff / 32767 * dev->width + 1);
		PLINT yoff = (PLINT)(pls->wpyoff / 24575 * dev->height + 1);
		PLINT kx, ky;

		PLINT kxLimit = dev->width - xoff;
		PLINT kyLimit = dev->height - yoff;

		if (nx < kxLimit) kxLimit = nx;
		if (ny < kyLimit) kyLimit = ny;

		char iclr1, ired, igrn, iblu;
		long curcolor;

		for (ix = 0; ix < kxLimit; ++ix) {
			for (iy = 0; iy < kyLimit; ++iy) {

				kx = xoff + ix;
				ky = yoff + iy;

				if (tru == 0 && chan == 0) {
					iclr1 = idata[iy*nx + ix];
					curcolor = RGB(pls->cmap1[iclr1].r, pls->cmap1[iclr1].g, pls->cmap1[iclr1].b);
					//	  printf("ix: %d  iy: %d  pixel: %d\n", ix,iy,curcolor.pixel);

				}
				else {
					if (chan == 0) {
						if (tru == 1) {
							ired = idata[3 * (iy*nx + ix) + 0];
							igrn = idata[3 * (iy*nx + ix) + 1];
							iblu = idata[3 * (iy*nx + ix) + 2];
						}
						else if (tru == 2) {
							ired = idata[nx*(iy * 3 + 0) + ix];
							igrn = idata[nx*(iy * 3 + 1) + ix];
							iblu = idata[nx*(iy * 3 + 2) + ix];
						}
						else if (tru == 3) {
							ired = idata[nx*(0 * ny + iy) + ix];
							igrn = idata[nx*(1 * ny + iy) + ix];
							iblu = idata[nx*(2 * ny + iy) + ix];
						}
						curcolor = RGB(ired, igrn, iblu); // It is reverse!
					}
					else if (chan == 1) {
						unsigned long pixel =
							GetPixel(hdc, ix, dev->height - 1 - ky) & 0xffff00;
						ired = idata[1 * (iy*nx + ix) + 0];
						curcolor = RGB(ired, 0, 0) + pixel;
						//curcolor = ired * 256 * 256 + pixel;
					}
					else if (chan == 2) {
						unsigned long pixel =
							GetPixel(hdc, ix, dev->height - 1 - ky) & 0xff00ff;
						igrn = idata[1 * (iy*nx + ix) + 1];
						curcolor = RGB(0, igrn, 0) + pixel;
						//curcolor = igrn * 256 + pixel;
					}
					else if (chan == 3) {
						unsigned long pixel =
							GetPixel(hdc, ix, dev->height - 1 - ky) & 0x00ffff;
						iblu = idata[1 * (iy*nx + ix) + 2];
						curcolor = RGB(0, 0, iblu) + pixel;
						//curcolor = iblu + pixel;
					}
				}

				if (ky < dev->height && kx < dev->width)
					SetPixel(hdc, kx, dev->height - 1 - ky, curcolor);
			}
		}

		return;
	}


	void TV(EnvT* e)
	{
		//    Graphics* actDevice = Graphics::GetDevice();

		SizeT nParam = e->NParam(1);
		PLStream* pls;

		GDLGStream* actStream = GetStream();
		if (actStream == NULL)
		{
			std::cerr << "TV: Internal error: plstream not set." << std::endl;
			exit(EXIT_FAILURE);
		}

		//    actStream->NextPlot( false);
		actStream->NoSub();
		plgpls(&pls);
		wingcc_Dev *dev = (wingcc_Dev *)pls->dev;

		int xSize, ySize, xPos, yPos;
		int actWin = ActWin();
		bool success = WSize(actWin, &xSize, &ySize, &xPos, &yPos);

		BaseGDL* p0 = e->GetParDefined(0);
		SizeT rank = p0->Rank();

		DLong tru = 0;

		e->AssureLongScalarKWIfPresent("TRUE", tru);
		if (rank < 1 || rank > 3) e->Throw("Image array must have rank 1, 2 or 3");
		if (rank <= 2 && tru != 0) e->Throw("Array must have 3 dimensions: " + e->GetParString(0));
		if (tru < 0 || tru > 3) e->Throw("Value of TRUE keyword is out of allowed range.");
		if (tru == 1 && !dev->truecolour) e->Throw("Device depth must be 24 or greater for true color display");

		DLong orderVal = SysVar::TV_ORDER();
		e->AssureLongScalarKWIfPresent("ORDER", orderVal);

		DByteGDL* p0B;
		Guard<BaseGDL> guardP0B;
		int width, height;
		if (rank == 1) {
			p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
			if (orderVal != 0) { p0B->Reverse(0); }
			width = p0B->Dim(0);
			height = 1;
		}
		else if (rank == 2) {
			p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
			if (orderVal != 0) { p0B->Reverse(1); }
			width = p0B->Dim(0);
			height = p0B->Dim(1);
		}
		else if (rank == 3) {
			if (tru == 1) {
				p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
				if (p0B->Dim(0) < 3) e->Throw("Array <BYTE     Array[" + i2s(p0B->Dim(0)) + "," +
					i2s(p0B->Dim(1)) + "," + i2s(p0B->Dim(2))
					+ "]> does not have enough elements.");
				if (orderVal != 0) { p0B->Reverse(2); }
				width = p0B->Dim(1);
				height = p0B->Dim(2);
			}
			if (tru == 2) {
				p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
				if (p0B->Dim(1) < 3) e->Throw("Array <BYTE     Array[" + i2s(p0B->Dim(0)) + "," +
					i2s(p0B->Dim(1)) + "," + i2s(p0B->Dim(2))
					+ "]> does not have enough elements.");
				if (orderVal != 0) { p0B->Reverse(2); }
				width = p0B->Dim(0);
				height = p0B->Dim(2);
			}
			if (tru == 3) {
				p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
				if (p0B->Dim(2) < 3) e->Throw("Array <BYTE     Array[" + i2s(p0B->Dim(0)) + "," +
					i2s(p0B->Dim(1)) + "," + i2s(p0B->Dim(2))
					+ "]> does not have enough elements.");
				if (orderVal != 0) { p0B->Reverse(1); }
				width = p0B->Dim(0);
				height = p0B->Dim(1);
			}
			if (tru == 0) {  // here we have a rank =3
				p0B = static_cast<DByteGDL*>(p0->Convert2(GDL_BYTE, BaseGDL::COPY));     guardP0B.Init(p0B);
				if (p0B->Dim(0) == 1) {
					width = p0B->Dim(1);
					height = p0B->Dim(2);
					if (orderVal != 0) { p0B->Reverse(2); }
				}
				else if (p0B->Dim(1) == 1) {
					width = p0B->Dim(0);
					height = p0B->Dim(2);
					if (orderVal != 0) { p0B->Reverse(2); }
				}
				else {
					width = p0B->Dim(0);
					height = p0B->Dim(1);
					if (orderVal != 0) { p0B->Reverse(1); }
				}
			}
		}
		int debug = 0;
		if (debug == 1) {
			std::cout << "==================== " << std::endl;
			std::cout << "true " << tru << std::endl;
			std::cout << "Rank " << rank << std::endl;
			std::cout << "width " << width << std::endl;
			std::cout << "height " << height << std::endl;
			std::cout << "nParam " << nParam << std::endl;
		}

		DLong xLL = 0, yLL = 0, pos = 0;
		if (nParam == 2) {
			int nx, ny, ix, iy;
			e->AssureLongScalarPar(1, pos);
			nx = xSize / width;
			ny = ySize / height;
			// AC 2011/11/06, bug 3433502
			if (nx > 0) { ix = pos % nx; }
			else ix = 0;
			if (ny > 0) { iy = (pos / nx) % ny; }
			else iy = 0;
			xLL = width*ix;
			yLL = ySize - height*(iy + 1);
		}
		else if (nParam >= 3) {
			if (e->KeywordSet(1)) { // NORMAL
				DDouble xLLf, yLLf;
				e->AssureDoubleScalarPar(1, xLLf);
				e->AssureDoubleScalarPar(2, yLLf);
				xLL = (DLong)rint(xLLf * xSize);
				yLL = (DLong)rint(yLLf * ySize);
			}
			else {
				e->AssureLongScalarPar(1, xLL);
				e->AssureLongScalarPar(2, yLL);
			}
		}

		actStream->vpor(0, 1.0, 0, 1.0);
		actStream->wind(1 - xLL, xSize - xLL, 1 - yLL, ySize - yLL);

		if (debug == 1) {
			std::cout << "xLL :" << xLL << std::endl;
			std::cout << "yLL :" << yLL << std::endl;
			std::cout << "xSize :" << xSize << std::endl;
			std::cout << "ySize :" << ySize << std::endl;
		}
		DLong channel = 0;
		e->AssureLongScalarKWIfPresent("CHANNEL", channel);
		if (channel < 0 || channel > 3)
			e->Throw("Value of Channel is out of allowed range.");

		Guard<BaseGDL> chan_guard;
		if (channel == 0) {
			plimage_gdl(pls, &(*p0B)[0], width, height, tru, channel);
		}
		else if (rank == 3) {
			// Rank == 3 w/channel
			SizeT dims[2];
			dims[0] = width;
			dims[1] = height;
			dimension dim(dims, 2);
			DByteGDL* p0B_chan = new DByteGDL(dim, BaseGDL::ZERO);
			for (SizeT i = (channel - 1); i<p0B->N_Elements(); i += 3) {
				(*p0B_chan)[i / 3] = (*p0B)[i];
			}
			// Send just single channel
			plimage_gdl(pls, &(*p0B_chan)[0], width, height, tru, channel);
			chan_guard.Init(p0B_chan); // delete upon exit
		}
		else if (rank == 2) {
			// Rank = 2 w/channel
			plimage_gdl(pls, &(*p0B)[0], width, height, tru, channel);
		}
	}

	int MaxWin() { ProcessDeleted(); return maxWin; }
	int ActWin() { ProcessDeleted(); return actWin; }

	bool Decomposed(bool value)
	{
		decomposed = value;
		return true;
	}



};

#endif
