#include "engine.hpp"
#include "meta.hpp"

#include "inugami/core.hpp"
#include "inugami/exception.hpp"

#include <fstream>
#include <iostream>
#include <exception>
#include <functional>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace Inugami;
using namespace std;

void dumpProfiles();
void errorMessage(const char*);

int main(int argc, char* argv[])
{
    profiler = new Profiler();
    ScopedProfile prof(profiler, "Main");

    std::ofstream logfile("log.txt");
    logger = new Logger<>(logfile);

    Core::RenderParams renparams;
    renparams.fsaaSamples = 4;

    {
        unordered_map<string, function<void()>> argf = {
              {"--fullscreen", [&]{renparams.fullscreen=true;}}
            , {"--windowed",   [&]{renparams.fullscreen=false;}}
            , {"--vsync",      [&]{renparams.vsync=true;}}
            , {"--no-vsync",   [&]{renparams.vsync=false;}}
        };

        while (*++argv)
        {
            auto iter = argf.find(*argv);
            if (iter != end(argf)) iter->second();
        }
    }

    try
    {
        logger->log("Creating Core...");
        Engine base(renparams);
        logger->log("Go!");
        base.go();
    }
    catch (const exception& e)
    {
        errorMessage(e.what());
        return -1;
    }
    catch (...)
    {
        errorMessage("Unknown error!");
        return -1;
    }

    dumpProfiles();

    return 0;
}

void dumpProfiles()
{
    using Prof = Profiler::Profile;

    ofstream pfile("profile.txt");

    auto all = profiler->getAll();

    function<void(const Prof::Ptr&, string)> dumProf;

    dumProf = [&](const Prof::Ptr& in, string indent)
    {
        pfile << indent << "Min: " << in->min     << "\n";
        pfile << indent << "Max: " << in->max     << "\n";
        pfile << indent << "Avg: " << in->average << "\n";
        pfile << indent << "Num: " << in->samples << "\n\n";
        for (auto& p : in->getChildren())
        {
            pfile << indent << p.first << ":\n";
            dumProf(p.second, indent+"\t");
        }
    };

    for (auto& p : all)
    {
        pfile << p.first << ":\n";
        dumProf(p.second, "\t");
    }
}

void errorMessage(const char* str)
{
#ifdef _WIN32
    MessageBoxA(nullptr, str, "Exception", MB_OK);
#endif
    logger->log(str);
}
