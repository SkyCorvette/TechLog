#ifndef OPTIONS_H
#define OPTIONS_H

#include <vector>
#include <string>
#include <boost/program_options.hpp>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

namespace po = boost::program_options;

class Options
{
public:
    Options();
    ~Options();
    void run(int argc, const char **argv);
    bool version();
    bool help();
    bool helpEvents();
    unsigned int stopAfter();
    bool fileName();
    bool lineNumber();
    std::vector<pcre2_code*> linePatterns();
    std::vector<std::pair<std::string, pcre2_code*>> propertyPatterns();
    std::vector<std::string> events();

    po::options_description visibleOptions();
    po::options_description eventOptions();

private:
    bool _version = false;
    bool _help = false;
    bool _helpEvents = false;
    unsigned int _stopAfter = 0;
    bool _fileName = false;
    bool _lineNumber = false;
    std::vector<pcre2_code*> _linePatterns{};
    std::vector<std::pair<std::string, pcre2_code*>> _propertyPatterns{};
    std::vector<std::string> _events{};

    po::options_description _allOptions{"Allowed options"};
    po::options_description _visibleOptions{"Allowed options"};

    po::options_description _patternOptions{"Patterns"};
    po::options_description _miscOptions{"Miscellaneous"};
    po::options_description _outputOptions{"Output control"};
    po::options_description _eventOptions{"Events"};
    po::positional_options_description _positionalOptions{};

    const std::vector<std::string> _availableEvents
    {
        "ADMIN", "ATTN", "CALL", "CONN", "CONFLOADFROMFILES", "CLSTR", "DB2", "DBMSSQL", "DBMSSQLCONN",
        "DBPOSTGRS", "DBORACLE", "DBV8DBENG", "EDS", "EXCP", "EXCPCNTX", "InputByString", "FTEXTCheck",
        "FTEXTUpd", "HASP", "LEAKS", "LIC", "MAILPARSEERR", "MEM", "PROC", "QERR", "SCALL", "SCOM",
        "SDBL", "SESN", "SRVC", "SYSTEM", "TDEADLOCK", "TTIMEOUT", "TLOCK", "VRSCACHE", "VRSREQUEST", "VRSRESPONSE"
    };
};

#endif //OPTIONS_H
