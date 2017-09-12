#ifndef __COORD_RAND_H_
#define __COORD_RAND_H_

class CoordinateRandomizer
{
  private:
  static int distribution;
  
  public:
  const static int UNIFORM = 0, DOUBLE_NORMAL = 1;
  
  static void setDistributionMode(int new_mode);
  static double getCoordinate();
  
  static double getUniformCoordinate();
  
#define MAX_DOUBLE_NORM_PROB 4.0
  static double getDoubleNormalCoordinate();
  static double getDoubleNormalProbability(double coord);
};

#endif

