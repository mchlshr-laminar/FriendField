#include "Field.hpp"
#include "DataExporter.hpp"
#include "SocialNode.hpp"
#include "CoordinateRandomizer.hpp"
#include "NeighborhoodFinder.hpp"
#include <cstdlib>
#include <time.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <string>

using std::vector;

void parse_arguments(char** args, int num_remaining);

int main(int nargs, char** args)
{
  srand(time(NULL));
  parse_arguments(args+1, nargs-1);
  
  std::string filename = "data/";
  for(int i = 1; i < nargs; i++)
  {
    filename += args[i];
    filename += '_';
  }
  filename += "data.txt";
  
  for(int i = 0; i < 100; i++)
  {
    Field friendField(100, 3);
    int t1 = SocialNode::utilityMode;
    int t2 = NeighborhoodFinder::getReachabilityMode();
    
    //Set up initial state
    SocialNode::setUtilityMode(SocialNode::IMMEDIATE_UTILITY);
    NeighborhoodFinder::setReachabilityMode(NeighborhoodFinder::GLOBAL);
    friendField.run(false);
    
    //Run the simulation
    SocialNode::setUtilityMode(t1);
    NeighborhoodFinder::setReachabilityMode(t2);
    friendField.run(true);
  }
  DataExporter::getExporter(filename)->exportData();
  
  /*vector<int> max_degrees;
  for(int p = atoi(args[1]); p <= atoi(args[2]); p++)
  {
    SocialNode::levelTwoAlpha = 1.0/pow(2.0, (double)p);
    for(int i = 0; i < 100; i++)
    {
      Field friendField(100, 3);
      friendField.run();
      max_degrees.push_back(friendField.findMaxVertexDegree());
    }
    DataExporter::getExporter()->addInitialAvgNeighborhoodSize(DataExporter::get_avg(max_degrees));
    max_degrees.clear();
    std::cout << "power -" << p << " done" << std::endl;
  }
  std::string name = "avg_max_deg_for_alpha_";
  name += args[1];
  name += "_to_";
  name += args[2];
  name += ".txt";
  DataExporter::getExporter(name)->exportData();*/
  
  return 0;
}

void parse_arguments(char** args, int num_remaining)
{
  if(num_remaining == 0) return;
  if(strcmp(args[0], "uniform") == 0)
    CoordinateRandomizer::setDistributionMode(CoordinateRandomizer::UNIFORM);
  else if(strcmp(args[0], "doublenormal") == 0)
    CoordinateRandomizer::setDistributionMode(CoordinateRandomizer::DOUBLE_NORMAL);
  else if(strcmp(args[0], "global") == 0)
    NeighborhoodFinder::setReachabilityMode(NeighborhoodFinder::GLOBAL);
  else if(strcmp(args[0], "twoaway") == 0)
    NeighborhoodFinder::setReachabilityMode(NeighborhoodFinder::TWO_AWAY);
  else if(strcmp(args[0], "asymm") == 0)
    SocialNode::setFriendshipMode(SocialNode::ASYMMETRIC_FRIENDSHIP);
  else if(strcmp(args[0], "symm") == 0)
    SocialNode::setFriendshipMode(SocialNode::SYMMETRIC_FRIENDSHIP);
  else if(strcmp(args[0], "inverse") == 0)
    SocialNode::setUtilityFunction(SocialNode::INVERSE);
  else if(strcmp(args[0], "exponential") == 0)
    SocialNode::setUtilityFunction(SocialNode::EXPONENTIAL);
  else if(strcmp(args[0], "linear") == 0)
    SocialNode::setUtilityFunction(SocialNode::LINEAR);
  else if(strcmp(args[0], "immediate") == 0)
    SocialNode::setUtilityMode(SocialNode::IMMEDIATE_UTILITY);
  else if(strcmp(args[0], "fof") == 0)
    SocialNode::setUtilityMode(SocialNode::FRIENDS_OF_FRIENDS);
  else if(strcmp(args[0], "mlvl_const") == 0)
    SocialNode::setUtilityMode(SocialNode::MULTILEVEL_CONSTANT);
  else if(strcmp(args[0], "mlvl_inv") == 0)
    SocialNode::setUtilityMode(SocialNode::MULTILEVEL_INVERSE);
  else if(strcmp(args[0], "mlvl_exp") == 0)
    SocialNode::setUtilityMode(SocialNode::MULTILEVEL_EXPONENTIAL);
  else if(strcmp(args[0], "friendlevels") == 0)
  {
    if(num_remaining < 2) return;
    SocialNode::numFriendLevels = atoi(args[1]);
    parse_arguments(args+2, num_remaining-2);
    return;
  }
  else if(atoi(args[0]) != 0)
    srand(atoi(args[0]));
  else std::cout << "Unrecognized argument: " << args[0] << '\n';
  parse_arguments(args+1, num_remaining-1);
}
