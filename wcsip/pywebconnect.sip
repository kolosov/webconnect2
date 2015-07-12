// webconnectwrap.sip
// Define the SIP wrapper to the word library.

%Module pywebconnect

//%Include src/string.sip

%Import _core.sip

class SimpleGeckoEngine {

%TypeHeaderCode
#include <webcontrol.h>
%End

public:
     SimpleGeckoEngine();
    ~SimpleGeckoEngine();

    void SetEnginePath(const wxString& path);
    void SetStoragePath(const wxString& path);

    bool Init();
    bool IsOk() const;
    void Uninit();
};

