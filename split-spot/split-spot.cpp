#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "global.hpp"
#include "logger.hpp"
#include "variables_map_converter.hpp"

using namespace std;
namespace bo = boost::program_options;

int real_main(const bo::variables_map& vm)
{
    string name;
    string seq;
    string comment;
    string qual;
    unsigned spot_len = 0;
    unsigned break_len = 0;
    size_t spot_count = 0;
    if (vm.count("break-len"))
    {
        break_len = vm.at("break-len").as< unsigned >();
    }
    while (getline(cin, name))
    {
        if (not (getline(cin, seq) and getline(cin, comment) and getline(cin, qual)))
        {
            cerr << "unterminated record for read: " << name << endl;
            exit(EXIT_FAILURE);
        }
        ++spot_count;
        if (spot_len == 0)
        {
            // detect spot length
            spot_len = seq.size();
            logger("main", info) << "detected: spot_len=" << spot_len << endl;
            if (break_len == 0)
            {
                // detect break length
                if (spot_len % 2 == 1)
                {
                    cerr << "spot_len is uneven; break_len is required" << endl;
                    exit(EXIT_FAILURE);
                }
                break_len = spot_len / 2;
                logger("main", info) << "detected: break_len=" << break_len << endl;
            }
        }
        if (seq.size() != spot_len)
        {
            cerr << "seq of unexpected length: " << seq.size() << "; expecting " << spot_len << endl;
            exit(EXIT_FAILURE);
        }
        if (qual.size() != spot_len)
        {
            cerr << "qual of unexpected length: " << qual.size() << "; expecting " << spot_len << endl;
            exit(EXIT_FAILURE);
        }
        if (vm.at("read-1").as< bool >())
        {
            cout
                << name << "/1" << endl
                << seq.substr(0, break_len) << endl
                << comment << endl
                << qual.substr(0, break_len) << endl;
        }
        if (vm.at("read-2").as< bool >())
        {
            cout
                << name << "/2" << endl
                << seq.substr(break_len) << endl
                << comment << endl
                << qual.substr(break_len) << endl;
        }
    } // end while
    logger("main", info) << "total: spot_count=" << spot_count << endl;
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    global::program_name() = argv[0];

    bo::options_description generic_opts_desc("Generic options");
    bo::options_description config_opts_desc("Configuration options");
    bo::options_description hidden_opts_desc("Hidden options");
    bo::options_description cmdline_opts_desc;
    bo::options_description visible_opts_desc("Allowed options");
    generic_opts_desc.add_options()
        ("help,?", "produce help message")
        // hack, see: http://lists.boost.org/boost-users/2010/01/55054.php
        ("log-level,d", bo::value< vector< string > >()->default_value(vector< string >(), ""), "log level")
        //("seed", bo::value< long >()->default_value(time(nullptr), "use time"), "random seed")
        //("progress-count,c", bo::value< unsigned >()->default_value(0), "progress count")
        ;
    config_opts_desc.add_options()
        ("read-1,1", bo::bool_switch(), "print read 1")
        ("read-2,2", bo::bool_switch(), "print read 2")
        ("break-len,b", bo::value< unsigned >(), "length of read 1")
        ;
    any_converter ac;
    ac.add_string_converter< string >();
    ac.add_string_converter< bool >();
    ac.add_string_converter< unsigned >();
    ac.add_string_converter< long >();
    ac.add_converter(&cont_to_ptree< vector< string > >);
    cmdline_opts_desc.add(generic_opts_desc).add(config_opts_desc).add(hidden_opts_desc);
    visible_opts_desc.add(generic_opts_desc).add(config_opts_desc);
    bo::variables_map vm;
    store(bo::command_line_parser(argc, argv).options(cmdline_opts_desc).run(), vm);
    notify(vm);

    // if help requested, print it and stop
    if (vm.count("help"))
    {
        cout << visible_opts_desc;
        exit(EXIT_SUCCESS);
    }
    // set log levels
    for (const auto& l : vm.at("log-level").as< vector< string > >())
    {
        size_t i = l.find(':');
        if (i == string::npos)
        {
            Logger::set_default_level(Logger::level_from_string(l));
            clog << "set default log level to: "
                 << static_cast< int >(Logger::get_default_level()) << "\n";
        }
        else
        {
            Logger::set_facility_level(l.substr(0, i), Logger::level_from_string(l.substr(i + 1)));
            clog << "set log level of '" << l.substr(0, i) << "' to: "
                 << static_cast< int >(Logger::get_facility_level(l.substr(0, i))) << "\n";
        }
    }
    // set random seed
    //srand48(vm.at("seed").as< long >());
    // print options
    logger("main", info) << variables_map_converter::to_ptree(vm, ac);

    if (not (vm.at("read-1").as< bool >() or vm.at("read-2").as< bool >()))
    {
        cerr << "at least one of -1 and -2 must be given" << endl;
        exit(EXIT_FAILURE);
    }
    return real_main(vm);
}
