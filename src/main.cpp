#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, const char** argv)
{
    po::options_description all("Allowed options");

    po::options_description regexp("Regexp selection and interpretation");
    regexp.add_options()("ignore-case,i", "ignore case distinctions");
    all.add(regexp);

    po::options_description misc("Miscellaneous");
    misc.add_options()("help", "display this help text and exit");
    all.add(misc);

    po::options_description output("Output control");
    output.add_options()("no-filename,h", "suppress the file name prefix on output");
    all.add(output);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, all), vm);
    po::notify(vm);

    if (vm.count("help"))
    {
        std::cout << all << "\n";
        return 1;
    }
}