///////////////////////////////////////////////////////////////////////////////
// Name:        nsinclude.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov
// Created:     2006-10-09
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_NSINCLUDE_H
#define __WXWEBCONNECT_NSINCLUDE_H


#undef NS_WC_OLD_WAY

#include "moz_config.h"
#undef HAVE_CPP_2BYTE_WCHAR_T
//#define HAVE_CPP_CHAR16_T

#include "xpcom-config.h"
//#include "nspr/prtypes.h"
#include "prtypes.h"
#include "nsID.h"

#include "nsIInterfaceRequestor.h"
#include "nsIProperties.h"
#include "nsXULAppAPI.h"
#include "nsXPCOMGlue.h"
#include "nsCOMPtr.h"

#undef HAVE_CPP_2BYTE_WCHAR_T
#include "nsStringAPI.h"
#include "nsComponentManagerUtils.h"
#include "nsServiceManagerUtils.h"
#include "nsDirectoryServiceUtils.h"
#include "nsIDirectoryService.h"
#include "nsNetUtil.h" // NS_NewURI()
#include "nsIComponentManager.h"
#include "nsEmbedCID.h"

#include "nsIFile.h"
#include "nsILocalFile.h"
#include "nsAppDirectoryServiceDefs.h"
#include "nsProfileDirServiceProvider.h"

#include "nsIPromptService2.h"
//#if MOZILLA_VERSION_1 > 1 || (MOZILLA_VERSION_2 == 9 && MOZILLA_VERSION_3 == 2 )
#include "nsIBadCertListener2.h"
#include "nsIX509Cert.h"
#include "nsIRecentBadCertsService.h"
#include "nsICertOverrideService.h"
//#endif

#include "nsIAuthInformation.h"
#include "nsIURIFixup.h"

#include "nsICertOverrideService.h"
#include "nsISSLStatus.h"
#include "nsIHelperAppLauncherDialog.h"
#include "nsIExternalHelperAppService.h"
#include "nsIWebBrowserChrome.h"

#include "nsIMIMEInfo.h"
#include "nsIWebBrowserFind.h"
#include "nsIClipboardCommands.h"
#include "nsIWebBrowserChromeFocus.h"
#include "nsIEmbeddingSiteWindow2.h"
#include "nsIWeakReference.h"
#include "nsWeakReference.h"

#include "nsIDOMEvent.h"

#if MOZILLA_VERSION_1 < 5
#include "nsIDOM3Document.h"
#endif

#include "nsIDOMMouseEvent.h"
#include "nsIURIContentListener.h"
#include "nsIDirectoryService.h"

#include "nsIContextMenuListener2.h"
#include "nsITooltipListener.h"

#include "nsIBaseWindow.h"
#include "nsIConsoleService.h"
#include "nsIDOMDocument.h"
#include "nsIDOMEventListener.h"
#include "nsIDocShell.h"
#include "nsIDocShellTreeItem.h"
#include "nsIDOMEventTarget.h"

#if MOZILLA_VERSION_1 < 7
#include "nsIDOMWindow2.h"
#include "nsIDOMWindowInternal.h"
#else
#include "nsIDOMWindow.h"
#endif

#include "nsIPrefBranch.h"
#include "nsIPrefService.h"
#include "nsIScriptContext.h"
#include "nsIScriptGlobalObject.h"
#include "nsIScriptObjectPrincipal.h"
#include "nsIURI.h"
#include "nsIURILoader.h"
#include "nsIWeakReference.h"
#include "nsIWeakReferenceUtils.h"
#include "nsIWebBrowser.h"
#include "nsIWebBrowserFocus.h"
#include "nsIWebBrowserStream.h"
#include "nsIWebNavigation.h"
#include "nsIWidget.h"
#include "nsIWindowCreator2.h"
#include "nsIWindowWatcher.h"
#include "nsIDOMAttr.h"
#include "nsIDOMText.h"
#include "nsIDOMNodeList.h"
#include "nsIDOMDocumentType.h"
#include "nsIDOMNamedNodeMap.h"

#include "nsIServiceManager.h"

#include "nsIDOMHTMLAnchorElement.h"
#include "nsIDOMHTMLButtonElement.h"
#include "nsIDOMHTMLInputElement.h"
#include "nsIDOMHTMLLinkElement.h"
#include "nsIDOMHTMLOptionElement.h"
#include "nsIDOMHTMLParamElement.h"
#include "nsIDOMHTMLSelectElement.h"
#include "nsIDOMHTMLTextAreaElement.h"

#if MOZILLA_VERSION_1 < 2
#include "nsIPref.h"
#else
#include "nsIPrefBranch2.h"
#endif
#include "nsITransfer.h"

//#include "nsptr.h"

#include "nsEmbedString.h"

#include "nsutils.h"
#include "nsIComponentRegistrar.h"
#include "nsICancelable.h"
#include "nsIWebBrowserPersist.h"
#include "nsICacheService.h"
#include "nsIPrintSettingsService.h"
#include "nsIPrintSettings.h"
#include "nsIContentViewer.h"
#include "nsIWebBrowserPrint.h"
#include "nsEvent.h"
#include "nsIScriptSecurityManager.h"
#include "nsIAppShell.h"

#include "nsISupportsUtils.h"
#include "nsIWeakReference.h"
#include "nsIWeakReferenceUtils.h"
#include "nsISupportsImpl.h"

//definition function
/*nsresult XPCOMGlueStartup(const char* xpcom_dll_path);
nsresult NS_InitXPCOM2(nsIServiceManager** result, nsIFile* bin_directory, nsIDirectoryServiceProvider* app_file_location_provider);
void*    NS_Alloc(PRSize size);
void     NS_Free(void* ptr);
nsresult NS_GetServiceManager(nsIServiceManager** result);
nsresult NS_GetComponentManager(nsIComponentManager** result);
nsresult NS_GetComponentRegistrar(nsIComponentRegistrar** result);
nsresult NS_StringContainerInit(nsStringContainer& str);
nsresult NS_StringContainerInit2(nsStringContainer& str, const PRUnichar* str_data, PRUint32 len = PR_UINT32_MAX, PRUint32 flags = 0);
void     NS_StringContainerFinish(nsStringContainer& str);
nsresult NS_NewNativeLocalFile(const nsACString& path, PRBool follow_links, nsILocalFile** result);
nsresult NS_StringSetData(nsAString& str, const PRUnichar* str_data, PRUint32 len);
PRUint32 NS_StringGetData(const nsAString& str, const PRUnichar** str_data, PRBool* terminated = NULL);
nsresult NS_CStringContainerInit(nsCStringContainer& str);
nsresult NS_CStringContainerInit2(nsCStringContainer& str, const char* str_data, PRUint32 len = PR_UINT32_MAX, PRUint32 flags = 0);
void     NS_CStringContainerFinish(nsCStringContainer& str);
PRUint32 NS_CStringGetData(const nsACString& str, const char** str_data, PRBool* terminated = NULL);
*/
//end function definition

#endif
