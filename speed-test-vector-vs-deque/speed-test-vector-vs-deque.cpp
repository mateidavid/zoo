#include <deque>
#include <vector>
#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>

namespace bo = boost::program_options;


struct Program_Options
{
    std::string cont_type;
    size_t num_cont;
    size_t avg_size;
    size_t num_lookups;
    long seed;
    bool print_res;
};


template < template < typename ... > class Cont >
int real_main(const Program_Options& po)
{
    typedef Cont< long > container_type;

    std::cout << "container: " << typeid(container_type).name() << "\n"
              << "num_cont: " << po.num_cont << "\n"
              << "avg_size: " << po.avg_size << "\n"
              << "num_lookups: " << po.num_lookups << "\n"
              << "seed: " << po.seed << "\n";

    // create containers
    std::vector< container_type > cont(po.num_cont);
    // grow containers
    for (size_t i = 0; i < po.num_cont * po.avg_size; ++i)
    {
        size_t cont_idx = static_cast< size_t >(drand48() * po.num_cont);
        size_t val = lrand48();
        cont[cont_idx].push_back(val);
    }
    std::cout << "container sizes:\n";
    for (size_t i = 0; i < po.num_cont; ++i)
    {
        std::cout << i << ": " << cont[i].size() << "\n";
    }

    auto start_time = std::chrono::system_clock::now();
    long res = 0;
    for (size_t i = 0; i < po.num_lookups; ++i)
    {
        size_t cont_idx = static_cast< size_t >(drand48() * po.num_cont);
        size_t val_idx = static_cast< size_t >(drand48() * cont[cont_idx].size());
        res ^= cont[cont_idx][val_idx];
    }
    auto end_time = std::chrono::system_clock::now();
    int elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    std::cout << "milliseconds: " << elapsed_milliseconds << "\n";

    if (po.print_res)
    {
        std::cout << "res: " << std::hex << res << "\n";
    }
}

int main(int argc, char* argv[])
{
    Program_Options po;

    bo::options_description generic_opts_desc("Generic options");
    bo::options_description config_opts_desc("Configuration options");
    bo::options_description hidden_opts_desc("Hidden options");
    bo::options_description cmdline_opts_desc;
    bo::options_description visible_opts_desc("Allowed options");
    generic_opts_desc.add_options()
        ("help,?", "produce help message")
        ;
    config_opts_desc.add_options()
        ("cont-type,c", bo::value< std::string >(&po.cont_type), "Container type")
        ("num-cont,m", bo::value< size_t >(&po.num_cont)->default_value(1), "Number of containers")
        ("avg-size,z", bo::value< size_t >(&po.avg_size)->default_value(1000000), "Average container size")
        ("num-lookups,n", bo::value< size_t >(&po.num_lookups)->default_value(1000000), "Average container size")
        ("seed", bo::value< long >(&po.seed), "Random seed")
        ;
    cmdline_opts_desc.add(generic_opts_desc).add(config_opts_desc).add(hidden_opts_desc);
    visible_opts_desc.add(generic_opts_desc).add(config_opts_desc);
    bo::variables_map vm;
    store(bo::command_line_parser(argc, argv).options(cmdline_opts_desc).run(), vm);
    notify(vm);

    // if help requested, print it and stop
    if (vm.count("help"))
    {
        std::cout << visible_opts_desc;
        exit(EXIT_SUCCESS);
    }
    // set random seed
    if (po.seed == 0)
    {
        po.seed = time(nullptr);
    }
    po.print_res = argc > 2000;
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
    */

    if (po.cont_type == "vector")
    {
        return real_main< std::vector >(po);
    }
    else if (po.cont_type == "deque")
    {
        return real_main< std::deque >(po);
    }
    else
    {
        std::cerr << "unknown container: " << po.cont_type << "\n";
        exit(EXIT_FAILURE);
    }
}
