#include "SocialNode.hpp"
#include <cstdlib>
#include <cmath>
#include <iostream>

using std::set;
using std::cout;

int SocialNode::friendshipMode = SocialNode::SYMMETRIC_FRIENDSHIP;
int SocialNode::utilityMode = SocialNode::IMMEDIATE_UTILITY;
int SocialNode::utilityFunction = SocialNode::EXPONENTIAL;
double SocialNode::levelTwoAlpha = (1.0/6.0);
int SocialNode::numFriendLevels = 2;

SocialNode::SocialNode(int maxFriends)
{
  coordinates = new double[2];
  for(int i = 0; i < 2; i++)
    coordinates[i] = ((double)rand())/RAND_MAX;
  friends = new SocialNode*[maxFriends];
  for(int i = 0; i < maxFriends; i++)
    friends[i] = NULL;
  currNumFriends = 0;
  this->maxNumFriends = maxFriends;
}

SocialNode::SocialNode(int maxFriends, double x, double y)
{
  coordinates = new double[2];
  coordinates[0] = x;
  coordinates[1] = y;
  friends = new SocialNode*[maxFriends];
  for(int i = 0; i < maxFriends; i++)
    friends[i] = NULL;
  currNumFriends = 0;
  this->maxNumFriends = maxFriends;
}

SocialNode::~SocialNode()
{
  delete coordinates;
  delete friends;
}

double* SocialNode::getCoordinates()
{ return coordinates; }

SocialNode** SocialNode::getFriends()
{ return friends; }

set<SocialNode*>* SocialNode::getPassiveFriends()
{ return &passiveFriends; }

int SocialNode::getNumFriends()
{ return currNumFriends; }

int SocialNode::getMaxFriends()
{ return maxNumFriends; }

bool SocialNode::canAddFriends()
{ return currNumFriends < maxNumFriends; }

int SocialNode::getLowestUtilityFriendIndex()
{
  if(currNumFriends < maxNumFriends) return currNumFriends;
  
  int result = 0;
  double util = computeFriendshipUtility(this, friends[0]);
  for(int i = 1; i < currNumFriends; i++)
  {
    double new_util = computeFriendshipUtility(this, friends[i]);
    if(new_util < util)
    {
      util = new_util;
      result = i;
    }
  }
  return result;
}

double SocialNode::computeCurrentUtility()
{
  double sum = 0.0;
  for(int i = 0; i < currNumFriends; i++)
  {
    sum += computeFriendshipUtility(this, friends[i]);
    if(areFriends(friends[i], this))
      sum += computeReceivedUtility(this, friends[i]);
  }
  for(set<SocialNode*>::iterator itr = passiveFriends.begin(); itr != passiveFriends.end(); itr++)
  {
    if(*itr == this) continue;
    sum += computeReceivedUtility(this, *itr);
  }
  return sum/2.0;
}

bool SocialNode::areFriends(SocialNode* friend1, SocialNode* friend2)
{
  for(int i = 0; i < friend1->currNumFriends; i++)
    if(friend1->friends[i] == friend2)
      return true;
  return false;
}

bool SocialNode::addFriendship(SocialNode* friend1, SocialNode* friend2)
{
  return friend1->addFriend(friend2, friendshipMode == SYMMETRIC_FRIENDSHIP);
}

bool SocialNode::addFriend(SocialNode* other, bool shouldReciprocate)
{
  if(other == this || areFriends(this, other) || currNumFriends >= maxNumFriends)
    return false;
  if(!shouldReciprocate || other->addFriend(this, false))
  {
    friends[currNumFriends] = other;
    currNumFriends++;
    
    if(friendshipMode == ASYMMETRIC_FRIENDSHIP)
    {
      removePassiveFriend(other);
      other->addPassiveFriend(this);
    }
    
    return true;
  }
  else return false;
}

bool SocialNode::addPassiveFriend(SocialNode* other)
{
  if(other == this || areFriends(this, other)) return false;
  
  return passiveFriends.insert(other).second;
}

bool SocialNode::removeFriendship(SocialNode* friend1, SocialNode* friend2)
{
  if(friend1 == friend2 || !areFriends(friend1, friend2)) return false;
  
  return friend1->removeFriend(friend2, friendshipMode == SYMMETRIC_FRIENDSHIP);
}

bool SocialNode::removeFriend(SocialNode* other, bool shouldReciprocate)
{
  if(other == NULL) return false;
  
  int i = 0, offset = 0;
  while(i+offset < currNumFriends)
  {
    friends[i] = friends[i+offset];
    if(friends[i] == other)
      offset++;
    else
      i++;
  }
  while(i < currNumFriends)
  {
    friends[i] = NULL;
    i++;
  }
  currNumFriends -= offset;
  
  if(friendshipMode == ASYMMETRIC_FRIENDSHIP && offset > 0)
    other->removePassiveFriend(this);
    
  if(shouldReciprocate) return (offset > 0) && other->removeFriend(this, false);
  else return offset > 0;
}

bool SocialNode::removePassiveFriend(SocialNode* other)
{
  return passiveFriends.erase(other) > 0;
}

bool SocialNode::considerFriendship(SocialNode* friend1, SocialNode* friend2)
{
  return friend1->considerFriend(friend2, friendshipMode == SYMMETRIC_FRIENDSHIP);
}

bool SocialNode::considerFriend(SocialNode* other, bool shouldReciprocate)
{
  if(other == this || areFriends(this, other)) return false;
  
  int least_friend = getLowestUtilityFriendIndex();
  double least_util = computeFriendshipUtility(this, friends[least_friend]);
  double new_util = computeFriendshipUtility(this, other);
  
  if(new_util > least_util && (!shouldReciprocate || other->considerFriend(this, false)))
  {
    removeFriendship(this, friends[least_friend]);
    addFriend(other, false); //Reciprocation of adding will have been done in reciprocal consideration.
    return true;
  }
  else
  {
    return false;
  }
}


//Computes the utility that would be gained or lost by node1 if node2 were added
//or removed as a friend.
double SocialNode::computeFriendshipUtility(SocialNode* node1, SocialNode* node2) //CHANGE
{
  if(node1 == NULL || node2 == NULL || node1 == node2) return 0.0;
  
  switch(utilityMode)
  {
    case IMMEDIATE_UTILITY:
      return computeImmediateUtility(node1, node2);
      break;
    case FRIENDS_OF_FRIENDS:
      return computeFriendsOfFriendsUtility(node1, node2);
      break;
    case MULTILEVEL_CONSTANT:
      return computeMultilevelUtility(node1, node2, numFriendLevels-1, 1.0);
      break;
    case MULTILEVEL_EXPONENTIAL:
      return computeMultilevelUtility(node1, node2, numFriendLevels-1, 1.0);
      break;
    case MULTILEVEL_INVERSE:
      return computeMultilevelUtility(node1, node2, numFriendLevels-1, 1.0);
      break;
    default:
      std::cerr << "Invalid utility mode" << std::endl;
      return 0.0;
      break;
  }
}

//Directed version:
double SocialNode::computeReceivedUtility(SocialNode* friend1, SocialNode* friend2)
{
  if(!areFriends(friend2, friend1)) return 0.0;
  
  switch(utilityMode)
  {
    case IMMEDIATE_UTILITY:
      return computeImmediateUtility(friend1, friend2);
      break;
    case FRIENDS_OF_FRIENDS:
      return computeReceivedFriendsOfFriends(friend1, friend2);
      break;
    case MULTILEVEL_CONSTANT:
      return computeReceivedMultilevel(friend1, friend2, numFriendLevels-1, 1.0);
      break;
    case MULTILEVEL_EXPONENTIAL:
      return computeReceivedMultilevel(friend1, friend2, numFriendLevels-1, 1.0);
      break;
    case MULTILEVEL_INVERSE:
      return computeReceivedMultilevel(friend1, friend2, numFriendLevels-1, 1.0);
      break;
    default:
      std::cerr << "Invalid utility mode" << std::endl;
      return 0.0;
      break;
  }
}

double SocialNode::computeReceivedFriendsOfFriends(SocialNode* friend1, SocialNode* friend2)
{
  double sum = computeImmediateUtility(friend1, friend2);
  for(set<SocialNode*>::iterator itr = friend2->passiveFriends.begin(); 
    itr != friend2->passiveFriends.end(); itr++)
  {
    if(*itr == friend1) continue;
    sum += levelTwoAlpha*computeImmediateUtility(*itr, friend1);
  }
  for(int i = 0; i < friend2->currNumFriends; i++)
    if(areFriends(friend2->friends[i], friend2))
      sum += levelTwoAlpha*computeImmediateUtility(friend2->friends[i], friend1);
  return sum;
}

double SocialNode::computeReceivedMultilevel(SocialNode* source, SocialNode* location, int remaining_depth, double alpha)
{
  if(location == source) return 0.0;
  double sum = alpha*computeImmediateUtility(source, location);
  if(remaining_depth > 0)
  {
    double next_alpha = computeAlphaForDepth(numFriendLevels-remaining_depth+1);
    for(set<SocialNode*>::iterator itr = location->passiveFriends.begin(); itr != location->passiveFriends.end(); itr++)
      sum += computeReceivedMultilevel(source, *itr, remaining_depth-1, next_alpha);
    for(int i = 0; i < location->currNumFriends; i++)
      if(areFriends(location->friends[i], location))
        sum += computeReceivedMultilevel(source, location->friends[i], remaining_depth-1, next_alpha);
  }
  return sum;
}

/*double SocialNode::computeFriendsOfFriendsUtility(SocialNode* node1, SocialNode* node2)
{
  double result = computeImmediateUtility(node1, node2);
  if(areFriends(node2, node1) && friendshipMode == ASYMMETRIC_FRIENDSHIP)
  {
    for(int i = 0; i < node2->currNumFriends; i++)
    {
      SocialNode* fr = node2->friends[i];
      //If remote friendship isn't bidirectional, there will be no gain in utility
      //(node1 already has the utility from it):
      if(fr == node1 || !areFriends(fr, node2)) continue; //Remove the areFriends check?
      result += levelTwoAlpha*computeImmediateUtility(node1, fr);
    }
  }
  else
  {
    for(int i = 0; i < node2->currNumFriends; i++)
    {
      result += levelTwoAlpha*computeImmediateUtility(node1, node2->friends[i]);
    }
    set<SocialNode*>* passive = node2->getPassiveFriends();
    for(set<SocialNode*>::iterator itr = passive->begin(); itr != passive->end(); itr++)
    {
      if(*itr == node1) continue;
      result += levelTwoAlpha*computeImmediateUtility(node1, *itr);
    }
  }
  return result;
}*/

//Directional version
double SocialNode::computeFriendsOfFriendsUtility(SocialNode* node1, SocialNode* node2)
{
  //Assumption: path in one direction does not affect utility from a path in the other direction.
  //This means incoming paths (which are uncontrollable) are irrelevant to utility change from
  //adding this friendship.
  double result = computeImmediateUtility(node1, node2);
  for(int i = 0; i < node2->currNumFriends; i++)
  {
    SocialNode* fr = node2->friends[i];
    if(fr == node1) continue;
    result += levelTwoAlpha*computeImmediateUtility(node1, fr);
  }
  return result;  
}

double SocialNode::computeMultilevelUtility(SocialNode* source, SocialNode* location, int remaining_depth, double alpha)
{
  //Will not count paths that go through the source.
  if(location == source) return 0.0; //location will initially be node2 in computeFriendshipUtility
  double sum = alpha*computeImmediateUtility(source, location);
  if(remaining_depth > 0)
  {
    double next_alpha = computeAlphaForDepth(numFriendLevels-remaining_depth+1);
    for(int i = 0; i < location->currNumFriends; i++)
      sum += computeMultilevelUtility(source, location->friends[i], remaining_depth-1, next_alpha);
  }
  //No set of visited nodes as multiple paths should be counted.
  return sum;
}

double SocialNode::computeAlphaForDepth(int depth)
{
  if(depth < 2) return 1.0;
  
  switch(utilityMode)
  {
    case MULTILEVEL_CONSTANT:
      return levelTwoAlpha;
      break;
    case MULTILEVEL_EXPONENTIAL:
      return (2.0*levelTwoAlpha)/(pow(2.0, (double)(depth-1)));
      break;
    case MULTILEVEL_INVERSE:
      return (2.0*levelTwoAlpha)/((double)depth);
      break;
    default:
      return 0.0;
      break;
  }
}

double SocialNode::computeImmediateUtility(SocialNode* node1, SocialNode* node2)
{
  if(node1 == NULL || node2 == NULL || node1 == node2) return 0.0;
  
  double* coord1 = node1->coordinates;
  double* coord2 = node2->coordinates;
  double d1 = coord1[0] - coord2[0];
  double d2 = coord1[1] - coord2[1];
  double dist = sqrt(d1*d1 + d2*d2);
  
  switch(utilityFunction)
  {
    case INVERSE:
      return 1.0/dist;
      break;
    case EXPONENTIAL:
    {
      double offset = exp(-1.0*sqrt(2));
      return (exp(-1.0*dist)-offset)/(1.0-offset);
      break;
    }
    case LINEAR:
      return 1.0-(dist/sqrt(2.0));
      break;
    default:
      cout << "Invalid utility function" << std::endl;
      return 0.0;
      break;
  }
}

void SocialNode::setFriendshipMode(int new_mode)
{ friendshipMode = new_mode; }

void SocialNode::setUtilityMode(int new_mode)
{ utilityMode = new_mode; }

void SocialNode::setUtilityFunction(int new_func)
{ utilityFunction = new_func; }
