#include "OptFinder.hpp"
#include <string>
#include <sstream>

using std::vector;
using std::string;
using std::stringstream;

OptFinder::OptFinder(vector<SocialNode*>& nodes)
{
  linear_program = glp_create_prob();
  glp_set_obj_dir(linear_program, GLP_MAX);
  add_cols(nodes);
  add_rows(nodes);
}

void OptFinder::add_cols(vector<SocialNode*>& nodes)
{
  //One-hop variables
  for(int i = 0; i < nodes.size(); i++)
  {
    for(int j = 0; j < nodes.size(); j++)
    {
      if(i == j) continue;
      int ind = glp_add_cols(linear_program, 1);
      stringstream colname;
      colname << i << "->" << j;
      glp_set_col_name(linear_program, ind, colname.str().c_str());
      glp_set_col_bnds(linear_program, ind, GLP_UP, 0.0, 1.0);
      glp_set_obj_coef(linear_program, ind, 2*SocialNode::computeImmediateUtility(nodes[i], nodes[j]));
    }
  }
  
  //Two-hop variables
  for(int i = 0; i < nodes.size(); i++)
  {
    for(int j = 0; j < nodes.size(); j++)
    {
      if(i == j) continue;
      for(int k = 0; k < nodes.size(); k++)
      {
        if(i == k || j == k) continue;
        int ind = glp_add_cols(linear_program, 1);
        stringstream colname;
        colname << i << "->" << j << "->" << k;
        glp_set_col_name(linear_program, ind, colname.str().c_str());
        glp_set_col_bnds(linear_program, ind, GLP_FR, 0.0, 0.0); //Two-hop vars are only restricted by the one-hop vars
        double coef = 2*SocialNode::levelTwoAlpha*SocialNode::computeImmediateUtility(nodes[i], nodes[k])
        glp_set_obj_coef(linear_program, ind, coef);
      }
    }
  }
  
  //Note that the 2* part of the obj_coef is because both nodes benefit; returned value must
  //be on same scale as util. from simulation
}

void OptFinder::add_rows(vector<SocialNode*>& nodes)
{
  //One-hop var constraints were accomplished with glp_set_col_bnds
  
  //Two-hop constraints
  for(int i = 0; i < nodes.size(); i++)
  {
    for(int j = 0; j < nodes.size(); j++)
    {
      if(i == j) continue;
      for(int k = 0; k < nodes.size(); k++)
      {
        //2 rows constraining a->b->c
        if(i == k || j == k) continue;
        int row_ind = glp_add_rows(linear_program, 2);
        glp_set_row_bnds(linear_program, row_ind, GLP_UP, 0.0, 0.0);
        glp_set_row_bnds(linear_program, row_ind+1, GLP_UP, 0.0, 0.0);
        
        //constraint from a->b
        stringstream colnames1;
        int col_inds[2];
        double col_vals[2];
        colnames1 << i << "->" << j;
        col_inds[0] = glp_find_col(linear_program, colnames1.str().c_str());
        col_vals[0] = -1.0;
        colnames1 << "->" << k;
        col_inds[1] = glp_find_col(linear_program, colnames1.str().c_str());
        col_vals[1] = 1.0;
        glp_set_mat_row(linear_program, row_ind, 2, col_inds, col_vals);
        
        //constraint from b->c
        stringstream colnames2;
        colnames2 << j << "->" << k;
        col_inds[0] = glp_find_col(linear_program, colnames2.str().c_str());
        glp_set_mat_row(linear_program, row_ind+1, 2, col_inds, col_vals);
        //Some values were reusable from the a->b constraint
      }
    }
  }
}

OptFinder::~OptFinder()
{
  glp_delete_prob(linear_program);
}

double OptFinder::solveOptimum()
{
  if(glp_simplex(linear_program, NULL) == 0 && glp_get_status(linear_program) == GLP_OPT)
    return glp_get_obj_val(linear_program);
  else
    return 0.0;
}
