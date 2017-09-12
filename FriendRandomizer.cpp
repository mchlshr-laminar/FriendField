#include "FriendRandomizer.hpp"
#include <cstdlib>
#include <iostream>

using std::vector;

FriendRandomizer::FriendRandomizer(vector<SocialNode*> & nodeList, int numFriends)
{
  for(unsigned int i = 0; i < nodeList.size(); i++)
    for(int j = 0; j < numFriends; j++)
      remainder.push_back(new SocialListNode(nodeList[i]));
  slotList = NULL;
  numIterations = 0;
  nodes = &nodeList;
}

FriendRandomizer::~FriendRandomizer()
{
  cleanList();
}

void FriendRandomizer::makeMatches()
{
  switch(SocialNode::friendshipMode)
  {
    case SocialNode::SYMMETRIC_FRIENDSHIP:
      while(nextMatch());
      break;
    case SocialNode::ASYMMETRIC_FRIENDSHIP:
      asymmetricMatch();
      break;
  }
}

bool FriendRandomizer::nextMatch()
{
  //No more matches to be made in this list:
  if(slotList == NULL || slotList->next == NULL)
  {
    //Put the remainder back in the list.
    while(slotList != NULL)
    {
      remainder.push_back(slotList);
      slotList = slotList->next;
    }
    
    //No more matches to be made at all:
    if(remainder.size() < 2 || numIterations >= maxIterations)
    {
      cleanList();
      return false;
    }
    randomizeRemainder();
    numIterations++;
  }
  
  //Attempt to match the next two slots
  if(SocialNode::addFriendship(slotList->node, slotList->next->node))
  {
    //Success: remove slots from list & return true.
    for(int i = 0; i < 1+(SocialNode::friendshipMode == SocialNode::SYMMETRIC_FRIENDSHIP); i++)
    {
      SocialListNode* temp = slotList->next;
      delete slotList;
      slotList = temp;
    }
    return true;
  }
  else
  {
    //Failure: move slots to remainder list and try again.
    remainder.push_back(slotList);
    remainder.push_back(slotList->next);
    slotList = slotList->next->next;
    return nextMatch();
  }
}

void FriendRandomizer::randomizeRemainder()
{
  //TEMP:
  //std::cout << remainder.size() << std::endl;
  //Check that the list isn't empty
  if(remainder.size() == 0)
  {
    slotList = NULL;
    return;
  }
  
  //Randomize the order of the list (Fisher-Yates shuffle)
  for(unsigned int i = 0; i < remainder.size()-1; i++)
  {
    int j = rand()%(remainder.size()-(i+1)) + (i+1);
    SocialListNode* temp = remainder[i];
    remainder[i] = remainder[j];
    remainder[j] = temp;
  }
  
  //Set up the links, store the first node, clear the remainder.
  for(unsigned int i = 0; i < remainder.size()-1; i++)
    remainder[i]->next = remainder[i+1];
  remainder[remainder.size()-1]->next = NULL;
  slotList = remainder[0];
  remainder.clear();
}

void FriendRandomizer::asymmetricMatch()
{
  for(unsigned int i = 0; i < nodes->size(); i++)
  {
    for(int j = 0; j < (*nodes)[i]->getMaxFriends(); j++)
    {
      unsigned int fr = rand()%(nodes->size()-1);
      if(fr >= i) fr++;
      SocialNode::addFriendship((*nodes)[i], (*nodes)[fr]);
    }
  }
}

void FriendRandomizer::cleanList()
{
  while(slotList != NULL)
  {
    SocialListNode* temp = slotList->next;
    delete slotList;
    slotList = temp;
  }
  slotList = NULL;
  
  for(unsigned int i = 0; i < remainder.size(); i++)
    delete remainder[i];
  remainder.clear();
}

SocialListNode::SocialListNode(SocialNode* contents)
{
  node = contents;
  next = NULL;
}
