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
#include <wx/msgdlg.h>
#endif

#include "envt.hpp"
#include "dialog.hpp"
#include "file.hpp"
#include "gdlwidget.hpp"

using namespace std;

namespace lib {
#ifdef HAVE_LIBWXWIDGETS
  BaseGDL* wxwidgets_exists(EnvT* e)
  {
    return new DLongGDL(1);
  }

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

    bool isdefault_extension = false;
    bool isdirectory = false;
    bool isdialog_parent = false;
    bool isdisplay_name = false;
    bool isfile = false;
    bool isfilter = false;
    bool isfix_filter = false;
    bool isgroup = false;
    bool ismultiple_files = false;
    bool ismust_exist = false;
    bool isoverwrite_prompt = false;
    bool ispath = false;
    bool isread = false;
    bool iswrite = false;
    bool isresource_name = false;
    bool istitle = false;

    static int default_extensionIx = e->KeywordIx("DEFAULT_EXTENSION"); // Partially implemented. See below for details.
    bool default_extensionKW = e->KeywordPresent(default_extensionIx);
    if (default_extensionKW) isdefault_extension = e->KeywordSet(default_extensionIx);

    static int directoryIx = e->KeywordIx("DIRECTORY");
    bool directoryKW = e->KeywordPresent(directoryIx);
    if (directoryKW) isdirectory = e->KeywordSet(directoryIx);

    static int dialog_parentIx = e->KeywordIx("DIALOG_PARENT"); // Not tested
    bool dialog_parentKW = e->KeywordPresent(dialog_parentIx);
    if (dialog_parentKW) isdialog_parent = e->KeywordSet(dialog_parentIx);

    static int display_nameIx = e->KeywordIx("DISPLAY_NAME"); // Not implemented
    bool display_nameKW = e->KeywordPresent(display_nameIx);
    if (display_nameKW) isdisplay_name = e->KeywordSet(display_nameIx);

    static int fileIx = e->KeywordIx("FILE");
    bool fileKW = e->KeywordPresent(fileIx);
    if (fileKW) isfile = e->KeywordSet(fileIx);

    static int filterIx = e->KeywordIx("FILTER");
    bool filterKW = e->KeywordPresent(filterIx);
    if (filterKW) isfilter = e->KeywordSet(filterIx);

    static int fix_filterIx = e->KeywordIx("FIX_FILTER"); // Not implemented
    bool fix_filterKW = e->KeywordPresent(fix_filterIx);
    if (fix_filterKW) isfix_filter = e->KeywordSet(fix_filterIx);

    static int get_pathIx = e->KeywordIx("GET_PATH");
    bool get_pathKW = e->KeywordPresent(get_pathIx);

    static int groupIx = e->KeywordIx("GROUP"); // Not tested
    bool groupKW = e->KeywordPresent(groupIx);
    if (groupKW) isgroup = e->KeywordSet(groupIx);

    static int multiple_filesIx = e->KeywordIx("MULTIPLE_FILES");
    bool multiple_filesKW = e->KeywordPresent(multiple_filesIx);
    if (multiple_filesKW) ismultiple_files = e->KeywordSet(multiple_filesIx);

    static int must_existIx = e->KeywordIx("MUST_EXIST");
    bool must_existKW = e->KeywordPresent(must_existIx);
    if (must_existKW) ismust_exist = e->KeywordSet(must_existIx);

    static int overwrite_promptIx = e->KeywordIx("OVERWRITE_PROMPT");
    bool overwrite_promptKW = e->KeywordPresent(overwrite_promptIx);
    if (overwrite_promptKW) isoverwrite_prompt = e->KeywordSet(overwrite_promptIx);

    static int pathIx = e->KeywordIx("PATH");
    bool pathKW = e->KeywordPresent(pathIx);
    if (pathKW) ispath = e->KeywordSet(pathIx);

    static int readIx = e->KeywordIx("READ");
    bool readKW = e->KeywordPresent(readIx);
    if (readKW) isread = e->KeywordSet(readIx);

    static int writeIx = e->KeywordIx("WRITE");
    bool writeKW = e->KeywordPresent(writeIx);
    if (writeKW) iswrite = e->KeywordSet(writeIx);

    static int resource_nameIx = e->KeywordIx("RESOURCE_NAME"); // Not implemented
    bool resource_nameKW = e->KeywordPresent(resource_nameIx);
    if (resource_nameKW) isresource_name = e->KeywordSet(resource_nameIx);

    static int titleIx = e->KeywordIx("TITLE");
    bool titleKW = e->KeywordPresent(titleIx);
    if (titleKW) istitle = e->KeywordSet(titleIx);
		
    if (ismultiple_files && isdirectory)
      Warning("DIALOG_PICKFILE: Selecting multiple directories is not supported.");
		
    if (get_pathKW) e->AssureGlobalKW(get_pathIx);

    // Set parent widget.
    // WARNING: THIS PART IS NOT TESTED
    GDLWidget *widget;
    wxWindow *parent;
    if (isgroup || isdialog_parent)
      {
	DLong groupLeader = 0;
	// GROUP == DIALOG_PARENT.
	// However GROUP is deprecated, so we prefer DIALOG_PARENT here.
	e->AssureLongScalarKWIfPresent(groupIx, groupLeader);
	e->AssureLongScalarKWIfPresent(dialog_parentIx, groupLeader);

	widget = GDLWidget::GetWidget(groupLeader);
	if (widget == NULL) ThrowGDLException("Message code is invalid.");
	parent = static_cast<wxWindow *> (widget->GetWxWidget());
      }
    else parent = 0;

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
    wxString wxtitlestr = wxT("Select File");
    if (istitle)
      {
	DString titlestr;
	e->AssureStringScalarKW(titleIx, titlestr);
      }
    else
      {
	if (iswrite)     wxtitlestr = wxT("Select File to Write");
	else if (isread) wxtitlestr = wxT("Select File to Read");
      }
 
    // Set default path, with recent Gnome, default path is "the previous"
    wxString wxpathstr = wxT("");
    DString pathstr;
    if (ispath) {
      e->AssureStringScalarKW(pathIx, pathstr);
    } else {
      pathstr=GetCWD();
    }
    wxpathstr = wxString(pathstr.c_str(), wxConvUTF8);


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
	wxString wxfilterstr;
	if (isfilter) {
	  DStringGDL* filterstrarr = e->IfDefGetKWAs<DStringGDL>(filterIx);
	  dimension dim = filterstrarr->Dim();
	  if (dim.Rank() > 2 || (dim.Rank() == 2 && dim[1] != 2))
	    ThrowGDLException("Filter must be a one dimensional or [N,2] string array.");
	  if (dim.Rank() <= 1)
	    {
	      for (int i = 0; i < filterstrarr->Size(); i++)
		{
		  DString filterstr = (*filterstrarr)[i];
		  if (i != 0) wxfilterstr += wxT("|");
		  wxfilterstr += wxString((filterstr + "|" + filterstr).c_str(), wxConvUTF8);
		}
	    }
	  else // rank == 2
	    {
	      long filtercnt = filterstrarr->Dim(0);
	      for (int i = 0; i < filtercnt; i++)
		{
		  if (i != 0) wxfilterstr += wxT("|");
		  wxfilterstr += wxString(((DString)((*filterstrarr)[i + filtercnt]) + "|" + (DString)((*filterstrarr)[i])).c_str(), wxConvUTF8);
		}
	    }
	}
	else wxfilterstr = wxT("*.*|*.*");

	wxString wxfilestr;
	if (isfile) {
	  DString filestr;
	  e->AssureStringScalarKW(fileIx, filestr);
	  wxfilestr = wxString(filestr.c_str(), wxConvUTF8);
	}

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
	      (*res)[r] = _D(wxpathstrarr[r]);
	  }
	else if (isdirectory) res = new DStringGDL(_D(wxpathstrarr[0]) + PathSeparator());
	else                  res = new DStringGDL(_D(wxpathstrarr[0]));
      }

    // Set the given GET_PATH variable
    if (get_pathKW) {
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
    }

    // Add default extension to the results
    if (isdefault_extension && !isdirectory && pathcnt > 0)
      {
	DString defaultextstr;
	e->AssureStringScalarKW(default_extensionIx, defaultextstr);
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
		(*res)[i] += "." + defaultextstr;
	    }
	  }
      }
    return res;
  }

  BaseGDL* dialog_message_wxwidgets(EnvT* e)
  {
    DStringGDL* messagestr;
    bool iscancel = false;
    bool iscenter = false;
    bool isdefault_cancel = false;
    bool isdefault_no = false;
    bool isdialog_parent = false;
    bool isdisplay_name = false;
    bool iserror = false;
    bool isinformation = false;
    bool isquestion = false;
    bool isresource_name = false;
    bool istitle = false;

    messagestr = e->GetParAs<DStringGDL>(0);

    static int cancelIx = e->KeywordIx("CANCEL");
    bool cancelKW = e->KeywordPresent(cancelIx);
    if (cancelKW) iscancel = e->KeywordSet(cancelIx);

    static int centerIx = e->KeywordIx("CENTER");
    bool centerKW = e->KeywordPresent(centerIx);
    if (centerKW) iscenter = e->KeywordSet(centerIx);

    static int default_cancelIx = e->KeywordIx("DEFAULT_CANCEL");
    bool default_cancelKW = e->KeywordPresent(default_cancelIx);
    if (default_cancelKW) isdefault_cancel = e->KeywordSet(default_cancelIx);

    static int default_noIx = e->KeywordIx("DEFAULT_NO");
    bool default_noKW = e->KeywordPresent(default_noIx);
    if (default_noKW) isdefault_no = e->KeywordSet(default_noIx);

    static int dialog_parentIx = e->KeywordIx("DIALOG_PARENT"); // Not tested
    bool dialog_parentKW = e->KeywordPresent(dialog_parentIx);
    if (dialog_parentKW) isdialog_parent = e->KeywordSet(dialog_parentIx);

    static int display_nameIx = e->KeywordIx("DISPLAY_NAME"); // Not implemented
    bool display_nameKW = e->KeywordPresent(display_nameIx);
    if (display_nameKW) isdisplay_name = e->KeywordSet(display_nameIx);

    static int errorIx = e->KeywordIx("ERROR");
    bool errorKW = e->KeywordPresent(errorIx);
    if (errorKW) iserror = e->KeywordSet(errorIx);

    static int informationIx = e->KeywordIx("INFORMATION");
    bool informationKW = e->KeywordPresent(informationIx);
    if (informationKW) isinformation = e->KeywordSet(informationIx);

    static int questionIx = e->KeywordIx("QUESTION");
    bool questionKW = e->KeywordPresent(questionIx);
    if (questionKW) isquestion = e->KeywordSet(questionIx);

    static int resource_nameIx = e->KeywordIx("RESOURCE_NAME"); // Not implemented
    bool resource_nameKW = e->KeywordPresent(resource_nameIx);
    if (resource_nameKW) isresource_name = e->KeywordSet(resource_nameIx);

    static int titleIx = e->KeywordIx("TITLE");
    bool titleKW = e->KeywordPresent(titleIx);
    if (titleKW) istitle = e->KeywordSet(titleIx);

#ifndef wxCANCEL_DEFAULT
#define wxCANCEL_DEFAULT 0
    if (isdefault_cancel)
      Warning("DIALOG_MESSAGE: Keyword DEFAULT_CANCEL is ignored.");
#endif

    // If two or three styles are specified simultaneously, set INFORMATION as default.
    if ((iserror && isquestion) || (iserror && isinformation) || (isquestion && isinformation))
      {
	iserror = false;
	isquestion = false;
	isinformation = true;
      }

    // Set style
    long style = wxOK|wxSTAY_ON_TOP;
    if (isquestion)                   style = wxYES_NO;
    if (iscancel || isdefault_cancel) style |= wxCANCEL;
    if (iscenter)                     style |= wxCENTRE; // On windows, dialog is always centered with or without this option.
    if (isdefault_cancel)             style |= wxCANCEL_DEFAULT;
    if (isquestion && isdefault_no)   style |= wxNO_DEFAULT;
    if (isquestion)                   style |= wxICON_QUESTION;
    else if (iserror)                 style |= wxICON_ERROR;
    else if (isinformation)           style |= wxICON_INFORMATION;
    else                              style |= wxICON_WARNING; // Default type is 'Warning'

    // Set parent widget.
    // WARNING: THIS PART IS NOT TESTED
    GDLWidget *widget;
    wxWindow *parent;
    if (isdialog_parent)
      {
	DLong groupLeader = 0;
	e->AssureLongScalarKWIfPresent(dialog_parentIx, groupLeader);

	widget = GDLWidget::GetWidget(groupLeader);
	if (widget == NULL) ThrowGDLException("Message code is invalid.");
	parent = static_cast<wxWindow *> (widget->GetWxWidget());
      }
    else parent = 0;

    // Set title
    wxString wxtitlestr;
    if (istitle)
      {
	DString titlestr;
	e->AssureStringScalarKW(titleIx, titlestr);
	wxtitlestr = wxString(titlestr.c_str(), wxConvUTF8);
      }
    else
      {
	if (iserror)            wxtitlestr = wxT("Error");
	else if (isinformation) wxtitlestr = wxT("Information");
	else if (isquestion)    wxtitlestr = wxT("Question");
	else                    wxtitlestr = wxT("Warning");
      }
    // convert eventual array of strings in a string:
    DString local_string;
    for( int i=0; i<messagestr->N_Elements(); ++i) local_string += (*messagestr)[i]+'\n'; 
    // remove last \n
    if (local_string.length() > 1)  local_string.resize(local_string.length()-1);
    // Show dialog
    wxMessageDialog gdlMessageDialog(parent, wxString(local_string.c_str(), wxConvUTF8), wxtitlestr, style);
    int rtn = gdlMessageDialog.ShowModal();
    if (wxID_OK == rtn)          return new DStringGDL("OK");
    else if (wxID_YES == rtn)    return new DStringGDL("Yes");
    else if (wxID_NO == rtn)     return new DStringGDL("No");
    else                         return new DStringGDL("Cancel");
  }
#else
  BaseGDL* wxwidgets_exists(EnvT* e) { return new DLongGDL(0); };
  BaseGDL* dialog_pickfile_wxwidgets(EnvT* e) { ThrowGDLException("wxWidgets is not available!"); return 0; }
  BaseGDL* dialog_message_wxwidgets(EnvT* e) { ThrowGDLException("wxWidgets is not available!"); return 0; }
#endif
}
