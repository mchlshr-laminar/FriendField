#ifndef __OPTFINDER_H_
#define __OPTFINDER_H_

#include <vector>
#include "SocialNode.hpp"

extern "C"
{
  #include <glpk.h>
}

//THIS IS ONLY USABLE FOR MULTILEVEL DEPTH 2

class OptFinder
{
  private:
  glp_prob* linear_program;
  
  void add_cols(std::vector<SocialNode*>& nodes); //Structural variables
  void add_rows(std::vector<SocialNode*>& nodes); //Auxilliary variables
  
  public:
  OptFinder(std::vector<SocialNode*>& nodes);
  ~OptFinder();
  
  double solveOptimum();
};

#endif
