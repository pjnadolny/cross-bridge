/*
This C++ program solves the following problem:

--- Begin Problem Description ---

Four people come to a river in the night. There is a narrow bridge, but it can
only hold two people at a time. They have one torch and, because it's night,
the torch has to be used when crossing the bridge. Person A can cross the
bridge in 1 minute, B in 2 minutes, C in 5 minutes, and D in 10 minutes. When
two people cross the bridge together, they must move at the slower person's
pace.

Write a program in C++ that determines the fastest time they can cross the
bridge. The entry to the program will be a yaml file that describes the speeds
of each Person. In this situation there are 4 people but your program should
assume any number of people can be passed in.

You will be judged on the following:

1. Strategy(s) - there are several ways to solve the problem, you can provide
more than one. The goal is to show us how you think.

2. Architecture - we want to see how well you architect solutions to complex
problems

3. Standards and best practices

4. Explanation - as a C++ thought leader in the organization you will be
working with and mentoring other engineers. How well you can describe and
explain your solution is very important.

Please provide a link to your github repo with your solution.

--- End Problem Description ---

--- Begin Problem Discussion ---

A brute force approach would be to compute every permutation of pairs of people
crossing the bridge and one of them returning.   If the number of people is N,
I estimate the brute force approach to be O(N!) because the number of
permutations of N items is N! (N factorial).  This blows up quickly as N gets
bigger and is very computationally intensive.  This solution is NOT implemented
here.

The Naive Method:  After examining some examples where N=3 or N=4, it can seem
(wrongly) that the solution is to always pair a person with the overall fastest
person, which minimizes all the return journeys.  In addition, it doesn't matter
what order the pairs cross the bridge, so no sorting is required. This algorithm
runs in O(N), taking one pass through all the people to find the fastest person,
then another pass to send the pairs across the bridge.  However, it doesn't
always yield the fastest time.  This is implemented by Bridge::crossNaively()

The Shielding Method:  The optimal solution is to let the slowest and second
slowest cross together, then send back one of the fastest people.  This
"shields" the time of the second slowest person.  This isn't always possible if
the fastest person isn't already on the other side, or if the second fastest and
second slowest times are too close.  In that case, use the naive strategy of
pairing the slowest with the fastest, then the second slowest with the fastest.
After the two slowest people are across, start over with the remaining people.
This strategy requires sorting the people and then making a pass through the
people, two at a time.  This is about NlogN + N which is O(NlogN), much better
than the brute force approach.  This is implemented by Bridge::crossOptimally()

Assumptions:

1. If there are no people, the total speed is 0.
2. If there is one person, the total speed is the speed of that person.
3. Multiple people may have the same speed.
4. The speeds are positive integers.
5. The total speed will be an integer that's less than MAXINT.
6. It doesn't matter if people's names are duplicated.

Implementation summary:

This C++11 code depends on the yaml-cpp library.

There are three Classes:

1. The Arguments class is used to read command line arguments.

2. The Person class is used to store information about each person waiting to
cross the bridge.

3. The Bridge class contains a vector of people waiting to cross the bridge, as
well as functions to implement each crossing method: Naive and Shielding.  There
is also a function to read a YAML file of people into a vector.

--- End Problem Discussion ---

Name:    cross-bridge.cpp
Author:  Paul J. Nadolny
(c) 2019

Date        Ver  Comments
2019 Jun 08 0.1  First version.

To compile on macOS High Sierra 10.13.6:
% export CPATH=~/homebrew/Cellar/yaml-cpp/0.6.2_1/include/
% export LIBRARY_PATH=~/homebrew/Cellar/yaml-cpp/0.6.2_1/lib
% g++ -std=c++11 -o cross-bridge cross-bridge.cpp -lyaml-cpp

To run:
% ./cross-bridge --people people.yaml

*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <getopt.h>

#include <yaml-cpp/yaml.h>

// ---------------------------------------------------------------------------
//                             Constants
// ---------------------------------------------------------------------------
const int DEBUG = 0;

// ---------------------------------------------------------------------------
//                             Global Variables
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//                             Forward Declarations
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
//                             Classes
// ---------------------------------------------------------------------------

class Arguments
{
  public:
    bool help;
		bool abort;
    std::string progName;
    std::string peopleFilename;

  private:
    std::istringstream optargStream;

  public:
    Arguments() : help(false), abort(false), progName(""), peopleFilename("")
    {
    }

		void printHelp()
		{
			std::cout << "Usage: " << progName << " --people <filename> [--help]" << std::endl;
		} // end Arguments::printHelp()

    // -----------------------------------------------------------------------
    void getArgs(int argc, char * argv[])
    {
      const char * shortopts = "h";
      struct option longopts[] =
      {
				// { const char* name, int has_arg, int* flag, int val }
				// Here is how to set a flag
				//{"verbose", no_argument,       &verbose_flag, 1},
				//{"brief",   no_argument,       &verbose_flag, 0},

      	{"people",       required_argument, nullptr, 'p'},
				{"help",         no_argument,       nullptr, 'h'},
				{nullptr,        0,                 nullptr, 0  }
      };
      int longoptsindex = 0;
      int opt;
      //opterr = 0; // supress error message from getopt_long

			optargStream.str(argv[0]);
			optargStream >> progName;

			if (DEBUG==1)
			{
      	int i;
      	for (i=0; i<argc; i++)
      		{
						std::cout << "Arg " << i << ": " << argv[i] << std::endl;
      		}
			}

			// GNU getopt_long() side effects: optarg, optind, opterr, optopt
			// Note: it returns 0 if it set a flag
			opt = getopt_long(argc, argv, shortopts, longopts, &longoptsindex);

			while (opt != -1)
			{
				optargStream.clear();

			  switch (opt)
			  {
			  case 0:
			    // If this option set a flag, do nothing else now.
			    if (longopts[longoptsindex].flag != 0)
			    {
			      break;
			    }
			    std::cout << "option " << longopts[longoptsindex].name;
			    if (optarg)
			    {
			      std::cout << " with arg " << optarg;
			    }
			    std::cout << std::endl;
			    break;

        case 'p':
          if (DEBUG==1) { std::cout << "option --poeple with value " << optarg << std::endl; }
          optargStream.str(optarg);
          optargStream >> peopleFilename;
          break;

			  case 'h':
					if (DEBUG==1) { std::cout << "option --help" << std::endl; }
					help = true;
					abort = true;
					break;

				case '?':
					// getopt_long already printed an error message.
					abort = true;
			    break;

			  case ':':
			    // getopt_long already printed an error message.
					abort = true;
			    break;

			  default:
			    std::cout << "Unexpected option: " << opt << std::endl;
			  } // end switch

				opt = getopt_long(argc, argv, shortopts, longopts, &longoptsindex);

			} // end while

			// Process any remaining command line arguments (not options)
			if (optind < argc)
			{
				abort = true;
	    	std::cout << "Error: unrecognized arguments: ";
				int i = optind;
		    while (i < argc)
		    {
		      std::cout << argv[i] << " ";
					i++;
		    }
		    std::cout << std::endl;
			}

    } // end Arguments::getArgs()

}; // end class Arguments


// This class represents the info about a person.
// The name and speed members are private, and have get and set functions.
// There is a member function to print a Person.
class Person
{
public:
  Person() : name(""), speed(0)
  {
  }

  Person(std::string n, int s) : name(n), speed(s)
  {
  }

  // This operator is for sorting
  bool operator<(const Person& other) const
  {
    return (speed < other.speed);
  }

  void print(std::ostream& os)
  {
    // Print a Person in this format:  (Fred,12)
    os << "(" << name << "," << speed << ")";
  }

  void setName(std::string n)
  {
    name = n;
  }

  void setSpeed(int s)
  {
    speed = s;
  }

  std::string getName()
  {
    return name;
  }

  int getSpeed()
  {
    return speed;
  }

private:
  std::string name;
  int speed; // the time to cross the bridge, in minutes
};  // end class Person

// helper function to print Person within a stream
std::ostream& operator<<(std::ostream& os, Person& p)
{
  p.print(os);
  return os;
};


// The Bridge class contains member functions to:
// - read the yaml file into a private vector of people
// - the Naive method to compute the shortest crossing time
// - the Shielding method to compute the shortest crossing time
// There is a private vector of waiting people.
class Bridge
{
public:
  Bridge() : waitingPeople()
  {
  }

  // Parse the yaml, put the resulting people into the waiting people vector
  // Isolate the file operations and yaml parsing in one function
  void readPeopleFile(std::string filename)
  {
    // The format of the yaml file is:
    // people:
    //   - name: A
    //     speed: 1
    //   - name: B
    //     speed: 2

    // Use the yaml-cpp C++ parser to parse the yaml file into a Node.
    YAML::Node peopleYAML = YAML::LoadFile(filename);

    if (DEBUG==1) {std::cout << "people:" << std::endl << peopleYAML["people"] << std::endl;}

    std::cout << std::endl;
    if (peopleYAML["people"].size() > 0)
    {
      std::cout << "List of all people:" << std::endl;
    }
    else
    {
      std::cout << "No people found in YAML input file" << std::endl;
    }
    for (int i=0; i<peopleYAML["people"].size(); i++)
    {
      std::cout << "Person " << i << " -  Name: " << peopleYAML["people"][i]["name"] << "  Speed: " << peopleYAML["people"][i]["speed"] << std::endl;

      // Add each person to the vector
      Person p;
      p.setName( peopleYAML["people"][i]["name"].as<std::string>() );
      p.setSpeed( peopleYAML["people"][i]["speed"].as<int>() );
      waitingPeople.emplace_back(p);
    }

  } // end Bridge::readPeopleFile()


  // Given a vector of people, compute the optimal minimum speed
  // for them all to cross the bridge.
  // This implements the Shielding Method.
  int crossOptimally()
  {
    int totalSpeed = 0;

    // Sort the people, fastest to slowest
    std::sort( waitingPeople.begin(), waitingPeople.end() );

    std::cout << std::endl;
    std::cout << "Optimal sequence of bridge crossings:" << std::endl;

    // Keep sending the two slowest people over the bridge,
    // as long as there are at least 4 total people left.
    while (waitingPeople.size() >= 4)
    {
      int totalShielding, totalNaive;
      int n = waitingPeople.size();

      // See how long it would take using each method

      // The Shielding Method: the two slowest people go together
      totalShielding = waitingPeople[1].getSpeed() +   // send the two fastest
                       waitingPeople[0].getSpeed() +   // the fastest returns
                       waitingPeople[n-1].getSpeed() + // send the two slowest
                       waitingPeople[1].getSpeed();    // second fastest returns

      // The Naive Method: always pair with the fastest person
      totalNaive = waitingPeople[n-1].getSpeed() + // slowest with fastest
                   waitingPeople[0].getSpeed() +   // the fastest returns
                   waitingPeople[n-2].getSpeed() + // next slowest with fastest
                   waitingPeople[0].getSpeed();    // the fastest returns

      if (totalShielding < totalNaive)
      {
        // Use the Shielding Method
        std::cout << waitingPeople[1] << " and " << waitingPeople[0] << " cross" << std::endl;
        std::cout << waitingPeople[0] << " returns" << std::endl;
        std::cout << waitingPeople[n-1] << " and " << waitingPeople[n-2] << " cross" << std::endl;
        std::cout << waitingPeople[1] << " returns" << std::endl;
        totalSpeed += totalShielding;
      }
      else
      {
        // Use the Naive Method
        std::cout << waitingPeople[n-1] << " and " << waitingPeople[0] << " cross" << std::endl;
        std::cout << waitingPeople[0] << " returns" << std::endl;
        std::cout << waitingPeople[n-2] << " and " << waitingPeople[0] << " cross" << std::endl;
        std::cout << waitingPeople[0] << " returns" << std::endl;
        totalSpeed += totalNaive;
      }
      // the two slowest people are now across, so remove them from the vector
      waitingPeople.pop_back();
      waitingPeople.pop_back();
    }

    // Handle the cases where there are 0 to 3 people left
    if (waitingPeople.size() == 0)
    {
      totalSpeed += 0;
    }
    else if (waitingPeople.size() == 1)
    {
      std::cout << waitingPeople[0] << " crosses" << std::endl;
      totalSpeed += waitingPeople[0].getSpeed();
      waitingPeople.pop_back();
    }
    else if (waitingPeople.size() == 2)
    {
      std::cout << waitingPeople[1] << " and " << waitingPeople[0] << " cross" << std::endl;
      totalSpeed += waitingPeople[1].getSpeed();
      waitingPeople.pop_back();
      waitingPeople.pop_back();
    }
    else if (waitingPeople.size() == 3)
    {
      std::cout << waitingPeople[2] << " and " << waitingPeople[0] << " cross" << std::endl;
      std::cout << waitingPeople[0] << " returns" << std::endl;
      std::cout << waitingPeople[1] << " and " << waitingPeople[0] << " cross" << std::endl;
      totalSpeed += waitingPeople[0].getSpeed() + waitingPeople[1].getSpeed() + waitingPeople[2].getSpeed();
      waitingPeople.pop_back();
      waitingPeople.pop_back();
      waitingPeople.pop_back();
    }

    return totalSpeed;
  } // end Bridge::crossOptimally()


  // Given a vector of people, compute the minimum speed
  // for them all to cross the bridge.
  // This implements the (incorrect) Naive Method.
  int crossNaively()
  {
    int totalSpeed = 0;

    // The Naive Method is to pair each person with the overall fastest person,
    // then send that fastest person back with the torch to get another
    // person.  This sometimes yields the fastest overall time, but not always.

    // If there are no people:
    if (waitingPeople.size() == 0)
    {
      return 0;
    }

    // If there is only one person:
    if (waitingPeople.size() == 1)
    {
      return waitingPeople[0].getSpeed();
    }

    // Find the fastest overall person
    // If two or more people have the fastest speed, only one will be picked
    // (Could have made this a member function, but decided to leave it here)
    Person fastest;
    int fastestindex = -1;

    fastest.setName("maxint");
    fastest.setSpeed( std::numeric_limits<int>::max() );
    for (int i=0; i<waitingPeople.size(); i++)
    {
      if ( waitingPeople[i].getSpeed() < fastest.getSpeed() )
      {
        fastest.setName( waitingPeople[i].getName() );
        fastest.setSpeed( waitingPeople[i].getSpeed() );
        fastestindex = i;
      }
    }

    std::cout << std::endl;
    std::cout << "Fastest overall person: " << fastest << std::endl;

    // Note: I could have saved time (namely one fewer pass through all the
    // people) by just processing vector and skipping the fastest person,
    // but I'm using a queue to make the last loop a bit more understandable.

    // Put every person into a queue, except the fastest
    std::queue<Person> q;
    for (int i=0; i<waitingPeople.size(); i++)
    {
      if (i != fastestindex)
      {
        Person p;
        p.setName( waitingPeople[i].getName() );
        p.setSpeed( waitingPeople[i].getSpeed() );
        q.emplace(p);
      }
    }

    // Process the queue
    std::cout << std::endl;
    std::cout << "Naive sequence of bridge crossings:" << std::endl;
    while (q.size() > 0)
    {
      // Compute the crossing time of the next pair.
      // Note that the slower of the pair is always the person on the queue,
      // because each person on the queue is paired with the fastest overall
      // person.
      totalSpeed += q.front().getSpeed(); // add the slower speed of the pair
      std::cout << q.front() << " and " << fastest << " cross" << std::endl;
      q.pop();

      // If anyone is left in the queue,
      // send the fastest person back over the bridge.
      if (q.size() > 0)
      {
        std::cout << fastest << " returns" << std::endl;
        totalSpeed += fastest.getSpeed(); // the faster person returns
      }
    }

    return totalSpeed;
  } // end Bridge::crossNaively()

private:
  std::vector<Person> waitingPeople;

}; // end class Bridge


// --------------------------------------------------------------------------
//                             Functions
// --------------------------------------------------------------------------


// -------------------------------------------------------------------------
//                             Main Program
// -------------------------------------------------------------------------
int main(int argc, char * argv[]) {
  int total = 0;

  std::cout << "Running..." << std::endl;

  // Process the command line arguments

  Arguments args;
  args.getArgs(argc, argv);
	if (args.help)
	{
		args.printHelp();
	}
	if (args.abort)
	{
		return 0;
	}
  if (args.peopleFilename == "")
  {
    std::cout << args.progName << ": ERROR: Missing option peopleFile" << std::endl;
    return 0;
  }
  if (DEBUG==1) {std::cout << "peopleFilename=<" << args.peopleFilename << ">" << std::endl;}

  // We have the arguments, now do the real stuff

  Bridge narrowBridge;

  narrowBridge.readPeopleFile(args.peopleFilename);

  // For comparison, do both the Naive and Shielding methods

  total = narrowBridge.crossNaively();
  std::cout << std::endl;
  std::cout << "The naive fastest total time is: " << total << std::endl;

  total = narrowBridge.crossOptimally();
  std::cout << std::endl;
  std::cout << "The optimal fastest total time is: " << total << std::endl;

  return 0;
}
