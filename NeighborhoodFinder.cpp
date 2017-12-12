#include "NeighborhoodFinder.hpp"
#include <iostream>
#include <utility>

using std::set;
using std::vector;

int NeighborhoodFinder::reachabilityMode = NeighborhoodFinder::GLOBAL;

void NeighborhoodFinder::setReachabilityMode(int new_mode)
{ reachabilityMode = new_mode; }

int NeighborhoodFinder::getReachabilityMode()
{ return reachabilityMode; }

set<SocialNode*>* NeighborhoodFinder::getNeighborhood(SocialNode* origin, vector<SocialNode*>& availableNodes)
{
  set<SocialNode*>* result = new set<SocialNode*>;
  switch(reachabilityMode)
  {
    case GLOBAL:
      for(unsigned int i = 0; i < availableNodes.size(); i++)
        result->insert(availableNodes[i]);
      break;
    case TWO_AWAY:
      getDistanceNeighborhood(origin, *result, 2);
      break;
    default:
      std::cerr << "Invalid reachability mode." << std::endl;
      delete result;
      result = NULL;
      break;
  }
  return result;
}

int NeighborhoodFinder::getDistanceNeighborhood(SocialNode* origin, set<SocialNode*>& neighbors, int dist)
{
  int result = 0;
  std::pair<set<SocialNode*>::iterator,bool> retval = neighbors.insert(origin);
  if(retval.second) //origin has not already been visited (i.e. was inserted successfully)
  {
    result++;
    if(dist > 0)
    {
      for(int i = 0; i < origin->getNumFriends(); i++)
        result += getDistanceNeighborhood(origin->getFriends()[i], neighbors, dist-1);
        
      set<SocialNode*>* passive = origin->getPassiveFriends();
      for(set<SocialNode*>::iterator itr = passive->begin(); itr != passive->end(); itr++)
        result += getDistanceNeighborhood(*itr, neighbors, dist-1);
    }
  }
  return result;
}
