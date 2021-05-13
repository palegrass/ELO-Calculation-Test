# ELO-Calculation-Test

An elo calculator that takes in .json files as an input and outputs another .json file with the calculated data that was made for the THC discord server.

## Instructions

First, you should configure the config file to your liking, and note that an output file will be created if one doesn't exist on your configured directory.

### Config File

Weight - An integer defining the volatility of the elo.

Standard Deviation - An integer that stores the standard deviation (unsure if this should ever be changed).

Average Elo - A double that stores the average elo (unsure if this should ever be changed).

Mega - A boolean defining whether the game is a mega THC or not.

Ask for confirmation - A boolean that determines whether the console window will do anything besides updating the elo.

Input file - The directory of the participants input

Output file - The directory of the participants output

### Participants File

Elo - This should be pretty self explanatory.

ID - Not really required, but I'd assume it'd be useful to put a Discord UUID here or something like that.

Placement - The player's placement.

Has Placed - A boolean deciding whether the player has placed or not, will only really need to be edited once since the program will add new players the place for you.

## Notes

For a draw scenario, simply set 2 players to both have a placement of 1, the calculator will do the rest.

The order of players **DOES** matter, it should be by placement in descending order.

I recommended using a different output file than the input so you can prevent the original data from being overwritten.

If you're using Windows, I'm pretty sure running this will give a windows security warning because that has happened with all other files I've ever distributed. If you don't trust me just check the source for yourself, or even build it yourself.

## Planned Features

If this goes well, I'll probably add a basic prefab system to let the user decide the ratios and placements that are counted in the case of other scenarios than just mega.

I also might make it so the order of players does not matter as long as the placements are correct, but probably not anytime soon.
