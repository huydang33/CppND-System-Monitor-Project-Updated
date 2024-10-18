#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>

#include "linux_parser.h"

#define STR_MEMTOTAL  "MemTotal:"
#define STR_MEMFREE   "MemFree:"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template<typename T>
T ParseData(std::string file_name, std::string key);

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{
  string line;
  string key;
  float mem_total = 0.0;
  float mem_free = 0.0;
  float value = 0.0;

  /* Read MemInfo from /proc/meminfo/ */
  mem_total = ParseData<float>(kMeminfoFilename, STR_MEMTOTAL);
  mem_free = ParseData<float>(kMeminfoFilename, STR_MEMFREE);

  /* Calculate Memory */
  value = (mem_total - mem_free) / mem_total;

  return value; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{
  long result = 0;

  /* Read /proc/uptime */
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) 
  {
    stream >> result;
  }

  return result; 
}

// Accessor function defined to access values from stat file.
template<typename T>
T ParseData(std::string file_name, std::string key)
{
  string line;
  string key_, value;
  std::ifstream filestream(LinuxParser::kProcDirectory + file_name); // streams from this file
  // Checks if the file is open
  if (filestream.is_open()) 
  {
    // Executes upto the last line
    while (std::getline(filestream, line)) 
    { 
      // reads a string until whitespace is reached (<sstream> lib)
      std::istringstream linestream(line);
      while (linestream >> key_ >> value) 
      {
        // extracts the keys and values. Here we need only the 1st and 2nd strings
        if (key_ == key) {
          if constexpr (std::is_same<T, int>::value) {
            return std::stoi(value);  //change to int
          } else if constexpr (std::is_same<T, long>::value) {
              return std::stol(value);  // change to long
          } else if constexpr (std::is_same<T, float>::value) {
              return std::stof(value);  // change to float
          } else if constexpr (std::is_same<T, string>::value) {
              return value;  // change to string
          } else {
              throw std::invalid_argument("Unsupported type");
          }
        }
      }
    }
  }
  return T();
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{
  long total = 0;
  vector<string> cpu_data = CpuUtilization();
  if (cpu_data.empty()) {
    return 0;  // Handle error if parsing fails
  }

  // Convert strings to long and sum the active jiffies:
  // Active jiffies = user + nice + system + irq + softirq + steal
  for(int i = kUser_; i <= kSteal_; i++) {
    total += stol(cpu_data[i]);
  }

  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{
  long total = 0;
  string value, line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    int i = 0;
    while (linestream >> value) {
      if (i >= 13 && i <= 16) {
        total += stol(value);
      }
      i++;
    }
  }
  return total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{
  return LinuxParser::Jiffies() + LinuxParser::IdleJiffies();
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{
  long total = 0;
  vector<string> cpu_data = CpuUtilization();
  if (cpu_data.empty()) {
    return 0;  // Handle error if parsing fails
  }

  for(int i = kIdle_; i <= kIOwait_; i++) {
    total += stol(cpu_data[i]);
  }
  return total;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{
  string value, line;
  vector<string> cpuValues;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      if (value != "cpu") {
        cpuValues.push_back(value);
      }
    }
  }
  return cpuValues;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{
  return ParseData<int>(LinuxParser::kStatFilename, "processes");
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  return ParseData<int>(LinuxParser::kStatFilename, "procs_running");
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{
  string cmd_line = "";
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kCmdlineFilename); // streams from this file
  
  // Checks if the file is open
  if (filestream.is_open()) 
  {
    std::getline(filestream, cmd_line);
  }

  return cmd_line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{
  return to_string(ParseData<int>(to_string(pid) + LinuxParser::kStatusFilename, "VmRSS:") / 1000); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) 
{
  return to_string(ParseData<int>(to_string(pid) + LinuxParser::kStatusFilename, "Uid:"));
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{
  string line, key, value, x, user_name;
  string uid = Uid(pid);
  std::ifstream filestream(LinuxParser::kPasswordPath);
  if (filestream.is_open()) 
  {
    while (std::getline(filestream, line)) 
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> x >> value; 
      if (value == uid) {
        user_name = key;
        return user_name;
      }
    }
  } 
  return user_name;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid)
{
  long uptime_process{0};
  string line, uptime;
  
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) + LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 22; ++i)
    {
      linestream >> uptime;
    }
  }
  uptime_process = LinuxParser::UpTime() - std::stol(uptime)/sysconf(_SC_CLK_TCK);
  return uptime_process;
}
