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
 * The Initial Developer of the Original Code is Nokia
 * Portions created by the Initial Developer are Copyright (C) 2008
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Tobias Hunger <tobias.hunger@gmail.com>
 *   Steffen.Imhof <steffen.imhof@googlemail.com>
 *   Tatiana Meshkova <tanya.meshkova@gmail.com>
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

#include "ConsoleListener.h"
#include "nsStringAPI.h"

#include "embed.h"

NS_IMPL_ISUPPORTS1(ConsoleListener,
                   nsIConsoleListener)

ConsoleListener::ConsoleListener(MozView *aOwner) :
    mOwner(aOwner)
{ }

ConsoleListener::~ConsoleListener()
{ }

void ConsoleListener::Detach()
{
    mOwner = NULL;
}

NS_IMETHODIMP ConsoleListener::Observe(nsIConsoleMessage * aMessage)
{
    if (!mOwner)
        return NS_OK;

    MozViewListener *listener = mOwner->GetListener();
    if (!listener)
        return NS_OK;

    nsString msg;
    nsresult rv = aMessage->GetMessageMoz(getter_Copies(msg));
    if (NS_SUCCEEDED(rv))
    {
        listener->OnConsoleMessage(NS_ConvertUTF16toUTF8(msg).get());
    }
    return NS_OK;
}
