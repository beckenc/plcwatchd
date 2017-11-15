#ifndef _LOG_HPP_
#define _LOG_HPP_

#include <iostream>
#include <fstream>

inline std::ostream & tcout() {
   time_t t = time(0);
   struct tm *now = localtime( &t );
   std::cout << 1900 + now->tm_year << "-" << 1 + now->tm_mon << "-" << now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ": ";
   return std::cout;
}

inline std::ostream & tcerr() {
   time_t t = time(0);
   struct tm *now = localtime( &t );
   std::cerr << 1900 + now->tm_year << "-" << 1 + now->tm_mon << "-" << now->tm_mday << " " << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ": ";
   return std::cerr;
}

#endif