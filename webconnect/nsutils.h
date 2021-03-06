///////////////////////////////////////////////////////////////////////////////
// Name:        nsutils.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSUTILS_H
#define __WXWEBCONNECT_NSUTILS_H

#include "moz_config.h"


#if defined(_WIN32)
#define defXULPathDir  GECKO_SDK_PATH_CONFIG "\\bin\\"
#define defXULPathFile defXULPathDir "xpcom.dll"
#elif defined(__linux__)
#define defXULPathDir  GECKO_SDK_PATH_CONFIG "/bin/"
#define defXULPathFile defXULPathDir "libxpcom.so"
#elif defined(__APPLE__)
#define defXULPathDir  GECKO_SDK_PATH_CONFIG "/bin/XUL.framework/Versions/Current/"
#define defXULPathFile defXULPathDir "XUL"
#else
#error "Unsupported platform!"
#endif


#include "nsCOMPtr.h"
#include "nsStringAPI.h"
#include "nsEmbedString.h"
#include "nsIWindowWatcher.h"
#include "nsIServiceManager.h"
#include "nsIPrefBranch.h"
#include "nsIProperties.h"
#include "nsIIOService.h"
#include "nsITransfer.h"
#include "nsIRequest.h"


//wxString ns2wx(nsString& str);
wxString ns2wx(nsEmbedCString& str);
wxString ns2wx(nsEmbedString& str);
wxString ns2wx(const char16_t* str);
void wx2ns(const wxString& wxstr, nsEmbedString& nsstr);
void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr);
char16_t* wxToUnichar(const wxString& wxstr);
void freeUnichar(char16_t* p);


nsCOMPtr<nsIWindowWatcher> nsGetWindowWatcherService();
nsCOMPtr<nsIPrefBranch> nsGetPrefService();

nsCOMPtr<nsIProperties> nsGetDirectoryService();
/*nsCOMPtr<nsISupports> nsGetService(const char* contract_id);
nsCOMPtr<nsISupports> nsCreateInstance(const char* contract_id);
nsCOMPtr<nsISupports> nsCreateInstance(const nsCID& cid);*/
nsCOMPtr<nsIFile> nsNewLocalFile(const wxString& filename);
nsCOMPtr<nsIURI> nsNewURI(const wxString& spec);

class wxWebProgressBase;


class ProgressListenerAdaptor : public nsITransfer
{
public:

    NS_DECL_ISUPPORTS

    ProgressListenerAdaptor(wxWebProgressBase* progress);
    
    void ClearProgressReference()
    {
        m_progress = NULL;
    }

    NS_IMETHOD Init(nsIURI* source,
                       nsIURI* target,
                       const nsAString& display_name,
                       nsIMIMEInfo* mime_info,
                       PRTime start_time,
                       nsIFile* temp_file,
                       nsICancelable* cancelable,
					   bool isPrivate);

    NS_IMETHOD SetSha256Hash(const nsACString & aHash);

    NS_IMETHOD SetSignatureInfo(nsIArray *aSignatureInfo);
                       
    NS_IMETHOD OnStateChange(nsIWebProgress* web_progress, 
                             nsIRequest* request,
                             PRUint32 state_flags, 
                             nsresult status);
                             
    NS_IMETHOD OnProgressChange(nsIWebProgress* web_progress,
                                nsIRequest* request,
                                PRInt32 cur_self_progress,
                                PRInt32 max_self_progress,
                                PRInt32 cur_total_progress,
                                PRInt32 max_total_progress);
    
    NS_IMETHOD OnProgressChange64(
                                 nsIWebProgress* web_progress,
                                 nsIRequest* request,
                                 PRInt64 cur_self_progress,
                                 PRInt64 max_self_progress,
                                 PRInt64 cur_total_progress,
                                 PRInt64 max_total_progress);
    NS_IMETHOD OnLocationChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
#if MOZILLA_VERSION_1 >= 11
                     nsIURI* location,
                     PRUint32 aflags);
#else
                     nsIURI* location);
#endif
    NS_IMETHOD OnStatusChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             nsresult status,
                             const char16_t* message);

    NS_IMETHOD OnSecurityChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state);
                             
    NS_IMETHOD OnRefreshAttempted(
                            nsIWebProgress* web_progress,
                            nsIURI* refresh_uri,
                            PRInt32 millis,
#if MOZILLA_VERSION_1 >=10
                            bool same_uri,
                            bool *retval);
#else
                            PRBool same_uri,
                            PRBool *retval);
#endif

#if MOZILLA_VERSION_1 >=32
                            NS_IMETHOD SetRedirects(nsIArray *aRedirects);
#endif


public:

    wxWebProgressBase* m_progress;

protected:
    ~ProgressListenerAdaptor();

};


nsIWebProgressListener* CreateProgressListenerAdaptor(wxWebProgressBase* progress);


#endif

