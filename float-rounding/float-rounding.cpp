#include <chrono>
#include <iostream>
#include <random>
#include <deque>
#include <sstream>
#include <vector>

#include <cmath>
#include <cfenv>

using namespace std;

// Ref:
//   http://stackoverflow.com/a/429812/717706
inline int float2int(double d)
{
    union Cast
    {
        double d;
        long l;
    };
    volatile Cast c;
    c.d = d + 6755399441055744.0;
    return c.l;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "use: " << argv[0] << " <seed> <version>" << endl;
        return EXIT_FAILURE;
    }
    size_t seed = 0;
    unsigned version = 0;
    istringstream(argv[1]) >> seed;
    istringstream(argv[2]) >> version;
    if (seed == 0)
    {
        seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    }
    clog << "seed: " << seed << endl;
    clog << "version: " << version << endl;
    const unsigned n = 1000000000;
    ranlux48_base rg(seed);
    uniform_real_distribution< float > unif;
    long res = 0;
    float res_f = 0.0;
    cout.precision(30);
    std::fesetround(FE_TONEAREST);

    // timers
    decltype(chrono::high_resolution_clock::now()) start_time;
    decltype(chrono::high_resolution_clock::now()) end_time;

    switch (version)
    {
    case 0:
        cout << "testing: float addition" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            res_f += a;
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    case 1:
        cout << "testing: long addition + implicit conversion" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            long ai = a;
            res += ai;
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    case 2:
        cout << "testing: long addition + implicit conversion plus .5f" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            long ai = a + 0.5f;
            res += ai;
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    case 3:
        cout << "testing: long addition + std::round" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            long ai = std::round(a);
            res += ai;
            /*
            if (std::round(a) != std::nearbyint(a))
            {
                cout << "a=" << scientific << a
                     << " std::round(a)=" << scientific << std::round(a)
                     << " std::nearbyint(a)=" << scientific << std::nearbyint(a) << endl;
            }
            if (std::round(a) != (float)(int)(a + .5f))
            {
                cout << "a=" << scientific << a
                     << " std::round(a)=" << scientific << std::round(a)
                     << " (float)(int)(a + .5f)=" << scientific << (float)(int)(a + .5f) << endl;
            }
            */
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    case 4:
        cout << "testing: long addition + std::nearbyint" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            long ai = std::nearbyint(a);
            res += ai;
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    case 5:
        cout << "testing: long addition + float2int" << endl;
        start_time = chrono::high_resolution_clock::now();
        for (unsigned i = 0; i < n; ++i)
        {
            float a = unif(rg);
            if (a == .5f) continue;
            long ai = float2int(a);
            res += ai;
            /*
            long ai2 = std::round(a);
            if (ai != ai2)
            {
                cout << "a=" << scientific << a
                     << " std::round(a)=" << scientific << std::round(a)
                     << " float2int(a)=" << scientific << float2int(a) << endl;
            }
            */
        }
        end_time = chrono::high_resolution_clock::now();
        break;
    default:
        cerr << "no such test" << endl;
        exit(EXIT_FAILURE);
        break;
    }

    cout << "res: " << res << endl
         << "res_f: " << res_f << endl
         << "time: " << chrono::duration_cast< chrono::milliseconds >(end_time - start_time).count() << endl;
}
