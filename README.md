# ELO-Calculation-Test

An elo calculator that takes in .json files as an input and outputs another .json file with the calculated data that was made for the THC discord server.

## Instructions

First, you should configure the config file to your liking, and note that an output file will be created if one doesn't exist on your configured directory.

Weight - A signed integer defining the volatility of the elo.

Mega - A boolean defining whether the game is a mega THC or not.

Ask for confirmation - A boolean that determines whether the console window will do anything besides updating the elo.

Input file - The directory of the participants input

Output file - The directory of the participants output

## Notes

For a draw scenario, simply set 2 players to both have a placement of 1, the calculator will do the rest.

## Planned Features

If this goes well, I'll probably add a basic prefab system to let the user decide the ratios and placements that are counted in the case of other scenarios than just mega.
