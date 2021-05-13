#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

/**
 * Struct for organizing all the player data
 */
struct Player
{
	std::string id = "player";
	double elo = 1500;
	int placement = 0;

	friend std::ostream& operator<<(std::ostream& output, const Player& p);
};

/**
 * The ostream operator overload for the Player struct
 * @param output The output to be returned
 * @param p The player struct
 * @return The final output
 */
std::ostream& operator<<(std::ostream& output, const Player &p)
{
	output << "Player ID: " << p.id << std::endl;
	output << "Player Elo: " << p.elo << std::endl;
	output << "Player Placement: " << p.placement << std::endl;

	return output;
}

/**
 * Struct for managing the config file
 */
struct Config
{
	int weight;
	bool mega;
	bool ask_for_confirmation;
	std::string input_file;
	std::string output_file;
};

/**
 * Function for asking confirmation via console
 * @param text The text to be displayed
 * @return The response from the user
 */
bool confirm_text(const std::string& text)
{
	std::cout << text;
	char answer;
	std::cin >> answer;
	switch(answer)
	{
	case 'y':
		return true;
		break;
	case 'n':
		return false;
		break;
	default:
		std::cout << "Error: Bad response\n\n";
		return confirm_text(text);
		break;
	}
}

/**
 * Basic function for parsing a json file
 * @param filepath The path to the file
 * @return The parsed json data
 */
json parse_json(const std::string& filepath)
{
	std::ifstream i(filepath, std::ios::in);
	json j;
	i >> j;
	return j;
}

/**
 * Function for parsing a Player struct to a .json file
 * @param j The json file
 * @param p The player struct
 */
void to_json(json& j, const Player& p) 
{
	j = json
	{
		{"id", p.id},
		{"elo", p.elo},
		{"placement", p.placement}
	};
}

/**
 * Function for parsing the .json data to a Player object
 * @param j The json file
 * @param p The config struct
 */
void from_json(const json& j, Player& p) 
{
	j.at("id").get_to(p.id);
	j.at("elo").get_to(p.elo);
	j.at("placement").get_to(p.placement);
}

/**
 * Function for parsing .json data to a Config object
 * @param j The json file
 * @param c The config struct
 */
void from_json(const json& j, Config& c) 
{
	j.at("weight").get_to(c.weight);
	j.at("mega").get_to(c.mega);
	j.at("ask_for_confirmation").get_to(c.ask_for_confirmation);
	j.at("input_file").get_to(c.input_file);
	j.at("output_file").get_to(c.output_file);
}

/**
 * Function for giving out all the elo accordingly
 * @param particpants The vector of all the participants
 * @param config The configuration file data
 */
void split_pot(std::vector<Player>& participants, const Config& config)
{
	const char placementCount = config.mega ? 10 : 3;
	std::vector<double> ratio = {0.7, 0.2, 0.1};
	if(config.mega)
	{
		ratio = {0.4, 0.2, 0.15, 0.1, 0.05, 0.02, 0.02, 0.02, 0.02, 0.02};
	}
	
	bool draw = false;

	{
		bool _winner;
		for(const auto& p : participants)
		{
			if(p.placement == 1)
			{
				if(_winner)
					draw = true;
				else
					_winner = true;
			}
		}
	}

	if(draw)
	{
		const double _sum = ratio[0] + ratio[1];
		ratio[0] = _sum * 0.5;
		ratio[1] = _sum * 0.5;
	}

	std::vector<Player> winners;
	for(const auto& p : participants)
	{
		if(p.placement > placementCount || p.placement < 1)
			continue;
		winners.push_back(p);
	}
	
	for(auto& w : winners)
	{
		if(w.placement == 1 && !draw)
			continue;
		w.elo -= config.weight;
	}

	// NOTE: Standard deviation or averageElo may be set to change, but for now, it's hardcoded
	// int standardDeviation = 200;
	// double averageElo = 1500;
	std::vector<double> eloGain;
	for(char i = 0; i < static_cast<char>(winners.size()); ++i)
	{
		eloGain.push_back(config.weight * ratio[i]);
		for(char j = 0; j < static_cast<char>(participants.size() + draw - 1); ++j)
			winners[i].elo += (eloGain[i] / (pow(2, (winners[i].elo - 1500) / 200))); 
	}

	for(auto& p : participants)
	{
		if(p.placement > placementCount || p.placement < 1)
			continue;
		
		for(const auto& w : winners)
		{
			if(p.placement == w.placement)
			{
				p.elo = w.elo;
			}
		}
	}
}

int main()
{
	const Config config = parse_json("config.json");

	std::vector<Player> participants = parse_json(config.input_file);

	if(config.ask_for_confirmation)
	{
		for(const auto& p : participants)
		{
			std::cout << p << std::endl;
		}

		// Ask for confirmation before splitting the pot
		if(!confirm_text("Continue? [y/n]: "))
		{
			return 0;
		}
	}

	// Split pot
	split_pot(participants, config);

	if(config.ask_for_confirmation)
	{
		std::cout << std::endl;
		for(const auto& p : participants)
		{
			std::cout << p << std::endl;
		}

		// Ask for confirmation before updating the data to the output json file
		if(!confirm_text("Apply ELO changes? [y/n]: "))
		{
			return 0;
		}
	}

	// Update the data to the output json file
	json pjson(participants);
	std::ofstream o(config.output_file);
	o << std::setw(4) << pjson << std::endl;

	return 0;
}
