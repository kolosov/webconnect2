// webconnect.sip
// Define the SIP wrapper to the word library.

%Module ( name=webconnect,
			keyword_arguments="All",
			language="C++" )

%Import _core.sip

%ModuleHeaderCode
#include <webcontrol.h>
%End


class SimpleGeckoEngine {
%Docstring
  SimpleGeckoEngine()
%End

//%TypeHeaderCode
//#include <webcontrol.h>
//%End
public:
     SimpleGeckoEngine();
    ~SimpleGeckoEngine();

    void SetEnginePath(const wxString& path);
    void SetStoragePath(const wxString& path);

    bool Init();
    bool IsOk() const;
    void Uninit();
};

class wxWebContentHandler;
class wxWebPreferences;
class wxWebPostData;
class wxWebProgressBase;
class wxDOMDocument;
class wxWebControl : public wxControl
{

public:

    static bool InitEngine(const wxString& path);
    static bool AddContentHandler(wxWebContentHandler* handler, bool take_ownership = false);
    static void AddPluginPath(const wxString& path);
    static void SetProfilePath(const wxString& path);
    
    static wxWebPreferences GetPreferences();
    static void SetIgnoreCertErrors(bool ignore);
    static bool GetIgnoreCertErrors();
    
    static bool SaveRequest(
                 const wxString& uri,
                 const wxString& destination_path,
                 wxWebPostData* post_data = NULL,
                 wxWebProgressBase* listener = NULL);
                 
    static bool SaveRequestToString(
                 const wxString& uri,
                 wxString* result = NULL,
                 wxWebPostData* post_data = NULL,
                 wxWebProgressBase* listener = NULL);
                 
    static bool ClearCache();
                 
public:

    wxWebControl(wxWindow* parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize);
    ~wxWebControl();
	wxString GeckoVersion();
    bool IsOk() const;
    
    // navigation
    void OpenURI(const wxString& uri,
                 unsigned int flags = wxWEB_LOAD_NORMAL,
                 wxWebPostData* post_data = NULL);
                     
    wxString GetCurrentURI() const;
    void GoForward();
    void GoBack();
    void Reload();
    void Stop();
    bool IsContentLoaded() const;
    
    // javascript
    bool Execute(const wxString& js_code);
    
    // printing
    void PrintPreview(bool silent = false);
    void Print(bool silent = false);
    void SetPageSettings(double page_width, double page_height,
                         double left_margin, double right_margin, double top_margin, double bottom_margin);
    void GetPageSettings(double* page_width, double* page_height,
                         double* left_margin, double* right_margin, double* top_margin, double* bottom_margin);

    // view source
    void ViewSource();
    void ViewSource(wxWebControl* source_web_browser);
    void ViewSource(const wxString& uri);
    
    // save
    bool SaveCurrent(const wxString& destination_path);
    
    // zoom
    void GetTextZoom(float* zoom);
    void SetTextZoom(float zoom);

    // find
    bool Find(const wxString& text, unsigned int flags = 0);
    
    // clipboard
    bool CanCutSelection();
    bool CanCopySelection();
    bool CanCopyLinkLocation();
    bool CanCopyImageLocation();
    bool CanCopyImageContents();
    bool CanPaste();
    void CutSelection();
    void CopySelection();
    void CopyLinkLocation();
    void CopyImageLocation();
    void CopyImageContents();
    void Paste();
    void SelectAll();
    void SelectNone();
    
    // other
    wxImage GetFavIcon() const;
    wxDOMDocument GetDOMDocument();

};


