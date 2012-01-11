///////////////////////////////////////////////////////////////////////////////
// Name:        domprivate.h
// Purpose:     wxwebconnect: embedded web browser control library
// Author:      Benjamin I. Williams
// Modified by:
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
    	//FIXME implement later
    	/*
        nsCOMPtr<nsISupports> supports = ptr;
        node_ptr = supports;
        attr_ptr = supports;
        text_ptr = supports;
        element_ptr = supports;
        doc_ptr = supports;
        htmlelement_ptr = supports;
        anchor_ptr = supports;
        button_ptr = supports;
        input_ptr = supports;
        link_ptr = supports;
        option_ptr = supports;
        param_ptr = supports;
        select_ptr = supports;
        textarea_ptr = supports;
        */
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
        //FIXME implement later
        /*
        event_ptr = supports;
        mouseevent_ptr = supports;
        */
    }
    
    nsCOMPtr<nsIDOMNode> event_ptr;
    nsCOMPtr<nsIDOMAttr> mouseevent_ptr;
};


#endif

