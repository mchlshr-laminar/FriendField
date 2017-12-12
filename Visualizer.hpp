#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "SocialNode.hpp"
#include <vector>
#include <string>

#ifndef __VISUALIZER_H_
#define __VISUALIZER_H_

class Visualizer
{
  private:
  cv::Mat base_frame;
  cv::VideoWriter movie;
  
  void createBaseFrame(std::vector<SocialNode*>& nodes);
  cv::Mat createFrame(std::vector<SocialNode*>& nodes);
  void addConnection(cv::Mat & frame, SocialNode* node1, SocialNode* node2);
  int getPixelFromCoordinate(double coord);
  
  public:
  Visualizer(std::vector<SocialNode*>& nodes);
  
  void addMovieFrame(std::vector<SocialNode*>& nodes);
  void createImage(std::string filename, std::vector<SocialNode*>& nodes);
};

#endif

