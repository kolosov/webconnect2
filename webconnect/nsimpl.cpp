///////////////////////////////////////////////////////////////////////////////
// Name:        nsimpl.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov
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

//#include "moz_config.h"

#ifdef __WXMSW__
#include <windows.h>
#else
#include <dlfcn.h>
#endif

//#include <iostream>
//using namespace std;

///////////////////////////////////////////////////////////////////////////////
//  function declarations
///////////////////////////////////////////////////////////////////////////////


typedef nsresult (*UnusedFunc)();
typedef nsresult (*InitFunc)(nsIServiceManager** result, nsIFile* bin_directory, nsIDirectoryServiceProvider* app_file_location_provider);
typedef void*    (*AllocFunc)(PRSize size);
typedef void     (*FreeFunc)(void* ptr);
typedef nsresult (*GetServiceManagerFunc)(nsIServiceManager** result);
typedef nsresult (*GetComponentManagerFunc)(nsIComponentManager** result);
typedef nsresult (*GetComponentRegistrarFunc)(nsIComponentRegistrar** result);
typedef nsresult (*StringContainerInitFunc)(nsStringContainer& str);
typedef nsresult (*StringContainerInit2Func)(nsStringContainer& str, const PRUnichar* str_data, PRUint32 len, PRUint32 flags);
typedef void     (*StringContainerFinishFunc)(nsStringContainer& str);
typedef nsresult (*NewNativeLocalFileFunc)(const nsACString& path, PRBool follow_links, nsILocalFile** result);
typedef nsresult (*StringSetDataFunc)(nsAString& str, const PRUnichar* str_data, PRUint32 len);
typedef PRUint32 (*StringGetDataFunc)(const nsAString& str, const PRUnichar** str_data, PRBool* terminated);
typedef nsresult (*CStringContainerInitFunc)(nsCStringContainer& str);
typedef nsresult (*CStringContainerInit2Func)(nsCStringContainer& str, const char* str_data, PRUint32 len, PRUint32 flags);
typedef void     (*CStringContainerFinishFunc)(nsCStringContainer& str);
typedef PRUint32 (*CStringGetDataFunc)(const nsACString& str, const char** str_data, PRBool* terminated);


struct XPCOMFunctionTable
{
    PRUint32 version;
    PRUint32 size;

    InitFunc Init;
    UnusedFunc /*ShutdownFunc*/ Shutdown;
    GetServiceManagerFunc GetServiceManager;
    GetComponentManagerFunc GetComponentManager;
    GetComponentRegistrarFunc GetComponentRegistrar;
    UnusedFunc /*GetMemoryManagerFunc*/ GetMemoryManager;
    UnusedFunc /*NewLocalFileFunc*/ NewLocalFile;
    NewNativeLocalFileFunc NewNativeLocalFile;
    UnusedFunc /*RegisterXPCOMExitRoutineFunc*/ RegisterExitRoutine;
    UnusedFunc /*UnregisterXPCOMExitRoutineFunc*/ UnregisterExitRoutine;
    UnusedFunc /*GetDebugFunc*/ GetDebug;
    UnusedFunc /*GetTraceRefcntFunc*/ GetTraceRefcnt;
    StringContainerInitFunc StringContainerInit;
    StringContainerFinishFunc StringContainerFinish;
    StringGetDataFunc StringGetData;
    StringSetDataFunc StringSetData;
    UnusedFunc /*StringSetDataRangeFunc*/ StringSetDataRange;
    UnusedFunc /*StringCopyFunc*/ StringCopy;
    CStringContainerInitFunc CStringContainerInit;
    CStringContainerFinishFunc CStringContainerFinish;
    CStringGetDataFunc CStringGetData;
    UnusedFunc /*CStringSetDataFunc*/ CStringSetData;
    UnusedFunc /*CStringSetDataRangeFunc*/ CStringSetDataRange;
    UnusedFunc /*CStringCopyFunc*/ CStringCopy;
    UnusedFunc /*CStringToUTF16*/ CStringToUTF16;
    UnusedFunc /*UTF16ToCString*/ Utf16ToCString;
    UnusedFunc /*StringCloneDataFunc*/ StringCloneData;
    UnusedFunc /*CStringCloneDataFunc*/ CStringCloneData;
    AllocFunc Alloc;
    UnusedFunc /*ReallocFunc*/ Realloc;
    FreeFunc Free;
    StringContainerInit2Func StringContainerInit2;
    CStringContainerInit2Func CStringContainerInit2;
    UnusedFunc /*StringGetMutableDataFunc*/ StringGetMutableData;
    UnusedFunc /*CStringGetMutableDataFunc*/ CStringGetMutableData;
    UnusedFunc /*Init3Func*/ Init3;
};


static XPCOMFunctionTable funcs;

static bool isLibraryOk()
{
    return (funcs.Init != 0) ? true : false;
}

typedef nsresult (PR_CALLBACK *GetFrozenFunctionsFunc)(XPCOMFunctionTable *func_table,
                                                       const char* path);




#ifdef __WXMSW__
#define XPCOM_PATH_SEPARATOR L'\\'
#else
#define XPCOM_PATH_SEPARATOR L'/'
#endif

/*

static void GetDependentLibraryList(const char* xpcom_dll_path, wxArrayString& arr)
{
    // first load dependent libraries
	//wxString fname = wxString::FromAscii(xpcom_dll_path);
	//find last path separator
	int s_len = strlen(xpcom_dll_path);
	int last_slash = 0;
	const char * s_path = xpcom_dll_path;
#ifdef __WXMSW__
	const char *s_separ = "\\";
#else
	const char *s_separ = "/";
#endif
	const char *s_file_name = "dependentlibs.list";
	for(int i=0;i<s_len;i++)
	{
		if(s_path[i]==XPCOM_PATH_SEPARATOR) last_slash=i;
	}
	int buf_len = last_slash+strlen(s_separ)+strlen(s_file_name)+1;
	char *deplib_list_fname_c = (char *)malloc(buf_len);
	if(!deplib_list_fname_c) return;
	bzero(deplib_list_fname_c, buf_len);
	strncat(deplib_list_fname_c, xpcom_dll_path, last_slash);
	strncat(deplib_list_fname_c, s_separ, strlen(s_separ));
	strncat(deplib_list_fname_c, s_file_name, strlen(s_file_name));

	//strncat(deplib_list_fname_c, xpc);

	//snprintf(deplib_list_fname_c, buf_len, "%s%s%s",s_path, s_separ,s_file_name);
    wxString fname = wxString::FromUTF8(xpcom_dll_path);
    wxString xpcom_dir;
    
    // remove a trailing slash, if any
    if (fname.Length() > 0 && fname.Last() == XPCOM_PATH_SEPARATOR)
        fname.RemoveLast();
    xpcom_dir = fname.BeforeLast(XPCOM_PATH_SEPARATOR);
    
    // load 'dependentlibs.list'
    wxString deplib_list_fname = xpcom_dir;
    cout << xpcom_dir.mb_str() << endl;
    deplib_list_fname += XPCOM_PATH_SEPARATOR;
    deplib_list_fname += wxT("dependentlibs.list");
   
    FILE* ff = fopen(deplib_list_fname_c, "r");
    if (ff)
    {
        wxString full_path;
        char buf[1024];
        while (fgets(buf, 1000, ff))
        {
            // ignore empty lines and comment lines
            if (!buf[0] || buf[0] == '#')
                continue;
            
            full_path = xpcom_dir;
            full_path += XPCOM_PATH_SEPARATOR;
            full_path += wxString::FromAscii(buf);
            
            // remove trailing \n
            if (full_path.Length() > 1 && full_path.Last() == L'\n')
                full_path.RemoveLast();
                
            arr.Add(full_path);
        }
        
        fclose(ff);
    }
}


#ifdef __WXMSW__
nsresult XPCOMGlueStartup(const char* xpcom_dll_path)
{
    // load library dependencies
    wxArrayString deplibs;
    GetDependentLibraryList(xpcom_dll_path, deplibs);
    size_t i, count = deplibs.GetCount();
    for (i = 0; i < count; ++i)
    {
        HMODULE h = LoadLibraryExA(deplibs.Item(i).mbc_str(),
                               0,
                               LOAD_WITH_ALTERED_SEARCH_PATH);
    }
    
    // now load the functions from xpcom.dll
    
    HMODULE h = LoadLibraryExA(xpcom_dll_path, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
    
    GetFrozenFunctionsFunc f;
    f = (GetFrozenFunctionsFunc)GetProcAddress(h, "NS_GetFrozenFunctions");
    
    if (!f)
    {
        FreeLibrary(h);
        return NS_ERROR_FAILURE;
    }
    
    nsresult res;
    funcs.version = 1;
    funcs.size = sizeof(XPCOMFunctionTable);
    res = f(&funcs, xpcom_dll_path);
    
    if (NS_FAILED(res))
    {
        FreeLibrary(h);
        return NS_ERROR_FAILURE;
    }

    return 0;
}

#else

nsresult XPCOMGlueStartup(const char* xpcom_dll_path)
{
    // load library dependencies
    wxArrayString deplibs;
    GetDependentLibraryList(xpcom_dll_path, deplibs);
    size_t i, count = deplibs.GetCount();
    for (i = 0; i < count; ++i)
    {
        void* handle = dlopen(deplibs.Item(i).mbc_str(), RTLD_GLOBAL | RTLD_LAZY);
    }
    
    // now load the functions from libxpcom.so
    
    void* h = dlopen(xpcom_dll_path, RTLD_GLOBAL | RTLD_LAZY);
    if (!h)
    {
    	char *s = dlerror();
        return NS_ERROR_FAILURE;
    }
    GetFrozenFunctionsFunc f =
    f = (GetFrozenFunctionsFunc)dlsym(h, "NS_GetFrozenFunctions");
    if (!f)
    {
        dlclose(h);
        return NS_ERROR_FAILURE;
    }
    
    nsresult res;
    funcs.version = 1;
    funcs.size = sizeof(XPCOMFunctionTable);
    res = f(&funcs, xpcom_dll_path);
    
    if (NS_FAILED(res))
    {
        dlclose(h);
        return NS_ERROR_FAILURE;
    }

    return 0;
}

#endif


nsresult NS_InitXPCOM2(nsIServiceManager** result,
                       nsIFile* bin_directory,
                       nsIDirectoryServiceProvider* app_file_location_provider)
{
    if (!funcs.Init)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.Init(result, bin_directory, app_file_location_provider);
}

void* NS_Alloc(PRSize size)
{
    if (!funcs.Alloc)
        return NULL;
        
    return funcs.Alloc(size);
}

void NS_Free(void* ptr)
{
    if (!funcs.Free)
        return;
    
    funcs.Free(ptr);
}

nsresult NS_GetServiceManager(nsIServiceManager** result)
{
    if (!funcs.GetServiceManager)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetServiceManager(result);
}

nsresult NS_GetComponentManager(nsIComponentManager** result)
{
    if (!funcs.GetComponentManager)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetComponentManager(result);
}

nsresult NS_GetComponentRegistrar(nsIComponentRegistrar** result)
{
    if (!funcs.GetComponentRegistrar)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.GetComponentRegistrar(result);
}

nsresult NS_StringContainerInit(nsStringContainer& str)
{
    if (!funcs.StringContainerInit)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringContainerInit(str);
}

nsresult NS_StringContainerInit2(nsStringContainer& str,
                                 const PRUnichar* str_data,
                                 PRUint32 len,
                                 PRUint32 flags)
{
    if (!funcs.StringContainerInit2)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringContainerInit2(str, str_data, len, flags);
}

void NS_StringContainerFinish(nsStringContainer& str)
{
    if (!funcs.StringContainerFinish)
        return;

    funcs.StringContainerFinish(str);
}

nsresult NS_NewNativeLocalFile(const nsACString& path,
                               PRBool follow_links,
                               nsILocalFile** result)
{
    if (!funcs.NewNativeLocalFile)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.NewNativeLocalFile(path, follow_links, result);
}

nsresult NS_StringSetData(nsAString& str,
                          const PRUnichar* str_data,
                          PRUint32 len)
{
    if (!funcs.StringSetData)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringSetData(str, str_data, len);
}

PRUint32 NS_StringGetData(const nsAString& str,
                          const PRUnichar** str_data,
                          PRBool* terminated)
{
    if (!funcs.StringGetData)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.StringGetData(str, str_data, terminated);
}

nsresult NS_CStringContainerInit(nsCStringContainer& str)
{
    if (!funcs.CStringContainerInit)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.CStringContainerInit(str);
}

nsresult NS_CStringContainerInit2(nsCStringContainer& str,
                                  const char* str_data,
                                  PRUint32 len,
                                  PRUint32 flags)
{
    if (!funcs.CStringContainerInit2)
        return NS_ERROR_NOT_INITIALIZED;

    return funcs.CStringContainerInit2(str, str_data, len, flags);
}

void NS_CStringContainerFinish(nsCStringContainer& str)
{
    if (!funcs.CStringContainerFinish)
        return;

    funcs.CStringContainerFinish(str);
}

PRUint32 NS_CStringGetData(const nsACString& str,
                           const char** str_data,
                           PRBool* terminated)
{
    if (!funcs.CStringGetData)
        return 0;

    return funcs.CStringGetData(str, str_data, terminated);
}
*/



///////////////////////////////////////////////////////////////////////////////
//  utility functions
///////////////////////////////////////////////////////////////////////////////

/*wxString ns2wx(nsString& str)
{
    wxString res;
    //res = wxString::FromAscii(str.get());
    return res;
}*/

wxString ns2wx(nsEmbedCString& str)
{
    wxString res;
    res = wxString::FromAscii(str.get());
    return res;
}


wxString ns2wx(nsEmbedString& str)
{
    wxString res;
    const PRUnichar* begin;
    const PRUnichar* end;
    
    PRUint32 i, len = NS_StringGetData(str, &begin);
    end = begin + len;
    
    res.Alloc(end - begin);
    for (i = 0; i < len; ++i)
        res += (wxChar)(*(begin+i));
    
    return res;
}

wxString ns2wx(const PRUnichar* str)
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
    PRUnichar* buf = new PRUnichar[len+1];
    for (i = 0; i < len; ++i)
        buf[i] = wxstr.GetChar(i);
    nsstr.Assign(buf, len);
    delete[] buf;
}

void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr)
{
    nsstr.Assign(wxstr.mbc_str());
}

PRUnichar* wxToUnichar(const wxString& wxstr)
{
    size_t i,len = wxstr.Length();
    PRUnichar* ret = (PRUnichar*)NS_Alloc((len+1) * sizeof(PRUnichar));
    for (i = 0; i < len; ++i)
        *(ret+i) = (PRUnichar)wxstr.GetChar(i);
    *(ret+len) = 0;
    return ret;
}

void freeUnichar(PRUnichar* p)
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
#if MOZILLA_VERSION_1 < 1
nsCOMPtr<nsIPref> nsGetPrefService()
{
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsCOMPtr<nsIPref> result;
    nsresult res;
    
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_IPREF_IID;
    res = service_mgr->GetServiceByContractID("@mozilla.org/preferences;1",
                                        iid,
                                        (void**)&result.p);
    
    return result;
}
#else
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
#endif

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
/*
nsCOMPtr<nsISupports> nsGetService(const char* contract_id)
{
    nsCOMPtr<nsISupports> result;
    nsCOMPtr<nsIServiceManager> service_mgr;
    nsresult res;
    
    res = NS_GetServiceManager(getter_AddRefs(service_mgr));
    if (NS_FAILED(res))
        return result;
    
    nsIID iid = NS_ISUPPORTS_IID;
    service_mgr->GetServiceByContractID(contract_id,
                                        iid,
                                        getter_AddRefs(result));
    
    return result;
}

nsCOMPtr<nsISupports> nsCreateInstance(const char* contract_id)
{
    nsCOMPtr<nsISupports> res;
    static nsIID nsISupportsIID = NS_ISUPPORTS_IID;
    nsresult result;
    
    nsCOMPtr<nsIComponentManager> comp_mgr;
    result = NS_GetComponentManager(getter_AddRefs(comp_mgr));
    if (comp_mgr)
    {
        result = comp_mgr->CreateInstanceByContractID(contract_id,
                                             0,
                                             nsISupportsIID,
                                             getter_AddRefs(res));
    }
    
    return res;
}

nsCOMPtr<nsISupports> nsCreateInstance(const nsCID& cid)
{
    nsCOMPtr<nsISupports> res;
    static nsIID nsISupportsIID = NS_ISUPPORTS_IID;
    
    nsCOMPtr<nsIComponentManager> comp_mgr;
    NS_GetComponentManager(getter_AddRefs(comp_mgr));
    if (comp_mgr)
    {
        comp_mgr->CreateInstance(cid,
                                 0,
                                 nsISupportsIID,
                                 getter_AddRefs(res));
    }
    
    return res;
}*/

nsCOMPtr<nsILocalFile> nsNewLocalFile(const wxString& filename)
{
    nsresult res = 0;
    nsCOMPtr<nsILocalFile> ret;
    
    res = NS_NewNativeLocalFile(nsDependentCString((const char*)filename.mbc_str()), TRUE, getter_AddRefs(ret));
    
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
    
    io_service->NewURI(nsDependentCString(cstr_spec.c_str()), nsnull, nsnull, getter_AddRefs(res));
    
    return res;
}




///////////////////////////////////////////////////////////////////////////////
//  ProgressListenerAdaptor18 class implementation
///////////////////////////////////////////////////////////////////////////////

#if MOZILLA_VERSION_1 < 1
ProgressListenerAdaptor18::ProgressListenerAdaptor18(wxWebProgressBase* progress)
{
    m_progress = progress;
    
    if (m_progress)
        m_progress->OnStart();
}

ProgressListenerAdaptor18::~ProgressListenerAdaptor18()
{
}

NS_IMETHODIMP ProgressListenerAdaptor18::Init(
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

NS_IMETHODIMP ProgressListenerAdaptor18::OnStateChange(
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

NS_IMETHODIMP ProgressListenerAdaptor18::OnProgressChange(
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

NS_IMETHODIMP ProgressListenerAdaptor18::OnProgressChange64(
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
                                     
        if (m_progress->IsCancelled())
            request->Cancel(0x804b0002 /*NS_BINDING_ABORTED*/);
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor18::OnLocationChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     nsIURI* location)
{
   return NS_OK;
}

NS_IMETHODIMP ProgressListenerAdaptor18::OnStatusChange(
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


NS_IMETHODIMP ProgressListenerAdaptor18::OnSecurityChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     PRUint32 state)
{
   return NS_OK;
}

             
NS_IMPL_ADDREF(ProgressListenerAdaptor18)
NS_IMPL_RELEASE(ProgressListenerAdaptor18)

NS_INTERFACE_MAP_BEGIN(ProgressListenerAdaptor18)
    NS_INTERFACE_MAP_ENTRY(nsISupports)
    NS_INTERFACE_MAP_ENTRY(nsITransfer18)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener2_18)
    NS_INTERFACE_MAP_ENTRY(nsIWebProgressListener)
NS_INTERFACE_MAP_END
#endif



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
                                     
        if (m_progress->IsCancelled())
            request->Cancel(0x804b0002 /*NS_BINDING_ABORTED*/);
    }
    
    return NS_OK;
}


NS_IMETHODIMP ProgressListenerAdaptor::OnLocationChange(
                     nsIWebProgress* web_progress,
                     nsIRequest* request,
                     nsIURI* location)
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
#if MOZILLA_VERSION_1 < 1
    if (wxWebControl::IsVersion18())
    {
        ProgressListenerAdaptor18* p = new ProgressListenerAdaptor18(progress);
        p->AddRef();
        return (nsIWebProgressListener*)p;
    }
     else
#endif
    {
        ProgressListenerAdaptor* p = new ProgressListenerAdaptor(progress);
        p->AddRef();
        return (nsIWebProgressListener*)p;
    }
}
