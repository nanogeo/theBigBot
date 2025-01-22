// The MIT License (MIT)
//
// Copyright (c) 2021-2022 Alexander Kurbatov

#include "theBigBot.h"

#include <sc2api/sc2_coordinator.h>
#include <sc2api/sc2_gametypes.h>
#include <sc2utils/sc2_arg_parser.h>

#include <iostream>

#ifdef BUILD_FOR_LADDER
namespace
{

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
    std::cerr << "Main begins\n";
    Options options;
    ParseArguments(argc, argv, &options);
    std::cerr << "Parse args\n";

    sc2::Coordinator coordinator;
    std::cerr << "Coord made\n";
    /*if (!coordinator.LoadSettings(argc, argv))
    {
        std::cerr << "Coord load setting failed\n";
        return 1;
    }*/
    std::cerr << "Coord load setting succeeded\n";
    sc2::TheBigBot bot;
    std::cerr << "Bot made\n";

    size_t num_agents = 2;
    coordinator.SetParticipants({ CreateParticipant(sc2::Race::Protoss, &bot, "theBigBot") });

    std::cerr << "Set parts done\n";
    std::cerr << "Gameport: " << std::to_string(options.GamePort) << "\n";
    std::cout << "Connecting to port " << options.GamePort << std::endl;
    coordinator.Connect(options.GamePort);
    std::cerr << "Connected\n";
    coordinator.SetupPorts(num_agents, options.StartPort, false);

    std::cerr << "Setup ports done\n";
    // NB (alkurbatov): Increase speed of steps processing.
    // Disables ability to control your bot during game.
    // Recommended for competitions.
    coordinator.SetRawAffectsSelection(true);

    coordinator.JoinGame();
    coordinator.SetTimeoutMS(10000);
    std::cout << "Successfully joined game" << std::endl;

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
	
	if (true)
	{
		coordinator.SetParticipants({
			CreateParticipant(sc2::Race::Protoss, &bot1),
            CreateComputer(sc2::Race::Terran, sc2::Difficulty::VeryEasy, sc2::AIBuild::Macro)
			});
		coordinator.SetRealtime(true);

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
		coordinator.StartGame("EternalEmpireLE.SC2Map");
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
