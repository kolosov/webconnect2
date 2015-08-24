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
/*
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

    */

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


//    DECLARE_EVENT_TABLE();
};


MyFrame::MyFrame(wxWindow* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
        : wxFrame(parent, id, title, pos, size, style)
{
    // set frame icon
    #ifdef __WXMSW__
    SetIcon(wxIcon(wxT("mondrian"), wxBITMAP_TYPE_ICO_RESOURCE, 16, 16));
    #endif


    // use wxAuiManager to manage this frame
    m_mgr.SetManagedWindow(this);
/*

    // create the menu
    wxMenuBar* mb = new wxMenuBar;

    wxMenu* file_menu = new wxMenu;
    file_menu->Append(ID_OpenLocation, _("&Open Location...\tCtrl+L"));
    file_menu->Append(ID_Close, _("&Close\tCtrl+W"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_SaveAs, _("&Save As...\tCtrl+S"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_PageSetup, _("Page Set&up..."));
    //file_menu->Append(ID_PrintPreview, _("Print pre&view..."));
    file_menu->Append(ID_Print, _("&Print...\tCtrl+P"));
    file_menu->AppendSeparator();
    file_menu->Append(ID_Exit, _("E&xit"));

    wxMenu* edit_menu = new wxMenu;
    edit_menu->Append(ID_Undo, _("&Undo\tCtrl+Z"));
    edit_menu->Append(ID_Redo, _("&Redo\tCtrl+Y"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ID_Cut, _("Cu&t\tCtrl+X"));
    edit_menu->Append(ID_Copy, _("&Copy\tCtrl+C"));
    edit_menu->Append(ID_Paste, _("&Paste\tCtrl+V"));
    edit_menu->Append(ID_SelectAll, _("Select &All\tCtrl+Z"));
    edit_menu->AppendSeparator();
    edit_menu->Append(ID_Find, _("&Find...\tCtrl+F"));
    edit_menu->Append(ID_FindAgain, _("Find A&gain\tCtrl+G"));

    wxMenu* view_menu = new wxMenu;
    view_menu->Append(ID_GoBack, _("Go &Back"));
    view_menu->Append(ID_GoForward, _("Go &Forward"));
    view_menu->Append(ID_GoHome, _("&Home"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_Stop, _("&Stop\tEsc"));
    view_menu->Append(ID_Reload, _("&Reload\tCtrl+R"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_ZoomIn, _("Zoom &In"));
    view_menu->Append(ID_ZoomOut, _("Zoom &Out"));
    view_menu->Append(ID_ZoomReset, _("Reset &Zoom"));
    view_menu->AppendSeparator();
    view_menu->Append(ID_ShowSource, _("Sho&w Source"));
    view_menu->Append(ID_ShowLinks, _("Show &Links"));

    wxMenu* help_menu = new wxMenu;
    help_menu->Append(ID_GoHelp, _("Help\tF1"));
    help_menu->Append(ID_GoForums, _("Forums"));
    help_menu->Append(ID_GoAbout, _("About"));

    mb->Append(file_menu, _("&File"));
    mb->Append(edit_menu, _("&Edit"));
    mb->Append(view_menu, _("&View"));
    mb->Append(help_menu, _("&Help"));
    SetMenuBar(mb);


    CreateStatusBar();
    GetStatusBar()->SetStatusText(_("Ready"));
*/


    // create the main toolbar
    wxAuiToolBar* toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                             wxAUI_TB_DEFAULT_STYLE);
//    toolbar->SetToolBitmapSize(wxSize(24,24));
//    toolbar->AddTool(ID_GoBack, wxT("Go Back"), wxArtProvider::GetBitmap(wxART_GO_BACK));
//    toolbar->AddTool(ID_GoForward, wxT("Go Forward"), wxArtProvider::GetBitmap(wxART_GO_FORWARD));
//    toolbar->AddTool(ID_GoHome, wxT("Go Home"), wxArtProvider::GetBitmap(wxART_GO_HOME));

    m_urlbar = new wxComboBox(toolbar, wxID_URL, wxT(""), wxPoint(0,0), wxSize(850,18));
    toolbar->AddControl(m_urlbar, wxT("Location"));

    toolbar->Realize();


    // create the main browser control
    m_browser = new wxWebControl(this, wxID_WEB, wxPoint(0,0), wxSize(800,600));

//    m_browser->CreateBrowser(this, wxID_WEB, wxPoint(0,0), wxSize(800,600));

    // add the toolbar to the manager
    m_mgr.AddPane(toolbar, wxAuiPaneInfo().
                  Name(wxT("Toolbar")).Caption(wxT("Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));


    // add the browser to the manager
    m_mgr.AddPane(m_browser, wxAuiPaneInfo().
                  Name(wxT("Browser")).
                  CenterPane().Show());


    // update the wxAUI manager
    m_mgr.Update();


    // set the default browser preferences; to learn
    // more about the Mozilla preference values, see
    // http://kb.mozillazine.org/About:config_entries

    // for the example, disable cookies and popups
    //wxWebPreferences webprefs = wxWebControl::GetPreferences();
    //webprefs.SetIntPref(wxT("network.cookie.cookieBehavior"), 2 /* disable all cookies */);
    //webprefs.SetIntPref(wxT("privacy.popups.policy"), 2 /* reject popups */);

    // example proxy settings
    // webprefs.SetIntPref(wxT("network.proxy.type"),           0 /* no proxy */);
    // webprefs.SetIntPref(wxT("network.proxy.type"),           1 /* manual proxy */);
    // webprefs.SetIntPref(wxT("network.proxy.type"),           4 /* auto-detect proxy */);
    // webprefs.SetStringPref( wxT("network.proxy.http"),       wxT(""));
    // webprefs.SetIntPref(    wxT("network.proxy.http_port"),  0);
    // webprefs.SetStringPref( wxT("network.proxy.ftp"),        wxT(""));
    // webprefs.SetIntPref(    wxT("network.proxy.ftp_port"),   0);
    // webprefs.SetStringPref( wxT("network.proxy.ssl"),        wxT(""));
    // webprefs.SetIntPref(    wxT("network.proxy.ssl_port"),   0);
    // webprefs.SetStringPref( wxT("network.proxy.socks"),      wxT(""));
    // webprefs.SetIntPref(    wxT("network.proxy.socks_port"), 0);

    // example cookie preferences
    // webprefs.SetIntPref(wxT("network.cookie.cookieBehavior"), 0 /* enable all cookies */);
    // webprefs.SetIntPref(wxT("network.cookie.cookieBehavior"), 2 /* disable all cookies */);
    // webprefs.SetIntPref(wxT("network.cookie.lifetime.days"),  0);
    // webprefs.SetIntPref(wxT("network.cookie.lifetimePolicy"), 2 /* accept for session only */);
    // webprefs.SetIntPref(wxT("network.cookie.lifetimePolicy"), 3 /* keep for number of days specified in "privacy.cookie.lifetime_days" */);

    // example popup preferences
    // webprefs.SetIntPref(wxT("privacy.popups.policy"), 1 /* accept popups */);
    // webprefs.SetIntPref(wxT("privacy.popups.policy"), 2 /* reject popups */);


    // set the default home and help URIs
    m_uri_home = wxT("http://www.kirix.com/labs");
    //m_uri_help = wxT("http://www.kirix.com/labs/wxwebconnect/documentation/quick-facts.html");
    //m_uri_forums = wxT("http://www.kirix.com/forums/");
    //m_uri_about = wxT("http://www.kirix.com/labs/wxwebconnect.html");

    // set the DOM content loaded flag
    //m_dom_contentloaded = false;

    // open the home location
    m_browser->OpenURI(m_uri_home);
}


MyFrame::~MyFrame()
{
    m_mgr.UnInit();
}

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

        //wxString xulrunner_path =  wxString::FromUTF8("");//empty string for default xulrunner path
        //wxWebControl::InitEngine(xulrunner_path);

        wxString xulrunner_path =  wxString::FromUTF8("");//empty string for default xulrunner path
        wxWebControl::InitEngine(xulrunner_path);

        // Create and show the frame
        wxFrame* frame = new MyFrame(NULL,
                                     wxID_ANY,
                                     wxT("Gecko Embedding Test"),
                                     wxDefaultPosition,
                                     wxSize(1024, 768));
        SetTopWindow(frame);
        frame->Show();

/*
        wxAuiManager m_mgr;
        // Create and show the frame
        wxFrame* frame1 = new wxFrame(NULL,
                                     wxID_ANY,
                                     wxT("Gecko Embedding Test"),
                                     wxDefaultPosition,
                                     wxSize(1024, 768));

        // create the main toolbar
        wxAuiToolBar* toolbar = new wxAuiToolBar(frame1, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                                 wxAUI_TB_DEFAULT_STYLE);
    //    toolbar->SetToolBitmapSize(wxSize(24,24));
    //    toolbar->AddTool(ID_GoBack, wxT("Go Back"), wxArtProvider::GetBitmap(wxART_GO_BACK));
    //    toolbar->AddTool(ID_GoForward, wxT("Go Forward"), wxArtProvider::GetBitmap(wxART_GO_FORWARD));
    //    toolbar->AddTool(ID_GoHome, wxT("Go Home"), wxArtProvider::GetBitmap(wxART_GO_HOME));

        wxComboBox* m_urlbar;
        m_urlbar = new wxComboBox(toolbar, wxID_URL, wxT(""), wxPoint(0,0), wxSize(850,18));
        toolbar->AddControl(m_urlbar, wxT("Location"));

        toolbar->Realize();


        m_mgr.AddPane(toolbar, wxAuiPaneInfo().
                      Name(wxT("Toolbar")).Caption(wxT("Toolbar")).
                      ToolbarPane().Top().
                      LeftDockable(false).RightDockable(false));



        m_mgr.SetManagedWindow(frame1);

        std::cout << "Creating wxWebControl object" << std::endl;
        wxWebControl *m_browser = new wxWebControl(frame1, wxID_WEB, wxPoint(0,0), wxSize(800,600));

        // add the browser to the manager
        m_mgr.AddPane(m_browser, wxAuiPaneInfo().
	                  Name(wxT("Browser")).
	                  CenterPane().Show());
        m_mgr.Update();
        //wxWindow win1 = this->GetTopWindow();
        //wxWebControl(frame1);

        SetTopWindow(frame1);
        frame1->Show();

        wxString m_uri_home = wxT("http://www.kirix.com/labs");
        m_browser->OpenURI(m_uri_home);
*/
	//m_browser->OpenURI("www.opennet.ru");

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


