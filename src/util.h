#ifndef AOC21_UTIL
#define AOC21_UTIL

#include <iostream>
#include <list>

template<class CharT, class Traits> std::list<unsigned int>
read_csv_line(std::basic_istream<CharT, Traits>& stream){
  std::list<unsigned int> nums;
  while(true){
    unsigned int i;
    stream >> i;
    // std::cout << i << std::endl;
    nums.push_back(i);
    char delim = stream.get();
    if (delim == '\n') break;
    else if (delim != ',') throw std::runtime_error("Not a csv line.");
  }
  return nums;
}

#endif
