// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "options.h"

Options::Options()
{
    _patternOptions.add_options()("pattern,p", po::value<std::vector<std::string>>()->multitoken(), ": use pattern ARG for matching");
    _patternOptions.add_options()("ignore-case,i", po::bool_switch(&_ignoreCase), ": ignore case distinctions");

    _miscOptions.add_options()("version", po::bool_switch(&_version), ": display version information and exit");
    _miscOptions.add_options()("help", po::bool_switch(&_help), ": display this help and exit");
    _miscOptions.add_options()("help-events", po::bool_switch(&_helpEvents), ": display events help and exit");

    _outputOptions.add_options()("stop-after,s", po::value(&_stopAfter), ": stop after ARG output lines");
    _outputOptions.add_options()("file-name,f", po::bool_switch(&_fileName), ": print the file name prefix on output");
    _outputOptions.add_options()("line-number,l", po::bool_switch(&_lineNumber), ": print line number with output lines");

    for (auto const &availableEvent: _availableEvents)
    {
        _eventOptions.add_options()(availableEvent.c_str(), "");
    }

    _positionalOptions.add("pattern", 1);

    _allOptions.add(_patternOptions).add(_outputOptions).add(_miscOptions).add(_eventOptions);
    _visibleOptions.add(_patternOptions).add(_outputOptions).add(_miscOptions);
}

Options::~Options()
{
    for(auto const& linePattern: _linePatterns)
    {
        pcre2_code_free(linePattern);
    }

    for(auto const& propertyPattern: _propertyPatterns)
    {
        pcre2_code_free(propertyPattern.second);
    }
}

void Options::run(int argc, const char **argv)
{
    try
    {
        po::variables_map vm;

        auto parsed = po::basic_command_line_parser<char>(argc, argv).options(_allOptions).allow_unregistered().positional(_positionalOptions).run();
        auto unrecognizedOptions = po::collect_unrecognized(parsed.options, po::exclude_positional);

        po::store(parsed, vm);

        po::notify(vm);

        for(auto const& availableEvent: _availableEvents)
        {
            if (vm.count(availableEvent))
            {
                _events.push_back(availableEvent);
            }
        }

        int errorcode;
        PCRE2_SIZE erroffset;

        if (vm.count("pattern"))
        {
            for(auto const& linePattern: vm["pattern"].as<std::vector<std::string>>())
            {
                _linePatterns.push_back(pcre2_compile(reinterpret_cast<PCRE2_SPTR>(linePattern.c_str()), PCRE2_ZERO_TERMINATED, PCRE2_DOTALL | (ignoreCase() ? PCRE2_CASELESS : 0), &errorcode, &erroffset, NULL));
                pcre2_jit_compile(_linePatterns.back(), PCRE2_JIT_COMPLETE);
            }
        }

        for(auto const& unrecognizedOption: unrecognizedOptions)
        {
            auto splitted = po::split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0)
            {
                _propertyPatterns.push_back(std::make_pair(splitted[0].substr(2), pcre2_compile(reinterpret_cast<PCRE2_SPTR>(splitted[1].c_str()), PCRE2_ZERO_TERMINATED, PCRE2_DOTALL | (ignoreCase() ? PCRE2_CASELESS : 0), &errorcode, &erroffset, NULL)));
                pcre2_jit_compile(_propertyPatterns.back().second, PCRE2_JIT_COMPLETE);
            }
            else
            {
                throw po::unknown_option(unrecognizedOption);
            }
        }
    }
    catch (std::exception &e)
    {
        throw;
    }
}

bool Options::version()
{
    return _version;
}

bool Options::help()
{
    return _help;
}

bool Options::helpEvents()
{
    return _helpEvents;
}

unsigned int Options::stopAfter()
{
    return _stopAfter;
}

bool Options::fileName()
{
    return _fileName;
}

bool Options::lineNumber()
{
    return _lineNumber;
}

bool Options::ignoreCase()
{
    return _ignoreCase;
}

std::vector<pcre2_code*> Options::linePatterns()
{
    return _linePatterns;
}

std::vector<std::pair<std::string, pcre2_code*>> Options::propertyPatterns()
{
    return _propertyPatterns;
}

std::vector<std::string> Options::events()
{
    return _events;
}

po::options_description Options::visibleOptions()
{
    return _visibleOptions;
}

po::options_description Options::eventOptions()
{
    return _eventOptions;
}