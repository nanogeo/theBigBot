// The MIT License (MIT)
//
// Copyright (c) 2021-2022 Alexander Kurbatov
#pragma warning(push)
#pragma warning(disable : 4702)

#include "theBigBot.h"
#include "test_bots.h"


#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>

#include <iostream>

#ifdef BUILD_FOR_LADDER

#include <csignal>
#include <cstdlib>

#ifdef __linux__

#include <execinfo.h>
#include <signal.h>
#include <unistd.h>

// General signal handler
void signalHandler(int signal, siginfo_t* info, void* context) 
{
    std::cerr << "Caught signal: " << signal << std::endl;

    // Handle specific signal types
    switch (signal) {
    case SIGSEGV:
        std::cerr << "Segmentation fault!" << std::endl;
        break;
    case SIGFPE:
        std::cerr << "Floating-point exception!" << std::endl;
        break;
    case SIGILL:
        std::cerr << "Illegal instruction!" << std::endl;
        break;
    case SIGABRT:
        std::cerr << "Aborted!" << std::endl;
        break;
    default:
        std::cerr << "Caught other signal" << std::endl;
        break;
    }

    void* array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    backtrace_symbols_fd(array, size, STDERR_FILENO);

    // Exit after catching the signal
    std::exit(EXIT_FAILURE);
}
#endif

struct Options
{
    Options(): GamePort(0), StartPort(0)
    {}

    int32_t GamePort;
    int32_t StartPort;
    std::string ServerAddress;
    std::string OpponentId;
};

void ParseArguments(int argc, char* argv[], Options* options_)
{
    sc2::ArgParser arg_parser(argv[0]);
    arg_parser.AddOptions(
        {
            {"-g", "--GamePort", "Port of client to connect to", false},
            {"-o", "--StartPort", "Starting server port", false},
            {"-l", "--LadderServer", "Ladder server address", false},
            {"-x", "--OpponentId", "PlayerId of opponent", false},
        });

    arg_parser.Parse(argc, argv);

    std::string GamePortStr;
    if (arg_parser.Get("GamePort", GamePortStr))
        options_->GamePort = atoi(GamePortStr.c_str());

    std::string StartPortStr;
    if (arg_parser.Get("StartPort", StartPortStr))
        options_->StartPort = atoi(StartPortStr.c_str());

    std::string OpponentId;
    if (arg_parser.Get("OpponentId", OpponentId))
        options_->OpponentId = OpponentId;

    arg_parser.Get("LadderServer", options_->ServerAddress);
}


int main(int argc, char* argv[])
{
#ifdef __linux__
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO; // Use siginfo_t
    sa.sa_sigaction = signalHandler; // Register signal handler
    sigemptyset(&sa.sa_mask);

    // Register for multiple signals
    if (sigaction(SIGSEGV, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }
    if (sigaction(SIGFPE, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }
    if (sigaction(SIGILL, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }
    if (sigaction(SIGABRT, &sa, nullptr) == -1) {
        perror("sigaction");
        return 1;
    }
#endif
    Options options;
    ParseArguments(argc, argv, &options);

    sc2::Coordinator coordinator;
    sc2::TheBigBot bot;

    size_t num_agents = 2;
    coordinator.SetParticipants({ CreateParticipant(sc2::Race::Protoss, &bot, "theBigBot") });

    coordinator.Connect(options.GamePort);
    coordinator.SetupPorts(num_agents, options.StartPort, false);

    // NB (alkurbatov): Increase speed of steps processing.
    // Disables ability to control your bot during game.
    // Recommended for competitions.
    coordinator.SetRawAffectsSelection(true);

    coordinator.JoinGame();
    coordinator.SetTimeoutMS(10000);
    std::cerr << "Successfully joined game" << std::endl;

    while (coordinator.Update())
    {
    }

    return 0;
}

#else

int main(int argc, char* argv[])
{
	sc2::Coordinator coordinator;
    if (!coordinator.LoadSettings(argc, argv)) {
        return 1;
    }

    coordinator.SetMultithreaded(true);
    // Add the custom bot, it will control the players.
    sc2::TheBigBot bot1;
	//sc2::ZealotRushBot bot2;
    //sc2::CannonRushBot bot2;
    //sc2::ZerglingFloodBot bot2;
    //sc2::Proxy4RaxBot bot2;
    //sc2::TwelvePoolBot bot2;
    sc2::BlankBot bot2;
	
	if (true)
	{
		coordinator.SetParticipants({ 
			CreateParticipant(sc2::Race::Protoss, &bot1),
            CreateComputer(sc2::Race::Terran, sc2::Difficulty::VeryHard, sc2::AIBuild::Macro)
			});
		coordinator.SetRealtime(!true);

		coordinator.LaunchStarcraft();
		coordinator.StartGame("TorchesAIE.SC2Map");
	}
	else
	{

		coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Protoss, &bot1),
            CreateParticipant(sc2::Race::Protoss, &bot2)
			});
		coordinator.SetRealtime(!true);

		coordinator.LaunchStarcraft();
		coordinator.StartGame("PersephoneAIE.SC2Map");
	}

    // 2025 season 1
    // AbyssalReefAIE, AcropolisAIE, AutomatonAIE, EphemeronAIE, InterloperAIE, ThunderbirdAIE
    // 2025 season 2
    // PersephoneAIE, PylonAIE, TorchesAIE
    // 
    // later : IncorporealAIE, LastFantasyAIE, LeyLinesAIE, MagannathaAIE, UltraloveAIE
	// incorporeal, persephone, torches, weird
	// EternalEmpireLE

    while (coordinator.Update()) {
    }
    
    return 0;
}

#endif
