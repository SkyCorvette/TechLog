#include <iostream>
#include <boost/filesystem/operations.hpp>
#include "version.h"

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "options.h"
#include "parser.h"
#include "file.h"

namespace fs = boost::filesystem;

int errorcode;
PCRE2_SIZE erroffset;

static const std::string color_filename {"\x1B[34;1m\x1B[K"}; // 32=blue, 1=bold
static const std::string color_match    {"\x1B[31;1m\x1B[K"}; // 31=red, 1=bold
static const std::string color_lineno   {"\x1B[32;1m\x1B[K"}; // 33=green, 1=bold
static const std::string color_normal   {"\x1B[0m\x1B[K"};    // Reset/normal (all attributes off).

inline unsigned int match(Parser* parser, Options* options)
{
    PCRE2_SIZE *ovector;
    auto tmp = strndupa(parser->recordBegin(), parser->recordLength());
    auto printLine = false;

    for(auto const& linePattern: options->linePatterns())
    {
        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(linePattern, NULL);
        auto rc = 0;

        while ((strlen(tmp) > 0) && ((rc = pcre2_jit_match(linePattern, reinterpret_cast<PCRE2_SPTR>(tmp), strlen(tmp), 0, 0, match_data, NULL)) > 0))
        {
            if (!printLine && options->fileName())
            {
                std::cerr << color_filename;
                std::cout << parser->fileName() << ":";
                std::cerr << color_normal;
            }

            if (!printLine && options->lineNumber())
            {
                std::cerr << color_lineno;
                std::cout << parser->recordNumber() << ":";
                std::cerr << color_normal;
            }

            printLine = true;
            ovector = pcre2_get_ovector_pointer(match_data);

            for (int i = 0; i < rc; i++)
            {
                std::cout << std::string(tmp, ovector[2 * i]);
                std::cerr << color_match;
                std::cout << std::string(tmp + ovector[2 * i], ovector[2 * i + 1] - ovector[2 * i]);
                std::cerr << color_normal;
                tmp = tmp + ovector[2 * i + 1];
            }
        }
        pcre2_match_data_free(match_data);
    }

    if (printLine)
    {
        std::cout << tmp << std::endl;
        return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, const char **argv)
{
    Options options;

    try
    {
        options.run(argc, argv);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 2;
    }

    if (options.version())
    {
        std::cout << "techlog "<< VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
        std::cout << LICENSE << std::endl;
        std::cout << WRITTENBY << std::endl;
	    return 1;
    }

    if (!options.linePatterns().size() && !options.propertyPatterns().size() && !options.events().size())
    {
        std::cout << "usage: techlog [-ifl] [-s num] [-p pattern] [--event] [--property=pattern] [pattern]" << std::endl;

        if (!options.help() && !options.helpEvents())
        {
            return 1;
        }
    }

    if (options.help())
    {
        std::cout << options.visibleOptions() << std::endl;
        return 1;
    }

    if (options.helpEvents())
    {
        std::cout << options.eventOptions() << std::endl;
        return 1;
    }

    bool isRegularFile;
    long unsigned linesSelected = 0;
    boost::system::error_code ec;

    pcre2_code *fileNamePattern = pcre2_compile(reinterpret_cast<PCRE2_SPTR>("^\\d{8}\\.log$"), PCRE2_ZERO_TERMINATED, PCRE2_CASELESS, &errorcode, &erroffset, NULL);
    pcre2_jit_compile(fileNamePattern, PCRE2_JIT_COMPLETE);
    pcre2_match_data *fileNameMatchData = pcre2_match_data_create_from_pattern(fileNamePattern, NULL);

    for (fs::recursive_directory_iterator it("./"); it != fs::recursive_directory_iterator();)
    {
        try
        {
            isRegularFile = fs::is_regular_file(it->path());
        }
        catch (const fs::filesystem_error& ex)
        {
            isRegularFile = false;
        }

        if (isRegularFile && (pcre2_jit_match(fileNamePattern, reinterpret_cast<PCRE2_SPTR>(it->path().filename().c_str()), it->path().filename().size(), 0, 0, fileNameMatchData, NULL) > 0))
        {
            File file(it->path().c_str());
            Parser parser(&file);

            while (parser.next())
            {
                linesSelected += match(&parser, &options);

                if (options.stopAfter() > 0 && linesSelected == options.stopAfter())
                {
                    break;
                }
            }

            if (options.stopAfter() > 0 && linesSelected == options.stopAfter())
            {
                break;
            }
        }
        it.increment(ec);
    }

    pcre2_match_data_free(fileNameMatchData);
    pcre2_code_free(fileNamePattern);

    return linesSelected > 0 ? 0 : 1;
}