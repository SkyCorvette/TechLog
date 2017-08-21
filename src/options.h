#ifndef OPTIONS_H
#define OPTIONS_H

#include <boost/program_options.hpp>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

using namespace std;
using namespace boost::program_options;

class Options
{
public:
    Options();
    ~Options();
    void run(int argc, const char **argv);
    bool version();
    bool help();
    bool helpEvents();
    unsigned stopAfter();
    bool fileName();
    bool lineNumber();
    bool ignoreCase();
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

    const vector<string> _availableEvents
    {
        "ADMIN", "ATTN", "CALL", "CONN", "CONFLOADFROMFILES", "CLSTR", "DB2", "DBMSSQL", "DBMSSQLCONN",
        "DBPOSTGRS", "DBORACLE", "DBV8DBENG", "EDS", "EXCP", "EXCPCNTX", "InputByString", "FTEXTCheck",
        "FTEXTUpd", "HASP", "LEAKS", "LIC", "MAILPARSEERR", "MEM", "PROC", "QERR", "SCALL", "SCOM",
        "SDBL", "SESN", "SRVC", "SYSTEM", "TDEADLOCK", "TTIMEOUT", "TLOCK", "VRSCACHE", "VRSREQUEST", "VRSRESPONSE"
    };
};

#endif //OPTIONS_H