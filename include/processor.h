#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
    long totalJiffiesStart;
    long activeJiffiesStart;
    long totalJiffiesEnd;
    long activeJiffiesEnd;
};

#endif