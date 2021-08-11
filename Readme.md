SUMMARY

This is the final project from my Fall 2020 course: Data Structures and Object Oriented Design

In this project, I built a version of Scrabble which supported both human and computer players. The game can be configured to run on any specified dictionary, tile bag, and board specifications (see "config").

While the solution executable was given to us as a reference, we were tasked with creating all the other files following an object-oriented framework laid out for us in the assignment description. For some of these files, starter code and instructions were given to us. Please see: "Object Oriented Layout" for details on how the different objects interact with one another.

The scrabble game was tested for functionality in a wide range of scenarios: the original configuration (english dictionary with standard board) as well as a variety of alternative dictionaries, tile bags, board layouts, and player moves.

ENVIRONMENT

The scrabble game is intended to be run in the Docker virtual environment that we have been using throughout the semester. Details on the environment are in the Repo: "Docker"

INSTRUCTIONS

Ensure you run 'make' to compile the executable if it has not been done so already!

1) To play scrabble using the default configuration, run: './scrabble config/config.txt'

2) To see the sample solution provided, run: './solution config/config.txt'

3) To see the test cases, cd into the tests folder and run "make"
