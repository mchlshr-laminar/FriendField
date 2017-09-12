#include "Visualizer.hpp"
#include <cstdio>

using cv::Mat;
using cv::VideoWriter;
using cv::Vec3b;
using cv::Point;
using cv::Scalar;
using std::vector;
using std::string;

#define FRAME_W 1000
#define NODE_RADIUS 5
#define CONNECT_THICKNESS 1
#define VISUALIZER_FPS 5

Visualizer::Visualizer(std::vector<SocialNode*>& nodes) : base_frame(FRAME_W, FRAME_W, CV_8UC3), 
  movie("stuff.avi", CV_FOURCC('D','I','V','X'), VISUALIZER_FPS, base_frame.size(), true)
{
  createBaseFrame(nodes);
}

void Visualizer::addMovieFrame(std::vector<SocialNode*>& nodes)
{
  //for(int i = 0; i < MOVIE_SLOWDOWN_FACTOR; i++)
    movie << createFrame(nodes);
}

void Visualizer::createImage(string filename, vector<SocialNode*>& nodes)
{
  imwrite(filename, createFrame(nodes));
}

void Visualizer::createBaseFrame(vector<SocialNode*>& nodes)
{
  //White background:
  for(int i = 0; i < FRAME_W; i++)
  {
    for(int j = 0; j < FRAME_W; j++)
    {
      base_frame.at<Vec3b>(i, j)[0] = 255;
      base_frame.at<Vec3b>(i, j)[1] = 255;
      base_frame.at<Vec3b>(i, j)[2] = 255;
    }
  }
  
  //Add the nodes
  for(unsigned int i = 0; i < nodes.size(); i++)
  {
    double* coords = nodes[i]->getCoordinates();
    int c_x = getPixelFromCoordinate(coords[0]);
    int c_y = getPixelFromCoordinate(coords[1]);
    
    Point p(c_x, c_y);
    circle(base_frame, p, NODE_RADIUS, Scalar(0,0,0), -1);
  }
}

Mat Visualizer::createFrame(vector<SocialNode*>& nodes)
{
  Mat result = base_frame.clone();
  for(unsigned int i = 0; i < nodes.size(); i++)
  {
    SocialNode** connections = nodes[i]->getFriends();
    int num = nodes[i]->getNumFriends();
    for(int j = 0; j < num; j++)
      addConnection(result, nodes[i], connections[j]);
  }
  return result;
}

void Visualizer::addConnection(Mat & frame, SocialNode* node1, SocialNode* node2)
{
  double* start_coord = node1->getCoordinates();
  double* end_coord = node2->getCoordinates();
  Point start(getPixelFromCoordinate(start_coord[0]), getPixelFromCoordinate(start_coord[1]));
  Point end(getPixelFromCoordinate(end_coord[0]), getPixelFromCoordinate(end_coord[1]));
  Point midpoint((start.x+end.x)/2.0, (start.y+end.y)/2.0);
  
  if(SocialNode::areFriends(node2, node1))
  {
    line(frame, start, end, Scalar(255, 0, 0), CONNECT_THICKNESS, CV_AA);
  }
  else
  {
    line(frame, start, midpoint, Scalar(255,0,0), CONNECT_THICKNESS, CV_AA);
    line(frame, midpoint, end, Scalar(0,0,255), CONNECT_THICKNESS, CV_AA);
  }
}

int Visualizer::getPixelFromCoordinate(double coord)
{ return (int)(FRAME_W*coord); }

