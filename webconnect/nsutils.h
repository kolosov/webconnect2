///////////////////////////////////////////////////////////////////////////////
// Name:        nsutils.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2006-10-08
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSUTILS_H
#define __WXWEBCONNECT_NSUTILS_H

#include "moz_config.h"

wxString ns2wx(nsEmbedCString& str);
wxString ns2wx(nsEmbedString& str);
wxString ns2wx(const PRUnichar* str);
void wx2ns(const wxString& wxstr, nsEmbedString& nsstr);
void wx2ns(const wxString& wxstr, nsEmbedCString& nsstr);
PRUnichar* wxToUnichar(const wxString& wxstr);
void freeUnichar(PRUnichar* p);


nsCOMPtr<nsIWindowWatcher> nsGetWindowWatcherService();
#if MOZILLA_VERSION_1 < 1
nsCOMPtr<nsIPref> nsGetPrefService();
#else
nsCOMPtr<nsIPrefBranch> nsGetPrefService();
#endif
nsCOMPtr<nsIProperties> nsGetDirectoryService();
nsCOMPtr<nsISupports> nsGetService(const char* contract_id);
nsCOMPtr<nsISupports> nsCreateInstance(const char* contract_id);
nsCOMPtr<nsISupports> nsCreateInstance(const nsCID& cid);
nsCOMPtr<nsILocalFile> nsNewLocalFile(const wxString& filename);
nsCOMPtr<nsIURI> nsNewURI(const wxString& spec);




class wxWebProgressBase;

#if MOZILLA_VERSION_1 < 1
class ProgressListenerAdaptor18 : public nsITransfer18
{
public:

    NS_DECL_ISUPPORTS

    ProgressListenerAdaptor18(wxWebProgressBase* progress);
    ~ProgressListenerAdaptor18();
    
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
                             nsIURI* location);
    NS_IMETHOD OnStatusChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             nsresult status,
                             const PRUnichar* message);

    NS_IMETHOD OnSecurityChange(
                             nsIWebProgress* web_progress,
                             nsIRequest* request,
                             PRUint32 state);
public:

    wxWebProgressBase* m_progress;
};
#endif


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
                             nsIURI* location);
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
                            PRBool same_uri,
                            PRBool *retval);

public:

    wxWebProgressBase* m_progress;
};


nsIWebProgressListener* CreateProgressListenerAdaptor(wxWebProgressBase* progress);


#endif
