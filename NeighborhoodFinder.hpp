#ifndef __NEIGHBORHOOD_FINDER_H_
#define __NEIGHBORHOOD_FINDER_H_

#include <set>
#include <vector>
#include "SocialNode.hpp"

class NeighborhoodFinder
{
  private:
  static int reachabilityMode;
  
  public:
  const static int GLOBAL = 0, TWO_AWAY = 1;
  
  static void setReachabilityMode(int new_mode);
  static std::set<SocialNode*>* getNeighborhood(SocialNode* origin, std::vector<SocialNode*>& availableNodes);
  
  static int getDistanceNeighborhood(SocialNode* origin, std::set<SocialNode*>& neighbors, int dist);
};

#endif
