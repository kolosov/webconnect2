///////////////////////////////////////////////////////////////////////////////
// Name:        nsimpl.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#define _CRT_SECURE_NO_WARNINGS


#include <string>
#include <wx/wx.h>
#include "webcontrol.h"
#include "nsinclude.h"

#ifdef __WXMSW__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

//#include <iostream>
//using namespace std;

#ifdef __WXMSW__
#define XPCOM_PATH_SEPARATOR L'\\'
#else
#define XPCOM_PATH_SEPARATOR L'/'
#endif

wxString ns2wx(nsEmbedCString& str)
{
    wxString res;
    res = wxString::FromAscii(str.get());
    return res;
}


wxString ns2wx(nsEmbedString& str)
{
    wxString res;
    const char16_t* begin;
    const char16_t* end;
    
    uint32_t i, len = NS_StringGetData(str, &begin);
    end = begin + len;
    
    res.Alloc(end - begin);
    for (i = 0; i < len; ++i)
        res += (wxChar)(*(begin+i));
    
    return res;
}

wxString ns2wx(const char16_t* str)
{
    if (!str)
        return wxT("");
        
    wxString res;
    size_t i, len = 0;
    while (*(str+len))
        len++;
    res.Alloc(len);
    for (i = 0; i < len; ++i)
        res += *(str+i);
    return res;
}

void wx2ns(const wxString& wxstr, nsEmbedString& nsstr)
{
    size_t i, len = wxstr.Length();
    char16_t* buf = new char16_t[len+1];
    for (i = 0; i < len; ++i)
        buf[i] = wxstr.GetChar(i);
    nsstr.Assign(buf, len);
    delete[] buf;
}

void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr)
{
    nsstr.Assign(wxstr.mbc_str());
}

char16_t* wxToUnichar(const wxString& wxstr)
{
    size_t i,len = wxstr.Length();
    char16_t* ret = (char16_t*)NS_Alloc((len+1) * sizeof(char16_t));
    for (i = 0; i < len; ++i)
        *(ret+i) = (char16_t)wxstr.GetChar(i);
    *(ret+len) = 0;
    return ret;
}

void freeUnichar(char16_t* p)
{
    NS_Free((void*)p);
}

nsCOMPtr<nsIWindowWatcher> nsGetWindowWatcherService()
{
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsCOMPtr<nsIWindowWatcher> result;
    nsresult res;
    
    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IWINDOWWATCHER_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/embedcomp/window-watcher;1",
                                        iid,
                                        getter_AddRefs(result));
    
    return result;
}

nsCOMPtr<nsIPrefBranch> nsGetPrefService()
{
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsCOMPtr<nsIPrefBranch> result;
    nsresult res;

    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return result;

    //nsIID iid = NS_IPREFSERVICE_IID;
    nsIID iid = NS_IPREFBRANCH_IID;
    res = service_mgr->GetServiceByContractID("@mozilla.org/preferences-service;1",
                                        iid,
                                        getter_AddRefs(result));

    return result;
}

nsCOMPtr<nsIProperties> nsGetDirectoryService()
{
    nsCOMPtr<nsIProperties> result;
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IPROPERTIES_IID;
    res = service_mgr->GetServiceByContractID("@mozilla.org/file/directory_service;1",
                                        iid,
                                        getter_AddRefs(result));
    
    return result;
}

nsCOMPtr<nsIIOService> nsGetIOService()
{
    nsCOMPtr<nsIIOService> result;
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IIOSERVICE_IID;
    service_mgr->GetServiceByContractID("@mozilla.org/network/io-service;1",
                                        iid,
                                        getter_AddRefs(result));
    
    return result;
}

nsCOMPtr<nsIFile> nsNewLocalFile(const wxString& filename)
{
    nsresult res;
    nsCOMPtr<nsIFile> ret;

    /*NS_NewNativeLocalFile(const nsACString &path,
                          bool followLinks,
                          nsIFile* *result);*/

    res = NS_NewNativeLocalFile(nsDependentCString((const char*)filename.mbc_str()), true, getter_AddRefs(ret));
    
    //res = NS_NewNativeLocalFile(nsDependentCString((const char*)filename.mbc_str()), TRUE, getter_AddRefs(ret));
    
    //if (NS_FAILED(res))
    //    ret.clear();
    
    return ret;
}


nsCOMPtr<nsIURI> nsNewURI(const wxString& spec)
{
    nsCOMPtr<nsIURI> res;
    
    nsCOMPtr<nsIIOService> io_service = nsGetIOService();
    //if (io_service.empty())
    if (!io_service) //FIXME?
        return res;
    
    std::string cstr_spec = (const char*)spec.mbc_str();

    io_service->NewURI(nsDependentCString(cstr_spec.c_str()), NULL, NULL, getter_AddRefs(res));
    
    return res;
}


///////////////////////////////////////////////////////////////////////////////
//  ProgressListenerAdaptor class implementation
///////////////////////////////////////////////////////////////////////////////


ProgressListenerAdaptor::ProgressListenerAdaptor(wxWebProgressBase* progress)
{
    m_progress = progress;
    
    if (m_progress)
        m_progress->OnStart();
}

ProgressListenerAdaptor::~ProgressListenerAdaptor()
{
}

NS_IMETHODIMP ProgressListenerAdaptor::Init(
                                           nsIURI* source,
                                           nsIURI* target,
                                           const nsAString& display_name,
                                           nsIMIMEInfo* mime_info,
                                           PRTime start_time,
                                           nsILocalFile* temp_file,
                                           nsICancelable* cancelable)
{
    return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnStateChange(
                         nsIWebProgress* web_progress,
                         nsIRequest* request,
                         PRUint32 state_flags,
                         nsresult status)
{
    if (state_flags & nsIWebProgressListener::STATE_STOP)
    {
        if (m_progress)
            m_progress->OnFinish();
    }
    
    return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnProgressChange(
                         nsIWebProgress* web_progress,
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

NS_IMETHODIMP ProgressListenerAdaptor::OnProgressChange64(
                         nsIWebProgress* web_progress,
                         nsIRequest* request,
                         PRInt64 cur_self_progress,
                         PRInt64 max_self_progress,
                         PRInt64 cur_total_progress,
                         PRInt64 max_total_progress)
{
    if (m_progress)
    {
        m_progress->OnProgressChange(wxLongLong(cur_self_progress),
                                     wxLongLong(max_self_progress));

        nsresult status; //TODO set status
                                     
        if (m_progress->IsCancelled())
        	request->Cancel(status);
            //request->Cancel(0x804b0002 /*NS_BINDING_ABORTED*/);
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor::OnLocationChange(
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

NS_IMETHODIMP ProgressListenerAdaptor::OnStatusChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     nsresult status,
                     const PRUnichar* message)
{
    if (NS_FAILED(status))
    {
        if (m_progress && !m_progress->IsCancelled())
            m_progress->OnError(ns2wx(message));
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor::OnSecurityChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     PRUint32 state)
{
   return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor::OnRefreshAttempted(
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
             
NS_IMPL_ADDREF(ProgressListenerAdaptor)
NS_IMPL_RELEASE(ProgressListenerAdaptor)

NS_INTERFACE_MAP_BEGIN(ProgressListenerAdaptor)
    NS_INTERFACE_MAP_ENTRY(nsISupports)
    NS_INTERFACE_MAP_ENTRY(nsITransfer)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener2)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
NS_INTERFACE_MAP_END





nsIWebProgressListener* CreateProgressListenerAdaptor(wxWebProgressBase* progress)
{
        ProgressListenerAdaptor* p = new ProgressListenerAdaptor(progress);
        p->AddRef();
        return (nsIWebProgressListener*)p;
}
