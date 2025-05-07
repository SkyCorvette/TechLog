#ifndef TECHLOG_OPTIONS_H
#define TECHLOG_OPTIONS_H

#include <boost/program_options.hpp>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

using namespace std;
using namespace boost::program_options;

class Options {
public:
    Options();
    ~Options();
    void run(int argc, const char **argv);
    [[nodiscard]] bool version() const;
    [[nodiscard]] bool help() const;
    [[nodiscard]] bool helpEvents() const;
    [[nodiscard]] unsigned stopAfter() const;
    [[nodiscard]] bool fileName() const;
    [[nodiscard]] bool lineNumber() const;
    [[nodiscard]] bool ignoreCase() const;
    vector<pcre2_code*> linePatterns();
    vector<pair<string, pcre2_code_8 *> > propertyPatterns();
    vector<string> events();

    options_description visibleOptions();
    options_description eventOptions();

private:
    bool _version = false;
    bool _help = false;
    bool _helpEvents = false;
    unsigned _stopAfter = 0;
    bool _fileName = false;
    bool _lineNumber = false;
    bool _ignoreCase = false;
    vector<pcre2_code*> _linePatterns{};
    vector<pair<string, pcre2_code_8 *> > _propertyPatterns{};
    vector<string> _events{};

    options_description _allOptions{"Allowed options"};
    options_description _visibleOptions{"Allowed options"};

    options_description _patternOptions{"Patterns"};
    options_description _miscOptions{"Miscellaneous"};
    options_description _outputOptions{"Output control"};
    options_description _eventOptions{"Events"};
    positional_options_description _positionalOptions{};

    const vector<string> _availableEvents {
        "ADDIN", "ADMIN", "ATTN","CALL","CLSTR","CONFLOADFROMFILES","CONN","DB2","DBCOPIES",
        "DBMSSQL","DBMSSQLCONN","DBORACLE","DBPOSTGRS","DBV8DBENG","DHIST","EDS","EVENTLOG",
        "EXCP","EXCPCNTX","FTEXTCHECK","FTEXTUPD","FTS","HASP","INPUTBYSTRING","LEAKS",
        "LIC","MAILPARSEERR","MEM","PROC","QERR","SCALL","SCOM","SDBL","SDGC","SESN",
        "SINTEG","SRVC","STORE","STT","STTADM","SYSTEM","TDEADLOCK","TLOCK","TTIMEOUT",
        "VIDEOCALL","VIDEOCONN","VIDEOSTATS","VRSCACHE","VRSREQUEST","VRSRESPONSE","WINCERT"
    };
};

#endif