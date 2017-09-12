#ifndef __FRIEND_RANDOMIZER_H_
#define __FRIEND_RANDOMIZER_H_

#include <vector>
#include "SocialNode.hpp"

class SocialListNode
{
  public:
  SocialNode* node;
  SocialListNode* next;
  
  SocialListNode(SocialNode* contents);
};

class FriendRandomizer
{
  private:
  SocialListNode* slotList;
  std::vector<SocialListNode*> remainder;
  std::vector<SocialNode*>* nodes;
  int numIterations;
  static const int maxIterations = 5;
  
  void randomizeRemainder();
  void cleanList();
  
  public:
  FriendRandomizer(std::vector<SocialNode*> & nodeList, int numFriends);
  ~FriendRandomizer();
  
  void makeMatches();
  bool nextMatch();
  void asymmetricMatch();
};

#endif

