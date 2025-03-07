// The MIT License (MIT)
//
// Copyright (c) 2021-2022 Alexander Kurbatov

#include "theBigBot.h"

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>

#include <iostream>

#ifdef BUILD_FOR_LADDER

#include <csignal>
#include <cstdlib>

namespace
{

// General signal handler
void signalHandler(int signal, siginfo_t* info, void* context) {
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

    // Exit after catching the signal
    std::exit(EXIT_FAILURE);
}


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

}  // namespace

int main(int argc, char* argv[])
{
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
	sc2::BlankBot bot2;
	
	if (!true)
	{
		coordinator.SetParticipants({
			CreateParticipant(sc2::Race::Protoss, &bot1),
            CreateComputer(sc2::Race::Protoss, sc2::Difficulty::VeryEasy, sc2::AIBuild::Macro)
			});
		coordinator.SetRealtime(false);

		coordinator.LaunchStarcraft();
		coordinator.StartGame("ThunderbirdAIE.SC2Map");
	}
	else
	{

		coordinator.SetParticipants({
            CreateParticipant(sc2::Race::Protoss, &bot2),
            CreateParticipant(sc2::Race::Protoss, &bot1)
			});
		coordinator.SetRealtime(true);

		coordinator.LaunchStarcraft();
		coordinator.StartGame("AbyssalReefAIE.SC2Map");
	}
	// LightShadeLE, AncientCisternAIE, DragonScalesAIE, GoldenAuraAIE, GresvanAIE, InfestationStationAIE, RoyalBloodAIE
    // Equilibrium512V2AIE, Goldenaura512V2AIE, HardLead512V2AIE, Oceanborn512V2AIE, SiteDelta512V2AIE, Gresvan512V2AIE
    // AbyssalReefAIE, AcropolisAIE, AutomatonAIE, EphemeronAIE, InterloperAIE, ThunderbirdAIE
	
	// EternalEmpireLE

    while (coordinator.Update()) {
    }
    
    return 0;
}

#endif
