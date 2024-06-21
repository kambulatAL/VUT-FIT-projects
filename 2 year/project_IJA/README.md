# Pacman

Pac-Man Adventure is a game where players control Pac-Man, 
a yellow character, as he navigates a maze, eating dots and avoiding ghosts. 

The objective is to find the key and leave the maze while avoiding the ghosts that pursue Pac-Man. 
Players must strategically maneuver Pac-Man through the maze to collect score.

## Install

### Maven

Standart way to compile and play the game is to use Maven tool:

mvn package

It should compile the project, create program documentation and create .jar file.
After .jar file is created, in order to launch the game use:

java -jar .\target\pacman-1.0.jar

With compilation, documentation will be created. For documentation was used Javadoc 3.3.2

### Makefile

There is also non-standart way to play the game, which is using Makefile.

If you are using Windows, run:

make -f Makefile

For unix-based systems (We used Ubuntu) run:

make

This should compile and run application right away. Good luck!

## Contributors

Team leader: Oleksandr Turytsia (xturyt00)
Team member: Kambulat Alakaev (xalaka00)