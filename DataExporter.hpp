#ifndef __DATA_EXPORTER_H_
#define __DATA_EXPORTER_H_

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

class DataExporter
{
  private:
  std::map<std::string, std::vector<double> > data;
  std::vector<std::vector<std::string> > output_cells;
  std::ofstream output;
  std::string curr_filename;
  
  static DataExporter* instance;
  
  DataExporter(const std::string& filename);
  void setFilename(const std::string& filename);
  void setOutputCell(int row, int col, const std::string& cell);
  void setOutputCell(int row, int col,  double cell);
  void printOutput();
  
  public:
  ~DataExporter();
  
  void submitData(std::string category, int value);
  void submitData(std::string category, double value);
  
  void exportData();
  void exportData(std::vector<std::string>& categories);
  void clearData();
  
  static double getAvg(std::vector<int>& vec);
  static double getAvg(std::vector<double>& vec);
  static double getAvg(std::string& category);
  
  static DataExporter* getExporter(const std::string filename);
  static DataExporter* getExporter();
};

#endif

