// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include "version.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "parser.h"
#include "file.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int errorcode;
PCRE2_SIZE erroffset;

static const std::string color_filename {"\x1B[32;1m\x1B[K"}; // 32=green, 1=bold
static const std::string color_match    {"\x1B[31;1m\x1B[K"}; // 31=red, 1=bold
static const std::string color_lineno   {"\x1B[33;1m\x1B[K"}; // 33=yellow, 1=bold
static const std::string color_normal   {"\x1B[0m\x1B[K"};    // Reset/normal (all attributes off).

struct
{
    bool version;
    bool help;
    bool helpEvents;
    unsigned int stopAfter;
    bool fileName;
    bool lineNumber;
    std::vector<pcre2_code*> linePatterns;
    std::vector<std::pair<std::string, pcre2_code*>> propertyPatterns;
    std::vector<std::string> events;
} options {};

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
    po::options_description allOptions("Allowed options");
    po::options_description visibleOptions("Allowed options");

    po::options_description patternOptions("Patterns");
    po::options_description miscOptions("Miscellaneous");
    po::options_description outputOptions("Output control");
    po::options_description eventOptions("Events");

    po::positional_options_description positionalOptions;

    patternOptions.add_options()("pattern,p", po::value<std::vector<std::string>>()->multitoken(), ": use pattern ARG for matching");

    miscOptions.add_options()("version", po::bool_switch(&options.version), ": display version information and exit");
    miscOptions.add_options()("help", po::bool_switch(&options.help), ": display this help and exit");
    miscOptions.add_options()("help-events", po::bool_switch(&options.helpEvents), ": display events help and exit");

    outputOptions.add_options()("stop-after,s", po::value(&options.stopAfter), ": stop after ARG output lines");
    outputOptions.add_options()("file-name,f", po::bool_switch(&options.fileName), ": print the file name prefix on output");
    outputOptions.add_options()("line-number,l", po::bool_switch(&options.lineNumber), ": print line number with output lines");

    for(auto const& availableEvent: availableEvents)
    {
        eventOptions.add_options()(availableEvent.c_str(), "");
    }

    positionalOptions.add("pattern", 1);

    allOptions.add(patternOptions).add(outputOptions).add(miscOptions).add(eventOptions);
    visibleOptions.add(patternOptions).add(outputOptions).add(miscOptions);

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

        if (vm.count("pattern"))
        {
            for(auto const& linePattern: vm["pattern"].as<std::vector<std::string>>())
            {
                options.linePatterns.push_back(pcre2_compile(reinterpret_cast<PCRE2_SPTR>(linePattern.c_str()), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL));
            }
        }

        for(auto const& unrecognizedOption: unrecognizedOptions)
        {
            auto splitted = po::split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0)
            {
                options.propertyPatterns.push_back(std::make_pair(splitted[0].substr(2), pcre2_compile(reinterpret_cast<PCRE2_SPTR>(splitted[1].c_str()), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL)));
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

    if (!options.linePatterns.size() && !options.propertyPatterns.size() && !options.events.size())
    {
        std::cout << "usage: techlog [-fl] [-s num] [-p pattern] [--event] [--property=pattern] [pattern]" << std::endl;

        if (!options.help && !options.helpEvents)
        {
            return 1;
        }
    }

    if (options.help)
    {
        std::cout << visibleOptions << std::endl;
        return 1;
    }

    if (options.helpEvents)
    {
        std::cout << eventOptions << std::endl;
        return 1;
    }

    PCRE2_SIZE *ovector;

    bool isRegularFile;
    unsigned int linesSelected = 0;
    boost::system::error_code ec;

    pcre2_code *fileNamePattern = pcre2_compile(reinterpret_cast<PCRE2_SPTR>("(?i)^\\d{8}\\.log$"), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL);
    pcre2_match_data *fileNameMatchData = pcre2_match_data_create_from_pattern(fileNamePattern, NULL);

    for (fs::recursive_directory_iterator it("./"); it != fs::recursive_directory_iterator();)
    {
        try
        {
            isRegularFile = boost::filesystem::is_regular_file(it->path());
        }
        catch (const boost::filesystem::filesystem_error& ex)
        {
            isRegularFile = false;
        }

        if (isRegularFile && (pcre2_match(fileNamePattern, reinterpret_cast<PCRE2_SPTR>(it->path().filename().c_str()), PCRE2_ZERO_TERMINATED, 0, 0, fileNameMatchData, NULL) > 0))
        {
            File file(it->path().c_str());
            Parser parser(&file);

            while (parser.next())
            {
                auto tmp = parser.recordBegin();
                std::string res;
                bool printLine = false;

                for(auto const& linePattern: options.linePatterns)
                {
                    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(linePattern, NULL);
                    auto rc = pcre2_match(linePattern, reinterpret_cast<PCRE2_SPTR>(tmp), parser.recordLength() - static_cast<size_t>(tmp - parser.recordBegin()), 0, 0, match_data, NULL);

                    while (rc > 0)
                    {
                        printLine = true;
                        ovector = pcre2_get_ovector_pointer(match_data);

                        for (int i = 0; i < rc; i++)
                        {
                            res.append(std::string(tmp, ovector[2 * i]));
                            res.append(color_match);
                            res.append(std::string(tmp + ovector[2 * i], ovector[2 * i + 1] - ovector[2 * i]));
                            res.append(color_normal);
                            tmp = tmp + ovector[2 * i + 1];
                        }
                        rc = pcre2_match(linePattern, reinterpret_cast<PCRE2_SPTR>(tmp), parser.recordLength() - static_cast<size_t>(tmp - parser.recordBegin()), 0, 0, match_data, NULL);
                    }
                    pcre2_match_data_free(match_data);
                }

                if (printLine)
                {
                    res.append(std::string(tmp, parser.recordLength() - static_cast<size_t>(tmp - parser.recordBegin())));

                    if (options.fileName)
                    {
                        std::cout << color_filename << it->path().c_str() << ":" << color_normal;
                    }

                    if (options.lineNumber)
                    {
                        std::cout << color_lineno << parser.recordNumber() << ":" << color_normal;
                    }

                    std::cout << res << std::endl;
                    linesSelected++;
                }
                if (options.stopAfter > 0 && linesSelected == options.stopAfter)
                {
                    break;
                }
            }

            if (options.stopAfter > 0 && linesSelected == options.stopAfter)
            {
                break;
            }
        }
        it.increment(ec);
    }

    pcre2_match_data_free(fileNameMatchData);
    pcre2_code_free(fileNamePattern);

    for(auto const& linePattern: options.linePatterns)
    {
        pcre2_code_free(linePattern);
    }

    for(auto const& propertyPattern: options.propertyPatterns)
    {
        pcre2_code_free(propertyPattern.second);
    }

    return linesSelected > 0 ? 0 : 1;
}