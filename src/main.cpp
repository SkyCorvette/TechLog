#include <iostream>
#include <boost/program_options.hpp>
#include "version.h"

namespace po = boost::program_options;

struct Options
{
    Options() : version(false), help(false), helpEvents(false), stopAfter(0), fileName(false), lineNumber(false) {}
    bool version;
    bool help;
    bool helpEvents;
    unsigned int stopAfter;
    bool fileName;
    bool lineNumber;
    std::vector<std::pair<std::string, std::string>> regexp;
};

int main(int argc, const char **argv)
{
    Options options;

    po::options_description all("Allowed options");
    po::options_description visible("Allowed options");

    po::options_description regexp("Regexp");
    po::options_description misc("Miscellaneous");
    po::options_description output("Output control");
    po::options_description events("Events");

    po::positional_options_description positional;

    regexp.add_options()("regexp,r", po::value<std::vector<std::string>>()->multitoken(), ": use pattern ARG for matching");

    misc.add_options()("version", po::bool_switch(&options.version), ": display version information and exit");
    misc.add_options()("help", po::bool_switch(&options.help), ": display this help and exit");
    misc.add_options()("help-events", po::bool_switch(&options.helpEvents), ": display events help and exit");

    output.add_options()("stop-after,s", po::value(&options.stopAfter), ": stop after ARG output lines");
    output.add_options()("file-name,f", po::bool_switch(&options.fileName), ": print the file name prefix on output");
    output.add_options()("line-number,l", po::bool_switch(&options.lineNumber), ": print line number with output lines");

    events.add_options()("ADMIN", "");
    events.add_options()("ATTN", "");
    events.add_options()("CALL", "");
    events.add_options()("CONN", "");
    events.add_options()("CONFLOADFROMFILES", "");
    events.add_options()("CLSTR", "");
    events.add_options()("DB2", "");
    events.add_options()("DBMSSQL", "");
    events.add_options()("DBMSSQLCONN", "");
    events.add_options()("DBPOSTGRS", "");
    events.add_options()("DBORACLE", "");
    events.add_options()("DBV8DBENG", "");
    events.add_options()("EDS", "");
    events.add_options()("EXCP", "");
    events.add_options()("EXCPCNTX", "");
    events.add_options()("InputByString", "");
    events.add_options()("FTEXTCheck", "");
    events.add_options()("FTEXTUpd", "");
    events.add_options()("HASP", "");
    events.add_options()("LEAKS", "");
    events.add_options()("LIC", "");
    events.add_options()("MAILPARSEERR", "");
    events.add_options()("MEM", "");
    events.add_options()("PROC", "");
    events.add_options()("QERR", "");
    events.add_options()("SCALL", "");
    events.add_options()("SCOM", "");
    events.add_options()("SDBL", "");
    events.add_options()("SESN", "");
    events.add_options()("SRVC", "");
    events.add_options()("SYSTEM", "");
    events.add_options()("TDEADLOCK", "");
    events.add_options()("TTIMEOUT", "");
    events.add_options()("TLOCK", "");
    events.add_options()("VRSCACHE", "");
    events.add_options()("VRSREQUEST", "");
    events.add_options()("VRSRESPONSE", "");

    positional.add("regexp", 1);

    all.add(regexp).add(output).add(misc).add(events);

    visible.add(regexp).add(output).add(misc);

    try
    {
        po::variables_map vm;

        auto parsed = po::basic_command_line_parser<char>(argc, argv).options(all).allow_unregistered().positional(positional).run();
        auto unrecognized = po::collect_unrecognized(parsed.options, po::exclude_positional);

        po::store(parsed, vm);

        po::notify(vm);

        if (vm.count("regexp"))
        {
            for(auto const& value: vm["regexp"].as<std::vector<std::string>>())
            {
                options.regexp.push_back(std::make_pair("", value));
            }
        }

        for(auto const& unrecognizedOption: unrecognized)
        {
            auto splitted = po::split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0)
            {
                options.regexp.push_back(std::make_pair(splitted[0].substr(2), splitted[1]));
            }
            else
            {
                throw po::unknown_option(unrecognizedOption);
            }
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        return 2;
    }

    if (options.version)
    {
        std::cout << "techlog "<< VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
        return 1;
    }

    if (options.help)
    {
        std::cout << visible << std::endl;
        return 1;
    }

    if (options.helpEvents)
    {
        std::cout << events << std::endl;
        return 1;
    }

    for(auto const& r: options.regexp)
    {
        std::cout << r.first << " : " << r.second << std::endl;
    }

    return 0;
}