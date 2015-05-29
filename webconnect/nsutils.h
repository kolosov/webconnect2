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

//wxString ns2wx(nsString& str);
wxString ns2wx(nsEmbedCString& str);
wxString ns2wx(nsEmbedString& str);
wxString ns2wx(const char16_t* str);
void wx2ns(const wxString& wxstr, nsEmbedString& nsstr);
void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr);
char16_t* wxToUnichar(const wxString& wxstr);
void freeUnichar(PRUnichar* p);


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
    ~ProgressListenerAdaptor();
    
    void ClearProgressReference()
    {
        m_progress = NULL;
    }

    NS_IMETHODIMP Init(nsIURI* source,
                       nsIURI* target,
                       const nsAString& display_name,
                       nsIMIMEInfo* mime_info,
                       PRTime start_time,
                       nsILocalFile* temp_file,
                       nsICancelable* cancelable);
                       
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
                             const PRUnichar* message);

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

public:

    wxWebProgressBase* m_progress;
};


nsIWebProgressListener* CreateProgressListenerAdaptor(wxWebProgressBase* progress);


#endif

