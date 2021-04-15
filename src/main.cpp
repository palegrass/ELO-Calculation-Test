#include <iostream>
#include <fstream>
#include <list>
#include <cmath>
#include <iomanip>
#include "json.hpp"

using json = nlohmann::json;

struct Player
{
	std::string name = "player";
	float elo = 1500;
	int placement = 0;
};
void to_json(json& j, const Player& p) 
	{
		j = json{{"name", p.name}, {"elo", p.elo}, {"placement", p.placement}};
	}

void from_json(const json& j, Player& p) 
{
	j.at("name").get_to(p.name);
	j.at("elo").get_to(p.elo);
	j.at("placement").get_to(p.placement);
}

void SplitPot(std::list<Player>& participants, int volatility)
{
	for(auto& p : participants)
	{
		p.elo -= 10;
		if(p.placement == 1)
		{
			p.elo += 10;
			int eloGain = 0;
			for(auto& l : participants)
			{
				if(l.placement != 1)
				{
					eloGain += volatility / pow(2, ((p.elo - l.elo) / 200 + 1) / 2);
				}
			}
			p.elo += eloGain;
		}
	}
}

int main(int argc, char* argv[])
{
	std::ifstream i("participants.json", std::ios::in);
	json j;
	i >> j;

	std::list<Player> participants = j;

	int volatility;

	std::cout << "Volatitlity: ";

	std::cin >> volatility;

	for(auto& p : participants)
	{
		std::cout << "Player Name: " << p.name << std::endl;
		std::cout << "Player ELO: " << p.elo << std::endl;
		std::cout << "Player Placement: " << p.placement << std::endl << std::endl;
	}

	system("pause");
	std::cout << std::endl;

	SplitPot(participants, volatility);

	for(auto& p : participants)
	{
		std::cout << "Player Name: " << p.name << std::endl;
		std::cout << "Player ELO: " << p.elo << std::endl;
		std::cout << "Player Placement: " << p.placement << std::endl << std::endl;
	}

	system("pause");

	json pjson(participants);

	std::ofstream o("participants.json");
	o << std::setw(4) << pjson << std::endl;

	return 0;
}