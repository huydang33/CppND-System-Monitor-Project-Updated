#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  Process::pid = pid;
}

// TODO: Return this process's ID
int Process::Pid() { return Process::pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization()
{ 
    long totalTime = LinuxParser::ActiveJiffies(pid);
    long startTime = LinuxParser::UpTime(pid);
    long upTime = LinuxParser::UpTime();
    long hertz = sysconf(_SC_CLK_TCK);  // tick/sec

    // Time for process run (second)
    long seconds = upTime - (startTime / hertz);

    if (seconds > 0) {
        return (static_cast<float>(totalTime) / hertz) / seconds;
    } else {
        return 0.0;
    }
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// TODO: Return this process's memory utilization
string Process::Ram() {
    return LinuxParser::Ram(pid); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
  return stol(LinuxParser::Ram(pid)) > stol(LinuxParser::Ram(a.pid));
}