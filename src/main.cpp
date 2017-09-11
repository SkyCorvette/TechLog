#include "version.h"
#include <boost/filesystem/operations.hpp>
#include <iostream>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include "file.h"
#include "options.h"
#include "parser.h"

using namespace boost::filesystem;
using namespace std;

int errorcode;
PCRE2_SIZE erroffset;

inline unsigned int match(Parser* parser, Options* options) {
    static const string color_filename {"\x1B[34;1m\x1B[K"}; // 32=blue, 1=bold
    static const string color_match    {"\x1B[31;1m\x1B[K"}; // 31=red, 1=bold
    static const string color_lineno   {"\x1B[32;1m\x1B[K"}; // 33=green, 1=bold
    static const string color_normal   {"\x1B[0m\x1B[K"};    // Reset/normal (all attributes off).

    PCRE2_SIZE *ovector;
    auto tmp = strndupa(parser->recordBegin(), parser->recordLength());
    auto printLine = false;

    for(auto const& linePattern: options->linePatterns()) {
        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(linePattern, nullptr);
        auto rc = 0;

        while ((strlen(tmp) > 0) && ((rc = pcre2_jit_match(linePattern, reinterpret_cast<PCRE2_SPTR>(tmp), strlen(tmp), 0, 0, match_data, nullptr)) > 0)) {
            if (!printLine && options->fileName()) {
                cerr << color_filename;
                cout << parser->fileName() << ":";
                cerr << color_normal;
            }

            if (!printLine && options->lineNumber()) {
                cerr << color_lineno;
                cout << parser->recordNumber() << ":";
                cerr << color_normal;
            }

            printLine = true;
            ovector = pcre2_get_ovector_pointer(match_data);

            for (int i = 0; i < rc; i++) {
                cout << string(tmp, ovector[2 * i]);
                cerr << color_match;
                cout << string(tmp + ovector[2 * i], ovector[2 * i + 1] - ovector[2 * i]);
                cerr << color_normal;
                tmp = tmp + ovector[2 * i + 1];
            }
        }
        pcre2_match_data_free(match_data);
    }

    if (printLine) {
        cout << tmp << endl;
        return 1;
    }
    return 0;
}

int main(int argc, const char **argv)
{
    Options options;

    try {
        options.run(argc, argv);
    }
    catch (exception &e) {
        cerr << e.what() << endl;
        return 2;
    }

    if (options.version()) {
        cout << "techlog "<< VERSION_MAJOR << "." << VERSION_MINOR << endl;
        cout << LICENSE << endl;
        cout << WRITTENBY << endl;
        return 1;
    }

    if (options.linePatterns().empty() && options.propertyPatterns().empty() && options.events().empty()) {
        cout << "usage: techlog [-ifl] [-s num] [-p pattern] [--event] [--property=pattern] [pattern]" << endl;
        if (!options.help() && !options.helpEvents()) {
            return 1;
        }
    }

    if (options.help()) {
        cout << options.visibleOptions() << endl;
        return 1;
    }

    if (options.helpEvents()) {
        cout << options.eventOptions() << endl;
        return 1;
    }

    bool isRegularFile;
    long unsigned linesSelected = 0;
    boost::system::error_code ec;

    pcre2_code *fileNamePattern = pcre2_compile(reinterpret_cast<PCRE2_SPTR>("^\\d{8}\\.log$"), PCRE2_ZERO_TERMINATED, PCRE2_CASELESS, &errorcode, &erroffset, nullptr);
    pcre2_jit_compile(fileNamePattern, PCRE2_JIT_COMPLETE);
    pcre2_match_data *fileNameMatchData = pcre2_match_data_create_from_pattern(fileNamePattern, nullptr);

    for (recursive_directory_iterator it("./"); it != recursive_directory_iterator();) {
        try {
            isRegularFile = is_regular_file(it->path());
        }
        catch (const filesystem_error& ex) {
            isRegularFile = false;
        }

        if (isRegularFile && (pcre2_jit_match(fileNamePattern, reinterpret_cast<PCRE2_SPTR>(it->path().filename().c_str()), it->path().filename().size(), 0, 0, fileNameMatchData, nullptr) > 0)) {
            File file(it->path().c_str());
            Parser parser(&file);

            while (parser.next()) {
                linesSelected += match(&parser, &options);

                if (options.stopAfter() > 0 && linesSelected == options.stopAfter()) {
                    break;
                }
            }

            if (options.stopAfter() > 0 && linesSelected == options.stopAfter()) {
                break;
            }
        }
        it.increment(ec);
    }

    pcre2_match_data_free(fileNameMatchData);
    pcre2_code_free(fileNamePattern);

    return linesSelected > 0 ? 0 : 1;
}