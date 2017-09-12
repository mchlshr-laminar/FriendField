#ifndef __SOCIAL_NODE_H_
#define __SOCIAL_NODE_H_

#include <set>

#define NUM_FRIENDS 3
//#define LEVEL_TWO_ALPHA 0.16666666666

class SocialNode
{
  private:
  double* coordinates;
  SocialNode** friends;
  std::set<SocialNode*> passiveFriends; //This node puts no effort into these
  //friendships, but the node on the other end does.
  
  int currNumFriends, maxNumFriends;
  
  bool removeFriend(SocialNode* other, bool shouldReciprocate);
  bool removePassiveFriend(SocialNode* other);
  bool addFriend(SocialNode* other, bool shouldReciprocate);
  bool addPassiveFriend(SocialNode* other);
  bool considerFriend(SocialNode* other, bool shouldReciprocate);
  
  public:
  static int friendshipMode;
  const static int SYMMETRIC_FRIENDSHIP = 0, ASYMMETRIC_FRIENDSHIP = 1;
  static int utilityMode;
  const static int IMMEDIATE_UTILITY = 0, FRIENDS_OF_FRIENDS = 1, MULTILEVEL_CONSTANT = 2, 
    MULTILEVEL_EXPONENTIAL = 3, MULTILEVEL_INVERSE = 4;
  static int utilityFunction;
  const static int INVERSE = 0, EXPONENTIAL = 1, LINEAR = 2;
  static double levelTwoAlpha;
  static int numFriendLevels; //MULTILEVEL_XXX with numFriendLevels is equivalent to FRIENDS_OF_FRIENDS
  
  SocialNode(int maxFriends);
  SocialNode(int maxFriends, double x, double y);
  ~SocialNode();
  
  double* getCoordinates();
  SocialNode** getFriends();
  std::set<SocialNode*>* getPassiveFriends();
  int getNumFriends();
  int getMaxFriends();
  
  int getLowestUtilityFriendIndex();
  bool canAddFriends();
  
  double computeCurrentUtility();
  
  static bool areFriends(SocialNode* friend1, SocialNode* friend2);
  static bool addFriendship(SocialNode* friend1, SocialNode* friend2);
  static bool removeFriendship(SocialNode* friend1, SocialNode* friend2);
  static bool considerFriendship(SocialNode* friend1, SocialNode* friend2);
  //The utility friend1 receives/would receive by putting effort into the relationship with friend2:
  static double computeFriendshipUtility(SocialNode* friend1, SocialNode* friend2);
  //The utility friend1 receives/would receive due to friend2 putting effort into the relationship:
  static double computeReceivedUtility(SocialNode* friend1, SocialNode* friend2);
  static double computeReceivedFriendsOfFriends(SocialNode* friend1, SocialNode* friend2);
  static double computeReceivedMultilevel(SocialNode* source, SocialNode* location, int remaining_depth, double alpha);
  static double computeFriendsOfFriendsUtility(SocialNode* friend1, SocialNode* friend2);
  static double computeImmediateUtility(SocialNode* friend1, SocialNode* friend2);
  static double computeMultilevelUtility(SocialNode* source, SocialNode* location, int remaining_depth, double alpha);
  static double computeAlphaForDepth(int depth);
  
  static void setFriendshipMode(int new_mode);
  static void setUtilityMode(int new_mode);
  static void setUtilityFunction(int new_func);
};

#endif

