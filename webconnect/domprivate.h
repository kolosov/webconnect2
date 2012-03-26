///////////////////////////////////////////////////////////////////////////////
// Name:        domprivate.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by: Sergey Kolosov <kolosov@gmail.com>
// Created:     2006-09-30
// RCS-ID:      
// Copyright:   (C) Copyright 2006-2010, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////


#ifndef __WXWEBCONNECT_DOMPRIVATE_H
#define __WXWEBCONNECT_DOMPRIVATE_H


struct wxDOMNodeData
{
    void setNode(nsISupports* ptr)
    {
    	nsCOMPtr<nsISupports> supports = ptr;
        node_ptr = do_QueryInterface(supports);
        attr_ptr = do_QueryInterface(supports);
        text_ptr = do_QueryInterface(supports);
        element_ptr = do_QueryInterface(supports);
        doc_ptr = do_QueryInterface(supports);
        htmlelement_ptr = do_QueryInterface(supports);
        anchor_ptr = do_QueryInterface(supports);
        button_ptr = do_QueryInterface(supports);
        input_ptr = do_QueryInterface(supports);
        link_ptr = do_QueryInterface(supports);
        option_ptr = do_QueryInterface(supports);
        param_ptr = do_QueryInterface(supports);
        select_ptr = do_QueryInterface(supports);
        textarea_ptr = do_QueryInterface(supports);

    }
    
    nsCOMPtr<nsIDOMNode> node_ptr;
    nsCOMPtr<nsIDOMAttr> attr_ptr;
    nsCOMPtr<nsIDOMText> text_ptr;
    nsCOMPtr<nsIDOMElement> element_ptr;
    nsCOMPtr<nsIDOMDocument> doc_ptr;
    nsCOMPtr<nsIDOMHTMLElement> htmlelement_ptr;
    nsCOMPtr<nsIDOMHTMLAnchorElement> anchor_ptr;
    nsCOMPtr<nsIDOMHTMLButtonElement> button_ptr;
    nsCOMPtr<nsIDOMHTMLInputElement> input_ptr;
    nsCOMPtr<nsIDOMHTMLLinkElement> link_ptr;
    nsCOMPtr<nsIDOMHTMLOptionElement> option_ptr;
    nsCOMPtr<nsIDOMHTMLParamElement> param_ptr;
    nsCOMPtr<nsIDOMHTMLSelectElement> select_ptr;
    nsCOMPtr<nsIDOMHTMLTextAreaElement> textarea_ptr;
};


struct wxDOMNodeListData
{
    nsCOMPtr<nsIDOMNodeList> ptr;
};


struct wxDOMNamedNodeMapData
{
    nsCOMPtr<nsIDOMNamedNodeMap> ptr;
};


struct wxDOMEventData
{
    void setPtr(nsISupports* ptr)
    {
        nsCOMPtr<nsISupports> supports = ptr;
        event_ptr = do_QueryInterface(supports);
        mouseevent_ptr = do_QueryInterface(supports);

    }
    
    nsCOMPtr<nsIDOMNode> event_ptr;
    nsCOMPtr<nsIDOMAttr> mouseevent_ptr;
};


#endif

