#include "DataExporter.hpp"
#include <utility>
#include <sstream>

#define EXPORTER_DEFAULT_FILENAME "output.txt"

using std::string;
using std::vector;
using std::map;
using std::pair;
using std::ofstream;
using std::stringstream;
using std::cout;
using std::endl;

DataExporter* DataExporter::instance = NULL;

DataExporter::DataExporter(const string& filename)
{
  curr_filename = filename;
}

DataExporter::~DataExporter()
{
  if(output.is_open()) output.close();
  clearData();
}

void DataExporter::setFilename(const string& filename)
{ curr_filename = filename; }

void DataExporter::setOutputCell(int row, int col, const string& cell)
{
  vector<string> temprow;
  while((unsigned int)row >= output_cells.size()) output_cells.push_back(temprow);
  string tempcell;
  while((unsigned int)col >= output_cells[row].size()) output_cells[row].push_back(tempcell);
  output_cells[row][col] = cell;
}
void DataExporter::setOutputCell(int row, int col,  double cell)
{
  stringstream text;
  text << cell;
  setOutputCell(row, col, text.str());
}

void DataExporter::printOutput()
{
  for(unsigned int row = 0; row < output_cells.size(); row++)
  {
    for(unsigned int col = 0; col < output_cells[row].size(); col++)
      output << output_cells[row][col] << '\t';
    output << '\n';
  }
}

void DataExporter::submitData(string category, int value)
{ submitData(category, (double)value); }

void DataExporter::submitData(string category, double value)
{ data[category].push_back(value); }

void DataExporter::exportData()
{
  output.open(curr_filename.c_str());
  output_cells.clear();
  setOutputCell(1, 0, "Averages:");
  
  int i = 0;
  for(map<string,vector<double> >::iterator itr = data.begin(); itr != data.end(); itr++)
  {
    setOutputCell(0, i+1, itr->first);
    setOutputCell(1, i+1, getAvg(itr->second));
    for(unsigned int j = 0; j < itr->second.size(); j++)
      setOutputCell(j+2, i+1, itr->second[j]);
    i++;
  }
  
  printOutput();
  output.close();
}

void DataExporter::exportData(vector<string>& categories)
{
  output.open(curr_filename.c_str());
  output_cells.clear();
  setOutputCell(1, 0, "Averages:");
  
  for(unsigned int i = 0; i < categories.size(); i++)
  {
    setOutputCell(0, i+1, categories[i]);
    setOutputCell(1, i+1, getAvg(categories[i]));
    vector<double>* values = &data[categories[i]];
    for(unsigned int j = 0; j < values->size(); j++)
      setOutputCell(j+2, i+1, (*values)[j]);
  }
  
  printOutput();
  output.close();
}

void DataExporter::clearData()
{
  data.clear();
}

double DataExporter::getAvg(vector<int>& vec)
{
  if(vec.size() == 0) return 0.0;
  double total = 0.0;
  for(unsigned int i = 0; i < vec.size(); i++)
    total += vec[i];
  return total/((double)vec.size());
}

double DataExporter::getAvg(vector<double>& vec)
{
  if(vec.size() == 0) return 0.0;
  double total = 0.0;
  for(unsigned int i = 0; i < vec.size(); i++)
    total += vec[i];
  return total/((double)vec.size());
}

double DataExporter::getAvg(string& category)
{ return getAvg(instance->data[category]); }

DataExporter* DataExporter::getExporter(string filename)
{
  if(instance == NULL) instance = new DataExporter(filename);
  else instance->setFilename(filename);
  
  return instance;
}

DataExporter* DataExporter::getExporter()
{
  if(instance == NULL) instance = new DataExporter(EXPORTER_DEFAULT_FILENAME);
  return instance;
}

