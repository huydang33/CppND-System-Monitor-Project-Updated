#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{ 
    int hh, mm, ss;
    string str_ss, str_mm,str_hh;

    hh = seconds / 3600;
    mm = (seconds%3600)/60;
    ss = (seconds%3600)%60;

    str_hh = to_string(hh);
    str_mm = to_string(mm);
    str_ss = to_string(ss);

    /* Output 2 digits */
    str_ss.insert(0, 2 - str_ss.length(), '0');
    str_mm.insert(0, 2 - str_mm.length(), '0');
    str_hh.insert(0, 2 - str_hh.length(), '0');

    return str_hh + ":" + str_mm + ":" + str_ss + ":";
}