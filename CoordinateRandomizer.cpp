#include "CoordinateRandomizer.hpp"
#include <cstdlib>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

int CoordinateRandomizer::distribution = CoordinateRandomizer::UNIFORM;

void CoordinateRandomizer::setDistributionMode(int new_mode)
{ distribution = new_mode; }

double CoordinateRandomizer::getCoordinate()
{
  switch(distribution)
  {
    case UNIFORM:
      return getUniformCoordinate();
      break;
    case DOUBLE_NORMAL:
      return getDoubleNormalCoordinate();
      break;
    default:
      std::cerr << "Invalid distribution mode" << std::endl;
      return 0.0;
      break;
  }
}

double CoordinateRandomizer::getUniformCoordinate()
{
  return ((double)rand())/RAND_MAX;
}

double CoordinateRandomizer::getDoubleNormalCoordinate()
{
  double result = 0.0;
  do
  {
    result = getUniformCoordinate();
  }
  while(getUniformCoordinate()*MAX_DOUBLE_NORM_PROB > getDoubleNormalProbability(result));
  return result;
}

double CoordinateRandomizer::getDoubleNormalProbability(double coord)
{
  double result = (1.0/sqrt(0.02*M_PI))*exp((-1.0/0.02)*pow(coord-0.25, 2.0));
  result += (1.0/sqrt(0.02*M_PI))*exp((-1.0/0.02)*pow(coord-0.75, 2.0));
  return result;
}

