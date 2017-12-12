#include "Field.hpp"
#include "FriendRandomizer.hpp"
#include "CoordinateRandomizer.hpp"
#include "NeighborhoodFinder.hpp"
#include "DataExporter.hpp"
#include "OptFinder.hpp"
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <utility>

using std::vector;
using std::set;

Field::Field(int nodeCount, int maxFriends)
{
  for(int i = 0; i < nodeCount; i++)
  {
    double x = CoordinateRandomizer::getCoordinate();
    double y = CoordinateRandomizer::getCoordinate();
    nodeList.push_back(new SocialNode(maxFriends, x, y));
  }
  FriendRandomizer initializer(nodeList, maxFriends);
  initializer.makeMatches();
  
  movieMaker = new Visualizer(nodeList);
  movieMaker->addMovieFrame(nodeList);
}

Field::~Field()
{
  for(unsigned int i = 0; i < nodeList.size(); i++)
    delete nodeList[i];
  delete movieMaker;
}

int Field::nodeCount()
{ return nodeList.size(); }

SocialNode* Field::getNode(int index)
{
  if(index >= 0 && nodeList.size() > (unsigned)index)
    return nodeList[index];
  else
    return NULL;
}

void Field::run(bool storeData)
{
  vector<SocialNode*> permutation = nodeList;
  for(unsigned int i = 0; i < permutation.size()-1; i++)
  {
    int j = rand()%(permutation.size()-i-1)+i+1;
    //int j = rand()%(permutation.size()-i)+i;
    SocialNode* temp = permutation[i];
    permutation[i] = permutation[j];
    permutation[j] = temp;
  }
  
  int run = 0;
  movieMaker->createImage("initial_state.png", nodeList);
  if(storeData)
  {
    DataExporter::getExporter()->submitData("Initial Clustering Coeff", computeClusteringCoefficient());
    DataExporter::getExporter()->submitData("Initial Utility", computeAverageUtility());
  }
  for(run = 1; run <= 5000; run++)
  {
    bool hasChanged = false;
    for(unsigned int i = 0; i < permutation.size(); i++)
    {
      hasChanged = hasChanged || tryNode(permutation[i]);
    }
    if(!hasChanged) break;
  }
  movieMaker->createImage("final_state.png", nodeList);
  
  
  /*if(run == 5001 && storeData)
    printf("====>DIDN\'T CONVERGE\n");
  else if(storeData)
    printf("%d runs\n", run);*/
  if(storeData)
  {
    DataExporter::getExporter()->submitData("Convergence Time", run);
    DataExporter::getExporter()->submitData("Final Clustering Coeff", computeClusteringCoefficient());
    DataExporter::getExporter()->submitData("Final Utility", computeAverageUtility());
  }
  else
    DataExporter::getExporter()->submitData("3-closest utility", computeAverageUtility());
}

bool Field::tryNode(SocialNode* node)
{
  bool hasChanged = false;
  set<SocialNode*>* allowable_matches = NeighborhoodFinder::getNeighborhood(node, nodeList);
  set<SocialNode*>::iterator match_itr = allowable_matches->begin();
  for(; match_itr != allowable_matches->end(); match_itr++)
  {
    if(*match_itr == node) continue;
    if(SocialNode::considerFriendship(node, *match_itr))
    {
      hasChanged = true;
      movieMaker->addMovieFrame(nodeList);
    }
    //hasChanged = hasChanged || SocialNode::considerFriendship(node, *match_itr);
  }
  delete allowable_matches;
  return hasChanged;
}

/*double Field::computeFriendshipUtility(SocialNode* node1, SocialNode* node2)
{
  double* coord1 = node1->getCoordinates();
  double* coord2 = node2->getCoordinates();
  double dist1 = coord1[0] - coord2[0];
  double dist2 = coord1[1] - coord2[1];
  return 1.0/sqrt(dist1*dist1 + dist2*dist2);
}

double Field::computeFriendshipUtility(int index1, int index2)
{
  SocialNode* node1 = getNode(index1);
  SocialNode* node2 = getNode(index2);
  if(node1 != NULL && node2 != NULL)
    return computeFriendshipUtility(node1, node2);
  else
    return 0.0;
}*/

double Field::computeAvgNeighborhoodSize(int maxDist)
{
  int sum = 0;
  for(unsigned int i = 0; i < nodeList.size(); i++)
  {
    set<SocialNode*> members;
    sum += NeighborhoodFinder::getDistanceNeighborhood(nodeList[i], members, maxDist);
  }
  return ((double)sum)/(nodeList.size());
}

double Field::computeClusteringCoefficient()
{
  int triples = 0;
  int connected_triples = 0;
  for(unsigned int i = 0; i < nodeList.size(); i++)
  {
    set<SocialNode*> neighbors;
    int num = NeighborhoodFinder::getDistanceNeighborhood(nodeList[i], neighbors, 1);
    triples += (num*(num-1))/2;
    for(set<SocialNode*>::iterator itr1 = neighbors.begin(); itr1 != neighbors.end(); itr1++)
    {
      set<SocialNode*>::iterator itr2 = itr1;
      for(itr2++; itr2 != neighbors.end(); itr2++)
        if(SocialNode::areFriends(*itr1, *itr2) || SocialNode::areFriends(*itr2, *itr1))
          connected_triples++;
    }
  }
  return ((double)connected_triples)/((double)triples);
}

double Field::computeAverageUtility()
{
  double sum = 0.0;
  for(unsigned int i = 0; i < nodeList.size(); i++)
    sum += nodeList[i]->computeCurrentUtility();
  return sum/((double)nodeList.size());
}

double Field::computeFractionalOptimumUtility()
{
  OptFinder finder(nodeList);
  return finder.solveOptimum();
}

int Field::findMaxVertexDegree()
{
  int max = 0;
  for(unsigned int i = 0; i < nodeList.size(); i++)
  {
    SocialNode* curr = nodeList[i];
    int deg = curr->getNumFriends() + curr->getPassiveFriends()->size();
    if(deg > max) max = deg;
  }
  return max;
}

/*int Field::exploreNeighborhood(SocialNode* source, set<SocialNode*>& members, int remaining_dist)
{
  int result = 0;
  std::pair<set<SocialNode*>::iterator,bool> retval = members.insert(source);
  if(retval.second) //source has not already been visited (i.e. was inserted successfully)
  {
    result++;
    if(remaining_dist > 0)
      for(int i = 0; i < source->getNumFriends(); i++)
        result += exploreNeighborhood(source->getFriends()[i], members, remaining_dist-1);
  }
  return result;
}*/
