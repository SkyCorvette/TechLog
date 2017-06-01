#include <iostream>
#include <sys/fcntl.h>
#include <boost/program_options.hpp>
#include <boost/filesystem/operations.hpp>
#include "version.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

static const size_t initialBufferSize = 32768;

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

char* endLine(char* buf, ssize_t length)
{
    auto src = (const char *) buf;
    const char* nl = nullptr;
    const char* dq = nullptr;
    const char* sq = nullptr;
    auto inDQ = false;
    auto inSQ = false;

    while ((nl = (char*) memchr(src, '\n', length - (src - buf))))
    {
        dq = inSQ ? nullptr : (char*) memchr(src, '"', nl - src);
        sq = inDQ ? nullptr : (char*) memchr(src, '\'', nl - src);

        if ((dq && sq && dq < sq) || (dq && !sq))
        {
            inDQ = !inDQ;
            src = dq + 1;
        }
        else if ((dq && dq > sq) || (!dq && sq))
        {
            inSQ = !inSQ;
            src = sq + 1;
        }
        else if (!inDQ && !inSQ)
        {
            return (char*) nl;
        }
        else
        {
            src = nl + 1;
        }
    }
    return nullptr;
}

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
                options.linePatterns.push_back(pcre2_compile((PCRE2_SPTR) linePattern.c_str(), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL));
            }
        }

        for(auto const& unrecognizedOption: unrecognizedOptions)
        {
            auto splitted = po::split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0)
            {
                options.propertyPatterns.push_back(std::make_pair(splitted[0].substr(2), pcre2_compile((PCRE2_SPTR) splitted[1].c_str(), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL)));
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

        if (!options.help)
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

    pcre2_code *fileNamePattern = pcre2_compile((PCRE2_SPTR)"(?i)^\\d{8}\\.log$", PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL);
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

        if (isRegularFile && (pcre2_match(fileNamePattern, (PCRE2_SPTR) it->path().filename().c_str(), (PCRE2_SIZE) strlen(it->path().filename().c_str()), 0, 0, fileNameMatchData, NULL) > 0))
        {
            int file = open(it->path().c_str(), O_RDONLY | O_NOCTTY | O_LARGEFILE | O_NOFOLLOW | O_NONBLOCK);

            if (file == -1)
            {
                continue;
            }

            posix_fadvise(file, 0 , 0, POSIX_FADV_SEQUENTIAL);

            char* buffer = (char*) malloc(initialBufferSize);
            char* bufferSaved = buffer;
            char* bufferEnd = nullptr;
            char* recordBegin = buffer;
            char* newLine = nullptr;

            ssize_t bytesRead = 0;

            int rc;
            unsigned int lineNumber = 0;

            while ((bytesRead = read(file, bufferSaved, initialBufferSize)))
            {
                bufferEnd = bufferSaved + bytesRead;

                while(*recordBegin == '\n' && recordBegin < bufferEnd)
                {
                    ++recordBegin;
                }

                while ((newLine = endLine(recordBegin, bufferEnd - recordBegin)))
                {
                    bool printLine = false;
                    ++lineNumber;
                    std::string res;

                    for(auto const& linePattern: options.linePatterns)
                    {
                        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(linePattern, NULL);
                        rc = pcre2_match(linePattern, (PCRE2_SPTR) recordBegin, newLine - recordBegin, 0, 0, match_data, NULL);

                        while (rc > 0)
                        {
                            printLine = true;
                            ovector = pcre2_get_ovector_pointer(match_data);

                            for (int i = 0; i < rc; i++)
                            {
                                res.append(std::string(recordBegin, ovector[2 * i]));
                                res.append(color_match);
                                res.append(std::string(recordBegin + ovector[2 * i], ovector[2 * i + 1] - ovector[2 * i]));
                                res.append(color_normal);
                                recordBegin = recordBegin + ovector[2 * i + 1];
                            }

                            rc = pcre2_match(linePattern, (PCRE2_SPTR) recordBegin, newLine - recordBegin, 0, 0, match_data, NULL);
                        }

                        pcre2_match_data_free(match_data);
                    }

                    if (printLine)
                    {
                        res.append(std::string(recordBegin, newLine - recordBegin));

                        if (options.fileName)
                        {
                            std::cout << color_filename << it->path().c_str() << ":" << color_normal;
                        }

                        if (options.lineNumber)
                        {
                            std::cout << color_lineno << lineNumber << ":" << color_normal;
                        }

                        std::cout << res << std::endl;
                        linesSelected++;
                    }

                    recordBegin = newLine;

                    while(*recordBegin == '\n' && recordBegin < bufferEnd)
                    {
                        ++recordBegin;
                    }

                    if (recordBegin == bufferEnd)
                    {
                        break;
                    }

                    if (options.stopAfter > 0 && linesSelected == options.stopAfter)
                    {
                        break;
                    }
                }

                if (recordBegin == bufferEnd)
                {
                    free(buffer);
                    buffer = (char*) malloc(initialBufferSize);
                    bufferSaved = buffer;
                }
                else
                {
                    size_t savedSize = bufferEnd - recordBegin;
                    memmove(buffer, recordBegin, savedSize);
                    buffer = (char*) realloc(buffer, savedSize + initialBufferSize);
                    bufferSaved = buffer + savedSize;
                }

                recordBegin = buffer;

                if (options.stopAfter > 0 && linesSelected == options.stopAfter)
                {
                    break;
                }
            }

            free(buffer);
            close(file);
        }

        if(options.stopAfter > 0 && linesSelected == options.stopAfter)
        {
            break;
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