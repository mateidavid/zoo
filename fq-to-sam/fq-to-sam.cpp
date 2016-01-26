#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "global.hpp"
#include "logger.hpp"
#include "variables_map_converter.hpp"

using namespace std;
namespace bo = boost::program_options;

int real_main(const bo::variables_map&)
{
    string name;
    string seq;
    string comment;
    string qual;
    size_t read_count = 0;
    size_t paired_count = 0;
    size_t unpaired_count = 0;
    size_t first_in_pair_count = 0;
    size_t second_in_pair_count = 0;
    while (getline(cin, name))
    {
        if (not (getline(cin, seq) and getline(cin, comment) and getline(cin, qual)))
        {
            cerr << "unterminated record for read: " << name << endl;
            exit(EXIT_FAILURE);
        }
        ++read_count;

        if (seq.size() != qual.size())
        {
            cerr << "seq and qual of different length: seq_len=" << seq.size() << " qual_len=" << qual.size() << endl;
            exit(EXIT_FAILURE);
        }

        // detect pairing from read name
        bool is_paired = false;
        int first_in_pair = true;
        if (name.size() > 2 and name.substr(name.size() - 2) == "/1")
        {
            is_paired = true;
            first_in_pair = true;
            ++paired_count;
            ++first_in_pair_count;
        }
        else if (name.size() > 2 and name.substr(name.size() - 2) == "/2")
        {
            is_paired = true;
            first_in_pair = false;
            ++paired_count;
            ++second_in_pair_count;
        }
        else
        {
            ++unpaired_count;
        }

        cout << (is_paired? name.substr(1, name.size() - 3) : name.substr(1)) << "\t"
             << (is_paired? (0x1 | 0x4 | 0x8 | (first_in_pair? 0x40 : 0x80)) : 0x4) << "\t"
             << "*" << "\t"
             << 0 << "\t"
             << 0 << "\t"
             << "*" << "\t"
             << "*" << "\t"
             << 0 << "\t"
             << 0 << "\t"
             << seq << "\t"
             << qual << endl;
    } // end while
    logger("main", info) << "total: read_count=" << read_count << endl;
    logger("main", info) << "total: paired_count=" << paired_count << endl;
    logger("main", info) << "total: unpaired_count=" << unpaired_count << endl;
    logger("main", info) << "total: first_in_pair_count=" << first_in_pair_count << endl;
    logger("main", info) << "total: second_in_pair_count=" << second_in_pair_count << endl;
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

    return real_main(vm);
}
