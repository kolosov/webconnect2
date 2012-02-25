/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Mozilla Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2007
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Tobias Hunger <tobias.hunger@gmail.com>
 *   Steffen Imhof <steffen.imhof@googlemail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "WebBrowserChrome.h"
#include "embed.h"

#include "nsStringAPI.h"

#include "nsIDOMWindow.h"
#include "nsIURI.h"
#include "nsIWebProgress.h"

// CRT headers
#include <iostream>
#include <string>
using namespace std;
WebBrowserChrome::WebBrowserChrome(MozView* pAMozView) :
    mChromeFlags(0),
    mSizeSet(PR_FALSE),
    pMozView(pAMozView),
    mIsModal(PR_FALSE)
{
    /* member initializers and constructor code */
}

WebBrowserChrome::~WebBrowserChrome()
{
    /* destructor code */
}

NS_IMPL_ISUPPORTS6(WebBrowserChrome,
                   nsIWebBrowserChrome,
                   nsIWebBrowserChromeFocus,
                   nsIInterfaceRequestor,
                   nsIEmbeddingSiteWindow,
                   nsIWebProgressListener,
                   nsISupportsWeakReference)

NS_IMETHODIMP WebBrowserChrome::GetInterface(const nsIID &aIID, void** aInstancePtr)
{
    NS_ENSURE_ARG_POINTER(aInstancePtr);

    *aInstancePtr = 0;
    if (aIID.Equals(NS_GET_IID(nsIDOMWindow)))
    {
        if (!mWebBrowser)
            return NS_ERROR_NOT_INITIALIZED;

        return mWebBrowser->GetContentDOMWindow((nsIDOMWindow **) aInstancePtr);
    }
    return QueryInterface(aIID, aInstancePtr);
}

NS_IMETHODIMP WebBrowserChrome::SetStatus(PRUint32 statusType, const PRUnichar *status)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->StatusChanged(NS_ConvertUTF16toUTF8(status).get(), statusType);
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::GetWebBrowser(nsIWebBrowser * *aWebBrowser)
{
    NS_ENSURE_ARG_POINTER(aWebBrowser);
    *aWebBrowser = mWebBrowser;
    NS_IF_ADDREF(*aWebBrowser);
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::SetWebBrowser(nsIWebBrowser * aWebBrowser)
{
    mWebBrowser = aWebBrowser;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::GetChromeFlags(PRUint32 *aChromeFlags)
{
    *aChromeFlags = mChromeFlags;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::SetChromeFlags(PRUint32 aChromeFlags)
{
    mChromeFlags = aChromeFlags;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::DestroyBrowserWindow()
{
    if (mIsModal)
        ExitModalEventLoop(NS_OK);
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP WebBrowserChrome::SizeBrowserTo(PRInt32 aCX, PRInt32 aCY)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->SizeTo(aCX, aCY);
    mSizeSet = PR_TRUE;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::ShowAsModal()
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    mIsModal = PR_TRUE;
    pListener->StartModal();
    return NS_OK;
}
#if MOZILLA_VERSION_1 >= 10
NS_IMETHODIMP WebBrowserChrome::IsWindowModal(bool *_retval)
#else
NS_IMETHODIMP WebBrowserChrome::IsWindowModal(PRBool *_retval)
#endif
{
    NS_ENSURE_ARG_POINTER(_retval);
    *_retval = mIsModal;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::ExitModalEventLoop(nsresult aStatus)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->ExitModal(aStatus);
    mIsModal = PR_FALSE;
    return NS_OK;
}

// ----- Progress Listener -----

NS_IMETHODIMP WebBrowserChrome::OnStateChange(nsIWebProgress * /*aWebProgress*/,
                                              nsIRequest * /*aRequest*/,
                                              PRUint32 aStateFlags,
                                              nsresult aStatus)
{
    MozViewListener* pListener = pMozView->GetListener();
    //cout << "OnStateChange 1" << endl;
    // XXX no one considered this case
    if (!pListener)
        return NS_OK;

    // whenever any window finished loading, tell the listener about this frame
    if ((aStateFlags & STATE_STOP) && (aStateFlags & STATE_IS_WINDOW))
        pListener->FrameLoaded();

    // instead of listening for STATE_IS_DOCUMENT, we use STATE_IS_WINDOW because it seems to come later on,
    // which is better for triggering the render process, STATE_IS_NETWORK is used to filter out child frames
    if ((aStateFlags & STATE_STOP) &&
        ((aStateFlags & (STATE_IS_WINDOW|STATE_IS_NETWORK)) == (STATE_IS_WINDOW|STATE_IS_NETWORK))) {
        // if it was a chrome window and no one has already specified a size,
        // size to content
        if (!mSizeSet && (mChromeFlags & nsIWebBrowserChrome::CHROME_OPENAS_CHROME)) {
            nsCOMPtr<nsIDOMWindow> contentWin;
            mWebBrowser->GetContentDOMWindow(getter_AddRefs(contentWin));
            if (contentWin)
                contentWin->SizeToContent();
            SetVisibility(PR_TRUE);
        }

 //       pListener->AttachAdditionalListeners();
        pListener->DocumentLoaded(aStatus == NS_OK);
    }
    if ((aStateFlags & STATE_START) &&
        ((aStateFlags & (STATE_IS_WINDOW|STATE_IS_NETWORK)) == (STATE_IS_WINDOW|STATE_IS_NETWORK)) )
        // whenever a window is "started" that is not a subframe (STATE_IS_NETWORK), notify the listener
        pListener->DocumentLoadStarted();
    
    //cout << "OnStateChange 2" << endl;

    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::OnProgressChange(nsIWebProgress * /*aWebProgress*/,
                                                 nsIRequest * /*aRequest*/,
                                                 PRInt32 /*aCurSelfProgress*/,
                                                 PRInt32 /*aMaxSelfProgress*/,
                                                 PRInt32 aCurTotalProgress,
                                                 PRInt32 aMaxTotalProgress)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    PRUint32 percentage = ( 100 * aCurTotalProgress ) / aMaxTotalProgress;
    if (percentage > 100)
        percentage = 100;
    pListener->ProgressChanged(percentage);
    //cout << "OnProgressChange " << percentage << "percent" << endl;
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::OnLocationChange(nsIWebProgress * aWebProgress,
                                                 nsIRequest * /*aRequest*/,
                                                 nsIURI *aLocation)
{
    NS_ENSURE_ARG_POINTER(aWebProgress);
    NS_ENSURE_ARG_POINTER(aLocation);

    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    nsCOMPtr<nsIDOMWindow> domWindow;
    nsCOMPtr<nsIDOMWindow> topWindow;

    aWebProgress->GetDOMWindow(getter_AddRefs(domWindow));

    if (domWindow)
        domWindow->GetTop(getter_AddRefs(topWindow));

    // notify location changes only for top window
    if (domWindow == topWindow)
    {
        nsCString spec;
        aLocation->GetSpec(spec);
        pListener->LocationChanged(spec.get());
    }

    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::OnStatusChange(nsIWebProgress * /*aWebProgress*/,
                                               nsIRequest * /*aRequest*/,
                                               nsresult /*aStatus*/,
                                               const PRUnichar * /*aMessage*/)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP WebBrowserChrome::OnSecurityChange(nsIWebProgress * /*aWebProgress*/,
                                                 nsIRequest * /*aRequest*/,
                                                 PRUint32 /*aState*/)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

// ----- Embedding Site Window

NS_IMETHODIMP WebBrowserChrome::SetDimensions(PRUint32 aFlags,
                                              PRInt32 aX, PRInt32 aY,
                                              PRInt32 aCx, PRInt32 aCy)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    // TODO: currently only does one size (no difference between outer and inner)
    if ((aFlags & DIM_FLAGS_SIZE_INNER) ||
         (aFlags & DIM_FLAGS_SIZE_OUTER))
    {
        pListener->SizeTo(aCx, aCy);
        mSizeSet = PR_TRUE;
    }

    if (aFlags & DIM_FLAGS_POSITION)
        pListener->MoveTo(aX,aY);

    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::GetDimensions(PRUint32 /*aFlags*/,
                                              PRInt32 * /*aX*/, PRInt32 * /*aY*/,
                                              PRInt32 * /*aCx*/, PRInt32 * /*aCy*/)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP WebBrowserChrome::SetFocus()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
#if MOZILLA_VERSION_1 >= 10
NS_IMETHODIMP WebBrowserChrome::GetVisibility(bool *aVisibility)
{
      NS_ENSURE_ARG_POINTER(aVisibility);
     *aVisibility = true;
     return NS_OK;
}
#else
NS_IMETHODIMP WebBrowserChrome::GetVisibility(PRBool *aVisibility)
{
      NS_ENSURE_ARG_POINTER(aVisibility);
     *aVisibility = PR_TRUE;
     return NS_OK;
}
#endif

#if MOZILLA_VERSION_1 >= 10
NS_IMETHODIMP WebBrowserChrome::SetVisibility(bool aVisibility)
#else
NS_IMETHODIMP WebBrowserChrome::SetVisibility(PRBool aVisibility)
#endif
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->SetVisibility(aVisibility);
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::GetTitle(PRUnichar ** /*aTitle*/)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP WebBrowserChrome::SetTitle(const PRUnichar *aTitle)
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->SetTitle(NS_ConvertUTF16toUTF8(aTitle).get());
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::GetSiteWindow(void * *aSiteWindow)
{
    NS_ENSURE_ARG_POINTER(aSiteWindow);
    *aSiteWindow = pMozView->GetParentWindow();
    return NS_OK;
}

// ----- WebBrowser Chrome Focus

NS_IMETHODIMP WebBrowserChrome::FocusNextElement()
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->OnFocusChanged(PR_TRUE);
    return NS_OK;
}

NS_IMETHODIMP WebBrowserChrome::FocusPrevElement()
{
    MozViewListener* pListener = pMozView->GetListener();
    if (!pListener)
        return NS_ERROR_NOT_IMPLEMENTED;

    pListener->OnFocusChanged(PR_FALSE);
    return NS_OK;
}
