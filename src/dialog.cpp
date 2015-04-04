/***************************************************************************
                dialog.cpp  -  GDL library function for dialogs
                             -------------------
    begin                : 04 Apr 2015
    Copyright            : (C) 2015 by Jeongbin Park
    email                : pjb7687@gmail.com

****************************************************************************/

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
#include <wx/filedlg.h>
#endif

#include "envt.hpp"
#include "dialog.hpp"
#include "file.hpp"
#include "gdlwidget.hpp"

using namespace std;

namespace lib {
	BaseGDL* wxwidgets_exists(EnvT* e)
	{
#ifdef HAVE_LIBWXWIDGETS
		return new DLongGDL(1);
#else
		return new DLongGDL(0);
#endif
	}
#ifdef HAVE_LIBWXWIDGETS
	BaseGDL* dialog_pickfile_wxwidgets(EnvT* e)
	{
		/*
			results = DIALOG_PICKFILE_WXWIDGETS(DEFAULT_EXTENSION=default_extension, $
			                                    DIRECTORY=directory, DIALOG_PARENT=dialog_parent, $
			                                    DISPLAY_NAME=display_name, FILE=file, FILTER=filter, $
			                                    FIX_FILTER=fix_filter, GET_PATH=get_path, GROUP=group, $
			                                    MULTIPLE_FILES=multiple_files, MUST_EXIST=must_exist, $
			                                    OVERWRITE_PROMPT=overwrite_prompt, PATH=path, $
			                                    READ=read, WRITE=write, RESOURCE_NAME=resource_name, $
			                                    TITLE=title)
		*/

		bool isdirectory = false;
		bool isfix_filter = false;
		bool ismultiple_files = false;
		bool ismust_exist = false;
		bool isoverwrite_prompt = false;
		bool isread = false;
		bool iswrite = false;

		static int default_extensionIx = e->KeywordIx("DEFAULT_EXTENSION");

		static int directoryIx = e->KeywordIx("DIRECTORY");
		isdirectory = e->KeywordSet(directoryIx);

		static int dialog_parentIx = e->KeywordIx("DIALOG_PARENT");

		static int display_nameIx = e->KeywordIx("DISPLAY_NAME"); // Not implemented yet

		static int fileIx = e->KeywordIx("FILE");

		static int filterIx = e->KeywordIx("FILTER");

		static int fix_filterIx = e->KeywordIx("FIX_FILTER"); // Not implemented yet

		static int get_pathIx = e->KeywordIx("GET_PATH");

		static int groupIx = e->KeywordIx("GROUP");

		static int multiple_filesIx = e->KeywordIx("MULTIPLE_FILES");
		ismultiple_files = e->KeywordSet(multiple_filesIx);

		static int must_existIx = e->KeywordIx("MUST_EXIST");
		ismust_exist = e->KeywordSet(must_existIx);

		static int overwrite_promptIx = e->KeywordIx("OVERWRITE_PROMPT");
		isoverwrite_prompt = e->KeywordSet(overwrite_promptIx);

		static int pathIx = e->KeywordIx("PATH");

		static int readIx = e->KeywordIx("READ");
		isread = e->KeywordSet(readIx);

		static int writeIx = e->KeywordIx("WRITE");
		iswrite = e->KeywordSet(writeIx);

		static int resource_nameIx = e->KeywordIx("RESOURCE_NAME"); // Not implemented yet

		static int titleIx = e->KeywordIx("TITLE");

		if (ismultiple_files && isdirectory)
			Warning("DIALOG_PICKFILE: Selecting multiple directories is not supported.");

		GDLWidget *widget;
		wxWindow *parent;
		DLong groupLeader = 0;
		e->AssureLongScalarKWIfPresent(groupIx, groupLeader);
		e->AssureLongScalarKWIfPresent(dialog_parentIx, groupLeader);
		if (groupLeader > 0) {
			widget = GDLWidget::GetWidget(groupLeader);
			if (widget == NULL)	ThrowGDLException("Message code is invalid.");
			parent = static_cast<wxWindow *> (widget->GetWxWidget());
		}
		else {
			parent = 0;
		}

		// Set style
		long style = 0;
		if (isdirectory)
		{
			style |= wxDD_DEFAULT_STYLE;
			if (ismust_exist)                  style |= wxDD_DIR_MUST_EXIST;
		}
		else
		{
			if (iswrite)                       style |= wxFD_SAVE;
			else                               style |= wxFD_OPEN;
			if (ismust_exist)                  style |= wxFD_FILE_MUST_EXIST;
			if (ismultiple_files)              style |= wxFD_MULTIPLE;
			if (iswrite && isoverwrite_prompt) style |= wxFD_OVERWRITE_PROMPT;
		}

		// Set title
		wxString wxtitlestr = "Select File";
		DString titlestr = "";
		e->AssureStringScalarKWIfPresent(titleIx, titlestr);
		if (!titlestr.empty()) wxtitlestr = titlestr.c_str();
		else if (iswrite)      wxtitlestr = "Select File to Write";
		else if (isread)       wxtitlestr = "Select File to Read";

		// Set default path
		wxString wxpathstr = "";
		DString pathstr = "";
		e->AssureStringScalarKWIfPresent("PATH", pathstr);
		if (!pathstr.empty())  wxpathstr = pathstr.c_str();

		// Show dialog
		wxArrayString wxpathstrarr;
		long pathcnt;
		if (isdirectory)
		{
			// If DIRECTORY is set, show DirDialog
			wxDirDialog gdlDirDialog(parent, wxtitlestr, wxpathstr, style);
			if (gdlDirDialog.ShowModal() != wxID_CANCEL)
			{
				wxpathstr = gdlDirDialog.GetPath();
				wxpathstrarr.push_back(wxpathstr);
				pathcnt = 1;
			}
			else pathcnt = 0;
		}
		else
		{
			// If DIRECTORY is not set, show FileDialog
			DStringGDL* filterstrarr = e->IfDefGetKWAs<DStringGDL>(filterIx);
			wxString wxfilterstr;
			if (filterstrarr)
			{
				dimension dim = filterstrarr->Dim();
				if (dim.Rank() > 2 || (dim.Rank() == 2 && dim[1] != 2))
					ThrowGDLException("Filter must be a one dimensional or [N,2] string array.");
				if (dim.Rank() <= 1)
				{
					for (int i = 0; i < filterstrarr->Size(); i++)
					{
						DString filterstr = (*filterstrarr)[i];
						if (i != 0) wxfilterstr += "|";
						wxfilterstr += (filterstr + "|" + filterstr).c_str();
					}
				}
				else // rank == 2
				{
					long filtercnt = filterstrarr->Dim(0);
					for (int i = 0; i < filtercnt; i++)
					{
						if (i != 0) wxfilterstr += "|";
						wxfilterstr += ((DString)((*filterstrarr)[i + filtercnt]) + "|" + (DString)((*filterstrarr)[i])).c_str();
					}
				}
			}
			else wxfilterstr = "*.*|*.*";

			wxString wxfilestr;
			DString filestr = "";
			e->AssureStringScalarKWIfPresent("FILE", filestr);
			wxfilestr = filestr.c_str();

			wxFileDialog gdlFileDialog(parent, wxtitlestr, wxpathstr, wxfilestr, wxfilterstr, style);
			if (gdlFileDialog.ShowModal() != wxID_CANCEL) {
				if (ismultiple_files)
				{
					gdlFileDialog.GetPaths(wxpathstrarr);
					pathcnt = wxpathstrarr.size();
				}
				else
				{
					wxString wxpath = gdlFileDialog.GetPath();
					wxpathstrarr.push_back(wxpath);
					pathcnt = 1;
				}
			}
			else pathcnt = 0;
		}

		// If MULTIPLE_FILES is set, result is string array of filenames; otherwise result is string scalar.
		// If DIRECTORY is set, result is always string scalar.
		DStringGDL* res;
		if (pathcnt == 0) res = new DStringGDL("");
		else
		{
			if (ismultiple_files && !isdirectory)
			{
				res = new DStringGDL(dimension(pathcnt), BaseGDL::NOZERO);
				for (SizeT r = 0; r < pathcnt; ++r)
					(*res)[r] = wxpathstrarr[r].c_str();
			}
			else if (isdirectory) res = new DStringGDL(DString(wxpathstrarr[0].c_str()) + PathSeparator());
			else                  res = new DStringGDL(DString(wxpathstrarr[0].c_str()));
		}

		// Set the given GET_PATH variable
		if (pathcnt > 0)
		{
			DStringGDL* pathstrgdl;
			if (isdirectory) pathstrgdl = new DStringGDL((*res)[0]);
			else
			{
				static int file_dirnameIx = LibFunIx("FILE_DIRNAME");
				EnvT *newEnv = new EnvT(e, libFunList[file_dirnameIx], NULL);
				Guard<EnvT> guard(newEnv);
				newEnv->SetNextPar(new DStringGDL((*res)[0]));
				pathstrgdl = (DStringGDL *)file_dirname(newEnv);
				guard.release();
				(*pathstrgdl)[0].append(PathSeparator());
			}
			e->SetKW(get_pathIx, pathstrgdl);
		}
		else e->SetKW(get_pathIx, new DStringGDL(""));

		// Add default extension to the results
		if (!isdirectory && pathcnt > 0)
		{
			DString defaultextstr = "";
			e->AssureStringScalarKWIfPresent(default_extensionIx, defaultextstr);
			if (!defaultextstr.empty())
			{
				bool fileexists;
				EnvT *newEnv;

				// TODO: below is not valid when 'write' and 'filter' keys are simultaneously specified.
				//       See http://www.exelisvis.com/docs/DIALOG_PICKFILE.html
				for (int i = 0; i < pathcnt; i++)
				{
					// Check file exists
					ifstream f((*res)[i].c_str());
					fileexists = f.good();
					f.close();

					if (!fileexists) {
						static int file_basenameIx = LibFunIx("FILE_BASENAME");
						newEnv = new EnvT(e, libFunList[file_basenameIx], NULL);
						Guard<EnvT> guard(newEnv);
						newEnv->SetNextPar(new DStringGDL((*res)[i]));
						DStringGDL *basenamestrgdl = (DStringGDL *)file_basename(newEnv);
						guard.release();

						// Add default extention when the file does not have one
						if ((*basenamestrgdl)[0].find(".") == string::npos)
							(*res)[i] += defaultextstr;
					}
				}
			}
		}

		return res;
	}
#endif
}