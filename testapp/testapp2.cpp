///////////////////////////////////////////////////////////////////////////////
// Name:        testapp.cpp
// Purpose:     wxwebconnect test application
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2007-05-14
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2009, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include <wx/dir.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/printdlg.h>
#include "wx/artprov.h"
#include "wx/aui/aui.h"
#include "../webconnect/webcontrol.h"

// web control and url combobox ids
const int wxID_WEB = 9001;
const int wxID_URL = 9002;

//debug vars
bool m_StartUpdate = false;


class MyFrame : public wxFrame
{
    enum
    {
        ID_About = wxID_HIGHEST+1,
        
        // file
        ID_OpenHref,
        ID_OpenLocation,
        ID_Close,
        ID_SaveAs,
        ID_PageSetup,
        ID_PrintPreview,
        ID_Print,
        ID_Exit,
        
        // edit
        ID_Undo,
        ID_Redo,
        ID_Cut,
        ID_Copy,
        ID_CopyLink,
        ID_Paste,
        ID_SelectAll,
        ID_Find,
        ID_FindAgain,
        
        // view
        ID_GoBack,
        ID_GoForward,
        ID_GoHome,
        ID_Stop,
        ID_Reload,
        ID_ZoomIn,
        ID_ZoomOut,
        ID_ZoomReset,
        ID_ShowSource,
        ID_ShowLinks,
        
        // help
        ID_GoHelp,
        ID_GoForums,
        ID_GoAbout
    };

public:
    MyFrame(wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

    ~MyFrame();

private:

    void OnSize(wxSizeEvent& evt);
    void OnEraseBackground(wxEraseEvent& evt);

    void OnUrlTextEntered(wxCommandEvent& evt);
    void OnUrlItemSelected(wxCommandEvent& evt);    

    void OnOpenHref(wxCommandEvent& evt);
    void OnOpenLocation(wxCommandEvent& evt);
    void OnClose(wxCommandEvent& evt);
    void OnSaveAs(wxCommandEvent& evt);
    void OnPageSetup(wxCommandEvent& evt);
    void OnPrintPreview(wxCommandEvent& evt);
    void OnPrint(wxCommandEvent& evt);
    void OnExit(wxCommandEvent& evt);

    void OnUndo(wxCommandEvent& evt);
    void OnRedo(wxCommandEvent& evt);
    void OnCut(wxCommandEvent& evt);
    void OnCopy(wxCommandEvent& evt);
    void OnCopyLink(wxCommandEvent& evt);
    void OnPaste(wxCommandEvent& evt);
    void OnSelectAll(wxCommandEvent& evt);
    void OnFind(wxCommandEvent& evt);
    void OnFindAgain(wxCommandEvent& evt);

    void OnGoBack(wxCommandEvent& evt);
    void OnGoForward(wxCommandEvent& evt);
    void OnGoHome(wxCommandEvent& evt);
    void OnStop(wxCommandEvent& evt);
    void OnReload(wxCommandEvent& evt);
    void OnZoomIn(wxCommandEvent& evt);
    void OnZoomOut(wxCommandEvent& evt);
    void OnZoomReset(wxCommandEvent& evt);
    void OnShowSource(wxCommandEvent& evt);    
    void OnShowLinks(wxCommandEvent& evt);

    void OnGoHelp(wxCommandEvent& evt);
    void OnGoForums(wxCommandEvent& evt);
    void OnGoAbout(wxCommandEvent& evt);        

    void OnStatusText(wxWebEvent& evt);
    void OnStatusChange(wxWebEvent& evt);
    void OnStateChange(wxWebEvent& evt);
    void OnLocationChange(wxWebEvent& evt);
    void OnTitleChange(wxWebEvent& evt);
    void OnShowContextMenu(wxWebEvent& evt);
    void OnInitDownload(wxWebEvent& evt);
    void OnShouldHandleContent(wxWebEvent& evt);
    void OnDOMContentLoaded(wxWebEvent& evt);

    void OnUpdateUI(wxUpdateUIEvent& evt);

private:

    void SetUrlBarValue(const wxString& str);
    void ShowLinks();
    void GetChildLinks(wxDOMNode node, wxArrayString& arr);

private:

    // frame manager
    wxAuiManager m_mgr;

    // web control and url bar
    wxWebControl* m_browser;
    wxComboBox* m_urlbar;

    // default locations
    wxString m_uri_home;
    wxString m_uri_help;
    wxString m_uri_forums;
    wxString m_uri_about;     

    // href location
    wxString m_uri_href;

    // find text/flags
    wxString m_find_text;
    unsigned int m_find_flags;

    // dom content
    bool m_dom_contentloaded;


    DECLARE_EVENT_TABLE();
};


class MyApp : public wxApp
{
public:

    bool OnInit()
    {
        // Locate the XULRunner engine; the following call will look for 
        // a directory named "xr"
        //wxString xulrunner_path = FindXulRunner(wxT("xr"));
        /*wxString xulrunner_path = wxT("/home/sk/work/mozilla/sdk/xulrunner-31/");
        if (xulrunner_path.IsEmpty())
        {
            wxMessageBox(wxT("Could not find xulrunner directory"));
            return false;
        }*/
    
        // Locate some common paths and initialize the control with
        // the plugin paths; add these common plugin directories to 
        // MOZ_PLUGIN_PATH
        //wxString program_files_dir;
        //::wxGetEnv(wxT("ProgramFiles"), &program_files_dir);
        //if (program_files_dir.Length() == 0 || program_files_dir.Last() != '\\')
        //    program_files_dir += wxT("\\");

        //wxString dir = program_files_dir;
        //dir += wxT("Mozilla Firefox\\plugins");
        //wxWebControl::AddPluginPath(dir);

        // to install the flash plugin automatically, if it exists, 
        // add a path to the flash location; for example, on windows,
        // if the system directory is given by system_dir, then, we have:
        //
        // wxString dir2 = system_dir;
        // dir2 += wxT("Macromed\\Flash");
        // wxWebControl::AddPluginPath(dir2);

        // Finally, initialize the engine; Calling wxWebControl::InitEngine()
        // is very important and has to be made before using wxWebControl.  
        // It instructs wxWebConnect where the xulrunner directory is.
        wxString xulrunner_path =  wxString::FromUTF8("");//empty string for default xulrunner path
        wxWebControl::InitEngine(xulrunner_path);

        // Create and show the frame
        wxFrame* frame1 = new wxFrame(NULL,
                                     wxID_ANY,
                                     wxT("Gecko Embedding Test"),
                                     wxDefaultPosition,
                                     wxSize(1024, 768));

        //wxWindow win1 = this->GetTopWindow();
        //wxWebControl(frame1);

        SetTopWindow(frame1);
        frame1->Show();

/*
        app = wx.App()
        frame = wx.Frame(None, -1, 'Pytho Gecko Test')
        wc = webconnect.wxWebControl(frame)
        wc.OpenURI('www.opennet.ru')
        frame.Show()*/

        return true;
    }
};

DECLARE_APP(MyApp);
IMPLEMENT_APP(MyApp);


