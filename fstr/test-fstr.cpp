#include "fstr.hpp"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "use: " << argv[0] << " <in_file> <out_file>\n";
        abort();
    }
    std::string s;
    fstr in_fs(argv[1]);
    fstr out_fs(argv[2], std::ios_base::out);
    while (getline(in_fs, s))
    {
        out_fs << s;
        if (not in_fs.eof())
        {
            out_fs << "\n";
        }
    }
}
