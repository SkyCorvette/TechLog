#include "options.h"

Options::Options() {
    _patternOptions.add_options()("pattern,p", value<vector<string>>()->multitoken(), ": use pattern ARG for matching");
    _patternOptions.add_options()("ignore-case,i", bool_switch(&_ignoreCase), ": ignore case distinctions");

    _miscOptions.add_options()("version", bool_switch(&_version), ": display version information and exit");
    _miscOptions.add_options()("help", bool_switch(&_help), ": display this help and exit");
    _miscOptions.add_options()("help-events", bool_switch(&_helpEvents), ": display events help and exit");

    _outputOptions.add_options()("stop-after,s", value(&_stopAfter), ": stop after ARG output lines");
    _outputOptions.add_options()("file-name,f", bool_switch(&_fileName), ": print the file name prefix on output");
    _outputOptions.add_options()("line-number,l", bool_switch(&_lineNumber), ": print line number with output lines");

    for (auto const &availableEvent: _availableEvents) {
        _eventOptions.add_options()(availableEvent.c_str(), "");
    }

    _positionalOptions.add("pattern", 1);

    _allOptions.add(_patternOptions).add(_outputOptions).add(_miscOptions).add(_eventOptions);
    _visibleOptions.add(_patternOptions).add(_outputOptions).add(_miscOptions);
}

Options::~Options() {
    for(auto const& linePattern: _linePatterns) {
        pcre2_code_free(linePattern);
    }

    for(auto const& propertyPattern: _propertyPatterns) {
        pcre2_code_free(propertyPattern.second);
    }
}

void Options::run(int argc, const char **argv) {
    try {
        variables_map vm;

        auto parsed = basic_command_line_parser<char>(argc, argv).options(_allOptions).allow_unregistered().positional(_positionalOptions).run();
        auto unrecognizedOptions = collect_unrecognized(parsed.options, exclude_positional);

        store(parsed, vm);

        notify(vm);

        for(auto const& availableEvent: _availableEvents) {
            if (vm.contains(availableEvent)) {
                _events.push_back(availableEvent);
            }
        }

        int errorcode;
        PCRE2_SIZE erroffset;

        if (vm.contains("pattern")) {
            for(auto const& linePattern: vm["pattern"].as<vector<string>>()) {
                _linePatterns.push_back(pcre2_compile(reinterpret_cast<PCRE2_SPTR>(linePattern.c_str()), PCRE2_ZERO_TERMINATED, PCRE2_DOTALL | (ignoreCase() ? PCRE2_CASELESS : 0), &errorcode, &erroffset, nullptr));
                pcre2_jit_compile(_linePatterns.back(), PCRE2_JIT_COMPLETE);
            }
        }

        for(auto const& unrecognizedOption: unrecognizedOptions) {
            auto splitted = split_unix(unrecognizedOption, "=");

            if (splitted.size() == 2 && unrecognizedOption.find("--") == 0) {
                _propertyPatterns.emplace_back(string(splitted[0].substr(2)), pcre2_compile(reinterpret_cast<PCRE2_SPTR>(splitted[1].c_str()), PCRE2_ZERO_TERMINATED, PCRE2_DOTALL | (ignoreCase() ? PCRE2_CASELESS : 0), &errorcode, &erroffset, nullptr));
                pcre2_jit_compile(_propertyPatterns.back().second, PCRE2_JIT_COMPLETE);
            }
            else {
                throw unknown_option(unrecognizedOption);
            }
        }
    }
    catch (exception&) {
        throw;
    }
}

bool Options::version() const {
    return _version;
}

bool Options::help() const {
    return _help;
}

bool Options::helpEvents() const {
    return _helpEvents;
}

unsigned int Options::stopAfter() const {
    return _stopAfter;
}

bool Options::fileName() const {
    return _fileName;
}

bool Options::lineNumber() const {
    return _lineNumber;
}

bool Options::ignoreCase() const {
    return _ignoreCase;
}

vector<pcre2_code_8 *> Options::linePatterns() {
    return _linePatterns;
}

vector<pair<string, pcre2_code_8 *> > Options::propertyPatterns() {
    return _propertyPatterns;
}

vector<string> Options::events() {
    return _events;
}

options_description Options::visibleOptions() {
    return _visibleOptions;
}

options_description Options::eventOptions() {
    return _eventOptions;
}