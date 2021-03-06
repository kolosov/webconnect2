///////////////////////////////////////////////////////////////////////////////
// Name:        webcontrol.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2006-09-22
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
#include "mozilla/Char16.h"
#endif

#define _CRT_SECURE_NO_WARNINGS


#include <string>
#include <wx/wx.h>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/file.h>
#include "webframe.h"
#include "webcontrol.h"

#include "nsutils.h"
//#include "nsinclude.h"

#include "nsDirectoryServiceDefs.h"
#include "nsDirectoryServiceUtils.h"
#include "nsIComponentRegistrar.h"
#include "nsStringGlue.h"
#include "nsIDOMDocument.h"
#include "nsIDOMText.h"
#include "nsIDOMNode.h"
#include "nsIDOMNodeList.h"
#include "nsIDOMAttr.h"
#include "nsIDOMHTMLAnchorElement.h"
#include "nsIDOMHTMLButtonElement.h"
#include "nsIDOMHTMLInputElement.h"
#include "nsIDOMHTMLLinkElement.h"
#include "nsIDOMHTMLOptionElement.h"
#include "nsIDOMHTMLSelectElement.h"
#include "nsIDOMHTMLTextAreaElement.h"
#include "nsIDOMEventListener.h"
#include "nsIDOMEventTarget.h"

#include "domprivate.h"
#include "promptservice.h"

#include "nsEmbedCID.h"
#include "nsNetCID.h"
#include "nsIDownload.h"

#include "xpcom-config.h"
//xpcom headers
#include "nsXULAppAPI.h"
#include "nsXPCOMGlue.h"

#include "nsIWebBrowser.h"
#include "nsIWebBrowserChrome.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIWebBrowserFind.h"
#include "nsIWebBrowserFocus.h"
#include "nsIBaseWindow.h"
#include "nsIEmbeddingSiteWindow.h"
#include "nsIContextMenuListener2.h"
#include "nsITooltipListener.h"
#include "nsIInterfaceRequestor.h"

#include "nsIWeakReference.h" //Utils?
#include "nsIWeakReferenceUtils.h" //Utils?
#include "nsIDOMWindow.h"
#include "nsIURI.h"
#include "nsIDOMEvent.h"
#include "nsIDOMMouseEvent.h"
#include "nsIURIContentListener.h"
#include "nsIStreamListener.h"
#include "nsIInputStream.h"
#include "nsIChannel.h"
#include "nsIDocShellTreeItem.h"
#include "nsICacheService.h"
#include "nsIWebBrowserPersist.h"
#include "nsIStringStream.h"


#include "nsIWindowCreator2.h"
#include "nsISimpleEnumerator.h"
#include "nsIFile.h"
#include "nsIDirectoryService.h"
#include "nsIComponentRegistrar.h"

#include "nsIWebNavigation.h"
#include "nsIClipboardCommands.h"
#include "nsIAppShell.h"

#include "nsIServiceManager.h"
#include "nsServiceManagerUtils.h"
#include "nsIInterfaceRequestorUtils.h"
#include "nsComponentManagerUtils.h"


// global preference for whether or not to show certificate errors
bool g_ignore_ssl_cert_errors = false;



///////////////////////////////////////////////////////////////////////////////
//  event declarations
///////////////////////////////////////////////////////////////////////////////


//wxDEFINE_EVENT(wxEVT_WEB_OPENURI, wxCommandEvent);

DEFINE_EVENT_TYPE(wxEVT_WEB_OPENURI)
DEFINE_EVENT_TYPE(wxEVT_WEB_TITLECHANGE)
DEFINE_EVENT_TYPE(wxEVT_WEB_LOCATIONCHANGE)
DEFINE_EVENT_TYPE(wxEVT_WEB_DOMCONTENTLOADED)
DEFINE_EVENT_TYPE(wxEVT_WEB_STATUSTEXT)
DEFINE_EVENT_TYPE(wxEVT_WEB_STATUSCHANGE)
DEFINE_EVENT_TYPE(wxEVT_WEB_STATECHANGE)
DEFINE_EVENT_TYPE(wxEVT_WEB_SHOWCONTEXTMENU)
DEFINE_EVENT_TYPE(wxEVT_WEB_CREATEBROWSER)
DEFINE_EVENT_TYPE(wxEVT_WEB_LEFTDOWN)
DEFINE_EVENT_TYPE(wxEVT_WEB_MIDDLEDOWN)
DEFINE_EVENT_TYPE(wxEVT_WEB_RIGHTDOWN)
DEFINE_EVENT_TYPE(wxEVT_WEB_LEFTUP)
DEFINE_EVENT_TYPE(wxEVT_WEB_MIDDLEUP)
DEFINE_EVENT_TYPE(wxEVT_WEB_RIGHTUP)
DEFINE_EVENT_TYPE(wxEVT_WEB_LEFTDCLICK)
DEFINE_EVENT_TYPE(wxEVT_WEB_DRAGDROP)
DEFINE_EVENT_TYPE(wxEVT_WEB_INITDOWNLOAD)
DEFINE_EVENT_TYPE(wxEVT_WEB_SHOULDHANDLECONTENT)
DEFINE_EVENT_TYPE(wxEVT_WEB_FAVICONAVAILABLE)
DEFINE_EVENT_TYPE(wxEVT_WEB_DOMEVENT)
IMPLEMENT_DYNAMIC_CLASS(wxWebEvent, wxNotifyEvent)


//XRE_InitEmbedding2Type XRE_InitEmbedding2 = 0;
XRE_InitEmbedding2Type XRE_InitEmbedding2 NS_HIDDEN;
XRE_TermEmbeddingType XRE_TermEmbedding NS_HIDDEN;
XRE_NotifyProfileType XRE_NotifyProfile NS_HIDDEN;
XRE_LockProfileDirectoryType XRE_LockProfileDirectory NS_HIDDEN;

//nsCOMPtr<nsIFile> prof_dir = 0;

/*
//Directory service provider
nsIDirectoryServiceProvider *sAppFileLocProvider = 0;
class wxDirSrvProvider : public nsIDirectoryServiceProvider2
{
public:
    NS_DECL_ISUPPORTS_INHERITED
    NS_DECL_NSIDIRECTORYSERVICEPROVIDER
    NS_DECL_NSIDIRECTORYSERVICEPROVIDER2
};

static const wxDirSrvProvider DirectoryProvider = wxDirSrvProvider();

NS_IMPL_QUERY_INTERFACE(wxDirSrvProvider,
                         nsIDirectoryServiceProvider,
                         nsIDirectoryServiceProvider2)


NS_IMETHODIMP_(nsrefcnt)
wxDirSrvProvider::AddRef()
{
    return 1;
}

NS_IMETHODIMP_(nsrefcnt)
wxDirSrvProvider::Release()
{
    return 1;
}


NS_IMETHODIMP
wxDirSrvProvider::GetFile(const char *aKey,
#if MOZILLA_VERSION_1 >= 10
		bool *aPersist,
#else
		PRBool *aPersist,
#endif
		nsIFile* *aResult)
{
    if (sAppFileLocProvider) {
        nsresult rv = sAppFileLocProvider->GetFile(aKey, aPersist, aResult);
        if (NS_SUCCEEDED(rv))
            return rv;
    }

    if (prof_dir && !strcmp(aKey, NS_APP_USER_PROFILE_50_DIR)) {
#if MOZILLA_VERSION_1 >= 10
        *aPersist = true;
#else
        *aPersist = PR_TRUE;
#endif
        return prof_dir->Clone(aResult);
    }

    if (prof_dir && !strcmp(aKey, NS_APP_PROFILE_DIR_STARTUP)) {
#if MOZILLA_VERSION_1 >= 10
        *aPersist = true;
#else
        *aPersist = PR_TRUE;
#endif
        return prof_dir->Clone(aResult);
    }

    if (prof_dir && !strcmp(aKey, NS_APP_CACHE_PARENT_DIR)) {
#if MOZILLA_VERSION_1 >= 10
        *aPersist = true;
#else
        *aPersist = PR_TRUE;
#endif
        return prof_dir->Clone(aResult);
    }

    return NS_ERROR_FAILURE;
}

NS_IMETHODIMP
wxDirSrvProvider::GetFiles(const char *aKey,
                                    nsISimpleEnumerator* *aResult)
{
    nsCOMPtr<nsIDirectoryServiceProvider2>
        dp2(do_QueryInterface(sAppFileLocProvider));

    if (!dp2)
        return NS_ERROR_FAILURE;

    return dp2->GetFiles(aKey, aResult);
}
*/

// the purpose of the EmbeddingPtrs structure is to allow us a way
// to access the ns interfaces without including them in any public
// header file.

struct EmbeddingPtrs
{
    nsCOMPtr<nsIWebBrowser> m_web_browser;
    nsCOMPtr<nsIWebBrowserFind> m_web_browser_find;
    nsCOMPtr<nsIBaseWindow> m_base_window;
    nsCOMPtr<nsIWebNavigation> m_web_navigation;
    nsCOMPtr<nsIDOMEventTarget> m_event_target;
    nsCOMPtr<nsIClipboardCommands> m_clipboard_commands;
    
    //nsCOMPtr<nsISupports> m_print_settings;
    //nsCOMPtr<nsIPrintSettings> m_print_settings;
};


// declare a wxArray for ContentListener

class ContentListener;
WX_DEFINE_ARRAY_PTR(ContentListener*, ContentListenerPtrArray);


class PluginListProvider;


// GeckoEngine is an internal class wchich manages the xulrunner engine;
// It does not need to be called publicly

class GeckoEngine
{
public:

    GeckoEngine();
    ~GeckoEngine();

    void SetEnginePath(const wxString& path);
    void SetStoragePath(const wxString& path);

    bool Init();
    bool IsOk() const;
    void Uninit();

    void AddContentListener(ContentListener* l);
    ContentListenerPtrArray& GetContentListeners();

    void AddPluginPath(const wxString& path);

private:

    wxString m_gecko_path;
    wxString m_storage_path;
    wxString m_history_filename;
    bool m_ok;

    ContentListenerPtrArray m_content_listeners;
    nsCOMPtr<nsIAppShell> m_appshell;
    PluginListProvider* m_plugin_provider;
    friend class WindowCreator;
};


// global instance of the GeckoEngine object

GeckoEngine g_gecko_engine;




///////////////////////////////////////////////////////////////////////////////
//  browser chrome implementation
///////////////////////////////////////////////////////////////////////////////


// this interface allows us to get the wxWebControl
// pointer from a browser chrome object in a safe way

#define NS_ICHROMEINTERNAL_IID \
  {0x7fe3c660, 0x376c, 0x43e9, \
  { 0x9f, 0xdd, 0x69, 0x85, 0x4a, 0xfe, 0xc9, 0x46 }}
  

class NS_NO_VTABLE nsIChromeInternal : public nsISupports
{
public:
	static const nsIID& GetIID() { static nsIID nsiid = NS_ICHROMEINTERNAL_IID; return nsiid;}
//#if (MOZILLA_VERSION_1 >= 2 ) ||  ((MOZILLA_VERSION_1 == 1) && (MOZILLA_VERSION_2 >= 9))
    //NS_DEFINE_STATIC_IID_ACCESSOR(nsIChromeInternal,NS_ICHROMEINTERNAL_IID) //FIXME
//    NS_DECLARE_STATIC_IID_ACCESSOR(NS_ICHROMEINTERNAL_IID) //FIXME
//#else
//    NS_DEFINE_STATIC_IID_ACCESSOR(NS_ICHROMEINTERNAL_IID)
//#endif

    virtual wxWebControl* GetWebControl() = 0;
};

//NS_DEFINE_STATIC_IID_ACCESSOR(nsIChromeInternal,NS_ICHROMEINTERNAL_IID) //FIXME


class BrowserChrome : public nsIWebBrowserChrome,
                      public nsIChromeInternal,
                      public nsIWebBrowserChromeFocus,
                      public nsIWebProgressListener,
                      public nsIEmbeddingSiteWindow,
                      public nsIInterfaceRequestor,
                      public nsIContextMenuListener2,
                      public nsITooltipListener,
                      public nsIDOMEventListener
{
public:

    NS_DECL_ISUPPORTS
    NS_DECL_NSIWEBBROWSERCHROME
    NS_DECL_NSIWEBBROWSERCHROMEFOCUS
    NS_DECL_NSIWEBPROGRESSLISTENER
    NS_DECL_NSIINTERFACEREQUESTOR
    NS_DECL_NSIEMBEDDINGSITEWINDOW
    NS_DECL_NSICONTEXTMENULISTENER2
    NS_DECL_NSITOOLTIPLISTENER
    NS_DECL_NSIDOMEVENTLISTENER

    BrowserChrome(wxWebControl* wnd);
    
    void ChromeInit();
    void ChromeUninit();
    
    wxWebControl* GetWebControl() { return m_wnd; }
    
public:
    nsCOMPtr<nsIWebBrowser> m_web_browser;
    wxWebControl* m_wnd;
    wxString m_title;
    PRUint32 m_chrome_mask;
    wxDialog* m_dialog;
    nsresult m_dialog_retval;

protected:
    virtual ~BrowserChrome();
};


static nsIDOMNode* GetAnchor(nsIDOMNode* node)
{
    // note: this function finds if there's an anchor element
    // in the parent hierarchy, and returns it if it exists;
    // otherwise, it returns null

    if (!node)
        return node;

    nsCOMPtr<nsIDOMNode> n = node;
    nsCOMPtr<nsIDOMHTMLAnchorElement> anchor = do_QueryInterface(n);

    if (anchor)
        return node;

    node->GetParentNode(&node);
    return GetAnchor(node);
}


NS_IMPL_ADDREF(BrowserChrome)
NS_IMPL_RELEASE(BrowserChrome)

NS_INTERFACE_MAP_BEGIN(BrowserChrome)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIWebBrowserChrome)
    NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChrome)
//    NS_INTERFACE_MAP_ENTRY(nsIChromeInternal)
    NS_INTERFACE_MAP_ENTRY(nsIWebBrowserChromeFocus)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
    NS_INTERFACE_MAP_ENTRY(nsIEmbeddingSiteWindow)
    NS_INTERFACE_MAP_ENTRY(nsIInterfaceRequestor)
    NS_INTERFACE_MAP_ENTRY(nsIContextMenuListener2)
    NS_INTERFACE_MAP_ENTRY(nsITooltipListener)
    NS_INTERFACE_MAP_ENTRY(nsIDOMEventListener)
NS_INTERFACE_MAP_END


BrowserChrome::BrowserChrome(wxWebControl* wnd)
{
    m_wnd = wnd;
    m_chrome_mask = nsIWebBrowserChrome::CHROME_ALL;
    m_dialog = NULL;
    m_dialog_retval = NS_OK;
}

BrowserChrome::~BrowserChrome()
{
}

void BrowserChrome::ChromeInit()
{
    nsresult res;

    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("mousedown"),
                                            this,
                                            true);


    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("mouseup"),
                                            this,
                                            true);
                                                   
    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("dblclick"),
                                            this,
                                            true);

    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("dragdrop"),
                                            this,
                                            true);
                                 
    // these two event types are used to capture favicon information
    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("DOMLinkAdded"),
                                            this,
                                            true);

    res = m_wnd->m_ptrs->m_event_target->AddEventListener(
                                            NS_LITERAL_STRING("DOMContentLoaded"),
                                            this,
                                            true);

}

void BrowserChrome::ChromeUninit()
{
    nsresult res;

    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("mousedown"),
                                            this,
											true);

    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("mouseup"),
                                            this,
											true);
                                                   
    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("dblclick"),
                                            this,
											true);
                                            
    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("dragdrop"),
                                            this,
											true);
                                            
    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("DOMLinkAdded"),
                                            this,
											true);
                                            
    res = m_wnd->m_ptrs->m_event_target->RemoveEventListener(
                                            NS_LITERAL_STRING("DOMContentLoaded"),
                                            this,
											true);

    m_wnd = NULL;
}

NS_IMETHODIMP BrowserChrome::GetWebBrowser(nsIWebBrowser** web_browser)
{
    NS_ENSURE_ARG_POINTER(web_browser);
    *web_browser = m_web_browser;
    NS_IF_ADDREF(*web_browser);
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetWebBrowser(nsIWebBrowser* web_browser)
{
    NS_ENSURE_ARG_POINTER(web_browser);
    m_web_browser = web_browser;
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::FocusNextElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP BrowserChrome::FocusPrevElement()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP BrowserChrome::GetChromeFlags(PRUint32* chrome_mask)
{
    NS_ENSURE_ARG_POINTER(chrome_mask);
    *chrome_mask = m_chrome_mask;
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetChromeFlags(PRUint32 chrome_mask)
{
    m_chrome_mask = chrome_mask;
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SizeBrowserTo(PRInt32 aCX, PRInt32 aCY)
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::ShowAsModal()
{
    if (!m_wnd)
        return NS_OK;
  
    wxWindow* parent = m_wnd->GetParent();
    wxDialog* dialog = NULL;
    while (parent)
    {
        if (parent->IsKindOf(CLASSINFO(wxDialog)))
        {
            dialog = (wxDialog*)parent;
            break;
        }
        
        parent = parent->GetParent();
    }
    
    if (!dialog)
        return NS_OK;
 
    m_dialog = dialog;
    m_dialog->ShowModal();
    m_dialog = NULL;

    return m_dialog_retval;
}

#if MOZILLA_VERSION_1 >=10
NS_IMETHODIMP BrowserChrome::IsWindowModal(bool* retval)
{
    *retval = m_dialog ? true : false;
    return NS_OK;
}
#else
NS_IMETHODIMP BrowserChrome::IsWindowModal(PRBool* retval)

{
    *retval = m_dialog ? PR_TRUE : PR_FALSE;
    return NS_OK;
}
#endif

NS_IMETHODIMP BrowserChrome::ExitModalEventLoop(nsresult status)
{
    if (m_dialog == NULL)
        return NS_OK;
    m_dialog_retval = status;
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetFocus()
{
    m_wnd->m_ptrs->m_base_window->SetFocus();
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::GetTitle(char16_t** title)
{
    //*title = wxToUnichar(m_title);
    *title = (char16_t*)NS_ConvertUTF8toUTF16((const char*)m_title.mb_str(wxConvUTF8)).get();
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetTitle(const char16_t* title)
{
    if (!m_wnd)
        return NS_OK;

    m_title = ns2wx(title);
    
    if (m_dialog)
    {
        // modal dialogs we do ourself
        m_dialog->SetTitle(ns2wx(title));
        return NS_OK;
    }
    //debug
    int win_id = m_wnd->GetId();
      
    wxWebEvent evt(wxEVT_WEB_TITLECHANGE, m_wnd->GetId());
    evt.SetEventObject(m_wnd);
    evt.SetString(m_title);
    m_wnd->GetEventHandler()->ProcessEvent(evt);
    
    return NS_OK;
}
#if MOZILLA_VERSION_1 >=10
NS_IMETHODIMP BrowserChrome::GetVisibility(bool* visibility)
{
    *visibility = true;
    return NS_OK;
}
#else
NS_IMETHODIMP BrowserChrome::GetVisibility(PRBool* visibility)
{
    *visibility = PR_TRUE;
    return NS_OK;
}
#endif

#if MOZILLA_VERSION_1 >=10
NS_IMETHODIMP BrowserChrome::SetVisibility(bool visibility)
#else
NS_IMETHODIMP BrowserChrome::SetVisibility(PRBool visibility)
#endif
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::DestroyBrowserWindow()
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetStatus(PRUint32 type, const char16_t* status)
{
    if (!m_wnd)
        return NS_OK;

    if (type == STATUS_LINK)
    {
        wxWebEvent evt(wxEVT_WEB_STATUSTEXT, m_wnd->GetId());
        evt.SetEventObject(m_wnd);
        evt.SetString(ns2wx(status));
        m_wnd->GetEventHandler()->ProcessEvent(evt);
    }
      
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::SetDimensions(PRUint32 flags,
                                           PRInt32 x, PRInt32 y,
                                           PRInt32 cx, PRInt32 cy)
{
    return NS_OK;
}


NS_IMETHODIMP BrowserChrome::GetDimensions(PRUint32 flags,
                                           PRInt32* x, PRInt32* y,
                                           PRInt32* cx, PRInt32* cy)
{
    if (!m_wnd)
    {
        // no window (almost never happens)
        *x = 0; *y = 0;
        *cx = 100; *cy = 100;
        return NS_OK;
    }

    wxPoint pos = m_wnd->GetPosition();
    wxSize size = m_wnd->GetSize();

    if (x)
        *x = pos.x;
    if (y)
        *y = pos.y;
    if (cx)
        *cx = size.x;
    if (cy)
        *cy = size.y;

    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::GetSiteWindow(void** site_window)
{
    NS_ENSURE_ARG_POINTER(site_window);
    #ifdef __WXGTK_
    *site_window = (void*)m_wnd->m_wxwindow;
    #else
    *site_window = (void*)m_wnd->GetHandle();
    #endif
    return NS_OK;
}

// nsIInterfaceRequestor::GetInterface()
NS_IMETHODIMP BrowserChrome::GetInterface(const nsIID& IID, void** instance_ptr)
{
    if(IID.Equals(NS_GET_IID(nsIDOMWindow)))
    {
        if (m_web_browser)
            return m_web_browser->GetContentDOMWindow((nsIDOMWindow**)instance_ptr);
        return NS_ERROR_NOT_INITIALIZED;
    }

    return QueryInterface(IID, instance_ptr);
}

// nsIWebProgressListener::OnProgressChange()
NS_IMETHODIMP BrowserChrome::OnProgressChange(nsIWebProgress* progress,
                                              nsIRequest* request,
                                              PRInt32 curSelfProgress,
                                              PRInt32 maxSelfProgress,
                                              PRInt32 curTotalProgress,
                                              PRInt32 maxTotalProgress)
{
    return NS_OK;
}

// nsIWebProgressListener::OnStateChange()
NS_IMETHODIMP BrowserChrome::OnStateChange(nsIWebProgress* progress,
                                           nsIRequest* request,
                                           PRUint32 progress_state_flags,
                                           nsresult status)
{
    if (!m_wnd)
        return NS_OK;

    wxWebEvent evt(wxEVT_WEB_STATECHANGE, m_wnd->GetId());
    evt.SetEventObject(m_wnd);
    
    int state = wxWEB_STATE_NONE;
    int res = wxWEB_RESULT_NONE;
    
    if (progress_state_flags & STATE_STOP)
        state |= wxWEB_STATE_STOP;
    if (progress_state_flags & STATE_START)
        state |= wxWEB_STATE_START;
    if (progress_state_flags & STATE_REDIRECTING)
        state |= wxWEB_STATE_REDIRECTING;
    if (progress_state_flags & STATE_TRANSFERRING)
        state |= wxWEB_STATE_TRANSFERRING;
    if (progress_state_flags & STATE_NEGOTIATING)
        state |= wxWEB_STATE_NEGOTIATING;

    if (progress_state_flags & STATE_IS_REQUEST)
        state |= wxWEB_STATE_IS_REQUEST;
    if (progress_state_flags & STATE_IS_DOCUMENT)
        state |= wxWEB_STATE_IS_DOCUMENT;
    if (progress_state_flags & STATE_IS_NETWORK)
        state |= wxWEB_STATE_IS_NETWORK;
    if (progress_state_flags & STATE_IS_WINDOW)
        state |= wxWEB_STATE_IS_WINDOW;
    
    /*
    if (status == NS_OK)
        res = wxWEB_RESULT_SUCCESS;
     else if (status == NS_ERROR_UNKNOWN_PROTOCOL)
        res = wxWEB_RESULT_UNKNOWN_PROTOCOL;
     else if (status == NS_ERROR_FILE_NOT_FOUND)
        res = wxWEB_RESULT_FILE_NOT_FOUND;
     else if (status == NS_ERROR_UNKNOWN_HOST)
        res = wxWEB_RESULT_UNKNOWN_HOST;
     else if (status == NS_ERROR_CONNECTION_REFUSED)
        res = wxWEB_RESULT_CONNECTION_REFUSED;
     else if (status == NS_ERROR_NET_INTERRUPT)
        res = wxWEB_RESULT_NET_INTERRUPT;
     else if (status == NS_ERROR_NET_TIMEOUT)
        res = wxWEB_RESULT_NET_TIMEOUT;
     else if (status == NS_ERROR_MALFORMED_URI)
        res = wxWEB_RESULT_MALFORMED_URI;
     else if (status == NS_ERROR_REDIRECT_LOOP)
        res = wxWEB_RESULT_REDIRECT_LOOP;
     else if (status == NS_ERROR_UNKNOWN_SOCKET_TYPE)
        res = wxWEB_RESULT_UNKNOWN_SOCKET_TYPE;
     else if (status == NS_ERROR_NET_RESET)
        res = wxWEB_RESULT_NET_RESET;
     else if (status == NS_ERROR_DOCUMENT_NOT_CACHED)
        res = wxWEB_RESULT_DOCUMENT_NOT_CACHED;
     else if (status == NS_ERROR_DOCUMENT_IS_PRINTMODE)
        res = wxWEB_RESULT_DOCUMENT_IS_PRINTMODE;
     else if (status == NS_ERROR_PORT_ACCESS_NOT_ALLOWED)
        res = wxWEB_RESULT_PORT_ACCESS_NOT_ALLOWED;
     else if (status == NS_ERROR_UNKNOWN_PROXY_HOST)
        res = wxWEB_RESULT_UNKNOWN_PROXY_HOST;
     else if (status == NS_ERROR_PROXY_CONNECTION_REFUSED)
        res = wxWEB_RESULT_PROXY_CONNECTION_REFUSED;
    */
        
    evt.SetState(state);
    evt.SetResult(res);
    m_wnd->GetEventHandler()->ProcessEvent(evt);
    
    return NS_OK;
}

// nsIWebProgressListener::OnLocationChange()
NS_IMETHODIMP BrowserChrome::OnLocationChange(nsIWebProgress* progress,
                                              nsIRequest* request,
#if MOZILLA_VERSION_1 >= 11
                     nsIURI* location,
                     PRUint32 aflags)
#else
                     nsIURI* location)
#endif
{
    if (!m_wnd)
        return NS_OK;

    nsEmbedCString url;
    location->GetSpec(url);

    wxWebEvent evt(wxEVT_WEB_LOCATIONCHANGE, m_wnd->GetId());
    evt.SetEventObject(m_wnd);
    evt.SetString(ns2wx(url));
    m_wnd->GetEventHandler()->ProcessEvent(evt);
    
    return NS_OK;
}

// nsIWebProgressListener::OnStatusChange()
NS_IMETHODIMP BrowserChrome::OnStatusChange(nsIWebProgress* progress,
                                            nsIRequest* request,
                                            nsresult status,
                                            const char16_t* message)
{
    if (!m_wnd)
        return NS_OK;

    wxWebEvent evt(wxEVT_WEB_STATUSCHANGE, m_wnd->GetId());
    evt.SetEventObject(m_wnd);
    evt.SetString(ns2wx(message));
    m_wnd->GetEventHandler()->ProcessEvent(evt);

    return NS_OK;
}

// nsIWebProgressListener::OnSecurityChange()
NS_IMETHODIMP BrowserChrome::OnSecurityChange(nsIWebProgress* progress,
                                              nsIRequest* request,
                                              PRUint32 state)
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::OnShowContextMenu(PRUint32 context_flags,
                                               nsIContextMenuInfo* info)
{
    if (!m_wnd)
        return NS_OK;

    wxWebEvent evt(wxEVT_WEB_SHOWCONTEXTMENU, m_wnd->GetId());
    
    nsCOMPtr<nsIDOMEvent> mouse_event;
    info->GetMouseEvent(getter_AddRefs(mouse_event));
    
    if (mouse_event)
    {
        nsCOMPtr<nsIDOMEventTarget> target;
        mouse_event->GetTarget(getter_AddRefs(target));
        
        evt.m_target_node.m_data->setNode(target);

        // note: following parallels code in mouse event; see if the target
        // or any of its parent nodes are anchors and if so, set the event href;
        // the reason we have to look through the parents of the target is
        // because the target may be a child node of the element where the
        // actual href is specified, and as a result, may not itself contain
        // the href; this happens, for example, when portions of the text in a
        // hyperlink are bold

        nsCOMPtr<nsIDOMNode> node = do_QueryInterface(target);
        node = GetAnchor(node);
        
        nsCOMPtr<nsIDOMHTMLAnchorElement> anchor = do_QueryInterface(node);

        if (anchor)
        {
            nsEmbedString nss;
            anchor->GetHref(nss);
            evt.SetHref(ns2wx(nss));
        }
    }

    evt.SetEventObject(m_wnd);
    m_wnd->GetEventHandler()->ProcessEvent(evt);
    
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::OnShowTooltip(PRInt32 x,
                                           PRInt32 y,
                                           const char16_t* tip_text)
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::OnHideTooltip()
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::Blur()
{
    return NS_OK;
}

NS_IMETHODIMP BrowserChrome::HandleEvent(nsIDOMEvent* evt)
{
    if (!m_wnd)
        return NS_OK;

    nsEmbedString nsstr_type;
    evt->GetType(nsstr_type);
    wxString type = ns2wx(nsstr_type);

    if (type == wxT("dragdrop"))
    {

    }
    
    
    if (type == wxT("DOMContentLoaded"))
    {
        m_wnd->OnDOMContentLoaded();
        
        nsCOMPtr<nsIURI> uri, uri2;
        m_wnd->m_ptrs->m_web_navigation->GetCurrentURI(getter_AddRefs(uri));
        if (!uri)
            return NS_OK;
    
        // skip https
        bool b = false;
        uri->SchemeIs("https", &b);
        if (b)
            return NS_OK;
        uri->SchemeIs("file", &b);
        if (b)
            return NS_OK;
            
        nsEmbedCString url_str;
        uri->Resolve(NS_LITERAL_CSTRING("/favicon.ico"), url_str);
        
        uri2 = nsNewURI(ns2wx(url_str));
        if (!uri2)
            return NS_OK;
            
        // let main control know that we should have a favicon
        // by now.  If we don't, load a default /favicon.ico
        m_wnd->FetchFavIcon((void*)uri2);
        
    }
    
    if (type == wxT("DOMLinkAdded"))
    {
        nsCOMPtr<nsIDOMEventTarget> target;
        evt->GetTarget(getter_AddRefs(target));

        nsCOMPtr<nsIDOMElement> element = do_QueryInterface(target);
        if (!element)
            return NS_OK;
        
        // make sure we're dealing with a link tag
        nsString value;
        wxString tagname, rel, href, spec;
        
        element->GetTagName(value);
        tagname = ns2wx(value);
        tagname.MakeLower();
        if (tagname != wxT("link"))
            return NS_OK;
            
        // make sure it has a rel attribute
        element->GetAttribute(NS_LITERAL_STRING("rel"), value);
        rel = ns2wx(value);
        rel.MakeLower();
        
        if (rel != wxT("shortcut icon") &&
            rel != wxT("icon"))
        {
            return NS_OK;
        }
        
        // get the href attribute
        element->GetAttribute(NS_LITERAL_STRING("href"), value);
        href = ns2wx(value);
       
        // now get the dom document
        nsCOMPtr<nsIDOMDocument> dom_doc;
        element->GetOwnerDocument(getter_AddRefs(dom_doc));
        nsCOMPtr<nsIDOMDocument> dom3_doc = dom_doc;
        if (!dom3_doc)
            return NS_OK;
        
        dom3_doc->GetDocumentURI(value);
        spec = ns2wx(value);
        
        nsEmbedCString chref;
        nsEmbedCString cvalue;
        wx2ns(href, chref);
        nsCOMPtr<nsIURI> doc_uri = nsNewURI(spec);
        doc_uri->Resolve(chref, cvalue);
        
        wxString favicon_url = ns2wx(cvalue);
        nsCOMPtr<nsIURI> result_uri = nsNewURI(favicon_url);
        m_wnd->FetchFavIcon((void*)result_uri);
        return NS_OK;
    }
    

    if (type == wxT("mousedown") ||
        type == wxT("mouseup") ||
        type == wxT("dblclick") ||
        type == wxT("dragdrop"))
    {
        if (type == wxT("mousedown"))
        {
            if (wxWindow::FindFocus() != m_wnd)
                m_wnd->SetFocus();
        }
        
        nsCOMPtr<nsIDOMEventTarget> target;
        evt->GetTarget(getter_AddRefs(target));
        
        nsCOMPtr<nsIDOMMouseEvent> mouse_evt = do_QueryInterface(evt);
        if (!mouse_evt)
            return NS_ERROR_NOT_IMPLEMENTED;
        
        int evtid;
        int16_t ns_button = 0;
        
        mouse_evt->GetButton(&ns_button);
        
        if (type == wxT("mousedown"))
        {
            switch (ns_button)
            {
                default:
                case 0: evtid = wxEVT_WEB_LEFTDOWN; break;
                case 1: evtid = wxEVT_WEB_MIDDLEDOWN; break;
                case 2: evtid = wxEVT_WEB_RIGHTDOWN; break;
            }
        }
         else if (type == wxT("mouseup"))
        {
            switch (ns_button)
            {
                default:
                case 0: evtid = wxEVT_WEB_LEFTUP; break;
                case 1: evtid = wxEVT_WEB_MIDDLEUP; break;
                case 2: evtid = wxEVT_WEB_RIGHTUP; break;
            }
        }
         else if (type == wxT("dblclick"))
        {
            evtid = wxEVT_WEB_LEFTDCLICK;
        }
         else if (type == wxT("dragdrop"))
        {
            evtid = wxEVT_WEB_DRAGDROP;
        }
         else
        {
            wxFAIL_MSG(wxT("NS event type needs to be mapped to wxWebConnect event type"));
        }
         
        
        // see if the target or any of its parent nodes are anchors and
        // if so, set the event href; note: the reason we have to look
        // through the parents of the target is because the target may
        // be a child node of the element where the actual href is specified,
        // and as a result, may not itself contain the href; this happens,
        // for example, when portions of the text in a hyperlink are bold
        //node = target;
        nsCOMPtr<nsIDOMNode> node = do_QueryInterface(target);
        node = GetAnchor(node);

        nsCOMPtr<nsIDOMHTMLAnchorElement> anchor = do_QueryInterface(node);

        // fill out and send a mouse event
        wxWebEvent evt(evtid, m_wnd->GetId());
        evt.m_target_node.m_data->setNode(target);
        evt.SetEventObject(m_wnd);
        if (anchor)
        {
            nsEmbedString nss;
            anchor->GetHref(nss);
            evt.SetHref(ns2wx(nss));
            
            // also, we can handle this here:  if a link was clicked, clear out
            // our favicon stuff to prepare for the next load
            m_wnd->ResetFavicon();
            
        }
        m_wnd->GetEventHandler()->ProcessEvent(evt);
    
        return NS_OK;
    }

    return NS_ERROR_NOT_IMPLEMENTED;
}


wxWebControl* GetWebControlFromBrowserChrome(nsIWebBrowserChrome* chrome)
{
    if (!chrome)
        return NULL;


    BrowserChrome* chrome_browser = static_cast <BrowserChrome*>(chrome);
    //TODO do it by do_QueryInterface

    return chrome_browser->GetWebControl();
}




///////////////////////////////////////////////////////////////////////////////
//  ContentListener class implementation
///////////////////////////////////////////////////////////////////////////////


// ContentListener implements the ns interfaces for
// content listeners and patches them through to our
// public wxWebContentHandler class

class ContentListener : public nsIURIContentListener,
                        public nsIStreamListener
{
public:

    NS_DECL_ISUPPORTS

    ContentListener(wxWebContentHandler* handler)
    {
        m_handler = handler;
    }
    
    NS_IMETHODIMP OnStartURIOpen(nsIURI* uri,
#if MOZILLA_VERSION_1 >= 10
    							bool* abort)
#else
                                 PRBool* abort)
#endif
    {
        nsEmbedCString spec;
        nsresult res = uri->GetSpec(spec);
        if (NS_FAILED(res))
            return NS_OK;
      
        m_current_uri = ns2wx(spec);

        *abort = PR_FALSE;
        return NS_OK;
    }

    NS_IMETHODIMP DoContent(
#if MOZILLA_VERSION_1 < 39
			const char* content_type,
#else
			const nsACString& /*aContentType*/,
#endif
    		bool is_content_preferred,
            nsIRequest* request,
            nsIStreamListener** content_handler,
            bool* retval)
    {
        *retval = false;
        *content_handler = static_cast<nsIStreamListener*>(this);
        (*content_handler)->AddRef();
        return NS_OK;
    }

    NS_IMETHODIMP IsPreferred(const char* content_type,
                              char** desired_content_type,
#if MOZILLA_VERSION_1 >= 10
                              bool* retval)
    {
        return CanHandleContent(content_type, true, desired_content_type, retval);
    }
#else
                              PRBool* retval)
    {
        return CanHandleContent(content_type, PR_TRUE, desired_content_type, retval);
    }
#endif


    NS_IMETHODIMP CanHandleContent(const char* _content_type,
#if MOZILLA_VERSION_1 >= 10
    		bool is_content_preferred,
    		char** desired_content_type,
    		bool* retval)
#else
            PRBool is_content_preferred,
            char** desired_content_type,
            PRBool* retval)
#endif
    {
        wxString content_type = wxString::FromAscii(_content_type);
        content_type.MakeLower();
#if MOZILLA_VERSION_1 >= 10
        *retval = m_handler->CanHandleContent(m_current_uri, content_type) ? true : false;
#else
        *retval = m_handler->CanHandleContent(m_current_uri, content_type) ? PR_TRUE : PR_FALSE;
#endif
        return NS_OK;
     }

    NS_IMETHODIMP GetLoadCookie(nsISupports** load_cookie)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP SetLoadCookie(nsISupports* load_cookie)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP GetParentContentListener(nsIURIContentListener** parent_content_listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP SetParentContentListener(nsIURIContentListener* parent_content_listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    // nsIStreamListener
    
    NS_IMETHODIMP OnStartRequest(nsIRequest* request, nsISupports* context)
    {
        nsCOMPtr<nsIRequest> sp = request;
        nsCOMPtr<nsIChannel> channel = do_QueryInterface(sp);
        nsCOMPtr<nsIURI> uri;
        channel->GetURI(getter_AddRefs(uri));
        
        nsEmbedCString spec;

        if (NS_FAILED(uri->GetSpec(spec)))
            return NS_OK;
            
        wxString url = ns2wx(spec);
        
        m_handler->OnStartRequest(url);
        return NS_OK;
    }
    
    NS_IMETHODIMP OnStopRequest(nsIRequest* request, nsISupports* context, nsresult status_code)
    {
        m_handler->OnStopRequest();
        return NS_OK;
    }
    
    NS_IMETHODIMP OnDataAvailable(nsIRequest* channel,
                                  nsISupports* context,
                                  nsIInputStream* in_stream,
                                  PRUint32 source_offset,
                                  PRUint32 count)
    {
        unsigned char* buf = new unsigned char[count];
        
        PRUint32 read;
        in_stream->Read((char*)buf, count, &read);
        
        m_handler->OnData(buf, count);
        
        delete[] buf;
        
        return NS_OK;
    }
    
private:

    wxWebContentHandler* m_handler;
    wxString m_current_uri;
protected:
    virtual ~ContentListener()
    {
    }
};

NS_IMPL_ADDREF(ContentListener)
NS_IMPL_RELEASE(ContentListener)

NS_INTERFACE_MAP_BEGIN(ContentListener)
    NS_INTERFACE_MAP_ENTRY(nsIURIContentListener)
    NS_INTERFACE_MAP_ENTRY(nsIStreamListener)
NS_INTERFACE_MAP_END




///////////////////////////////////////////////////////////////////////////////
//  MainURIListener class implementation
///////////////////////////////////////////////////////////////////////////////


class MainURIListener : public nsIURIContentListener

{
    friend class wxWebControl;

public:

    NS_DECL_ISUPPORTS

    MainURIListener(wxWebControl* wnd, nsIWebBrowser* browser)
    {
        m_wnd = wnd;
        m_docshell_uri_listener = do_GetInterface(browser);
    }
    
    NS_IMETHODIMP OnStartURIOpen(nsIURI* uri,
#if MOZILLA_VERSION_1 >=10
                                 bool* abort)
#else
                                 PRBool* abort)
#endif
    {
        if (!m_wnd)
            return NS_OK;
        
        wxASSERT(uri);
        wxASSERT(abort);
        
        // set default behavior
#if MOZILLA_VERSION_1 >=10
        *abort = false;
#else
        *abort = PR_FALSE;
#endif
        
        nsresult res;
        
        nsEmbedCString spec;
        res = uri->GetSpec(spec);
        if (NS_FAILED(res))
        {
            // should never happen, but allow the open if
            // something went wrong while getting the uri spec
            return NS_OK;
        }
        
        m_current_url = ns2wx(spec);
        
        wxWebEvent evt(wxEVT_WEB_OPENURI, m_wnd->GetId());
        evt.SetEventObject(m_wnd);
        evt.SetHref(m_current_url);
        if (m_wnd->GetEventHandler()->ProcessEvent(evt))
        {
            if (!evt.IsAllowed())
            {
#if MOZILLA_VERSION_1 >= 10
            	*abort = true;
#else
                *abort = PR_TRUE;
#endif
                return NS_OK;
            }
        }
        
        
        // let all other content listeners know about this
        ContentListenerPtrArray& arr = g_gecko_engine.GetContentListeners();
        int i = 0, count = arr.GetCount();
        for (i = 0; i < count; ++i)
        {
            arr.Item(i)->OnStartURIOpen(uri, abort);
            if (*abort)
                return NS_OK;
        }
        
        
        return NS_OK;
    }

    NS_IMETHODIMP DoContent(
#if MOZILLA_VERSION_1 < 39
		const char* content_type,
#else
		const nsACString& ,
#endif
        bool is_content_preferred,
        nsIRequest* request,
        nsIStreamListener** content_handler,
        bool* retval)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }
#if MOZILLA_VERSION_1 >= 10
    NS_IMETHODIMP IsPreferred(const char* content_type,
                                  char** desired_content_type,
                                  bool* retval)
     {
      return CanHandleContent(content_type, true, desired_content_type, retval);
     }
#else
    NS_IMETHODIMP IsPreferred(const char* content_type,
                              char** desired_content_type,
                              PRBool* retval)
    {
     return CanHandleContent(content_type, PR_TRUE, desired_content_type, retval);
    }
#endif

    NS_IMETHODIMP CanHandleContent(const char* _content_type,
#if MOZILLA_VERSION_1 >= 10
    		bool is_content_preferred,
    		char** desired_content_type,
    		bool* retval)
#else
                                   PRBool is_content_preferred,
                                   char** desired_content_type,
                                   PRBool* retval)
#endif
    {
        if (!m_wnd)
        {
            // no window, so return false (can't handle content)
#if MOZILLA_VERSION_1 >= 10
        	*retval = false;
#else
            *retval = PR_FALSE;
#endif
            return NS_OK;
        }

        wxString content_type = wxString::FromAscii(_content_type);
        content_type.MakeLower();
        
        // this event will decide if the _browser_ should handle the content
        // or not.  If the browser doesn't handle the content, the content
        // listener(s) as specified by wxWebControl::AddContentHandler
        // are given a chance.
        
        // If a type conversion is specified, the browser will ask again
        // if the content can be handled
        
        wxWebEvent evt(wxEVT_WEB_SHOULDHANDLECONTENT, m_wnd->GetId());
        evt.SetEventObject(m_wnd);
        evt.SetContentType(content_type);
        evt.SetHref(m_current_url);
        if (m_wnd->GetEventHandler()->ProcessEvent(evt))
        {
            if (!evt.GetSkipped())
            {
#if MOZILLA_VERSION_1 >=10
            	*retval = evt.m_should_handle;
#else
                *retval = evt.m_should_handle ? PR_TRUE : PR_FALSE;
#endif
                wxString output_content_type = evt.m_output_content_type;
                output_content_type.MakeLower();
                if (output_content_type.Length() > 0 &&
                    output_content_type != content_type)
                {
                    *desired_content_type = (char*)NS_Alloc(output_content_type.Length()+1);
                    strcpy(*desired_content_type, (const char*)output_content_type.mbc_str());
                }
                
                return NS_OK;
            }
        }

        // default processing
        return m_docshell_uri_listener->CanHandleContent(_content_type,
                                                  is_content_preferred,
                                                  desired_content_type,
                                                  retval);
    }

    NS_IMETHODIMP GetLoadCookie(nsISupports** load_cookie)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP SetLoadCookie(nsISupports* load_cookie)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP GetParentContentListener(nsIURIContentListener** parent_content_listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

    NS_IMETHODIMP SetParentContentListener(nsIURIContentListener* parent_content_listener)
    {
        return NS_ERROR_NOT_IMPLEMENTED;
    }

private:

    wxWebControl* m_wnd;
    wxString m_current_url;
    nsCOMPtr<nsIURIContentListener> m_docshell_uri_listener;

protected:
    virtual ~MainURIListener()
    {
    }
};


NS_IMPL_ADDREF(MainURIListener)
NS_IMPL_RELEASE(MainURIListener)

NS_INTERFACE_MAP_BEGIN(MainURIListener)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIURIContentListener)
    NS_INTERFACE_MAP_ENTRY(nsIURIContentListener)
NS_INTERFACE_MAP_END




///////////////////////////////////////////////////////////////////////////////
//  WindowCreator implementation
///////////////////////////////////////////////////////////////////////////////


class WindowCreator : public nsIWindowCreator2

{
public:
    WindowCreator()
    {
        //NS_INIT_ISUPPORTS();
    }
    
    //virtual ~WindowCreator()
    
    NS_DECL_ISUPPORTS
    NS_DECL_NSIWINDOWCREATOR
    NS_DECL_NSIWINDOWCREATOR2

protected:
    ~WindowCreator()
    {
    }
};

NS_IMPL_ISUPPORTS(WindowCreator, nsIWindowCreator, nsIWindowCreator2)
//NS_IMPL_ISUPPORTS1(WindowCreator, nsIWindowCreator)


NS_IMETHODIMP
WindowCreator::CreateChromeWindow(nsIWebBrowserChrome* parent,
                                 PRUint32 chrome_flags,
                                 nsIWebBrowserChrome** retval)
{
	wxWebControl* web_control = GetWebControlFromBrowserChrome(parent);
    if (!web_control)
    {
        // no web control, so we can't create a new window
        // (this shouldn't happen)
        return NS_ERROR_FAILURE;
    }

    int wx_chrome_flags = 0;


    // TODO: add more flags as necessary
    if (chrome_flags & nsIWebBrowserChrome::CHROME_MODAL)
        wx_chrome_flags |= wxWEB_CHROME_MODAL;
    if (chrome_flags & nsIWebBrowserChrome::CHROME_WINDOW_RESIZE)
        wx_chrome_flags |= wxWEB_CHROME_RESIZABLE;
    if (chrome_flags & nsIWebBrowserChrome::CHROME_CENTER_SCREEN)
        wx_chrome_flags |= wxWEB_CHROME_CENTER;



    wxWebEvent evt(wxEVT_WEB_CREATEBROWSER, web_control->GetId());
    evt.SetEventObject(web_control);
    evt.SetCreateChromeFlags(wx_chrome_flags);
    web_control->GetEventHandler()->ProcessEvent(evt);

    if (!evt.IsAllowed())
    {
        // owner blocked creation of new browser
        // window with a veto
        return NS_ERROR_FAILURE;
    }

    if (evt.m_create_browser)
    {
        // owner supplied its own window
        *retval = static_cast<nsIWebBrowserChrome*>(evt.m_create_browser->m_chrome);
        NS_ADDREF(*retval);

        return NS_OK;
    }
     else
    {
        // owner did nothing, so we'll create our own window
        wxWebFrame* frame = new wxWebFrame(NULL, -1, wxT(""), wxPoint(50, 50), wxSize(550, 500));
        wxWebControl* ctrl = frame->GetWebControl();
        frame->Show(true);

        *retval = static_cast<nsIWebBrowserChrome*>(ctrl->m_chrome);
        NS_ADDREF(*retval);
    }

    return NS_OK;
}

NS_IMETHODIMP
WindowCreator::CreateChromeWindow2(nsIWebBrowserChrome *aParent,
                                   uint32_t aChromeFlags,
								   uint32_t ,
                                   nsIURI * ,
#if MOZILLA_VERSION_1 >=32
								   nsITabParent * ,
#endif
								   bool * ,
                                   nsIWebBrowserChrome **_retval)
{
    return CreateChromeWindow(aParent, aChromeFlags, _retval);
}



///////////////////////////////////////////////////////////////////////////////
//  PluginEnumerator implementation
///////////////////////////////////////////////////////////////////////////////


class PluginEnumerator : public nsISimpleEnumerator

{
public:

    NS_DECL_ISUPPORTS
    
    PluginEnumerator()
    {
        m_cur_item = 0;
    }

#if MOZILLA_VERSION_1 >= 10
    NS_IMETHODIMP HasMoreElements(bool* retval)
        {
            if (!retval)
                return NS_ERROR_NULL_POINTER;

            if (m_cur_item >= m_paths.GetCount())
                *retval = false;
                 else
                *retval = true;

            return NS_OK;
        }
#else
    NS_IMETHODIMP HasMoreElements(PRBool* retval)
    {
        if (!retval)
            return NS_ERROR_NULL_POINTER;
        
        if (m_cur_item >= m_paths.GetCount())
            *retval = PR_FALSE;
             else
            *retval = PR_TRUE;
            
        return NS_OK;
    }
#endif

    NS_IMETHODIMP GetNext(nsISupports** retval)
    {
        if (!retval)
            return NS_ERROR_NULL_POINTER;

        nsCOMPtr<nsIFile> file;
        nsresult res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_paths[m_cur_item].mbc_str()), PR_TRUE, getter_AddRefs(file));
        if (NS_FAILED(res))
            return NS_ERROR_NULL_POINTER;
        
        //FIXME
        //*retval = getter_AddRefs(file);
        //(*retval)->AddRef();

        ++m_cur_item;
        
        return NS_OK;
    }

    void SetPaths(const wxArrayString& paths)
    {
        m_paths = paths;
    }
    
private:

    wxArrayString m_paths;
    size_t m_cur_item;
    
protected:
    virtual ~PluginEnumerator()
    {
    }

};

NS_IMPL_ISUPPORTS(PluginEnumerator, nsISimpleEnumerator)




///////////////////////////////////////////////////////////////////////////////
//  PluginListProvider implementation
///////////////////////////////////////////////////////////////////////////////


class PluginListProvider : public nsIDirectoryServiceProvider2

{
public:

    NS_DECL_ISUPPORTS

    PluginListProvider()
    {
    }

    
    void AddPaths(nsCOMPtr<nsISimpleEnumerator> paths)
    {
#if MOZILLA_VERSION_1 >=10
    	bool more = false;
#else
        PRBool more = PR_FALSE;
#endif
        
        while (1)
        {
            paths->HasMoreElements(&more);
            if (!more)
                break;
            //FIXME
            nsCOMPtr<nsISupports> element;
            paths->GetNext(getter_AddRefs(element));
            if (!element)
                continue;
                
            //nsCOMPtr<nsIFile> file = nsToSmart(element);
            
            nsCOMPtr<nsIFile> file;
            //element->Release();
            
            if (file)
            {
                nsEmbedCString path;
                file->GetNativePath(path);

                wxString wxpath = ns2wx(path);
                m_paths.Add(wxpath);
            }
        }
        
    }
    
    void AddPath(const wxString& path)
    {
        m_paths.Add(path);
    }
#if MOZILLA_VERSION_1 >= 10
    NS_IMETHODIMP GetFile(const char* prop, bool* persistant, nsIFile** retval)
#else
    NS_IMETHODIMP GetFile(const char* prop, PRBool* persistant, nsIFile** retval)
#endif
    {
        if (!retval)
            return NS_ERROR_NULL_POINTER;
            
        // nothing returned by this method;
        // let next directory provider handle the request
        return NS_ERROR_FAILURE;
    }

    NS_IMETHODIMP GetFiles(const char* prop, nsISimpleEnumerator** retval)
    {
        if (!retval)
            return NS_ERROR_NULL_POINTER;

        if (0 == strcmp(prop, "APluginsDL"))
        {
            PluginEnumerator* enumerator = new PluginEnumerator;
            enumerator->SetPaths(m_paths);
            *retval = enumerator;
            (*retval)->AddRef();
            return NS_OK;
        }
        
        // let next directory provider handle the request
        return NS_ERROR_FAILURE;
    }
  
private:

    wxArrayString m_paths;

protected:
    virtual ~PluginListProvider()
    {
    }
};


NS_IMPL_ADDREF(PluginListProvider)
NS_IMPL_RELEASE(PluginListProvider)

NS_INTERFACE_MAP_BEGIN(PluginListProvider)
    NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIDirectoryServiceProvider)
    NS_INTERFACE_MAP_ENTRY(nsIDirectoryServiceProvider)
    NS_INTERFACE_MAP_ENTRY(nsIDirectoryServiceProvider2)
NS_INTERFACE_MAP_END

///////////////////////////////////////////////////////////////////////////////
//  SimpleGeckoEngine class implementation
///////////////////////////////////////////////////////////////////////////////


SimpleGeckoEngine::SimpleGeckoEngine()
{
    m_ok = false;
    //m_plugin_provider = new PluginListProvider;
    //m_plugin_provider->AddRef();

    //path to xulrunner binary!
    m_gecko_path = wxString::FromUTF8(defXULPathDir);
}

SimpleGeckoEngine::~SimpleGeckoEngine()
{
    //m_plugin_provider->Release();
}

bool SimpleGeckoEngine::IsOk() const
{
    return m_ok;
}
void SimpleGeckoEngine::Uninit()
{
	if(IsOk())
	{
		NS_LogInit();
		if(!XRE_TermEmbedding) return;
		XRE_TermEmbedding();
		nsresult res;
		//res = XPCOMGlueShutdown();
		NS_LogTerm();
	}
}

/*
 * Set path to gecko binary, example:
 * linux: somepath/xulrunner-31/bin
 * windows: somepath\\xulrunner-31\bin
 * OSX: somepath/xulrunner-31/bin/XUL.framework/Versions/Current
 */
/*
void SimpleGeckoEngine::SetEnginePath(const char* path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));
    wxString wxPath = wxString(path);
    m_gecko_path = wxPath;
}

void SimpleGeckoEngine::SetStoragePath(const char* path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));

    wxString wxPath = wxString(path);
    m_storage_path = wxPath;

    wxChar path_separator = wxFileName::GetPathSeparator();
    m_history_filename = m_storage_path;
    if (m_history_filename.IsEmpty() || m_history_filename.Last() != path_separator)
        m_history_filename += path_separator;

    m_history_filename += wxT("kwkh01.dat");
}*/

void SimpleGeckoEngine::SetEnginePath(const wxString& path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));
    m_gecko_path = path;
}

void SimpleGeckoEngine::SetStoragePath(const wxString& path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));

    m_storage_path = path;

    wxChar path_separator = wxFileName::GetPathSeparator();
    m_history_filename = m_storage_path;
    if (m_history_filename.IsEmpty() || m_history_filename.Last() != path_separator)
        m_history_filename += path_separator;

    m_history_filename += wxT("kwkh01.dat");
}

bool SimpleGeckoEngine::Init()
{
    nsresult res;

    if (IsOk())
        return true;

    //if (m_gecko_path.IsEmpty())
    //    return false;

    if (m_storage_path.IsEmpty())
    {
        wxLogNull log;

        wxString default_storage_path = wxStandardPaths::Get().GetTempDir();
        printf("Storage path1: %s\n", (const char*)default_storage_path.mb_str());
        wxChar path_separator = wxFileName::GetPathSeparator();
        if (default_storage_path.IsEmpty() || default_storage_path.Last() != path_separator)
            default_storage_path += path_separator;
        printf("Storage path2: %s\n", (const char*)default_storage_path.mb_str());
        //default_storage_path += wxString(wxT("kwkh01.tmp"));
        default_storage_path += wxString::FromUTF8("kwkh01.tmp");
        //wxString suf1 = wxString::FromUTF8("kwkh01.tmp");
        //printf("Storage path22: %s\n", (const char*)suf1.mb_str());

        printf("Storage path3: %s\n", (const char*)default_storage_path.mb_str());

#ifdef WIN32
        ::wxMkDir(default_storage_path);
#else
#if wxMAJOR_VERSION == 3
        ::wxMkDir(default_storage_path, 0700);
#else
        ::wxMkDir((const char*)default_storage_path.mbc_str(), 0700);
#endif
#endif

        m_storage_path = default_storage_path;
    }

    SetStoragePath(m_storage_path);

    //std::string xpcom_path = std::string(GECKO_SDK_PATH_CONFIG);

    char path_separator = (char)wxFileName::GetPathSeparator();
    std::string gecko_path = (const char*)m_gecko_path.mb_str();
    std::string xpcom_path = gecko_path;
    if (xpcom_path.empty() || xpcom_path[xpcom_path.length()-1] != path_separator)
        xpcom_path += path_separator;
    #if defined __WXMSW__
    xpcom_path += "xpcom.dll";
    #elif defined __WXMAC__
    xpcom_path += "libxpcom.dylib";
    #else
    xpcom_path += "libxpcom.so";
    #endif

    std::cout << "xpcom: " << xpcom_path << std::endl;

    res = XPCOMGlueStartup(xpcom_path.c_str());
    if (NS_FAILED(res))
        return false;

    NS_LogInit();

        // load XUL functions
    nsDynamicFunctionLoad nsFuncs[] = {
                {"XRE_InitEmbedding2", (NSFuncPtr*)&XRE_InitEmbedding2},
                {"XRE_TermEmbedding", (NSFuncPtr*)&XRE_TermEmbedding},
                {"XRE_NotifyProfile", (NSFuncPtr*)&XRE_NotifyProfile},
                {"XRE_LockProfileDirectory", (NSFuncPtr*)&XRE_LockProfileDirectory},
                {0, 0}
    };

    res = XPCOMGlueLoadXULFunctions(nsFuncs);
       if (NS_FAILED(res)) {
            return false;
       }


    nsCOMPtr<nsIFile> gre_dir;
#if MOZILLA_VERSION_1 >=10
    res = NS_NewNativeLocalFile(nsCString(defXULPathDir), true, getter_AddRefs(gre_dir));
#else
    res = NS_NewNativeLocalFile(nsDependentCString(gecko_path.c_str()), PR_TRUE, getter_AddRefs(gre_dir));
#endif
    if (NS_FAILED(res))
         return false;

    nsCOMPtr<nsIFile> prof_dir;
#if MOZILLA_VERSION_1 >=10
#if wxMAJOR_VERSION == 3
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mb_str(wxConvUTF8)), true, getter_AddRefs(prof_dir));
    std::cout << "Storage path: " << std::string((const char*)m_storage_path.mb_str(wxConvUTF8)) << std::endl;
#else
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mbc_str()), true, getter_AddRefs(prof_dir));
#endif
#else
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mbc_str()), PR_TRUE, getter_AddRefs(prof_dir));
#endif
    if (NS_FAILED(res))
            return false;

    // init embedding
#if MOZILLA_VERSION_1 < 2
    const nsStaticModuleInfo* aComps = 0;
    int aNumComps = 0;

	//res = XRE_InitEmbedding(gre_dir, prof_dir,
	//		const_cast<wxDirSrvProvider*>(&DirectoryProvider),aComps, aNumComps);
	res = XRE_InitEmbedding(gre_dir, prof_dir,
			nsnull,aComps, aNumComps);

#else
    //res = XRE_InitEmbedding2(gre_dir, prof_dir, const_cast<wxDirSrvProvider*>(&DirectoryProvider));
    res = XRE_InitEmbedding2(gre_dir, prof_dir, nullptr);
#endif
    if (NS_FAILED(res))
            return false;

    // initialize profile:
    XRE_NotifyProfile();

    NS_LogTerm();

    // set the window creator
    //nsCOMPtr<nsIWindowCreator> wnd_creator = static_cast<nsIWindowCreator*>(new WindowCreator);
    nsCOMPtr<WindowCreator> wnd_creator = new WindowCreator();

    //set window watcher
    nsCOMPtr<nsIWindowWatcher> window_watcher = nsGetWindowWatcherService();
    //nsCOMPtr<nsIWindowWatcher> window_watcher(do_GetService(NS_WINDOWWATCHER_CONTRACTID));
    if (!window_watcher)
        return false;

    window_watcher->SetWindowCreator(wnd_creator);



    return true;
}

///////////////////////////////////////////////////////////////////////////////
//  GeckoEngine class implementation
///////////////////////////////////////////////////////////////////////////////


GeckoEngine::GeckoEngine()
{
    m_ok = false;
    m_plugin_provider = new PluginListProvider;
    m_plugin_provider->AddRef();

    //path to xulrunner binary!
    m_gecko_path = wxString::FromUTF8(defXULPathDir);
}

GeckoEngine::~GeckoEngine()
{
    m_plugin_provider->Release();
}

bool GeckoEngine::IsOk() const
{
    return m_ok;
}
void GeckoEngine::Uninit()
{
	if(IsOk())
	{
		NS_LogInit();
		if(!XRE_TermEmbedding) return;
		XRE_TermEmbedding();
		nsresult res;
		//res = XPCOMGlueShutdown();
		NS_LogTerm();
	}
}

/*
 * Set path to gecko binary, example:
 * linux: somepath/xulrunner-31/bin
 * windows: somepath\\xulrunner-31\bin
 * OSX: somepath/xulrunner-31/bin/XUL.framework/Versions/Current
 */

void GeckoEngine::SetEnginePath(const wxString& path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));
    m_gecko_path = path;
}

void GeckoEngine::SetStoragePath(const wxString& path)
{
    wxASSERT_MSG(!m_ok, wxT("This must be called before the first wxWebControl is instantiated"));
    
    m_storage_path = path;
    
    wxChar path_separator = wxFileName::GetPathSeparator();
    m_history_filename = m_storage_path;
    if (m_history_filename.IsEmpty() || m_history_filename.Last() != path_separator)
        m_history_filename += path_separator;
    
    m_history_filename += wxT("kwkh01.dat");
}


// DelayedWindowDestroy is a utility class that will destroy
// a window a specified number of seconds later

class DelayedWindowDestroy : public wxTimer
{
public:
    DelayedWindowDestroy(wxWindow* wnd, int seconds)
    {
        m_wnd = wnd;
        Start(seconds*1000, wxTIMER_ONE_SHOT);
    }
    
    void Notify()
    {
        m_wnd->Destroy();
        
        if (!wxPendingDelete.Member(this))
            wxPendingDelete.Append(this);
  
    }
    
private:
    wxWindow* m_wnd;
};


class DelayedWindowCreator : public wxTimer
{
public:
    DelayedWindowCreator(wxWebControl* ctrl, int seconds)
    {
    	std::cout << "Start delayed browser creation" << std::endl;
    	m_ctrl = ctrl;
        Start(seconds*1000, wxTIMER_ONE_SHOT);
    }

    void Notify()
    {
    	std::cout << "Run create browser" << std::endl;
    	m_ctrl->CreateBrowser();

        //if (!wxPendingDelete.Member(this))
        //    wxPendingDelete.Append(this);

    }

private:
    wxWebControl* m_ctrl;
};


class DelayedFirstURILoader : public wxTimer
{
	//friend class wxWebControl;
public:
	DelayedFirstURILoader(wxWebControl* ctrl, int seconds)
    {
    	std::cout << "Start delayed first uri loading" << std::endl;
    	m_ctrl = ctrl;
        Start(seconds*1000, wxTIMER_ONE_SHOT);
    }

    void Notify()
    {
    	std::cout << "Run DelayedFirstURILoader" << std::endl;
    	m_ctrl->m_loadurl_ready = true;

        //now check and load uri
        if(m_ctrl->m_loadurl_pending) {
        	m_ctrl->m_loadurl_pending = false;
        	m_ctrl->RealOpenURI(m_ctrl->m_pending_uri, m_ctrl->m_pending_load_flags, m_ctrl->m_pending_post_data);
        }

        //if (!wxPendingDelete.Member(this))
        //    wxPendingDelete.Append(this);

    }

private:
    wxWebControl* m_ctrl;
};


// initialize the gecko engine; his is automatically called
// when wxWebControl objects are created.

bool GeckoEngine::Init()
{
    nsresult res;

    if (IsOk())
        return true;
    
    //if (m_gecko_path.IsEmpty())
    //    return false;
    
    if (m_storage_path.IsEmpty())
    {
        wxLogNull log;

        wxString default_storage_path = wxStandardPaths::Get().GetTempDir();
        printf("Storage path1: %s\n", (const char*)default_storage_path.mb_str());
        wxChar path_separator = wxFileName::GetPathSeparator();
        if (default_storage_path.IsEmpty() || default_storage_path.Last() != path_separator)
            default_storage_path += path_separator;
        printf("Storage path2: %s\n", (const char*)default_storage_path.mb_str());
        //default_storage_path += wxString(wxT("kwkh01.tmp"));
        default_storage_path += wxString::FromUTF8("kwkh01.tmp");
        //wxString suf1 = wxString::FromUTF8("kwkh01.tmp");
        //printf("Storage path22: %s\n", (const char*)suf1.mb_str());
        
        printf("Storage path3: %s\n", (const char*)default_storage_path.mb_str());

#ifdef WIN32
        ::wxMkDir(default_storage_path);
#else
#if wxMAJOR_VERSION == 3
        ::wxMkDir(default_storage_path, 0700);
#else
        ::wxMkDir((const char*)default_storage_path.mbc_str(), 0700);
#endif
#endif

        m_storage_path = default_storage_path;
    }

    SetStoragePath(m_storage_path);

    //std::string xpcom_path = std::string(GECKO_SDK_PATH_CONFIG);
    
    char path_separator = (char)wxFileName::GetPathSeparator();
    std::string gecko_path = (const char*)m_gecko_path.mb_str();
    std::string xpcom_path = gecko_path;
    if (xpcom_path.empty() || xpcom_path[xpcom_path.length()-1] != path_separator)
        xpcom_path += path_separator;
    #if defined __WXMSW__
    xpcom_path += "xpcom.dll";
    #elif defined __WXMAC__
    xpcom_path += "libxpcom.dylib";
    #else
    xpcom_path += "libxpcom.so";
    #endif

    std::cout << "xpcom: " << xpcom_path << std::endl;

    res = XPCOMGlueStartup(xpcom_path.c_str());
    if (NS_FAILED(res))
        return false;
    
    NS_LogInit();

        // load XUL functions
    nsDynamicFunctionLoad nsFuncs[] = {
                {"XRE_InitEmbedding2", (NSFuncPtr*)&XRE_InitEmbedding2},
                {"XRE_TermEmbedding", (NSFuncPtr*)&XRE_TermEmbedding},
                {"XRE_NotifyProfile", (NSFuncPtr*)&XRE_NotifyProfile},
                {"XRE_LockProfileDirectory", (NSFuncPtr*)&XRE_LockProfileDirectory},
                {0, 0}
    };

    std::cout << "xpcom glue load xul functions" << std::endl;
    res = XPCOMGlueLoadXULFunctions(nsFuncs);
       if (NS_FAILED(res)) {
            return false;
       }


    nsCOMPtr<nsIFile> gre_dir;
#if MOZILLA_VERSION_1 >=10
    res = NS_NewNativeLocalFile(nsCString(defXULPathDir), true, getter_AddRefs(gre_dir));
#else
    res = NS_NewNativeLocalFile(nsDependentCString(gecko_path.c_str()), PR_TRUE, getter_AddRefs(gre_dir));
#endif
    if (NS_FAILED(res))
         return false;

    nsCOMPtr<nsIFile> prof_dir;
#if MOZILLA_VERSION_1 >=10
#if wxMAJOR_VERSION == 3
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mb_str(wxConvUTF8)), true, getter_AddRefs(prof_dir));
    std::cout << "Storage path: " << std::string((const char*)m_storage_path.mb_str(wxConvUTF8)) << std::endl;
#else
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mbc_str()), true, getter_AddRefs(prof_dir));
#endif
#else
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mbc_str()), PR_TRUE, getter_AddRefs(prof_dir));
#endif
    if (NS_FAILED(res))
            return false;

    // init embedding
    std::cout << "Init embedding" << std::endl;
#if MOZILLA_VERSION_1 < 2
    const nsStaticModuleInfo* aComps = 0;
    int aNumComps = 0;

	//res = XRE_InitEmbedding(gre_dir, prof_dir,    		
	//		const_cast<wxDirSrvProvider*>(&DirectoryProvider),aComps, aNumComps);
	res = XRE_InitEmbedding(gre_dir, prof_dir,    		
			nsnull,aComps, aNumComps);

#else
    //res = XRE_InitEmbedding2(gre_dir, prof_dir, const_cast<wxDirSrvProvider*>(&DirectoryProvider));
    res = XRE_InitEmbedding2(gre_dir, prof_dir, nullptr);
#endif
    if (NS_FAILED(res))
            return false;

    // initialize profile:
    std::cout << "Notify profile" << std::endl;
    XRE_NotifyProfile();

    NS_LogTerm();
    
    // set the window creator
    //nsCOMPtr<nsIWindowCreator> wnd_creator = static_cast<nsIWindowCreator*>(new WindowCreator);
    std::cout << "Create windowcreator" << std::endl;
    nsCOMPtr<WindowCreator> wnd_creator = new WindowCreator();

    //set window watcher
    std::cout << "Create window watcher" << std::endl;
    nsCOMPtr<nsIWindowWatcher> window_watcher = nsGetWindowWatcherService();
    //nsCOMPtr<nsIWindowWatcher> window_watcher(do_GetService(NS_WINDOWWATCHER_CONTRACTID));
    if (!window_watcher)
        return false;

    window_watcher->SetWindowCreator(wnd_creator);


    // set up our own custom prompting service
    std::cout << "Set custom prompting service" << std::endl;
    nsCOMPtr<nsIComponentRegistrar> comp_reg;
    res = NS_GetComponentRegistrar(getter_AddRefs(comp_reg));
    if (NS_FAILED(res))
        return false;
    
    nsCOMPtr<nsIFactory> prompt_factory;
    CreatePromptServiceFactory(getter_AddRefs(prompt_factory));

    nsCID prompt_cid = NS_PROMPTSERVICE_CID;
    res = comp_reg->RegisterFactory(prompt_cid,
                                    "Prompt Service",
                                    NS_PROMPTSERVICE_CONTRACTID,
                                    //"@mozilla.org/embedcomp/prompt-service;1",
                                    prompt_factory);

    //prompt_factory.clear();
    CreatePromptServiceFactory(getter_AddRefs(prompt_factory));
    
    nsCID nssdialogs_cid = NS_NSSDIALOGS_CID;
    res = comp_reg->RegisterFactory(nssdialogs_cid,
                                    "PSM Dialog Impl",
                                    "@mozilla.org/nsBadCertListener;1",
                                    prompt_factory);

    // set up our own download progress service
    std::cout << "Set download progress service" << std::endl;
    
    nsCOMPtr<nsIFactory> transfer_factory;
    CreateTransferFactory(getter_AddRefs(transfer_factory));

    nsCID download_cid = NS_DOWNLOAD_CID;
    res = comp_reg->RegisterFactory(download_cid,
                                    "Transfer",
                                    "@mozilla.org/transfer;1",
                                    transfer_factory);
                                    
    res = comp_reg->RegisterFactory(download_cid,
                                    "Transfer",
                                    "@mozilla.org/download;1",
                                    transfer_factory);


    nsCOMPtr<nsIFactory> unknowncontenttype_factory;
    CreateUnknownContentTypeHandlerFactory(getter_AddRefs(unknowncontenttype_factory));

    nsCID unknowncontenthtypehandler_cid = NS_UNKNOWNCONTENTTYPEHANDLER_CID;
    res = comp_reg->RegisterFactory(unknowncontenthtypehandler_cid,
                                    "Helper App Launcher Dialog",
                                    "@mozilla.org/helperapplauncherdialog;1",
                                    unknowncontenttype_factory);


    // set up cert override service
    
    nsCOMPtr<nsIFactory> certoverride_factory;
    CreateCertOverrideFactory(getter_AddRefs(certoverride_factory));
    
    nsCID certoverride_cid = NS_CERTOVERRIDE_CID;
    res = comp_reg->RegisterFactory(certoverride_cid,
                                    "PSM Cert Override Settings Service",
                                    "@mozilla.org/security/certoverride;1",
                                    certoverride_factory);


    // set up some history file (which appears to be
    // required for downloads to work properly, even if we
    // don't store any history entries)

    //nsCOMPtr<nsIDirectoryService> dir_service = nsGetDirectoryService();
    std::cout << "Get directory service" << std::endl;
    nsCOMPtr<nsIProperties> dir_service_props = nsGetDirectoryService();
    
    /*nsCOMPtr<nsILocalFile> history_file;

    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_history_filename.mbc_str()), PR_TRUE, getter_AddRefs(history_file));
    if (NS_FAILED(res))
        return false;
    */

/*
    res = dir_service_props->Set((const char*)"UHist", history_file);
    if (NS_FAILED(res))
        return false;

    // set up a profile directory, which is necessary for many
    // parts of the gecko engine, including ssl on linux

    //FIXME implement later
    nsCOMPtr<nsILocalFile> prof_dir;
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)m_storage_path.mbc_str()), PR_TRUE, getter_AddRefs(prof_dir));
    if (NS_FAILED(res))
        return false;

    res = dir_service_props->Set("ProfD", getter_AddRefs(prof_dir));
    if (NS_FAILED(res))
        return false;
    
    */
    // replace the old plugin directory enumerator with our own
    // but keep all the entries that were in there
    nsCOMPtr<nsISimpleEnumerator> plugin_enum;
    res = dir_service_props->Get("APluginsDL", NS_GET_IID(nsISimpleEnumerator), getter_AddRefs(plugin_enum));
    if (NS_FAILED(res) || !plugin_enum)
        return false;
    // FIXME implement later
    //m_plugin_provider->AddPaths(getter_AddRefs(plugin_enum));
	//m_plugin_provider->AddPaths(plugin_enum);
    //res = dir_service->RegisterProvider(m_plugin_provider);
    //if (NS_FAILED(res) || !plugin_enum)
    //    return false;
    

    // set up preferences
    std::cout << "Get settings"  << std::endl;
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();

    if (!prefs)
        return false;
    
    // this was originally so that we wouldn't have to set
    // up a prompting service.
    std::cout << "Set security.warn_submit_insecure setting"  << std::endl;
    prefs->SetBoolPref("security.warn_submit_insecure", false);
    
    // don't store a history
    std::cout << "Set browser.history_expire_days setting"  << std::endl;
    prefs->SetIntPref("browser.history_expire_days", 0);
    
    // set path for our cache directory
    std::cout << "Set browser.cache.disk.parent_directory setting"  << std::endl;
    prefs->SetCharPref("browser.cache.disk.parent_directory", (const char*)m_storage_path.mbc_str());
    m_ok = true;

    return true;
}


void GeckoEngine::AddContentListener(ContentListener* l)
{
    m_content_listeners.Add(l);
}

ContentListenerPtrArray& GeckoEngine::GetContentListeners()
{
    return m_content_listeners;
}

void GeckoEngine::AddPluginPath(const wxString& path)
{
    // check first if the path exists
    if (!wxFileName::DirExists(path))
        return;
        
    m_plugin_provider->AddPath(path);
}




///////////////////////////////////////////////////////////////////////////////
//  wxWebPostData class implementation
///////////////////////////////////////////////////////////////////////////////


void wxWebPostData::Add(const wxString& variable, const wxString& value)
{
    m_vars.Add(variable);
    m_values.Add(value);
}

static wxString urlEscape(const wxString& input)
{
    wxString result;
    result.Alloc(input.Length() + 10);
    
    const wxChar* ch = input.c_str();
    unsigned int c;
    
    wxString u = wxT(" ");

    while ((c = *ch))
    {
		//FIXME
        /*if (c >= 128)
        {
            // we need to utf-8 encode this character per RFC-3986
            u.SetChar(0, *ch);
            const wxCharBuffer utf8b = u.utf8_str();
            const char* utf8 = (const char*)utf8b;
            while (*utf8)
            {
                result += wxString::Format(wxT("%%%02X"), (unsigned char)*utf8);
                ++utf8;
            }
            
            ch++;
            continue;
        }*/
        
        if (c <= 0x1f ||
            c == '%' || c == ' ' || c == '&' || c == '=' ||
            c == '+' || c == '$' || c == '#' || c == '{' ||
            c == '}' || c == '\\' ||c == '|' || c == '^' ||
            c == '~' || c == '[' || c == ']' || c == '`' ||
            c == '<' || c == '>')
        {
            result += wxString::Format(wxT("%%%02X"), c);
        }
         else
        {
            result += *ch;
        }
        
        ++ch;
    }

    return result;
}

wxString wxWebPostData::GetPostString()
{
    wxString result;
    wxString post_string;
    
    size_t i, cnt = m_vars.GetCount();
    
    for (i = 0; i < cnt; ++i)
    {
        if (i > 0)
            post_string += wxT("&");
        post_string += urlEscape(m_vars[i]);
        post_string += wxT("=");
        post_string += urlEscape(m_values[i]);
    }
    
    result = wxString::Format(wxT("Content-Length: %d\r\n"), post_string.Length());
    result += wxT("Content-Type: application/x-www-form-urlencoded\r\n\r\n");
    result += post_string;

    return result;
}

bool wxWebControl::InitEngine(const wxString& path)
{
    if (g_gecko_engine.IsOk())
    {
        wxFAIL_MSG(wxT("wxWebControl::InitEngine() should only be called once"));
    }

    if(!path.empty()) {
    	g_gecko_engine.SetEnginePath(path);
    }

    return g_gecko_engine.Init();
}

wxWebPreferences wxWebControl::GetPreferences()
{
    wxWebPreferences p;
    return p;
}

void wxWebControl::SetIgnoreCertErrors(bool ignore)
{
    g_ignore_ssl_cert_errors = ignore;
}

bool wxWebControl::GetIgnoreCertErrors()
{
    return g_ignore_ssl_cert_errors;
}

///////////////////////////////////////////////////////////////////////////////
//  wxWebFavIconProgress class implementation
///////////////////////////////////////////////////////////////////////////////


class wxWebFavIconProgress : public wxWebProgressBase
{
public:

    wxWebFavIconProgress(wxWebControl* ctrl)
    {
        m_progress = NULL;
        m_ctrl = ctrl;
    }
    
    void SetFilename(const wxString& filename)
    {
        m_filename = filename;
    }
    
    ~wxWebFavIconProgress()
    {
        if (m_progress)
        {
                ((ProgressListenerAdaptor*)m_progress)->ClearProgressReference();
            
            m_progress->Release();
        }
    }

    void OnFinish()
    {
        m_ctrl->OnFavIconFetched(m_filename);
    }
    
    void SetProgressListener(nsIWebProgressListener* prog)
    {
        if (m_progress)
        {
            m_progress->Release();
        }
        
        m_progress = prog;
        
        if (m_progress)
        {
            m_progress->AddRef();
        }
    }
    
private:

    nsIWebProgressListener* m_progress;
    wxWebControl* m_ctrl;
    wxString m_filename;
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebWaitUntilFinished class implementation
///////////////////////////////////////////////////////////////////////////////


class wxWebWaitUntilFinished : public wxWebProgressBase
{
public:

    wxWebWaitUntilFinished(bool* ptr)
    {
        m_ptr = ptr;
        *m_ptr = false;
    }
    
    void OnFinish()
    {
        *m_ptr = true;
    }
    
    void OnError(const wxString& message)
    {
        *m_ptr = true;
    }
    
private:
    bool* m_ptr;
};




///////////////////////////////////////////////////////////////////////////////
//  wxWebControl class implementation
///////////////////////////////////////////////////////////////////////////////


BEGIN_EVENT_TABLE(wxWebControl, wxControl)
    EVT_SIZE(wxWebControl::OnSize)
    EVT_SET_FOCUS(wxWebControl::OnSetFocus)
    EVT_KILL_FOCUS(wxWebControl::OnKillFocus)
	EVT_ERASE_BACKGROUND(wxWebControl::OnEraseBackground)
END_EVENT_TABLE()

// (CONSTRUCTOR) wxWebControl::wxWebControl
// Description: Creates a new wxWebControl object.
//
// Syntax: wxWebControl::wxWebControl(wxWindow* parent,
//                                    wxWindowID id,
//                                    const wxPoint& pos,
//                                    const wxSize& size)
//
// Remarks: Creates a new wxWebControl object.

wxWebControl::wxWebControl(wxWindow* parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size)
                           : wxControl(parent, id, pos, size, wxNO_BORDER)
{
    // set return value for IsOk() to false until initialization can be
    // verified as successful (end of the constructor)
    m_ok = false;
    m_content_loaded = true;
    m_loadurl_ready = false;

    m_favicon_progress = NULL;

    m_ptrs = new EmbeddingPtrs;
    
    // create browser chrome
    BrowserChrome* chrome = new BrowserChrome(this);
    chrome->AddRef();
    m_chrome = chrome;

    // make sure gecko is initialized
    if (!g_gecko_engine.IsOk())
    {
        if (!g_gecko_engine.Init())
        {
            m_chrome->Release();
            m_chrome = NULL;
            return;
        }
    }

    nsresult res;

    // create gecko web browser component
	m_ptrs->m_web_browser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID,&res);
	if(NS_FAILED(res))
    {
        wxASSERT(0);
        return;
    }

    //chrome->m_web_browser = m_ptrs->m_web_browser;

    m_ptrs->m_base_window = do_QueryInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_base_window)
    {
        wxASSERT(0);
        return;
    }

    //////////////////////////////////
    //////////////////////////////////
    return;
    //////////////////////////////////
    //////////////////////////////////

    // get base window interface and set its native window
    #ifdef __WXGTK__
    void* native_handle = (void*)m_wxwindow;
    #else
    void* native_handle = (void*)GetHandle();
    #endif

    wxSize cli_size = GetClientSize();
    res = m_ptrs->m_base_window->InitWindow(native_handle,
                                            nullptr,
                                            0, 0,
                                            cli_size.x, cli_size.y);
    if (NS_FAILED(res))
    {
        wxASSERT(0);
        return;
    }


	chrome->m_web_browser = m_ptrs->m_web_browser;
	
    // create browser chrome
    res = m_ptrs->m_web_browser->SetContainerWindow(static_cast<nsIWebBrowserChrome*>(m_chrome));

    // set the type to contentWrapper
    nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface(m_ptrs->m_web_browser);
    if (dsti)
    {
        dsti->SetItemType(nsIDocShellTreeItem::typeContentWrapper);
    }

    return;


    res = m_ptrs->m_base_window->Create();
    if (NS_FAILED(res))
    {
        wxASSERT(0);
        return;
    }
    
    // set our web progress listener
    //webconnect way:
    nsIWeakReference* weak = NS_GetWeakReference((nsIWebProgressListener*)m_chrome);
    res = m_ptrs->m_web_browser->AddWebBrowserListener(weak, NS_GET_IID(nsIWebProgressListener));
    //weak->Release();
    //gecko way:
    // register the progress listener
    //nsCOMPtr<nsIWebProgressListener> listener = do_QueryInterface(mPrivate->mChrome);
    //nsCOMPtr<nsIWeakReference> thisListener(do_GetWeakReference(listener));
    //mPrivate->mWebBrowser->AddWebBrowserListener(thisListener, NS_GET_IID(nsIWebProgressListener));



    

    // set our URI content listener

    m_main_uri_listener = new MainURIListener(this, m_ptrs->m_web_browser);
    m_main_uri_listener->AddRef();
    res = m_ptrs->m_web_browser->SetParentURIContentListener(static_cast<nsIURIContentListener*>(m_main_uri_listener));

    // get the event target

    nsCOMPtr<nsIDOMWindow> dom_window;
    res = m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    if (!dom_window)
    {
        wxASSERT(0);
        return;
    }
    
    nsCOMPtr<nsIDOMWindow> dom_window2(dom_window);

    if (dom_window2)
    {
        res = dom_window2->GetWindowRoot(getter_AddRefs(m_ptrs->m_event_target));
        if (NS_FAILED(res))
        {
            wxASSERT(0);
            return;
        }
    }
     else
    {
        res = dom_window2->GetWindowRoot(getter_AddRefs(m_ptrs->m_event_target));
        if (NS_FAILED(res))
        {
            wxASSERT(0);
            return;
        }
    }



    // initialize chrome events
    m_chrome->ChromeInit();


    // get the nsIClipboardCommands interface
    m_ptrs->m_clipboard_commands = do_GetInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_clipboard_commands)
    {
        wxASSERT(0);
        return;
    }

    // get the nsIWebBrowserFind interface
    m_ptrs->m_web_browser_find = do_GetInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_web_browser_find)
    {
        wxASSERT(0);
        return;
    }

    // get the nsIWebNavigation interface
    m_ptrs->m_web_navigation = do_QueryInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_web_navigation)
    {
        wxASSERT(0);
        return;
    }

    m_favicon_progress = new wxWebFavIconProgress(this);

    // now that initialization is complete (and successful, tell IsOk()
    // to return true)
    m_ok = true;
    
    //FIXME
    //PRUnichar* ns_uri = wxToUnichar(L"about:blank");
	const char *ns_uri = "about:blank";
	
    //m_ptrs->m_web_navigation->LoadURI(ns_uri,
	/*FIXME!!!
	m_ptrs->m_web_navigation->LoadURI(NS_ConvertUTF8toUTF16(ns_uri).get(),
                                      nsIWebNavigation::LOAD_FLAGS_NONE,
                                      0,
                                      0,
                                      0);*/
    //freeUnichar(ns_uri);
    
    // show the browser component
    res = m_ptrs->m_base_window->SetVisibility(true);
}

bool wxWebControl::CreateBrowser()
{
    // set return value for IsOk() to false until initialization can be
    // verified as successful (end of the constructor)
    //m_ok = false;
    //m_loadurl_ready = false;

    std::cout << "wxWebControl::CreateBrowser stating" << std::endl;
/*
    m_content_loaded = true;

    m_favicon_progress = NULL;

    m_ptrs = new EmbeddingPtrs;

    // create browser chrome
    BrowserChrome* chrome = new BrowserChrome(this);
    chrome->AddRef();
    m_chrome = chrome;

    // make sure gecko is initialized
    if (!g_gecko_engine.IsOk())
    {
        if (!g_gecko_engine.Init())
        {
            m_chrome->Release();
            m_chrome = NULL;
            return;
        }
    }

    nsresult res;

    // create gecko web browser component
	m_ptrs->m_web_browser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID,&res);
	if(NS_FAILED(res))
    {
        wxASSERT(0);
        return false;
    }

    //m_chrome->m_web_browser = m_ptrs->m_web_browser;

    m_ptrs->m_base_window = do_QueryInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_base_window)
    {
        wxASSERT(0);
        return false;
    }
*/

    nsresult res;
    // get base window interface and set its native window
    #ifdef __WXGTK__
    void* native_handle = (void*)m_wxwindow;
    #else
    void* native_handle = (void*)GetHandle();
    #endif

    wxSize cli_size = GetClientSize();
    std::cout << "wxWebControl::CreateBrowser InitWindow" << std::endl;
    res = m_ptrs->m_base_window->InitWindow(native_handle,
                                            nullptr,
                                            0, 0,
                                            cli_size.x, cli_size.y);
    if (NS_FAILED(res))
    {
        wxASSERT(0);
        return false;
    }


	m_chrome->m_web_browser = m_ptrs->m_web_browser;

    std::cout << "wxWebControl::CreateBrowser SetContainerWindow" << std::endl;
    // create browser chrome
    res = m_ptrs->m_web_browser->SetContainerWindow(static_cast<nsIWebBrowserChrome*>(m_chrome));

    // set the type to contentWrapper
    nsCOMPtr<nsIDocShellTreeItem> dsti = do_QueryInterface(m_ptrs->m_web_browser);
    if (dsti)
    {
        dsti->SetItemType(nsIDocShellTreeItem::typeContentWrapper);
    }

    //nsresult res;
    std::cout << "Create base window" << std::endl;
    res = m_ptrs->m_base_window->Create();
    if (NS_FAILED(res))
    {
        wxASSERT(0);
        return false;
    }

    // set our web progress listener
    std::cout << "Set progress listener" << std::endl;
    nsIWeakReference* weak = NS_GetWeakReference((nsIWebProgressListener*)m_chrome);
    res = m_ptrs->m_web_browser->AddWebBrowserListener(weak, NS_GET_IID(nsIWebProgressListener));
    //weak->Release();



    // set our URI content listener
    std::cout << "Set URI listener" << std::endl;
    m_main_uri_listener = new MainURIListener(this, m_ptrs->m_web_browser);
    m_main_uri_listener->AddRef();
    res = m_ptrs->m_web_browser->SetParentURIContentListener(static_cast<nsIURIContentListener*>(m_main_uri_listener));

    // get the event target

    nsCOMPtr<nsIDOMWindow> dom_window;
    res = m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    if (!dom_window)
    {
        wxASSERT(0);
        return false;
    }

    nsCOMPtr<nsIDOMWindow> dom_window2(dom_window);

    if (dom_window2)
    {
        res = dom_window2->GetWindowRoot(getter_AddRefs(m_ptrs->m_event_target));
        if (NS_FAILED(res))
        {
            wxASSERT(0);
            return false;
        }
    }
     else
    {
        res = dom_window2->GetWindowRoot(getter_AddRefs(m_ptrs->m_event_target));
        if (NS_FAILED(res))
        {
            wxASSERT(0);
            return false;
        }
    }



    // initialize chrome events
    std::cout << "Chrome Init" << std::endl;
    m_chrome->ChromeInit();


    // get the nsIClipboardCommands interface
    m_ptrs->m_clipboard_commands = do_GetInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_clipboard_commands)
    {
        wxASSERT(0);
        return false;
    }

    // get the nsIWebBrowserFind interface
    m_ptrs->m_web_browser_find = do_GetInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_web_browser_find)
    {
        wxASSERT(0);
        return false;
    }

    // get the nsIWebNavigation interface
    m_ptrs->m_web_navigation = do_QueryInterface(m_ptrs->m_web_browser);
    if (!m_ptrs->m_web_navigation)
    {
        wxASSERT(0);
        return false;
    }

    m_favicon_progress = new wxWebFavIconProgress(this);

    // now that initialization is complete (and successful, tell IsOk()
    // to return true)
    m_ok = true;

    //delayed loader
	std::cout << "Create timer object for URI" << std::endl;
	//TODO do it as singleton
	DelayedFirstURILoader *tmp_uri_loader = new DelayedFirstURILoader(this,1);


    // show the browser component
    res = m_ptrs->m_base_window->SetVisibility(true);
    return true;
}

wxWebControl::~wxWebControl()
{
    m_main_uri_listener->m_wnd = NULL;
    m_main_uri_listener->Release();

    if (m_ok)
    {
        // destroy web browser
        m_ptrs->m_base_window->Destroy();
        //m_ptrs->m_base_window.clear();
    
        // release chrome
        m_chrome->ChromeUninit();
        m_chrome->Release();
		//FIXME
		//g_gecko_engine.Uninit();
    }


    // delete any web content handlers that we 'own'
    size_t i, count;
    for (i = 0, count = m_to_delete.GetCount(); i < count; ++i)
    {
        wxWebContentHandler* handler = m_to_delete.Item(i);
        delete handler;
    }
    
    delete m_favicon_progress;
    delete m_ptrs;
}

// (METHOD) wxWebControl::IsOk
// Description:
//
// Syntax: bool wxWebControl::IsOk() const
//
// Remarks:
//
// Returns: Returns true if the wxWebControl is valid, and false otherwise.

bool wxWebControl::IsOk() const
{
    return m_ok;
}

// (METHOD) wxWebControl::GeckoVersion
// Description:
//
// Syntax: wxString wxWebControl::GeckoVersion() const
//
// Remarks:
//
// Returns: Returns gecko version by MOZILLA_VERSION definition

wxString wxWebControl::GeckoVersion()
{
	wxString version;

#if wxMAJOR_VERSION == 3
	version 
		<< wxString::Format("%i",MOZILLA_VERSION_1)
		//<< wxString( std::to_string(MOZILLA_VERSION_1))
		<< wxString(".") 
		<< wxString::Format("%i",MOZILLA_VERSION_2)
		//<< wxString(std::to_string(MOZILLA_VERSION_2))
		<< wxString(".") 
		<< wxString::Format("%i",MOZILLA_VERSION_3);
		//<< wxString(std::to_string(MOZILLA_VERSION_3));
	//version << wxString(std::to_string(MOZILLA_VERSION_1)) << wxString(".") << wxString(std::to_string(MOZILLA_VERSION_2))
	//		<< wxString(".") << wxString(std::to_string(MOZILLA_VERSION_3));
#else
	version << MOZILLA_VERSION_1 << wxT(".") << MOZILLA_VERSION_2 << wxT(".") << MOZILLA_VERSION_3;
#endif
	return version;
}
// (METHOD) wxWebControl::Find
// Description:
//
// Syntax: bool wxWebControl::Find(const wxString& text, 
//                                 unsigned int flags)
//
// Remarks:
//
// Returns:

bool wxWebControl::Find(const wxString& text, 
                        unsigned int flags)
{
    if (!(m_ptrs->m_web_browser_find))
        return false;
    //*title = (char16_t*)NS_ConvertUTF8toUTF16((const char*)m_title.mb_str(wxConvUTF8)).get();
    char16_t* find_text = (char16_t*)NS_ConvertUTF8toUTF16((const char*)text.mb_str(wxConvUTF8)).get();
    m_ptrs->m_web_browser_find->SetSearchString(find_text);
    //freeUnichar(find_text);
#if MOZILLA_VERSION_1 >=10
    m_ptrs->m_web_browser_find->SetFindBackwards((flags & wxWEB_FIND_BACKWARDS) != 0 ? true : false);
    m_ptrs->m_web_browser_find->SetWrapFind((flags & wxWEB_FIND_WRAP) != 0 ? true : false);
    m_ptrs->m_web_browser_find->SetEntireWord((flags & wxWEB_FIND_ENTIRE_WORD) != 0 ? true : false);
    m_ptrs->m_web_browser_find->SetMatchCase((flags & wxWEB_FIND_MATCH_CASE) != 0 ? true : false);
    m_ptrs->m_web_browser_find->SetSearchFrames((flags & wxWEB_FIND_SEARCH_FRAMES) != 0 ? true : false);

    bool retval = false;
    m_ptrs->m_web_browser_find->FindNext(&retval);
    return retval;
#else
    m_ptrs->m_web_browser_find->SetFindBackwards((flags & wxWEB_FIND_BACKWARDS) != 0 ? PR_TRUE : PR_FALSE);
    m_ptrs->m_web_browser_find->SetWrapFind((flags & wxWEB_FIND_WRAP) != 0 ? PR_TRUE : PR_FALSE);
    m_ptrs->m_web_browser_find->SetEntireWord((flags & wxWEB_FIND_ENTIRE_WORD) != 0 ? PR_TRUE : PR_FALSE);
    m_ptrs->m_web_browser_find->SetMatchCase((flags & wxWEB_FIND_MATCH_CASE) != 0 ? PR_TRUE : PR_FALSE);
    m_ptrs->m_web_browser_find->SetSearchFrames((flags & wxWEB_FIND_SEARCH_FRAMES) != 0 ? PR_TRUE : PR_FALSE);

    PRBool retval = PR_FALSE;
    m_ptrs->m_web_browser_find->FindNext(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::AddContentHandler
// Description:
//
// Syntax: bool wxWebControl::AddContentHandler(wxWebContentHandler* handler,
//                                              bool take_ownership)
//
// Remarks:
//
// Returns:

bool wxWebControl::AddContentHandler(wxWebContentHandler* handler,
                                     bool take_ownership)
{
    nsresult res;
    
    nsCOMPtr<nsIServiceManager> service_mgr;
    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return false;
    
    //FIXME
    nsCOMPtr<nsISupports> uri_loader;
    
    
    nsIID iid = NS_ISUPPORTS_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/uriloader;1",
                                        iid,
                                        getter_AddRefs(uri_loader));
                                        
    if (!uri_loader)
        return false;
    /*
    nsCOMPtr<nsIURILoader> uri_loader19 = do_QueryInterface(uri_loader);
    if (uri_loader19)
    {
        ContentListener* l = new ContentListener(handler);
        l->AddRef(); // will be released later
        res = uri_loader19->RegisterContentListener(static_cast<nsIURIContentListener*>(l));
        if (NS_FAILED(res))
            return false;
        g_gecko_engine.AddContentListener(l);
    }*/
    return true;
}

// (METHOD) wxWebControl::AddPluginPath
// Description:
//
// Syntax: static void wxWebControl::AddPluginPath(const wxString& path)
//
// Remarks:
//
// Returns:

void wxWebControl::AddPluginPath(const wxString& path)
{
    g_gecko_engine.AddPluginPath(path);
}



// (METHOD) wxWebControl::SetProfilePath
// Description:
//
// Syntax: static void wxWebControl::SetProfilePath(const wxString& path)
//
// Remarks:
//
// Returns:

void wxWebControl::SetProfilePath(const wxString& path)
{
    g_gecko_engine.SetStoragePath(path);
}



// (METHOD) wxWebControl::SaveRequest
// Description:
//
// Syntax: static bool wxWebControl::SaveRequest(const wxString& uri_str,
//                                               const wxString& destination_path,
//                                               wxWebPostData* post_data,
//                                               wxWebProgressBase* listener)
//
// Remarks:
//
// Returns:

bool wxWebControl::SaveRequest(const wxString& uri_str,
                               const wxString& destination_path,
                               wxWebPostData* post_data,
                               wxWebProgressBase* listener)
{
//	nsresult rv;
    // make uri object out of the request uri
    nsCOMPtr<nsIURI> uri = nsNewURI(uri_str);
	return false;
    if (!uri)
        return false;
    //FIXME
 /*   nsCOMPtr<nsIWebBrowserPersist> persist = do_CreateInstance("@mozilla.org/embedding/browser/nsWebBrowserPersist;1");
    if (!persist)
        return false;

    nsCOMPtr<nsILocalFile> file = nsNewLocalFile(destination_path);
    if (!file)
    {
        wxFAIL_MSG(wxT("wxWebControl::SaveURI(): could not create temporary file"));
        return false;
    }

    // post data
    nsCOMPtr<nsIInputStream> sp_post_data;
    if (post_data)
    {
        nsCOMPtr<nsIStringInputStream> strs = nsCreateInstance("@mozilla.org/io/string-input-stream;1");
        wxASSERT(strs.p);
        
        if (strs)
        {
            wxString poststr = post_data->GetPostString();
            strs->SetData((const char*)poststr.mbc_str(), poststr.Length());
            sp_post_data = strs;
        }
    }
    

    nsresult rv;
    
    wxWebWaitUntilFinished* wuf = NULL;
    bool finished = false;
    
    if (listener)
    {
        // caller desires its own progress listener.  Caller is responsible
        // for maintaining a non-blocked main gui thread, otherwise
        // the callbacks will never be called.
        nsIWebProgressListener* la = CreateProgressListenerAdaptor(listener);
        persist->SetProgressListener(la);
        la->Release();
    }
     else
    {
        // caller wants to block until the request is finished
        wuf = new wxWebWaitUntilFinished(&finished);
        nsIWebProgressListener* la = CreateProgressListenerAdaptor(wuf);
        persist->SetProgressListener(la);
        la->Release();
    }
    
    persist->SetPersistFlags(nsIWebBrowserPersist::PERSIST_FLAGS_BYPASS_CACHE);
    
    rv = persist->SaveURI(uri, nsnull, nsnull, getter_AddRefs(sp_post_data), nsnull, file);
    
    if (NS_FAILED(rv))
    {
        // free up progress listener
        persist->SetProgressListener(nsnull);
    }
     else
    {
        if (wuf)
        {
            wxStopWatch sw;
            wxWindowDisabler wd;
            
            while (!finished)
            {
                // gecko uses the main thread for status callbacks
                // so this wxSafeYield call is necessary
                
                if (wxThread::IsMain())
                {
                    ::wxWakeUpIdle();
                    ::wxYield();
                }

                wxThread::Sleep(10);
                
                // timeout, right now hardcoded
                if (sw.Time() > 30000)
                    break;
            }
        }
    }
    
    return NS_SUCCEEDED(rv) ? true : false;
    */
}

// (METHOD) wxWebControl::SaveRequestToString
// Description:
//
// Syntax: static bool wxWebControl::SaveRequestToString(const wxString& uri_str,
//                                                       wxString* result,
//                                                       wxWebPostData* post_data,
//                                                       wxWebProgressBase* listener)
//
// Remarks:
//
// Returns:

bool wxWebControl::SaveRequestToString(const wxString& uri_str,
                                       wxString* result,
                                       wxWebPostData* post_data,
                                       wxWebProgressBase* listener)
{
    
    wxString filename = wxFileName::CreateTempFileName(wxT("wwc"));
    if (!SaveRequest(uri_str, filename, post_data, listener))
        return false;
    
    if (!result)
    {
        ::wxRemoveFile(filename);
        return true;
    }
        
    *result = wxT("");
    
    wxFile f;
    if (f.Open(filename) && result)
    {
        wxString res;
        
        char buf[1025];
        while (1)
        {
            size_t r = f.Read(buf, 1024);
            if (r == 0)
                break;
            buf[r] = 0;
            
            res += wxString::FromAscii(buf);
            
            if (r != 1024)
                break;
        }
        
        f.Close();
        
        *result = res;
    }
    
    ::wxRemoveFile(filename);
    return true;
}

// (METHOD) wxWebControl::ClearCache
// Description:
//
// Syntax: static bool wxWebControl::ClearCache()
//
// Remarks:
//
// Returns:

bool wxWebControl::ClearCache()
{
	nsresult rv;
    //nsCOMPtr<nsICacheService> cache_service = nsGetService("@mozilla.org/network/cache-service;1");
    nsCOMPtr<nsICacheService> cache_service (do_GetService(NS_CACHESERVICE_CONTRACTID, &rv));
    if (cache_service)
    {
        cache_service->EvictEntries(0 /*nsICache::STORE_ANYWHERE*/);
        cache_service->EvictEntries(1 /*nsICache::STORE_IN_MEMORY*/);
        cache_service->EvictEntries(2 /*nsICache::STORE_ON_DISK*/);
        cache_service->EvictEntries(3 /*nsICache::STORE_ON_DISK_AS_FILE*/);
        return true;
    }
     else
    {
        return false;
    }
}

void wxWebControl::FetchFavIcon(void* _uri)
{

	if (m_favicon_fetched)
        return;
    m_favicon_fetched = true;

    nsEmbedCString ns_spec;
    wxString spec;

    nsIURI* raw_uri = (nsIURI*)_uri;
    nsCOMPtr<nsIURI> uri = raw_uri;
    uri->GetSpec(ns_spec);
    spec = ns2wx(ns_spec);
    
    nsresult rv;
    //m_ptrs->m_web_browser = do_CreateInstance(NS_WEBBROWSER_CONTRACTID, &rv);
    nsCOMPtr<nsIWebBrowserPersist> persist = do_CreateInstance("@mozilla.org/embedding/browser/nsWebBrowserPersist;1",&rv);
    //nsCOMPtr<nsIWebBrowserPersist> persist = nsCreateInstance("@mozilla.org/embedding/browser/nsWebBrowserPersist;1");

    if (!persist)
        return;

    wxString extension = spec.AfterLast(L'.');
    extension = extension.Left(4);
    //if (extension.IsEmpty()) extension = L"tmp";
	if (extension.IsEmpty()) extension = wxT("tmp");
    extension.MakeLower();
    //wxString filename = wxFileName::CreateTempFileName(L"fav");
	wxString filename = wxFileName::CreateTempFileName(wxT("fav"));
    filename += wxT(".");
    filename += extension;
    nsCOMPtr<nsIFile> file = nsNewLocalFile(filename);

    m_favicon_progress->SetFilename(filename);
    nsIWebProgressListener* la = CreateProgressListenerAdaptor(m_favicon_progress);
    m_favicon_progress->SetProgressListener(la);
    persist->SetProgressListener(la);
    la->Release();

#if MOZILLA_VERSION_1 < 36
    rv = persist->SaveURI(uri, NULL, NULL, NULL, NULL, file, NULL);
#else
    rv = persist->SaveURI(uri, nullptr, nullptr, 0, nullptr, nullptr, file, nullptr);
#endif
    
    if (NS_FAILED(rv))
    {
        persist->SetProgressListener(NULL);
        return;
    }
}


void wxWebControl::ResetFavicon()
{
    m_favicon = wxImage();
    m_favicon_fetched = false;
    m_content_loaded = false;
}
   
void wxWebControl::OnFavIconFetched(const wxString& filename)
{
    wxLogNull nulllog;
    
    m_favicon = wxImage();
    if (!m_favicon.LoadFile(filename))
        return;
    ::wxRemove(filename);
    
    wxWebEvent cevt(wxEVT_WEB_FAVICONAVAILABLE, GetId());
    cevt.SetEventObject(this);
    GetEventHandler()->ProcessEvent(cevt);
}


void wxWebControl::OnDOMContentLoaded()
{
    m_content_loaded = true;
    
    // fire the DOMContentLoaded event
    wxWebEvent evt(wxEVT_WEB_DOMCONTENTLOADED, GetId());
    evt.SetEventObject(this);
    GetEventHandler()->ProcessEvent(evt);
}

// (METHOD) wxWebControl::GetFavIcon
// Description:
//
// Syntax: wxImage wxWebControl::GetFavIcon() const
//
// Remarks:
//
// Returns:

wxImage wxWebControl::GetFavIcon() const
{
    return m_favicon;
}

// (METHOD) wxWebControl::GetDOMDocument
// Description:
//
// Syntax: wxDOMDocument wxWebControl::GetDOMDocument()
//
// Remarks:
//
// Returns:

wxDOMDocument wxWebControl::GetDOMDocument()
{
    wxDOMDocument doc;
    
    
    nsCOMPtr<nsIDOMWindow> dom_window;
    m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    if (!dom_window)
        return doc;
    
    nsCOMPtr<nsIDOMDocument> dom_doc;
    dom_window->GetDocument(getter_AddRefs(dom_doc));
    doc.m_data->setNode(dom_doc);
    
    wxASSERT(doc.IsOk());
    
    return doc;
}


void wxWebControl::RealOpenURI(const wxString& uri,
                           unsigned int load_flags,
                           wxWebPostData* post_data)
{
	std::cout << "RealOpenURI, url:" << uri.ToUTF8() << std::endl;
    if (!IsOk())
        return;

    m_favicon = wxImage();
    m_favicon_fetched = false;
    m_content_loaded = false;

    unsigned int ns_load_flags = nsIWebNavigation::LOAD_FLAGS_NONE;

    if (load_flags & wxWEB_LOAD_LINKCLICK)
        ns_load_flags |= nsIWebNavigation::LOAD_FLAGS_IS_LINK;

    // post data
    nsCOMPtr<nsIInputStream> sp_post_data;
    if (post_data)
    {
        nsCOMPtr<nsIStringInputStream> strs;
        strs = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
        //wxASSERT(strs.p);

        if (strs)
        {
            wxString poststr = post_data->GetPostString();
            strs->SetData((const char*)poststr.mbc_str(), poststr.Length());
            sp_post_data = strs;
        }
    }


    nsresult res;
    res = m_ptrs->m_web_navigation->LoadURI(NS_ConvertUTF8toUTF16(uri.ToUTF8().data()).get(),
                                            ns_load_flags,
                                            NULL,
                                            //getter_AddRefs(sp_post_data),
                                            NULL,
                                            NULL);

    //freeUnichar(ns_uri);


    nsCOMPtr<nsIWebBrowserFocus> focus;// = nsRequestInterface(m_ptrs->m_web_browser);
    focus = do_QueryInterface(m_ptrs->m_web_browser);
    if (!focus)
        return;

    focus->Activate();
}


// (METHOD) wxWebControl::OpenURI
// Description:
//
// Syntax: void wxWebControl::OpenURI(const wxString& uri,
//                                    unsigned int load_flags,
//                                    wxWebPostData* post_data)
//
// Remarks:
//
// Returns:

void wxWebControl::OpenURI(const wxString& uri,
                           unsigned int load_flags,
                           wxWebPostData* post_data)
{
    //if (!IsOk() )
    //    return;

    if (m_loadurl_ready) {
    	std::cout << "OpenURI, can run RealOpenURI, url:" << uri.ToUTF8() << std::endl;
    	RealOpenURI(uri, load_flags, post_data);
    } else {
    	std::cout << "OpenURI, still cannot run RealOpenURI, url:" << uri.ToUTF8() << std::endl;
    	//still not initialized
    	//store data, set pending and exit
    	m_loadurl_pending = true;
    	m_pending_uri = uri;
    	m_pending_load_flags = load_flags;
    	m_pending_post_data = post_data; //TODO check post_data
    }

	return;

    m_favicon = wxImage();
    m_favicon_fetched = false;
    m_content_loaded = false;
    
    unsigned int ns_load_flags = nsIWebNavigation::LOAD_FLAGS_NONE;

    if (load_flags & wxWEB_LOAD_LINKCLICK)
        ns_load_flags |= nsIWebNavigation::LOAD_FLAGS_IS_LINK;
        
    // post data
    nsCOMPtr<nsIInputStream> sp_post_data;
    if (post_data)
    {
        nsCOMPtr<nsIStringInputStream> strs;
        strs = do_CreateInstance("@mozilla.org/io/string-input-stream;1");
        //wxASSERT(strs.p);
        
        if (strs)
        {
            wxString poststr = post_data->GetPostString();
            strs->SetData((const char*)poststr.mbc_str(), poststr.Length());
            sp_post_data = strs;
        }
    }
    

    //char16_t* ns_uri = wxToUnichar(uri);

    //nsDependentCString((const char*)m_storage_path.mb_str(wxConvUTF8));
    //const char16_t* ns_uri = NS_ConvertUTF8toUTF16((const char*)uri.mb_str(wxConvUTF8)).get();
    //char16_t* ns_uri = u"www.opennet.ru";

    //std::cout << "Load URL: " << ns_uri << ", origin: " << uri.ToUTF8().data() << std::endl;

    //test
    //NS_ConvertUTF8toUTF16(aUri).get()
    //PRUnichar* ns_uri_1 = L"www.google.com";
    //const char* ns_uri_1 = "www.google.com";
    nsresult res;
    //res = m_ptrs->m_web_navigation->LoadURI(NS_ConvertUTF8toUTF16(ns_uri_1).get(),
    //res = m_ptrs->m_web_navigation->LoadURI(ns_uri,
    res = m_ptrs->m_web_navigation->LoadURI(NS_ConvertUTF8toUTF16(uri.ToUTF8().data()).get(),
                                            ns_load_flags,
                                            NULL,
                                            //getter_AddRefs(sp_post_data),
                                            NULL,
                                            NULL);

    //freeUnichar(ns_uri);


    nsCOMPtr<nsIWebBrowserFocus> focus;// = nsRequestInterface(m_ptrs->m_web_browser);
    focus = do_QueryInterface(m_ptrs->m_web_browser);
    if (!focus)
        return;

    focus->Activate();
}

// (METHOD) wxWebControl::GetCurrentURI
// Description:
//
// Syntax: wxString wxWebControl::GetCurrentURI() const
//
// Remarks:
//
// Returns:

wxString wxWebControl::GetCurrentURI() const
{
    nsCOMPtr<nsIURI> uri;
    
    m_ptrs->m_web_navigation->GetCurrentURI(getter_AddRefs(uri));
    
    if (!uri)
        return wxEmptyString;
    
    nsEmbedCString spec;

    if (NS_FAILED(uri->GetSpec(spec)))
        return wxEmptyString;
        
    return ns2wx(spec);
}

// (METHOD) wxWebControl::GoForward
// Description:
//
// Syntax: void wxWebControl::GoForward()
//
// Remarks:
//
// Returns:

void wxWebControl::GoForward()
{
    if (!IsOk())
        return;

    m_ptrs->m_web_navigation->GoForward();
}

// (METHOD) wxWebControl::GoBack
// Description:
//
// Syntax: void wxWebControl::GoBack()
//
// Remarks:
//
// Returns:

void wxWebControl::GoBack()
{
    if (!IsOk())
        return;

    m_ptrs->m_web_navigation->GoBack();
}

// (METHOD) wxWebControl::Reload
// Description:
//
// Syntax: void wxWebControl::Reload()
//
// Remarks:
//
// Returns:

void wxWebControl::Reload()
{
    if (!IsOk())
        return;

    m_ptrs->m_web_navigation->Reload(nsIWebNavigation::LOAD_FLAGS_NONE);
}

// (METHOD) wxWebControl::Stop
// Description:
//
// Syntax: void wxWebControl::Stop()
//
// Remarks:
//
// Returns:

void wxWebControl::Stop()
{
    if (!IsOk())
        return;

    m_ptrs->m_web_navigation->Stop(nsIWebNavigation::STOP_ALL);
}

// (METHOD) wxWebControl::IsContentLoaded
// Description:
//
// Syntax: bool wxWebControl::IsContentLoaded() const
//
// Remarks:
//
// Returns:

bool wxWebControl::IsContentLoaded() const
{
    return m_content_loaded;
}

wxString wxWebControl::GetCurrentLoadURI()
{
    return m_main_uri_listener->m_current_url;
}

void wxWebControl::InitPrintSettings()
{
    nsresult rv;
	/*
	if (!m_ptrs->m_print_settings)
    {
    	nsCOMPtr<nsIPrintSettingsService> print_settings_service;
    	//print_settings_service = do_GetService("@mozilla.org/gfx/printsettings-service;1");
        nsCOMPtr<nsIServiceManager> service_mgr;

        rv = NS_GetServiceManager(getter_AddRefs(service_mgr));
        if (NS_FAILED(rv))
            return;

        nsIID iid = NS_PRINTSETTINGSSERVICE_IID;
        service_mgr->GetServiceByContractID("@mozilla.org/gfx/printsettings-service;1",
                                            iid,
                                            getter_AddRefs(print_settings_service));

        if (print_settings_service)
        {
            nsCOMPtr<nsIPrintSettings> print_settings;
            
            rv = print_settings_service->GetGlobalPrintSettings(getter_AddRefs(print_settings));

            char16_t* printer_name = NULL;
            rv = print_settings_service->GetDefaultPrinterName(&printer_name);
            if (printer_name)
                rv = print_settings_service->InitPrintSettingsFromPrinter(printer_name, print_settings);
                
            rv = print_settings_service->InitPrintSettingsFromPrefs(print_settings,
                                                             PR_TRUE, 
                                                             nsIPrintSettings::kInitSaveAll);
                                                             
            m_ptrs->m_print_settings = print_settings;
        }
         else
        {
            nsCOMPtr<nsIWebBrowserPrint> web_browser_print = do_GetInterface(m_ptrs->m_web_browser);
            if (!web_browser_print)
            {
                wxASSERT(0);
                return;
            }

            nsCOMPtr<nsIPrintSettings> supports;
            rv = web_browser_print->GetGlobalPrintSettings(getter_AddRefs(supports));
            m_ptrs->m_print_settings = supports;
        }
    }*/
}

// (METHOD) wxWebControl::PrintPreview
// Description:
//
// Syntax: void wxWebControl::PrintPreview(bool silent)
//
// Remarks:
//
// Returns:

void wxWebControl::PrintPreview(bool silent)
{
    return; //TODO implement
	/*
	nsresult rv;
	nsCOMPtr<nsIWebBrowserPrint> web_browser_print = do_GetInterface(m_ptrs->m_web_browser);
    if (!web_browser_print)
    {
        wxASSERT(0);
        return;
    }

    InitPrintSettings();

    nsCOMPtr<nsIDOMWindow> dom_window;
    rv = m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    if (!dom_window) return;

    nsCOMPtr<nsIPrintSettings> settings19 = m_ptrs->m_print_settings;
    if (settings19)
    {
        settings19->SetShowPrintProgress(false);
        settings19->SetPrintSilent(silent);
        //web_browser_print->PrintPreview(settings19,dom_window,m_chrome);
        web_browser_print->PrintPreviewNavigate(0, 0);
    }*/

}

// (METHOD) wxWebControl::Print
// Description:
//
// Syntax: void wxWebControl::Print(bool silent)
//
// Remarks:
//
// Returns:

void wxWebControl::Print(bool silent)
{	
	/*
    nsCOMPtr<nsIWebBrowserPrint> web_browser_print = do_GetInterface(m_ptrs->m_web_browser);
    if (!web_browser_print)
    {
        wxASSERT(0);
        return;
    }
    InitPrintSettings();
    nsCOMPtr<nsIPrintSettings> settings19 = m_ptrs->m_print_settings;
    if (settings19)
    {
        settings19->SetShowPrintProgress(false);
		settings19->SetPrintSilent(silent);
        web_browser_print->Print(settings19, NULL);
    }*/

	/*nsCOMPtr<nsIPrintSettings> print_settings;
	nsresult rv = web_browser_print->GetGlobalPrintSettings(getter_AddRefs(print_settings));
	if (NS_SUCCEEDED(rv))
    {
        print_settings->SetShowPrintProgress(PR_FALSE);
        //print_settings->SetPrintSilent(silent ? PR_TRUE : PR_FALSE);
		print_settings->SetPrintSilent(PR_TRUE);
        web_browser_print->Print(print_settings, nsnull);
    }*/
}

// (METHOD) wxWebControl::SetPageSettings
// Description:
//
// Syntax: void wxWebControl::SetPageSettings(double page_width, double page_height,
//                                            double left_margin, double right_margin, 
//                                            double top_margin, double bottom_margin)
//
// Remarks:
//
// Returns:

void wxWebControl::SetPageSettings(double page_width, double page_height,
                                   double left_margin, double right_margin, 
                                   double top_margin, double bottom_margin)
{
	/*
	nsCOMPtr<nsIWebBrowserPrint> web_browser_print = do_GetInterface(m_ptrs->m_web_browser);
    if (!web_browser_print)
    {
        wxASSERT(0);
        return;
    }

    InitPrintSettings();

    nsCOMPtr<nsIPrintSettings> settings19 = m_ptrs->m_print_settings;
    if (settings19)
    {
        // if the page width is greater than the page height,
        // set the proper orientation
        settings19->SetOrientation(settings19->kPortraitOrientation);
        if (page_width > page_height)
        {
            double t = page_width;
            page_width = page_height;
            page_height = t;

            settings19->SetOrientation(settings19->kLandscapeOrientation);
        }

        settings19->SetPaperWidth(page_width);
        settings19->SetPaperHeight(page_height);
        settings19->SetMarginLeft(left_margin);
        settings19->SetMarginRight(right_margin);
        settings19->SetMarginTop(top_margin);
        settings19->SetMarginBottom(bottom_margin);
    }*/
}

// (METHOD) wxWebControl::GetPageSettings
// Description:
//
// Syntax: void wxWebControl::GetPageSettings(double* page_width, double* page_height,
//                                            double* left_margin, double* right_margin, 
//                                            double* top_margin, double* bottom_margin)
//
// Remarks:
//
// Returns:

void wxWebControl::GetPageSettings(double* page_width, double* page_height,
                                   double* left_margin, double* right_margin, 
                                   double* top_margin, double* bottom_margin)
{
	/*
	nsCOMPtr<nsIWebBrowserPrint> web_browser_print = do_GetInterface(m_ptrs->m_web_browser);
    if (!web_browser_print)
    {
        wxASSERT(0);
        return;
    }

    InitPrintSettings();

    nsCOMPtr<nsIPrintSettings> settings19 = m_ptrs->m_print_settings;
    if (settings19)
    {
        settings19->GetPaperWidth(page_width);
        settings19->GetPaperHeight(page_height);
        settings19->GetMarginLeft(left_margin);
        settings19->GetMarginRight(right_margin);
        settings19->GetMarginTop(top_margin);
        settings19->GetMarginBottom(bottom_margin);
        
        // if the orientation is set, reverse the page width
        // and page height
        PRInt32 orientation;
        settings19->GetOrientation(&orientation);
        if (orientation == settings19->kLandscapeOrientation)
        {
            double t = *page_width;
            *page_width = *page_height;
            *page_height = t;
        }
    }*/
}

// (METHOD) wxWebControl::ViewSource
// Description:
//
// Syntax: void wxWebControl::ViewSource()
// Syntax: void wxWebControl::ViewSource(wxWebControl* source_web_browser)
// Syntax: void wxWebControl::ViewSource(const wxString& uri)
//
// Remarks:
//
// Returns:

void wxWebControl::ViewSource()
{
    ViewSource(this);
}

void wxWebControl::ViewSource(wxWebControl* source_web_browser)
{
    ViewSource(source_web_browser->GetCurrentURI());

/*
    // this code is supposed to download the page from the cache,
    // but it's not working right now.  nsIWebBrowserPersist::SaveURI
    // isn't finished saving the document by the time that OpenURI is called
    
    nsCOMPtr<nsIWebBrowserPersist> persist = source_web_browser->m_ptrs->m_web_browser;
    
    if (persist.empty())
    {
        wxFAIL_MSG(wxT("wxWebControl::ViewSource(): nsIWebBrowserPersist interface not available"));
        return;
    }
        
    wxString filename = wxFileName::CreateTempFileName(wxT("wwc"));
    wxString new_filename = filename + wxT(".html");
    ::wxRenameFile(filename, new_filename);
    filename = new_filename;
    
    nsCOMPtr<nsILocalFile> file = nsNewLocalFile(filename);
    
    if (file.empty())
    {
        wxFAIL_MSG(wxT("wxWebControl::ViewSource(): could not create temporary file"));
        return;
    }

    nsresult res = persist->SaveURI(nsnull, nsnull, nsnull, nsnull, nsnull, file.p);
    
    wxString view_url = wxT("view-source:file:///");
    filename.Replace(wxT("\\"), wxT("/"));
    view_url += filename;
    
    OpenURI(view_url);
*/
}

void wxWebControl::ViewSource(const wxString& uri)
{
    wxString loc = wxT("view-source:");
    loc += uri;
    OpenURI(loc);
}

// (METHOD) wxWebControl::SaveCurrent
// Description:
//
// Syntax: bool wxWebControl::SaveCurrent(const wxString& destination_path)
//
// Remarks:
//
// Returns:

bool wxWebControl::SaveCurrent(const wxString& destination_path)
{
    // this code is supposed to download the page from the cache,
    // but it's not working right now.  nsIWebBrowserPersist::SaveURI
    // isn't finished saving the document by the time that OpenURI is called

	/*
    nsCOMPtr<nsIWebBrowserPersist> persist = m_ptrs->m_web_browser;
    
    if (!persist)
    {
        wxFAIL_MSG(wxT("wxWebControl::ViewSource(): nsIWebBrowserPersist interface not available"));
        return false;
    }
        
    
    nsCOMPtr<nsILocalFile> file = nsNewLocalFile(destination_path);
    
    if (!file)
    {
        wxFAIL_MSG(wxT("wxWebControl::ViewSource(): could not create temporary file"));
        return false;
    }

    nsresult res = persist->SaveURI(nsnull, // url - nsnull equals "this document"
                                    nsnull, // cache key
                                    nsnull, // nsIURI referrer
                                    nsnull, // post data
                                    nsnull, // extra headers
                                    getter_AddRefs(file)); // target file
    
    if (NS_FAILED(res))
        return false;
        */
    return true;
}

// (METHOD) wxWebControl::GetTextZoom
// Description:
//
// Syntax: void wxWebControl::GetTextZoom(float* zoom)
//
// Remarks:
//
// Returns:

void wxWebControl::GetTextZoom(float* zoom)
{
    if (!IsOk())
        return;

    nsCOMPtr<nsIDOMWindow> dom_window;
    m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    dom_window->GetTextZoom(zoom);
}

// (METHOD) wxWebControl::SetTextZoom
// Description:
//
// Syntax: void wxWebControl::SetTextZoom(float zoom)
//
// Remarks:
//
// Returns:

void wxWebControl::SetTextZoom(float zoom)
{
    if (!IsOk())
        return;

    nsCOMPtr<nsIDOMWindow> dom_window;
    m_ptrs->m_web_browser->GetContentDOMWindow(getter_AddRefs(dom_window));
    dom_window->SetTextZoom(zoom);
}

// (METHOD) wxWebControl::CanCutSelection
// Description:
//
// Syntax: bool wxWebControl::CanCutSelection()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanCutSelection()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanCutSelection(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanCutSelection(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CanCopySelection
// Description:
//
// Syntax: bool wxWebControl::CanCopySelection()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanCopySelection()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanCopySelection(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanCopySelection(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CanCopyLinkLocation
// Description:
//
// Syntax: bool wxWebControl::CanCopyLinkLocation()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanCopyLinkLocation()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanCopyLinkLocation(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanCopyLinkLocation(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CanCopyImageLocation
// Description:
//
// Syntax: bool wxWebControl::CanCopyImageLocation()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanCopyImageLocation()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanCopyImageLocation(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanCopyImageLocation(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CanCopyImageContents
// Description:
//
// Syntax: bool wxWebControl::CanCopyImageContents()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanCopyImageContents()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanCopyImageContents(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanCopyImageContents(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CanPaste
// Description:
//
// Syntax: bool wxWebControl::CanPaste()
//
// Remarks:
//
// Returns:

bool wxWebControl::CanPaste()
{
    if (!IsOk())
        return false;
#if MOZILLA_VERSION_1 >=10
    bool retval;
    m_ptrs->m_clipboard_commands->CanPaste(&retval);
    return retval;
#else
    PRBool retval;
    m_ptrs->m_clipboard_commands->CanPaste(&retval);
    return retval ? true : false;
#endif
}

// (METHOD) wxWebControl::CutSelection
// Description:
//
// Syntax: void wxWebControl::CutSelection()
//
// Remarks:
//
// Returns:

void wxWebControl::CutSelection()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->CutSelection();
}

// (METHOD) wxWebControl::CopySelection
// Description:
//
// Syntax: void wxWebControl::CopySelection()
//
// Remarks:
//
// Returns:

void wxWebControl::CopySelection()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->CopySelection();
}

// (METHOD) wxWebControl::CopyLinkLocation
// Description:
//
// Syntax: void wxWebControl::CopyLinkLocation()
//
// Remarks:
//
// Returns:

void wxWebControl::CopyLinkLocation()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->CopyLinkLocation();
}

// (METHOD) wxWebControl::CopyImageLocation
// Description:
//
// Syntax: void wxWebControl::CopyImageLocation()
//
// Remarks:
//
// Returns:

void wxWebControl::CopyImageLocation()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->CopyImageLocation();
}

// (METHOD) wxWebControl::CopyImageContents
// Description:
//
// Syntax: void wxWebControl::CopyImageContents()
//
// Remarks:
//
// Returns:

void wxWebControl::CopyImageContents()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->CopyImageContents();
}

// (METHOD) wxWebControl::Paste
// Description:
//
// Syntax: void wxWebControl::Paste()
//
// Remarks:
//
// Returns:

void wxWebControl::Paste()
{
    if (!IsOk())
        return;
    
    m_ptrs->m_clipboard_commands->Paste();
}

// (METHOD) wxWebControl::SelectAll
// Description:
//
// Syntax: void wxWebControl::SelectAll()
//
// Remarks:
//
// Returns:

void wxWebControl::SelectAll()
{
    if (!IsOk())
        return;
        
    m_ptrs->m_clipboard_commands->SelectAll();
}

// (METHOD) wxWebControl::SelectNone
// Description:
//
// Syntax: void wxWebControl::SelectNone()
//
// Remarks:
//
// Returns:

void wxWebControl::SelectNone()
{
    if (!IsOk())
        return;
        
    m_ptrs->m_clipboard_commands->SelectNone();
}

void wxWebControl::OnSetFocus(wxFocusEvent& evt)
{
    if (!IsOk())
        return;

    std::cout << "OnSetFocus" << std::endl;

    nsCOMPtr<nsIWebBrowserFocus> focus = do_QueryInterface(m_ptrs->m_web_browser);
    if (!focus)
        return;
    
    focus->Activate();
}

void wxWebControl::OnKillFocus(wxFocusEvent& evt)
{
	std::cout << "OnKillFocus" << std::endl;
}

void wxWebControl::OnEraseBackground(wxEraseEvent& evt)
{
	std::cout << "OnEraseBackground" << std::endl;
	if(!m_browser_ready) {
		m_browser_ready = true;
		std::cout << "Create timer object" << std::endl;
		//TODO do it as singleton
		DelayedWindowCreator *creator = new DelayedWindowCreator(this,1);
	}
}


void wxWebControl::OnSize(wxSizeEvent& evt)
{
    if (!IsOk())
        return;

    wxRect cli_rect = this->GetClientRect();

    if (m_ptrs->m_base_window)
    {
        std::cout << "wxWebControl::OnSize" << std::endl;
        m_ptrs->m_base_window->SetPositionAndSize(0, 0,
                                    cli_rect.GetWidth(),
                                    cli_rect.GetHeight(),
                                    true);
    }
}


bool wxWebControl::Execute(const wxString& js_code)
{
    nsresult rv;
	//FIXME enable again
	/*
    nsCOMPtr<nsIScriptSecurityManager> security_manager;
    security_manager = do_GetService("@mozilla.org/scriptsecuritymanager;1");
    if (!security_manager)
        return false;
    */
	return false;
	/*
    nsCOMPtr<nsIPrincipal> principal;
    security_manager->GetSystemPrincipal(getter_AddRefs(principal));
    if (!principal)
        return false;*/
	/*
    nsCOMPtr<nsIScriptGlobalObject> sgo = do_QueryInterface(m_ptrs->m_web_browser);
    if (!sgo)
        return false;
    nsCOMPtr<nsIScriptContext> ctx = sgo->GetContext();
    if (!ctx)
        return false;
	*/
    /*
    void* obj = sgo->GetGlobalJSObject();
    
    nsEmbedString str;
    wx2ns(js_code, str);
    */
#if MOZILLA_VERSION_1 >= 11
    //JS::Value out;
#elif MOZILLA_VERSION_1 < 9
    //jsval out;
#else
	void* out;
#endif
	/*
    rv = ctx->EvaluateStringWithValue(
        str,
        sgo->GetGlobalJSObject(),
        principal,
        "mozembed",
        0,
        nsnull,
        &out,
        nsnull);
        */
    return true;
}
