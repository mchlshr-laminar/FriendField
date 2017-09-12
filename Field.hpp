#ifndef __FIELD_H_
#define __FIELD_H_

#include "SocialNode.hpp"
#include "Visualizer.hpp"
#include <vector>
#include <set>

class Field
{
  private:
  std::vector<SocialNode*> nodeList;
  Visualizer* movieMaker;
  
  int exploreNeighborhood(SocialNode* source, std::set<SocialNode*>& members, int remaining_dist);
  
  public:
  Field(int nodeCount, int maxFriends);
  ~Field();
  
  int nodeCount();
  SocialNode* getNode(int index);
  void run(bool storeData);
  bool tryNode(SocialNode* node);
  
  double computeFriendshipUtility(SocialNode* node1, SocialNode* node2);
  double computeFriendshipUtility(int index1, int index2);
  
  double computeAvgNeighborhoodSize(int maxDist);
  double computeClusteringCoefficient();
  double computeAverageUtility();
  double computeFractionalOptimumUtility();
  int findMaxVertexDegree();
};

#endif

