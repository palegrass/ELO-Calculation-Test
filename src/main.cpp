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
	std::string discord_id = "player";
	double elo = 1500;
	int placement = 0;
	bool has_placed = false;

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
	output << "Player ID: " << p.discord_id << std::endl;
	output << "Player Elo: " << p.elo << std::endl;
	output << "Player Placement: " << p.placement << std::endl;
	const std::string _temp = p.has_placed ? "true" : "false";
	output << "Player Has Placed?: " << _temp << std::endl;

	return output;
}

/**
 * Struct for managing the config file
 */
struct Config
{
	std::string prefab_filepath;
	bool ask_for_confirmation;
	bool update_leaderboard;
	bool format_leaderboard;
	std::string leaderboard_message_filepath;
	std::string participants_input_file;
	std::string participants_output_file;
	std::string stat_storage_input_file;
	std::string stat_storage_output_file;
	std::string date;
};

struct Prefab
{
	int weight;
	std::vector<double> ratio;
	int standard_deviation;
	double average_elo;
};

/**
 * Function for parsing a Player struct to a .json file
 * @param j The json file
 * @param p The player struct
 */
void to_json(json& j, const Player& p)
{
	j = json
	{
		{"discord_id", p.discord_id},
		{"elo", p.elo},
		{"placement", p.placement},
		{"has_placed", p.has_placed}
	};
}

/**
 * Function for parsing the .json data to a Player object
 * @param j The json file
 * @param p The player struct
 */
void from_json(const json& j, Player& p)
{
	j.at("discord_id").get_to(p.discord_id);
	j.at("elo").get_to(p.elo);
	j.at("placement").get_to(p.placement);
	j.at("has_placed").get_to(p.has_placed);
}

/**
 * Function for parsing .json data to a Config object
 * @param j The json file
 * @param c The config struct
 */
void from_json(const json& j, Config& c)
{
	j.at("prefab_filepath").get_to(c.prefab_filepath);
	j.at("ask_for_confirmation").get_to(c.ask_for_confirmation);
	j.at("update_leaderboard").get_to(c.update_leaderboard);
	j.at("format_leaderboard").get_to(c.format_leaderboard);
	j.at("leaderboard_message_filepath").get_to(c.leaderboard_message_filepath);
	j.at("participants_input_file").get_to(c.participants_input_file);
	j.at("participants_output_file").get_to(c.participants_output_file);
	j.at("stat_storage_input_file").get_to(c.stat_storage_input_file);
	j.at("stat_storage_output_file").get_to(c.stat_storage_output_file);
	j.at("date").get_to(c.date);
}

/**
 * Function for parsing .json data to a Prefab object
 * @param j The json file
 * @param p The prefab struct
 */
void from_json(const json& j, Prefab& p)
{
	j.at("weight").get_to(p.weight);
	j.at("ratio").get_to(p.ratio);
	j.at("standard_deviation").get_to(p.standard_deviation);
	j.at("average_elo").get_to(p.average_elo);
}

/**
 * Function for giving out all the elo accordingly
 * @param particpants The vector of all the participants
 * @param config The configuration file data
 * @param prefab The prefab you are using
 */
void split_pot(std::vector<Player>& participants, const Config& config, Prefab& prefab)
{
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
	} //NOTE: fuck you if you judge how many indents there are

	if(draw)
	{
		const double _sum = prefab.ratio[0] + prefab.ratio[1];
		prefab.ratio[0] = _sum * 0.5;
		prefab.ratio[1] = _sum * 0.5;
	}

	std::vector<Player> winners;
	for(auto& p : participants)
	{
		if(p.placement > static_cast<char>(prefab.ratio.size()) || p.placement < 1)	
			continue;

		if(p.has_placed)
			p.elo -= prefab.weight;
		else
			p.has_placed = true;
		winners.push_back(p);
	}
	
	for(auto& w : winners)
	{
		if(w.placement == 1 && !draw)
			continue;
		w.elo -= prefab.weight;
	}

	// NOTE: standard_deviation or average_elo may never be changed, but in the original source it was not hardcoded
	std::vector<double> eloGain;
	for(unsigned char i = 0; i < static_cast<unsigned char>(winners.size()); ++i)
	{
		eloGain.push_back(prefab.weight * prefab.ratio[i]);
		for(unsigned char j = 0; j < static_cast<unsigned char>(participants.size() + draw - 1); ++j)
			winners[i].elo += (eloGain[i] / (pow(2, (winners[i].elo - prefab.average_elo) / prefab.standard_deviation))); 
	}

	for(auto& p : participants)
	{
		if((p.placement > static_cast<unsigned char>(prefab.ratio.size()) || p.placement < 1) && !p.has_placed)
			continue;
		
		for(const auto& w : winners)
		{
			if(p.placement == w.placement)
			{
				// NOTE: round the new elo so there are no discrepancies between this program and the original
				p.elo = round(w.elo);
			}
		}
	}
}

/**
 * Function to swap players, only used in sort method
 * @param a First player to swap
 * @param b Second player to swap
 */
void swap(Player* a, Player* b)
{
	Player _a = *a;
	*a = *b;
	*b = _a;
}

/**
 * Method used for sorting the Players by elo,
 * Only used for formatting the leaderboard discord message
 * @param player_list The vector of players to be sorted
 */
void sort(std::vector<Player>& player_list) {
	bool swapped;
	do
	{
		swapped = false;
		for(size_t i = 0; i < player_list.size() - 1; ++i)
		{
			if(player_list[i].elo > player_list[i + 1].elo)
			{
				swap(&player_list[i], &player_list[i + 1]);
				swapped = true;
			}
		}
	} while(swapped);
	// I don't know enough about sorting to know,
	// but I need to reverse the output
	std::reverse(player_list.begin(), player_list.end());
}


/**
 * Function for asking confirmation via console
 * @param text The text to be displayed
 * @return The response from the user
 */
bool confirm_text(const std::string& text)
{
	std::cout << "\n\n" << text;
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
json parse_from_json(const std::string& filepath)
{
	std::ifstream i(filepath, std::ios::in);
	json j;
	i >> j;
	return j;
}

/**
 * Basic function for parsing a json file
 * @param player_list The list of players to be output to the file
 * @param filepath The path to the file
 * @return The parsed json data
 */
void parse_to_json(std::vector<Player>& player_list, const std::string& filepath)
{
	json j(player_list);
	std::ofstream o(filepath);
	o << std::setw(4) << j;
}

const Config config = parse_from_json("config.json");
Prefab prefab = parse_from_json(config.prefab_filepath);

std::vector<Player> participants = parse_from_json(config.participants_input_file);
std::vector<Player> placed_players = parse_from_json(config.stat_storage_input_file);

int main()
{
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
	split_pot(participants, config, prefab);

	if(config.ask_for_confirmation)
	{
		std::cout << std::endl << std::endl << std::endl;
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

	// Update the data to the output json file(s)
	parse_to_json(participants, config.participants_output_file);

	if(config.update_leaderboard)
	{
		for(const auto& p1 : participants)
		{
			bool _added = false;
			for(auto& p2 : placed_players)
			{
				if(p1.discord_id == p2.discord_id)
				{
					p2 = p1;
					_added = true;
				}
			}
			if(!_added && p1.has_placed)
			{
				placed_players.push_back(p1);
			}
		}

		// Bubble sort the placed_players vector by their elo
		sort(placed_players);

		// There is DEFINITELY a better way of doing this
		// rather than starting another for loop
		for(size_t i = 0; i < placed_players.size(); ++i)
		{
			placed_players[i].placement = i + 1;
		}

		// This is suboptimal, I could care less
		// NOTE: I might delete this if ad32 goes against tied placements
		{
			for(auto& p1 : placed_players)
			{
				for(auto& p2 : placed_players)
				{
					if(p1.elo == p2.elo)
					{
						p2.placement = p1.placement;
					}
				}
			}
		}

		if(config.ask_for_confirmation)
		{
			std::cout << std::endl << std::endl << std::endl;
			for(const auto& p : placed_players)
			{
				std::cout << p << std::endl;
			}

			// Ask for confirmation before updating the data to the stat storage output json file
			if(!confirm_text("Update total ELO? [y/n]: "))
			{
				return 0;
			}
		}

		// Update the elo leaderboard json file
		parse_to_json(placed_players, config.stat_storage_output_file);
	}

	if(config.format_leaderboard)
	{
		// Format the leaderboard
		std::ofstream o(config.leaderboard_message_filepath);
		o << "ELO Leaderboard as of " << config.date << ":\n";
		for(const auto& p : placed_players)
		{
			o << p.placement << ". <@" << p.discord_id << "> - " << p.elo << std::endl;
		}
	}

	return 0;
}
