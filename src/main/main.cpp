// Copyright 2014 Stellar Development Foundation and contributors. Licensed
// under the Apache License, Version 2.0. See the COPYING file at the root
// of this distribution or at http://www.apache.org/licenses/LICENSE-2.0

#include "main/CommandLine.h"
#include "main/DeprecatedCommandLine.h"
#include "util/Logging.h"

#include "crypto/ByteSliceHasher.h"
#include <cstdlib>
#include <sodium/core.h>
#include <xdrpp/marshal.h>


#include <config/bitcoin-config.h>
#include <chainparams.h>
#include <clientversion.h>
#include <compat.h>
#include <fs.h>
#include <interfaces/chain.h>
#include <rpc/server.h>
#include <init.h>
#include <noui.h>
#include <shutdown.h>
#include <util/system.h>
#include <httpserver.h>
#include <httprpc.h>
#include <util/strencodings.h>
#include <walletinitinterface.h>

#include <stdio.h>

INITIALIZE_EASYLOGGINGPP

const std::function<std::string(const char*)> G_TRANSLATION_FUN = nullptr; 

namespace stellar
{
static void
outOfMemory()
{
    std::fprintf(stderr, "Unable to allocate memory\n");
    std::fflush(stderr);
    std::abort();
}
}

static void WaitForShutdown()
{
    while (!ShutdownRequested())
    {
        MilliSleep(200);
    }
    Interrupt();
}

static bool AppInit(int argc, char* argv[])
{
    InitInterfaces interfaces;
    interfaces.chain = interfaces::MakeChain();

    bool fRet = false;

    //
    // Parameters
    //
    // If Qt is used, parameters/bitcoin.conf are parsed in qt/bitcoin.cpp's main()
    SetupServerArgs();
    std::string error;
    if (!gArgs.ParseParameters(argc, argv, error)) {
        fprintf(stderr, "Error parsing command line arguments: %s\n", error.c_str());
        return false;
    }

    // Process help and version before taking care about datadir
    if (HelpRequested(gArgs) || gArgs.IsArgSet("-version")) {
        std::string strUsage = PACKAGE_NAME " Daemon version " + FormatFullVersion() + "\n";

        if (gArgs.IsArgSet("-version"))
        {
            strUsage += FormatParagraph(LicenseInfo()) + "\n";
        }
        else
        {
            strUsage += "\nUsage:  bitcoind [options]                     Start " PACKAGE_NAME " Daemon\n";
            strUsage += "\n" + gArgs.GetHelpMessage();
        }

        fprintf(stdout, "%s", strUsage.c_str());
        return true;
    }

    try
    {
        if (!fs::is_directory(GetDataDir(false)))
        {
            fprintf(stderr, "Error: Specified data directory \"%s\" does not exist.\n", gArgs.GetArg("-datadir", "").c_str());
            return false;
        }
        if (!gArgs.ReadConfigFiles(error, true)) {
            fprintf(stderr, "Error reading configuration file: %s\n", error.c_str());
            return false;
        }
        // Check for -testnet or -regtest parameter (Params() calls are only valid after this clause)
        try {
            SelectParams(gArgs.GetChainName());
        } catch (const std::exception& e) {
            fprintf(stderr, "Error: %s\n", e.what());
            return false;
        }

        // Error out when loose non-argument tokens are encountered on command line
        for (int i = 1; i < argc; i++) {
            if (!IsSwitchChar(argv[i][0])) {
                fprintf(stderr, "Error: Command line contains unexpected token '%s', see bitcoind -h for a list of options.\n", argv[i]);
                return false;
            }
        }

        // -server defaults to true for bitcoind but not for the GUI so do this here
        gArgs.SoftSetBoolArg("-server", true);
        // Set this early so that parameter interactions go to console
        InitLogging();
        InitParameterInteraction();
        if (!AppInitBasicSetup())
        {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }
        if (!AppInitParameterInteraction())
        {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }
        if (!AppInitSanityChecks())
        {
            // InitError will have been called with detailed error, which ends up on console
            return false;
        }
        if (gArgs.GetBoolArg("-daemon", false))
        {
#if HAVE_DECL_DAEMON
            fprintf(stdout, "Bitcoin server starting\n");

            // Daemonize
            if (daemon(1, 0)) { // don't chdir (1), do close FDs (0)
                fprintf(stderr, "Error: daemon() failed: %s\n", strerror(errno));
                return false;
            }
#else
            fprintf(stderr, "Error: -daemon is not supported on this operating system\n");
            return false;
#endif // HAVE_DECL_DAEMON
        }
        // Lock data directory after daemonization
        if (!AppInitLockDataDirectory())
        {
            // If locking the data directory failed, exit immediately
            return false;
        }
        fRet = AppInitMain(interfaces);
        std::cout<< fRet << "= 1 ==============================================================================================================\n";
    }
    catch (const std::exception& e) {
        PrintExceptionContinue(&e, "AppInit()");
    } catch (...) {
        PrintExceptionContinue(nullptr, "AppInit()");
    }

    return fRet;
}

int
main(int argc, char* const* argv)
{
    using namespace stellar;
    
    // Abort when out of memory
    std::set_new_handler(outOfMemory);

    Logging::init();
    if (sodium_init() != 0)
    {
        LOG(FATAL) << "Could not initialize crypto";
        return 1;
    }
    shortHash::initialize();

    xdr::marshaling_stack_limit = 1000;

    //bitcoin impl  
    std::cout << "Bitcoin Initialization Starts.....................\n";
    
    SetupEnvironment();

    // Connect bitcoind signal handlers
    noui_connect();


    std::cout<< "initialization  of appInit - bitcoin \n";

    char* argvArrayBC[] = {"bitcoind", "-regtest"};
    bool test = AppInit(2, argvArrayBC);

    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    std::cout<< "---------------------------------------NON BLOCKING -------------------------------------------------- \n";
    
    auto result = handleCommandLine(argc, argv);
    if (result)
    {
        return *result;
    }

    return handleDeprecatedCommandLine(argc, argv);

    //  std::cout<< fRet << "= 2 ==============================================================================================================\n";

    // if (!fRet)
    // {
    //     std::cout<< fRet << "= 3 ==============================================================================================================\n";
    //     Interrupt();
    // } else {
    //     std::cout<< fRet << "= 4 ==============================================================================================================\n";
    //     WaitForShutdown();
    //     std::cout<< fRet << "= 5 ==============================================================================================================\n";
    // }
    // Shutdown(interfaces);

}
