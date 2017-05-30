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
    std::vector<std::pair<std::string, std::string>> regexps;
    std::vector<std::string> events;
};

std::vector<std::string> availableEvents
{
    "ADMIN",
    "ATTN",
    "CALL",
    "CONN",
    "CONFLOADFROMFILES",
    "CLSTR",
    "DB2",
    "DBMSSQL",
    "DBMSSQLCONN",
    "DBPOSTGRS",
    "DBORACLE",
    "DBV8DBENG",
    "EDS",
    "EXCP",
    "EXCPCNTX",
    "InputByString",
    "FTEXTCheck",
    "FTEXTUpd",
    "HASP",
    "LEAKS",
    "LIC",
    "MAILPARSEERR",
    "MEM",
    "PROC",
    "QERR",
    "SCALL",
    "SCOM",
    "SDBL",
    "SESN",
    "SRVC",
    "SYSTEM",
    "TDEADLOCK",
    "TTIMEOUT",
    "TLOCK",
    "VRSCACHE",
    "VRSREQUEST",
    "VRSRESPONSE"
};

int main(int argc, const char **argv)
{
    Options options;

    po::options_description allOptions("Allowed options");
    po::options_description visibleOptions("Allowed options");

    po::options_description regexpOptions("Regexp");
    po::options_description miscOptions("Miscellaneous");
    po::options_description outputOptions("Output control");
    po::options_description eventsOptions("Events");

    po::positional_options_description positionalOptions;

    regexpOptions.add_options()("regexp,r", po::value<std::vector<std::string>>()->multitoken(), ": use pattern ARG for matching");

    miscOptions.add_options()("version", po::bool_switch(&options.version), ": display version information and exit");
    miscOptions.add_options()("help", po::bool_switch(&options.help), ": display this help and exit");
    miscOptions.add_options()("help-events", po::bool_switch(&options.helpEvents), ": display events help and exit");

    outputOptions.add_options()("stop-after,s", po::value(&options.stopAfter), ": stop after ARG output lines");
    outputOptions.add_options()("file-name,f", po::bool_switch(&options.fileName), ": print the file name prefix on output");
    outputOptions.add_options()("line-number,l", po::bool_switch(&options.lineNumber), ": print line number with output lines");

    for(auto const& availableEvent: availableEvents)
    {
        eventsOptions.add_options()(availableEvent.c_str(), "");
    }

    positionalOptions.add("regexp", 1);

    allOptions.add(regexpOptions).add(outputOptions).add(miscOptions).add(eventsOptions);
    visibleOptions.add(regexpOptions).add(outputOptions).add(miscOptions);

    try
    {
        po::variables_map vm;

        auto parsed = po::basic_command_line_parser<char>(argc, argv).options(allOptions).allow_unregistered().positional(positionalOptions).run();
        auto unrecognizedOptions = po::collect_unrecognized(parsed.options, po::exclude_positional);

        po::store(parsed, vm);

        po::notify(vm);

        for(auto const& availableEvent: availableEvents)
        {
            if (vm.count(availableEvent))
            {
                options.events.push_back(availableEvent);
            }
        }

        if (vm.count("regexp"))
        {
            for(auto const& value: vm["regexp"].as<std::vector<std::string>>())
            {
                options.regexps.push_back(std::make_pair("", value));
            }
        }

        for(auto const& unrecognizedOption: unrecognizedOptions)
        {
            auto splitted = po::split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0)
            {
                options.regexps.push_back(std::make_pair(splitted[0].substr(2), splitted[1]));
            }
            else
            {
                throw po::unknown_option(unrecognizedOption);
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 2;
    }

    if (options.version)
    {
        std::cout << "techlog "<< VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
        std::cout << LICENSE << std::endl;
        std::cout << WRITTENBY << std::endl;
	    return 1;
    }

    if (options.help)
    {
        std::cout << visibleOptions << std::endl;
        return 1;
    }

    if (options.helpEvents)
    {
        std::cout << eventsOptions << std::endl;
        return 1;
    }

    for(auto const& event: options.events)
    {
        std::cout << event << std::endl;
    }

    for(auto const& regexp: options.regexps)
    {
        std::cout << regexp.first << " : " << regexp.second << std::endl;
    }

    return 0;
}