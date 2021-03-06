///////////////////////////////////////////////////////////////////////////////
// Name:        promptservice.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2006-10-07
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
#include "mozilla/Char16.h"
#endif

#include <string>
#include <wx/wx.h>
#include <wx/artprov.h>
#include "webcontrol.h"



//#include "nsinclude.h"


#include "nsutils.h"
#include "promptservice.h"

#include "nsCOMPtr.h"
#include "nsIWebBrowserChrome.h"
#include "nsIDOMWindow.h"
#include "nsIWindowWatcher.h"
#include "nsIPromptService2.h"
#include "nsIBadCertListener2.h"

#include "nsIAuthInformation.h"
#include "nsIURIFixup.h"
#include "nsIHelperAppLauncherDialog.h"
#include "nsIExternalHelperAppService.h"
#include "nsIURI.h"
#include "nsIMIMEInfo.h"
#include "nsIFile.h"
#include "nsICertOverrideService.h"

#include "nsIServiceManager.h"
#include "nsServiceManagerUtils.h"
#include "nsIInterfaceRequestorUtils.h"




///////////////////////////////////////////////////////////////////////////////
//  utilities
///////////////////////////////////////////////////////////////////////////////


wxWebControl* GetWebControlFromBrowserChrome(nsIWebBrowserChrome* chrome);

wxWebControl* GetWebControlFromDOMWindow(nsIDOMWindow* window)
{
    nsCOMPtr<nsIWindowWatcher> window_watcher = nsGetWindowWatcherService();
    nsCOMPtr<nsIWebBrowserChrome> chrome;
    
    if (window == NULL || window_watcher == NULL)//FIXME?
    {
        // we don't have either a dom window pointer or
        // access to the window watcher service.  return error
        return NULL;
    }
    
    window_watcher->GetChromeForWindow(window, getter_AddRefs(chrome));
    
    return GetWebControlFromBrowserChrome(chrome);
}

wxWindow* GetTopFrameFromDOMWindow(nsIDOMWindow* window)
{
    wxWindow* win = GetWebControlFromDOMWindow(window);
    if (!win)
        return NULL;
        
    // now that we have a window, go up the window
    // hierarchy to find a frame
    
    wxWindow* w = win;
    while (1)
    {
        if (w->IsKindOf(CLASSINFO(wxFrame)))
            return w;
        
        wxWindow* old_win = w;
        w = w->GetParent();
        if (!w)
            return old_win;
    }
    
    return win;
}




///////////////////////////////////////////////////////////////////////////////
//  various dialogs
///////////////////////////////////////////////////////////////////////////////


class PromptDlgPassword : public wxDialog
{

    enum
    {
        ID_UsernameTextCtrl = wxID_HIGHEST+1,
        ID_PasswordTextCtrl
    };
    
public:

    PromptDlgPassword(wxWindow* parent)
                         : wxDialog(parent,
                                    -1,
                                    _("Authentication Required"),
                                    wxDefaultPosition,
                                    wxSize(400, 200),
                                    wxDEFAULT_DIALOG_STYLE |
                                    wxCENTER)
    {
        // create the username sizer
        
        wxStaticText* label_username = new wxStaticText(this,
                                                        -1,
                                                        _("User Name:"),
                                                        wxDefaultPosition,
                                                        wxDefaultSize);
        m_username_ctrl = new wxTextCtrl(this, ID_UsernameTextCtrl, m_username);
        
        wxBoxSizer* username_sizer = new wxBoxSizer(wxHORIZONTAL);
        username_sizer->Add(label_username, 0, wxALIGN_CENTER);
        username_sizer->Add(m_username_ctrl, 1, wxEXPAND);
        
        
        // create the password sizer
        
        wxStaticText* label_password = new wxStaticText(this,
                                                        -1,
                                                        _("Password:"),
                                                        wxDefaultPosition,
                                                        wxDefaultSize);
        m_password_ctrl = new wxTextCtrl(this,
                                         ID_PasswordTextCtrl,
                                         wxEmptyString,
                                         wxDefaultPosition,
                                         wxDefaultSize,
                                         wxTE_PASSWORD);
        
        wxBoxSizer* password_sizer = new wxBoxSizer(wxHORIZONTAL);
        password_sizer->Add(label_password, 0, wxALIGN_CENTER);
        password_sizer->Add(m_password_ctrl, 1, wxEXPAND);


        // create a platform standards-compliant OK/Cancel sizer
        
        wxButton* ok_button = new wxButton(this, wxID_OK);
        wxButton* cancel_button = new wxButton(this, wxID_CANCEL);
        
        wxStdDialogButtonSizer* ok_cancel_sizer = new wxStdDialogButtonSizer;
        ok_cancel_sizer->AddButton(ok_button);
        ok_cancel_sizer->AddButton(cancel_button);
        ok_cancel_sizer->Realize();
        ok_cancel_sizer->AddSpacer(5);
        
        ok_button->SetDefault();
        
        // this code is necessary to get the sizer's bottom margin to 8
        wxSize min_size = ok_cancel_sizer->GetMinSize();
        min_size.SetHeight(min_size.GetHeight()+16);
        ok_cancel_sizer->SetMinSize(min_size);
        
        
        // code to allow us to line up the static text elements
        wxSize s1 = label_username->GetSize();
        wxSize s2 = label_password->GetSize();
        wxSize max_size = wxSize(wxMax(s1.x, s2.x), wxMax(s1.y, s2.y));
        max_size.x += 10;
        username_sizer->SetItemMinSize(label_username, max_size);
        password_sizer->SetItemMinSize(label_password, max_size);


        // create username/password sizer
        
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_MESSAGE_BOX);
        wxStaticBitmap* bitmap_question = new wxStaticBitmap(this, -1, bmp);
        m_message_ctrl = new wxStaticText(this, -1, m_message);
        
        wxBoxSizer* vert_sizer = new wxBoxSizer(wxVERTICAL);
        vert_sizer->Add(m_message_ctrl, 0, wxEXPAND);
        vert_sizer->AddSpacer(16);
        vert_sizer->Add(username_sizer, 0, wxEXPAND);
        vert_sizer->AddSpacer(8);
        vert_sizer->Add(password_sizer, 0, wxEXPAND);
        
        // create top sizer
        
        wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
        top_sizer->AddSpacer(7);
        top_sizer->Add(bitmap_question, 0, wxTOP, 7);
        top_sizer->AddSpacer(15);
        top_sizer->Add(vert_sizer, 1, wxEXPAND | wxTOP, 7);

        // create main sizer
        
        wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
        main_sizer->AddSpacer(8);
        main_sizer->Add(top_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
        main_sizer->AddStretchSpacer();
        main_sizer->Add(ok_cancel_sizer, 0, wxEXPAND);

        SetSizer(main_sizer);
        Layout();
    }
    
    ~PromptDlgPassword()
    {
        // clear out password in memory
        m_password = wxT("            ");
    }

    wxString GetUserName() const
    {
        return m_username;
    }
    
    wxString GetPassword() const
    {
        return m_password;
    }
    
    void SetMessage(const wxString& message)
    {
        m_message = message;
        m_message_ctrl->SetLabel(m_message);
        wxSizer* sizer = m_message_ctrl->GetContainingSizer();
        m_message_ctrl->Wrap(sizer->GetSize().GetWidth());
        Layout();
    }
    
    void SetUserName(const wxString& username)
    {
        m_username = username;
    }
    
    
private:

    // event handlers
    
    void OnOK(wxCommandEvent& evt)
    {
        m_username = m_username_ctrl->GetValue();
        m_password = m_password_ctrl->GetValue();
        
        EndModal(wxID_OK);
    }
    
    void OnCancel(wxCommandEvent& evt)
    {
        m_username = wxT("");
        m_password = wxT("");
        
        EndModal(wxID_CANCEL);
    }

private:
    
    wxString m_message;
    wxString m_username;
    wxString m_password;
    
    wxStaticText* m_message_ctrl;
    wxTextCtrl* m_username_ctrl;
    wxTextCtrl* m_password_ctrl;
    
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(PromptDlgPassword, wxDialog)
    EVT_BUTTON(wxID_OK, PromptDlgPassword::OnOK)
    EVT_BUTTON(wxID_CANCEL, PromptDlgPassword::OnCancel)
END_EVENT_TABLE()


/*
class PromptDlgBadCert : public wxDialog
{

    enum
    {
        ID_UsernameTextCtrl = wxID_HIGHEST+1,
        ID_PasswordTextCtrl
    };
    
public:

    PromptDlgBadCert(wxWindow* parent)
                         : wxDialog(parent,
                                    -1,
                                    _("Authentication Required"),
                                    wxDefaultPosition,
                                    wxSize(400, 200),
                                    wxDEFAULT_DIALOG_STYLE |
                                    wxCENTER)
    {
        // create a platform standards-compliant OK/Cancel sizer
        
        wxButton* yes_button = new wxButton(this, wxID_YES);
        wxButton* no_button = new wxButton(this, wxID_NO);
        
        wxStdDialogButtonSizer* yes_no_sizer = new wxStdDialogButtonSizer;
        yes_no_sizer->AddButton(yes_button);
        yes_no_sizer->AddButton(no_button);
        yes_no_sizer->Realize();
        yes_no_sizer->AddSpacer(5);
        
        no_button->SetDefault();
        
        // this code is necessary to get the sizer's bottom margin to 8
        wxSize min_size = yes_no_sizer->GetMinSize();
        min_size.SetHeight(min_size.GetHeight()+16);
        yes_no_sizer->SetMinSize(min_size);
        
        
        // create username/password sizer
        
        wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_MESSAGE_BOX);
        wxStaticBitmap* bitmap_question = new wxStaticBitmap(this, -1, bmp);
        m_message_ctrl = new wxStaticText(this, -1, m_message);
        
        wxBoxSizer* vert_sizer = new wxBoxSizer(wxVERTICAL);
        vert_sizer->Add(m_message_ctrl, 0, wxEXPAND);
        vert_sizer->AddSpacer(16);

        // create top sizer
        
        wxBoxSizer* top_sizer = new wxBoxSizer(wxHORIZONTAL);
        top_sizer->AddSpacer(7);
        top_sizer->Add(bitmap_question, 0, wxTOP, 7);
        top_sizer->AddSpacer(15);
        top_sizer->Add(vert_sizer, 1, wxEXPAND | wxTOP, 7);

        // create main sizer
        
        wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
        main_sizer->AddSpacer(8);
        main_sizer->Add(top_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);
        main_sizer->AddStretchSpacer();
        main_sizer->Add(yes_no_sizer, 0, wxEXPAND);

        SetSizer(main_sizer);
        Layout();
    }
    
    ~PromptDlgBadCert()
    {
    }

    void SetMessage(const wxString& message)
    {
        m_message = message;
        m_message_ctrl->SetLabel(m_message);
        wxSizer* sizer = m_message_ctrl->GetContainingSizer();
        m_message_ctrl->Wrap(sizer->GetSize().GetWidth());
        Layout();
    }

private:

    // event handlers
    
    void OnYes(wxCommandEvent& evt)
    {
        EndModal(wxID_YES);
    }
    
    void OnNo(wxCommandEvent& evt)
    {
        EndModal(wxID_NO);
    }

private:
    
    wxString m_message;
    
    wxStaticText* m_message_ctrl;
    wxCheckBox* m_username_ctrl;
    
    DECLARE_EVENT_TABLE()
};


BEGIN_EVENT_TABLE(PromptDlgBadCert, wxDialog)
    EVT_BUTTON(wxID_YES, PromptDlgBadCert::OnYes)
    EVT_BUTTON(wxID_NO, PromptDlgBadCert::OnNo)
END_EVENT_TABLE()
*/


///////////////////////////////////////////////////////////////////////////////
//  PromptService class implementation
///////////////////////////////////////////////////////////////////////////////

class PromptService : public nsIPromptService2,
#if MOZILLA_VERSION_1 < 1
                      public nsIBadCertListener,
#endif
                      public nsIBadCertListener2
{
public:

    PromptService();

    void onBadCertificate(const wxString& message, nsIDOMWindow* dom_window);
    
protected:
    virtual ~PromptService();

    NS_DECL_ISUPPORTS
    NS_DECL_NSIPROMPTSERVICE
    NS_DECL_NSIPROMPTSERVICE2
#if MOZILLA_VERSION_1 < 1
    NS_DECL_NSIBADCERTLISTENER
#endif
    NS_DECL_NSIBADCERTLISTENER2
};


NS_IMPL_ADDREF(PromptService)
NS_IMPL_RELEASE(PromptService)

NS_INTERFACE_MAP_BEGIN(PromptService)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIPromptService)
    NS_INTERFACE_MAP_ENTRY(nsIPromptService)
    NS_INTERFACE_MAP_ENTRY(nsIPromptService2)
#if MOZILLA_VERSION_1 < 1
    NS_INTERFACE_MAP_ENTRY(nsIBadCertListener)
#endif
NS_INTERFACE_MAP_END


PromptService::PromptService()
{
}

PromptService::~PromptService()
{
}

NS_IMETHODIMP PromptService::Alert(nsIDOMWindow* parent,
                                   const char16_t* ns_dialog_title,
                                   const char16_t* ns_text)
{
    wxString title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);


    if (text.Find(wxT("ssl_error_")) != -1 ||
        text.Find(wxT("sec_error_")) != -1)
    {
        onBadCertificate(text, parent);
        return NS_OK;
    }




    wxMessageBox(text,
                 title,
                 wxOK,
                 GetTopFrameFromDOMWindow(parent));
                 
    return NS_OK;
}

NS_IMETHODIMP PromptService::AlertCheck(
                                    nsIDOMWindow* parent,
                                    const char16_t* ns_dialog_title,
                                    const char16_t* ns_text,
                                    const char16_t* ns_check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* checkValue)
#else
                                    PRBool* checkValue)
#endif
{
    wxString dialog_title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);
    wxString check_msg = ns2wx(ns_check_msg);
    
    return NS_OK;
}

NS_IMETHODIMP PromptService::Confirm(
                                    nsIDOMWindow* parent,
                                    const char16_t* ns_dialog_title,
                                    const char16_t* ns_text,
#if MOZILLA_VERSION_1 >= 10
                                    bool* retval)
#else
                                    PRBool* retval)
#endif
{
    wxString dialog_title = ns2wx(ns_dialog_title);
    wxString text = ns2wx(ns_text);
    
    int res = wxMessageBox(text,
             dialog_title,
             wxYES_NO,
             GetTopFrameFromDOMWindow(parent));
    
    if (!retval)
        return NS_ERROR_NULL_POINTER;
#if MOZILLA_VERSION_1 >= 10
    if (res == wxYES)
            *retval = true;
             else
            *retval = false;
#else
    if (res == wxYES)
        *retval = PR_TRUE;
         else
        *retval = PR_FALSE;
#endif
           
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmCheck(
                                    nsIDOMWindow* parent,
                                    const char16_t* dialog_title,
                                    const char16_t* text,
                                    const char16_t* check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* check_value,
                                    bool* retval)
#else
                                    PRBool* check_value,
                                    PRBool* retval)
#endif
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmEx(
                                    nsIDOMWindow* parent,
                                    const char16_t* dialog_title,
                                    const char16_t* text,
                                    uint32_t buttonFlags,
                                    const char16_t* button0_title,
                                    const char16_t* button1_title,
                                    const char16_t* button2_title,
                                    const char16_t* check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* check_value,
#else
                                    PRBool* check_value,
#endif
                                    PRInt32* button_pressed)
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::Prompt(
                                    nsIDOMWindow* parent,
                                    const char16_t* _dialog_title,
                                    const char16_t* _text,
									char16_t** _value,
                                    const char16_t* check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* check_value,
                                    bool* retval)
#else
                                    PRBool* check_value,
                                    PRBool* retval)
#endif
{
    // check message and check value aren't implemented yet
    
    wxString dialog_title = ns2wx(_dialog_title);
    wxString text = ns2wx(_text);
    wxString value = ns2wx(*_value);
    
    wxTextEntryDialog dlg(GetTopFrameFromDOMWindow(parent),
                          text,
                          dialog_title,
                          value,
                          wxOK | wxCANCEL | wxCENTER);

    int res = dlg.ShowModal();
    if (res == wxID_OK)
    {
        //*_value = wxToUnichar(dlg.GetValue());
    	*_value = (char16_t*)NS_ConvertUTF8toUTF16((const char*)dlg.GetValue().mb_str(wxConvUTF8)).get();
#if MOZILLA_VERSION_1 >= 10
        *retval = true;
#else
        *retval = PR_TRUE;
#endif
    }
     else
    {
#if MOZILLA_VERSION_1 >= 10
    	 *retval = false;
#else
        *retval = PR_FALSE;
#endif
    }

    return NS_OK;
}

NS_IMETHODIMP PromptService::PromptUsernameAndPassword(
                                    nsIDOMWindow* parent,
                                    const char16_t* dialog_title,
                                    const char16_t* text,
									char16_t** username,
									char16_t** password,
                                    const char16_t* check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* check_value,
                                    bool* retval)
#else
                                    PRBool* check_value,
                                    PRBool* retval)
#endif
{
    // this version is used by xulrunner 1.8.x -- see below for the newer version
    wxWindow* wxparent = GetTopFrameFromDOMWindow(parent);
    
    PromptDlgPassword dlg(wxparent);
    dlg.SetMessage(ns2wx(text));
    
    int res = dlg.ShowModal();
    if (res == wxID_OK)
    {
        //*username = wxToUnichar(dlg.GetUserName());
    	*username = (char16_t*)NS_ConvertUTF8toUTF16((const char*)dlg.GetUserName().mb_str(wxConvUTF8)).get();
        //*password = wxToUnichar(dlg.GetPassword());
    	*password = (char16_t*)NS_ConvertUTF8toUTF16((const char*)dlg.GetPassword().mb_str(wxConvUTF8)).get();
#if MOZILLA_VERSION_1 >= 10
        *retval = true;
#else
        *retval = PR_TRUE;
#endif
    }
     else
    {
#if MOZILLA_VERSION_1 >= 10
    	 *retval = false;
#else
        *retval = PR_FALSE;
#endif
    }
    
    return NS_OK;
}

NS_IMETHODIMP PromptService::PromptPassword(
                                    nsIDOMWindow* parent,
                                    const char16_t* dialog_title,
                                    const char16_t* text,
									char16_t** password,
                                    const char16_t* check_msg,
#if MOZILLA_VERSION_1 >= 10
                                    bool* check_value,
                                    bool* retval)
#else
                                    PRBool* check_value,
                                    PRBool* retval)
#endif
{
    return NS_OK;
}

NS_IMETHODIMP PromptService::Select(nsIDOMWindow* parent,
                                    const char16_t* dialog_title,
                                    const char16_t* text,
                                    PRUint32 count,
                                    const char16_t** select_list,
                                    PRInt32* out_selection,
#if MOZILLA_VERSION_1 >= 10
                                    bool* retval)
#else
                                    PRBool* retval)
#endif
{
#if MOZILLA_VERSION_1 >= 10
	*retval = true;
#else
    *retval = PR_TRUE;
#endif
    return NS_OK;
}


#if MOZILLA_VERSION_1 < 1
NS_IMETHODIMP PromptService::ConfirmUnknownIssuer(
                                    nsIInterfaceRequestor* socketInfo,
                                    nsIX509Cert* cert,
                                    PRInt16* certAddType,
                                    PRBool* retval)
{
    if (wxWebControl::GetIgnoreCertErrors())
    {
        *retval = PR_TRUE;
        return NS_OK;
    }

    int res = wxMessageBox(
        _("The requested web page is certified by an unknown authority.  Would you like to continue?"),
        _("Website Certified by an Unknown Authority"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
        
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }
        
    *certAddType = nsIBadCertListener::ADD_TRUSTED_FOR_SESSION;
    *retval = PR_TRUE;
    return NS_OK;
}
#endif

NS_IMETHODIMP PromptService::PromptAuth(nsIDOMWindow* parent,
                                        nsIChannel* channel,
                                        PRUint32 level,
                                        nsIAuthInformation* auth_info,
                                        const char16_t* checkbox_label,
#if MOZILLA_VERSION_1 >= 10
                                        bool* check_value,
                                        bool* retval)
#else
                                        PRBool* check_value,
                                        PRBool* retval)
#endif
{
    // this version used by newer >= 1.9.1 versions of xulrunner
    wxWindow* wxparent = GetTopFrameFromDOMWindow(parent);
    
    PromptDlgPassword dlg(wxparent);
    dlg.SetMessage(_("Please enter a username and password:"));
    
    int res = dlg.ShowModal();
    if (res == wxID_OK)
    {
        nsEmbedString ns_username;
        nsEmbedString ns_password;

        wx2ns(dlg.GetUserName(), ns_username);
        wx2ns(dlg.GetPassword(), ns_password);
        
        auth_info->SetUsername(ns_username);
        auth_info->SetPassword(ns_password);
#if MOZILLA_VERSION_1 >= 10
        *retval = true;
#else
        *retval = PR_TRUE;
#endif
    }
     else
    {
#if MOZILLA_VERSION_1 >= 10
    	 *retval = false;
#else
        *retval = PR_FALSE;
#endif
    }
    
    return NS_OK;
}

NS_IMETHODIMP PromptService::AsyncPromptAuth(nsIDOMWindow* parent,
                                        nsIChannel* channel,
                                        nsIAuthPromptCallback* callbck,
                                        nsISupports* contxt,
                                        PRUint32 level,
                                        nsIAuthInformation* auth_info,
                                        const char16_t* checkbox_label,
#if MOZILLA_VERSION_1 >= 10
                                        bool* check_value,
#else
                                        PRBool* check_value,
#endif
                                        nsICancelable** retval)
{
    return NS_OK;
}

#if MOZILLA_VERSION_1 < 1
NS_IMETHODIMP PromptService::ConfirmMismatchDomain(
                                    nsIInterfaceRequestor* socketInfo,
                                    const nsACString& targetURL,
                                    nsIX509Cert *cert,
                                    PRBool* retval)
{
    if (wxWebControl::GetIgnoreCertErrors())
    {
        *retval = PR_TRUE;
        return NS_OK;
    }

    int res = wxMessageBox(
        _("The requested web page uses a certificate which does not match the domain.  Would you like to continue?"),
        _("Domain Mismatch"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
        
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }

    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::ConfirmCertExpired(
                                    nsIInterfaceRequestor* socketInfo,
                                    nsIX509Cert* cert,
                                    PRBool* retval)
{
    if (wxWebControl::GetIgnoreCertErrors())
    {
        *retval = PR_TRUE;
        return NS_OK;
    }

    int res = wxMessageBox(
        _("The requested web page uses a certificate which has expired.  Would you like to continue?"),
        _("Certificate Expired"),
        wxICON_QUESTION | wxCENTER | wxYES_NO,
        NULL);
    
    if (res != wxYES)
    {
        *retval = PR_FALSE;
        return NS_OK;
    }
    
    *retval = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP PromptService::NotifyCrlNextupdate(
                                    nsIInterfaceRequestor* socketInfo,
                                    const nsACString& targetURL,
                                    nsIX509Cert* cert)
{
    return NS_OK;
}
#endif

NS_IMETHODIMP PromptService::NotifyCertProblem(
                                    nsIInterfaceRequestor *socket_info,
                                    nsISSLStatus *status,
                                    const nsACString& target_site,
#if MOZILLA_VERSION_1 >= 10
                                    bool *retval)
#else
                                    PRBool *retval)
#endif
{
    if (wxWebControl::GetIgnoreCertErrors())
    {
#if MOZILLA_VERSION_1 >= 10
    	*retval = true;
#else
        *retval = PR_TRUE;
#endif
        return NS_OK;
    }

#if MOZILLA_VERSION_1 >= 10
    	*retval = true;
#else
        *retval = PR_TRUE;
#endif
    return NS_OK;
}

void PromptService::onBadCertificate(const wxString& message, nsIDOMWindow* dom_window)
{
    wxString msg_text = message;
    msg_text += wxT("\n");
    msg_text += _("Would you like to accept this certificate and continue?");
    
    int res = wxMessageBox(msg_text,
             _("Secure Connection Warning"),
             wxYES_NO,
             GetTopFrameFromDOMWindow(dom_window));

    if (res != wxYES)
        return;


    wxWebControl* ctrl = GetWebControlFromDOMWindow(dom_window);
    if (!ctrl)
        return;

    //nsCOMPtr<nsIURIFixup> uri_fixup = nsGetService("@mozilla.org/docshell/urifixup;1");
    nsCOMPtr<nsIURIFixup> uri_fixup(do_GetService("@mozilla.org/docshell/urifixup;1"));
    if (!uri_fixup)
        return;
    
    wxString load_uri = ctrl->GetCurrentLoadURI();
    
    nsCOMPtr<nsIURI> uri;
    nsEmbedCString load_uri_text;
    wx2ns(load_uri, load_uri_text);
    uri_fixup->CreateFixupURI(load_uri_text, 0,NULL, getter_AddRefs(uri));

    if (!uri)
        return;
    
    /*
    nsEmbedCString ns_host;
    PRInt32 ns_port = 0;
    uri->GetAsciiHost(ns_host);
    uri->GetPort(&ns_port);
    if (ns_port <= 0)
        ns_port = 443;
    
    //nsCOMPtr<nsIRecentBadCertsService> bad_certs = nsGetService("@mozilla.org/security/recentbadcerts;1");
    nsCOMPtr<nsIRecentBadCertsService> bad_certs(do_GetService(NS_RECENTBADCERTS_CONTRACTID));
    if (bad_certs)
    {
        wxString wx_host_port = ns2wx(ns_host);
        wx_host_port += wxString::Format(wxT(":%d"), ns_port);
        
        nsEmbedString ns_host_port;
        wx2ns(wx_host_port, ns_host_port);
            
        nsCOMPtr<nsISSLStatus> status;
        bad_certs->GetRecentBadCert(ns_host_port, getter_AddRefs(status));
        if (status)
        {
            //nsCOMPtr<nsICertOverrideService> cert_override = nsGetService("@mozilla.org/security/certoverride;1");
            nsCOMPtr<nsICertOverrideService> cert_override (do_GetService("@mozilla.org/security/certoverride;1"));
            if (cert_override)
            {
                nsCOMPtr<nsIX509Cert> cert;
                status->GetServerCert(getter_AddRefs(cert));
                
                if (cert)
                {
#if MOZILLA_VERSION_1 >= 10
                	bool is_untrusted, is_domain_mismatch, is_not_valid_at_this_time;
#else
                    PRBool is_untrusted, is_domain_mismatch, is_not_valid_at_this_time;
#endif
                    
                    status->GetIsUntrusted(&is_untrusted);
                    status->GetIsDomainMismatch(&is_domain_mismatch);
                    status->GetIsNotValidAtThisTime(&is_not_valid_at_this_time);
                
                    unsigned int flags = 0;
                    if (is_untrusted)
                        flags |= nsICertOverrideService::ERROR_UNTRUSTED;
                    if (is_domain_mismatch)
                        flags |= nsICertOverrideService::ERROR_MISMATCH;
                    if (is_not_valid_at_this_time)
                        flags |= nsICertOverrideService::ERROR_TIME;

                    cert_override->RememberValidityOverride(ns_host, ns_port, cert, flags, false);
                    
                    ctrl->OpenURI(load_uri);
                }
            }
        }
    }*/
}



///////////////////////////////////////////////////////////////////////////////
//  PromptServiceFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class PromptServiceFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    PromptServiceFactory()
    {
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        
        PromptService* obj = new PromptService;
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        
        return res;
    }
#if MOZILLA_VERSION_1 >= 10
    NS_IMETHOD LockFactory(bool lock)
#else
    NS_IMETHOD LockFactory(PRBool lock)
#endif
    {
        return NS_OK;
    }

protected:
    ~PromptServiceFactory()
    {
    }

};

NS_IMPL_ISUPPORTS(PromptServiceFactory, nsIFactory);


void CreatePromptServiceFactory(nsIFactory** result)
{
    PromptServiceFactory* obj = new PromptServiceFactory;
    obj->AddRef();
    *result = obj;
}

///////////////////////////////////////////////////////////////////////////////
//  TransferService class implementation
///////////////////////////////////////////////////////////////////////////////


class TransferService : public nsITransfer
{
public:

    NS_DECL_ISUPPORTS

    TransferService()
    {
    }

    NS_IMETHODIMP Init(nsIURI* source,
                       nsIURI* target,
                       const nsAString& display_name,
                       nsIMIMEInfo* mime_info,
                       PRTime start_time,
                       nsILocalFile* temp_file,
                       nsICancelable* cancelable)
    {
        return NS_OK;
    }

    NS_IMETHOD OnStateChange(nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state_flags,
                             nsresult status)
    {
        return NS_OK;
    }

    NS_IMETHOD OnProgressChange(nsIWebProgress* web_progress,
                                nsIRequest* request,
                                PRInt32 cur_self_progress,
                                PRInt32 max_self_progress,
                                PRInt32 cur_total_progress,
                                PRInt32 max_total_progress)
    {
        return OnProgressChange64(web_progress,
                                  request,
                                  cur_self_progress,
                                  max_self_progress,
                                  cur_total_progress,
                                  max_total_progress);
    }
    
    NS_IMETHOD OnProgressChange64(
                                 nsIWebProgress* web_progress,
                                 nsIRequest* request,
                                 PRInt64 cur_self_progress,
                                 PRInt64 max_self_progress,
                                 PRInt64 cur_total_progress,
                                 PRInt64 max_total_progress)
    {
       return NS_OK;
    }
    
    NS_IMETHOD OnLocationChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
#if MOZILLA_VERSION_1 >= 11
                     nsIURI* location,
                     PRUint32 aflags)
#else
                     nsIURI* location)
#endif
    {
       return NS_OK;
    }

    NS_IMETHOD OnStatusChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             nsresult status,
                             const PRUnichar* message)
    {
        return NS_OK;
    }


    NS_IMETHOD OnSecurityChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state)
    {
       return NS_OK;
    }
    
    NS_IMETHOD OnRefreshAttempted(
                        nsIWebProgress* web_progress,
                        nsIURI* refresh_uri,
                        PRInt32 millis,
#if MOZILLA_VERSION_1 >= 10
                        bool same_uri,
                        bool *retval)
#else
                        PRBool same_uri,
                        PRBool *retval)
#endif
    {
        return NS_OK;
    }

protected:
    ~TransferService()
    {
    }
};

NS_IMPL_ADDREF(TransferService)
NS_IMPL_RELEASE(TransferService)

NS_INTERFACE_MAP_BEGIN(TransferService)
    NS_INTERFACE_MAP_ENTRY(nsISupports)
    NS_INTERFACE_MAP_ENTRY(nsITransfer)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener2)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
NS_INTERFACE_MAP_END



///////////////////////////////////////////////////////////////////////////////
//  TransferFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class TransferFactory : public nsIFactory
{
public:

    NS_DECL_ISUPPORTS
    
    TransferFactory()
    {
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;       
/*
#if MOZILLA_VERSION_1 < 1
		if (wxWebControl::IsVersion18())
        {
            TransferService18* obj = new TransferService18();
            if (!obj)
                return NS_ERROR_OUT_OF_MEMORY;
            obj->AddRef();
            res = obj->QueryInterface(iid, result);
            obj->Release();
        }
         else
#endif
        {
            TransferService* obj = new TransferService();
            if (!obj)
                return NS_ERROR_OUT_OF_MEMORY;
            obj->AddRef();
            res = obj->QueryInterface(iid, result);
            obj->Release();
        }
        */
        
        return res;
    }
#if MOZILLA_VERSION_1 >= 10
    NS_IMETHOD LockFactory(bool lock)
#else
    NS_IMETHOD LockFactory(PRBool lock)
#endif
    {
        return NS_OK;
    }

protected:
    ~TransferFactory()
    {
    }
};

NS_IMPL_ISUPPORTS(TransferFactory, nsIFactory);


void CreateTransferFactory(nsIFactory** result)
{
    TransferFactory* obj = new TransferFactory;
    obj->AddRef();
    *result = obj;
}




///////////////////////////////////////////////////////////////////////////////
//  UnknownContentTypeHandler class implementation
///////////////////////////////////////////////////////////////////////////////

class UnknownContentTypeHandler :
	public nsIHelperAppLauncherDialog
#if MOZILLA_VERSION_1 < 1
	,
    public nsIHelperAppLauncherDialog18
#endif
{
public:

    NS_DECL_ISUPPORTS

    NS_IMETHOD Show(nsIHelperAppLauncher* launcher,
                    nsISupports* _context,
                    PRUint32 reason)
    {     
        nsCOMPtr<nsISupports> context = _context;

        nsCOMPtr<nsIDOMWindow> parent = do_GetInterface(context);
        wxWebControl* ctrl = GetWebControlFromDOMWindow(parent);
        if (!ctrl)
        {
            // nobody to handle event, default action
            // is to save file to disk
            
            // BIW 7 May 2007 - this was causing save as dialogs to appear
            // during page loads.. we'll do nothing here instead
            return NS_OK;
        }
        
        
        wxString url;
        nsCOMPtr<nsIURI> uri;
        launcher->GetSource(getter_AddRefs(uri));
        if (uri)
        {
            nsEmbedCString ns_spec;
            if (NS_SUCCEEDED(uri->GetSpec(ns_spec)))
                url = ns2wx(ns_spec);
        }
        
        
        
        nsEmbedString ns_filename;
        launcher->GetSuggestedFileName(ns_filename);
        wxString filename = ns2wx(ns_filename);
        
        
        // fetch mime type
        nsEmbedCString ns_mimetype;
        //nsCOMPtr<nsISupports> mime_info_supports;
        nsCOMPtr<nsIMIMEInfo > mime_info_supports;
        //launcher->GetMIMEInfo((nsIMIMEInfo**)getter_AddRefs(mime_info_supports));
        launcher->GetMIMEInfo(getter_AddRefs(mime_info_supports));
        wxString mime_type;

#if MOZILLA_VERSION_1 < 1
        nsCOMPtr<nsIMIMEInfo18> mime_info18 = mime_info_supports;
        if (mime_info18)
        {
            mime_info18->GetMIMEType(ns_mimetype);
            mime_type = ns2wx(ns_mimetype);
        }
#endif
        nsCOMPtr<nsIMIMEInfo> mime_info = mime_info_supports;
        if (mime_info)
        {
            mime_info->GetMIMEType(ns_mimetype);
            mime_type = ns2wx(ns_mimetype);
        }        
        
        
        // setup event object
        wxWebEvent evt(wxEVT_WEB_INITDOWNLOAD, ctrl->GetId());
        evt.SetEventObject(ctrl);
        evt.SetFilename(filename);
        evt.SetContentType(mime_type);
        evt.SetHref(url);
        bool handled = ctrl->GetEventHandler()->ProcessEvent(evt);

        if (handled)
        {
        
            switch (evt.m_download_action)
            {
                case wxWEB_DOWNLOAD_SAVE:
                    launcher->SaveToDisk(NULL, false);
                    break;
                case wxWEB_DOWNLOAD_SAVEAS:
                    wxASSERT_MSG(evt.m_download_action_path.Length() > 0, wxT("You must specify a filename in the event object"));
                    if (evt.m_download_action_path.IsEmpty())
                    {
                        // no filename specified
                    	nsresult res;
                        //launcher->Cancel(0x804b0002 ); // = NS_BINDING_ABORTED
                    	launcher->Cancel(res);
                        return NS_OK;
                    }
                     else
                    {
                        std::string fname = (const char*)evt.m_download_action_path.mbc_str();
                        
                        nsIFile* filep = NULL;
                        NS_NewNativeLocalFile(nsDependentCString(fname.c_str()), true, &filep);

                        launcher->SaveToDisk(filep, PR_FALSE);

                        if (filep)
                            filep->Release();
                    }
                    break;
                case wxWEB_DOWNLOAD_OPEN:
                    launcher->LaunchWithApplication(NULL, PR_FALSE);
                    break;
                case wxWEB_DOWNLOAD_CANCEL:
                	nsresult res;
                    //launcher->Cancel(0x804b0002  ); // = NS_BINDING_ABORTED
                    launcher->Cancel(res); // = NS_BINDING_ABORTED
                    break;
            }
            
            
            if (evt.m_download_listener)
            {
                evt.m_download_listener->Init(url, evt.m_download_action_path);
                nsIWebProgressListener* progress = CreateProgressListenerAdaptor(evt.m_download_listener);
#if MOZILLA_VERSION_1 < 1
                if (wxWebControl::IsVersion18())
                    launcher->SetWebProgressListener((nsIWebProgressListener2*)(nsIWebProgressListener2_18*)progress);
                     else
#endif
                    launcher->SetWebProgressListener((nsIWebProgressListener2*)progress);
                
                progress->Release();
            }
        
        }
         else
        {
            launcher->SaveToDisk(NULL, false);
            return NS_OK;
/*
            OpenOrSaveDlg dlg(GetTopFrameFromDOMWindow(parent), filename);
            int result = dlg.ShowModal();
            
            switch (result)
            {
                case wxID_OPEN:
                    break;
                case wxID_SAVE:
                    launcher->SaveToDisk(nsnull, PR_FALSE);
                    break;
                case wxID_CANCEL:
                    launcher->Cancel(0x804b0002 ); // = NS_BINDING_ABORTED
                    return NS_OK;
            }
            */
        }
            
        return NS_OK;
    }

    NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher* launcher,
                                   nsISupports* window_context,
                                   const char16_t* default_file,
                                   const char16_t* suggested_file_extension,
                                   nsIFile** new_file)
    {
        return PromptForSaveToFile(launcher, window_context, default_file, suggested_file_extension, false, new_file);
    }
    
    NS_IMETHOD PromptForSaveToFile(nsIHelperAppLauncher* launcher,
                                   nsISupports* window_context,
                                   const char16_t* default_file,
                                   const char16_t* suggested_file_extension,
#if MOZILLA_VERSION_1 >= 10
                                   bool force_prompt,
#else
                                   PRBool force_prompt,
#endif
                                   nsIFile** new_file)
    {
        nsCOMPtr<nsISupports> context = window_context;
        //FIXME later
        //nsCOMPtr<nsIDOMWindow> parent = nsRequestInterface(context);
        nsCOMPtr<nsIDOMWindow> parent;
        wxString default_filename = ns2wx(default_file);
        
        wxString filter;
        filter += _("All Files");
        filter += wxT(" (*.*)|*.*|");
        filter.RemoveLast(); // get rid of the last pipe sign

        wxFileDialog dlg(GetTopFrameFromDOMWindow(parent),
                         _("Save As"),
                         wxT(""),
                         default_filename,
                         filter,
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        
        if (dlg.ShowModal() != wxID_OK)
        {
            return NS_ERROR_FAILURE;
        }
        
        
        std::string fname = (const char*)dlg.GetPath().mbc_str();

        NS_NewNativeLocalFile(nsDependentCString(fname.c_str()), true, new_file);
        return NS_OK;
    }

protected:
    ~UnknownContentTypeHandler()
    {
    }
};


NS_IMPL_ADDREF(UnknownContentTypeHandler)
NS_IMPL_RELEASE(UnknownContentTypeHandler)

NS_INTERFACE_MAP_BEGIN(UnknownContentTypeHandler)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIHelperAppLauncherDialog)
    NS_INTERFACE_MAP_ENTRY(nsIHelperAppLauncherDialog)
#if MOZILLA_VERSION_1 < 1
    NS_INTERFACE_MAP_ENTRY(nsIHelperAppLauncherDialog18)
#endif
NS_INTERFACE_MAP_END



///////////////////////////////////////////////////////////////////////////////
//  UnknownContentTypeHandlerFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class UnknownContentTypeHandlerFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    UnknownContentTypeHandlerFactory()
    {
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        /*
        UnknownContentTypeHandler* obj = new UnknownContentTypeHandler;
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        */
        return res;
    }
#if MOZILLA_VERSION_1 >=10
    NS_IMETHOD LockFactory(bool lock)
#else
    NS_IMETHOD LockFactory(PRBool lock)
#endif
    {
        return NS_OK;
    }

protected:
    ~UnknownContentTypeHandlerFactory()
    {
    }
};

NS_IMPL_ISUPPORTS(UnknownContentTypeHandlerFactory, nsIFactory);


void CreateUnknownContentTypeHandlerFactory(nsIFactory** result)
{
    UnknownContentTypeHandlerFactory* obj = new UnknownContentTypeHandlerFactory;
    obj->AddRef();
    *result = obj;
}


class CertOverrideService : public nsICertOverrideService
{
public:

    NS_DECL_ISUPPORTS


    CertOverrideService()
    {
    }

    NS_IMETHOD RememberValidityOverride(const nsACString& host_name,
                                        PRInt32 port, nsIX509Cert* cert,
                                        PRUint32 override_bits,
#if MOZILLA_VERSION_1 >= 10
                                        bool temporary)
#else
                                        PRBool temporary)
#endif
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHOD HasMatchingOverride(const nsACString& host_name,
                                   PRInt32 port,
                                   nsIX509Cert* cert,
                                   PRUint32* override_bits,
#if MOZILLA_VERSION_1 >= 10
                                   bool* is_temporary,
                                   bool *_retval)
#else
                                   PRBool* is_temporary,
                                   PRBool *_retval)
#endif
    {
        *override_bits = 3;
#if MOZILLA_VERSION_1 >= 10
        *_retval = true;
#else
        *_retval = PR_TRUE;
#endif
        return NS_OK;
    }

    NS_IMETHOD GetValidityOverride(const nsACString& host_name,
                                   PRInt32 port,
                                   nsACString& hash_alg,
                                   nsACString& fingerprint,
                                   PRUint32* override_bits,
#if MOZILLA_VERSION_1 >= 10
                                   bool* is_temporary,
                                   bool* _retval)
#else
                                   PRBool* is_temporary,
                                   PRBool* _retval)
#endif
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHOD ClearValidityOverride(const nsACString& host_name,
                                     PRInt32 port)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHOD GetAllOverrideHostsWithPorts(PRUint32* count,
                                            PRUnichar*** hosts_with_ports_array)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHOD IsCertUsedForOverrides(nsIX509Cert *cert,
#if MOZILLA_VERSION_1 >= 10
            bool check_temporaries,
            bool check_permanents,
#else
                                      PRBool check_temporaries,
                                      PRBool check_permanents,
#endif
                                      PRUint32 *_retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

protected:
    virtual ~CertOverrideService()
    {
    }

};


NS_IMPL_ISUPPORTS(CertOverrideService, nsICertOverrideService);





///////////////////////////////////////////////////////////////////////////////
//  CertOverrideFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class CertOverrideFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    CertOverrideFactory()
    {
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        /*
        CertOverrideService* obj = new CertOverrideService;
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        obj->Release();
        */
        return res;
    }
#if MOZILLA_VERSION_1 >=10
    NS_IMETHOD LockFactory(bool lock)
#else
    NS_IMETHOD LockFactory(PRBool lock)
#endif
    {
        return NS_OK;
    }
protected:
    ~CertOverrideFactory()
    {
    }
};

NS_IMPL_ISUPPORTS(CertOverrideFactory, nsIFactory);


void CreateCertOverrideFactory(nsIFactory** result)
{
    CertOverrideFactory* obj = new CertOverrideFactory;
    obj->AddRef();
    *result = obj;
}
