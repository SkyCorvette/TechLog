#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, const char** argv)
{
    try
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

        if (vm.count("help")) {
            std::cout << all << std::endl;
            return 1;
        }

        if (vm.count("ignore-case")) {
            std::cout << "ignore-case" << std::endl;
        }

        if (vm.count("no-filename")) {
            std::cout << "no-filename" << std::endl;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 1;
}