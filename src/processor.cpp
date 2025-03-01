#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{
    float result = 0.0;
    totalJiffiesStart = LinuxParser::Jiffies();
    activeJiffiesStart = LinuxParser::ActiveJiffies();
    
    usleep(100000); // microseconds --> 100 milliseconds
    
    totalJiffiesEnd = LinuxParser::Jiffies();
    activeJiffiesEnd = LinuxParser::ActiveJiffies();
    
    long totalDelta = totalJiffiesEnd - totalJiffiesStart;
    long activeDelta = activeJiffiesEnd - activeJiffiesStart;
    
    if (totalDelta == 0) {
        result = 0.0;
    }
    
    // Multi-cores CPU
    int numCores = sysconf(_SC_NPROCESSORS_ONLN);  // number of CPU cores
    result = (float(activeDelta) / float(totalDelta)) * numCores;

    return result;
}