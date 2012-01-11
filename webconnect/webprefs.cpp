///////////////////////////////////////////////////////////////////////////////
// Name:        webprefs.cpp
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2007-04-23
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#include <wx/wx.h>
#include "webframe.h"
#include "webcontrol.h"
#include "nsinclude.h"



///////////////////////////////////////////////////////////////////////////////
//  wxWebPreferences class implementation
///////////////////////////////////////////////////////////////////////////////


wxWebPreferences::wxWebPreferences()
{
}

bool wxWebPreferences::GetBoolPref(const wxString& name)
{
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    wxASSERT(!prefs.empty());
    if (!prefs)
        return false;
        
    PRBool val;
    if (NS_FAILED(prefs->GetBoolPref((const char*)name.mbc_str(), &val)))
        return false;
        
    return (val == PR_TRUE ? true : false);
}

wxString wxWebPreferences::GetStringPref(const wxString& name)
{
    wxString val;
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    wxASSERT(!prefs.empty());
    if (!prefs)
        return val;

    char* cstr = NULL;
    
    if (NS_FAILED(prefs->GetCharPref((const char*)name.mbc_str(), &cstr)))
        return val;
    
    if (cstr)
    {
        val = wxString::FromAscii(cstr);
        NS_Free(cstr);
    }
    
    return val;
}

int wxWebPreferences::GetIntPref(const wxString& name)
{
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    wxASSERT(!prefs.empty());
    if (!prefs)
        return 0;
    
    int val = 0;
    
    if (NS_FAILED(prefs->GetIntPref((const char*)name.mbc_str(), &val)))
        return 0;
        
    return val;
}

void wxWebPreferences::SetIntPref(const wxString& name, int value)
{
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    if (!prefs)
        return;
        
    prefs->SetIntPref((const char*)name.mbc_str(), value);
}

void wxWebPreferences::SetStringPref(const wxString& name, const wxString& value)
{
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    if (!prefs)
        return;

    prefs->SetCharPref((const char*)name.mbc_str(), (const char*)value.mbc_str());
}

void wxWebPreferences::SetBoolPref(const wxString& name, bool value)
{
#if MOZILLA_VERSION_1 < 2
    nsCOMPtr<nsIPref> prefs = nsGetPrefService();
#else
    nsCOMPtr<nsIPrefBranch> prefs = nsGetPrefService();
#endif
    if (!prefs)
        return;

    prefs->SetBoolPref((const char*)name.mbc_str(), value ? PR_TRUE : PR_FALSE);
}

