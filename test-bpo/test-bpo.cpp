#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "variables_map_converter.hpp"

using namespace std;
namespace bo = boost::program_options;

int real_main(const bo::variables_map&)
{
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    //Program_Options po;
    //program_name() = argv[0];

    bo::options_description generic_opts_desc("Generic options");
    bo::options_description config_opts_desc("Configuration options");
    bo::options_description hidden_opts_desc("Hidden options");
    bo::options_description cmdline_opts_desc;
    bo::options_description visible_opts_desc("Allowed options");
    generic_opts_desc.add_options()
        ("help,?,h", "produce help message")
        ;
    config_opts_desc.add_options()
        ("input-file,i", bo::value< string >()->default_value("in"), "input file")
        ("value,v", bo::value< int >()->default_value(0), "input value")
        ("uvalue,u", bo::value< unsigned int >()->default_value(0), "input unsigned value")
        ("stats-file,x", bo::value< string >(), "stats file")
        ("log-level,d", bo::value< vector< string > >()->composing(), "default log level")
        ;
    /*
        ("supercontig-lengths-file,l", bo::value< string >(&po.supercontig_lengths_file), "supercontig lengths file")
        ("mutations-file,M", bo::value< string >(&po.mutations_file), "mutations file")
        ("unmappable-contigs-file,U", bo::value< string >(&po.unmappable_contigs_file), "unmappable contigs file")
        ("terminal-reads", bo::value< string >(&po.terminal_reads_file), "terminal reads file")
        ("progress-count,c", bo::value< size_t >(&po.progress_count)->default_value(0), "progress count")
        ("unmap-trigger-len,u", bo::value< size_t >(&po.unmap_trigger_len)->default_value(9), "unmap trigger len")
        ("cat-step,s", bo::bool_switch(&po.cat_at_step), "cat contigs at each step")
        ("cat-end,e", bo::bool_switch(&po.cat_at_end), "cat contigs at end")
        ("unmap-read-ends", bo::bool_switch(&po.unmap_read_ends), "unmap read ends")
        ("print-at-step,G", bo::bool_switch(&po.print_at_step), "print graph at each step")
        ("print-at-end,g", bo::bool_switch(&po.print_at_end), "print graph at end")
        ("check-at-step,C", bo::bool_switch(&po.check_at_step), "check graph at each step")
        ("resolve-unmappable", bo::bool_switch(&po.resolve_unmappable_regions), "resolve unmappable regions")
        ("unmap-single-chunks", bo::bool_switch(&po.unmap_single_chunks), "unmap single chunks")
        ("unmap-mut-clusters", bo::bool_switch(&po.unmap_mut_clusters), "unmap mutation clusters")
        ("interactive", bo::bool_switch(&po.interactive), "run interactive commands")
        ("log-level,d", bo::value< vector< string > >(&po.log_level)->composing(), "default log level")
        ("save,S", bo::value< string >(&po.save_file), "save file")
        ("load,L", bo::value< string >(&po.load_file), "load file")
        ("seed", bo::value< long >(&po.seed), "RNG seed")
        ;
    */

    any_converter ac;
    ac.add_string_converter< int >();
    ac.add_string_converter< unsigned int >();
    ac.add_string_converter< long >();
    ac.add_string_converter< size_t >();
    ac.add_string_converter< string >();
    ac.add_converter< vector< string > >([] (const boost::any& v) {
            std::ostringstream os;
            os << "(";
            auto vec = boost::any_cast< vector< string > >(v);
            bool first = true;
            for (auto e : vec)
            {
                if (first)
                {
                    first = false;
                }
                else
                {
                    os << ",";
                }
                os << e;
            }
            os << ")";
            return boost::any(os.str());
        });

    any_converter ac_ptree;
    ac_ptree.add_string_converter< int >();
    ac_ptree.add_string_converter< unsigned int >();
    ac_ptree.add_string_converter< long >();
    ac_ptree.add_string_converter< size_t >();
    ac_ptree.add_string_converter< string >();
    /*
    ac_ptree.add_string_converter< vector< string > >([] (const boost::any& v) {
            return boost::any(cont_to_ptree< vector< string > >(boost::any_cast< vector< string > >(v)));
        });
    */
    ac_ptree.add_converter(&cont_to_ptree< vector< string > >);

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
    /*
    // set log levels
    for (const auto& l : po.log_level)
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
    if (po.seed == 0)
    {
        po.seed = time(nullptr);
    }
    */

    cout << "to_stream:" << endl;
    variables_map_converter::to_stream(vm, ac, cout);
    cout << "to_ptree:" << endl;
    cout << variables_map_converter::to_ptree(vm, ac_ptree);
    return real_main(vm);
}
