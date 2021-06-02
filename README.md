# ELO-Calculation-Test

An elo calculator that takes in .json files as an input and outputs another .json file with the calculated data that was made for the THC discord server.

## Instructions

First, you should configure the config and prefab file to your liking, and note that an output file will be created if one doesn't exist on your configured directory.

### Config File

Prefab Filepath - The directory of the prefab you want to use.

Ask For Confirmation - A boolean that determines whether the console window will do anything besides updating the elo.

Update Leaderboard - A boolean that determines whether the leaderboard .json file will output anything to the .json output.

Format Leaderboard - A boolean that determines whether the leader .json file will output anything to the leaderboard message filepath.

Leaderboard Filepath - The directory of the formatted leaderboard.

Participants Input File - The directory of the participants input.

Participants Output File - The directory of the participants output.

Leaderboard Input File - The directory of the leaderboard input.

Leaderboard Output File - The directory of the leaderboard output.

Date - The date to be used in the leaderboard formatting.

### Participants/Placed Players File(s)

Elo - This should be pretty self explanatory.

Discord ID - The player's discord id. Note that this field IS required if you want to do anything with the leaderboard files.

Placement - The player's placement in the participant file and their elo ranking in the placed players file.

### Prefab Folder/Files

Weight - An integer defining the volatility of the elo.

Ratio - An array of doubles that suggests how many placements will gain elo and how much they gain.

Standard Deviation - An integer that stores the standard deviation (unsure if this should ever be changed).

Average Elo - A double that stores the average elo (unsure if this should ever be changed).

Negatives - A boolean that determines whether the calculator will output negative numbers for elo. If set to false, negatives won't be outputted

## Notes

For a draw scenario, simply set 2 players to both have a placement of 1, the calculator will do the rest.

The order of players **DOES** matter, it should be by placement in descending order.

I recommended using a different output file than the input so you can prevent the original data from being overwritten.

If you're using Windows, I'm pretty sure running this will give a windows security warning because that has happened with all other files I've ever distributed. If you don't trust me just check the source for yourself, or even build it yourself.

## Planned Features

I also might make it so the order of players does not matter as long as the placements are correct, but probably not anytime soon.
