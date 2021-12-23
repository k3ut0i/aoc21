#include "util.h"
#include <fstream>
#include <iostream>
#include <list>
#include <array>

unsigned long step_day(std::list<unsigned long>& fishes){ // doesn't work for part2
  unsigned long count = 0;
  unsigned long new_fishes = 0;
  for(auto fi = fishes.begin(); fi != fishes.end(); fi++){
    count++;
    if(*fi == 0){
      *fi = 6;
      new_fishes++;
    } else {
      *fi -= 1;
    }
  }
  for(unsigned long i = 0; i < new_fishes; i++) fishes.push_back(8);
  count += new_fishes;
  return count;
}

unsigned long step_day_b(std::array<unsigned long, 9>& fishes){
  unsigned long count = 2*fishes[0];
  unsigned long new_fishes = fishes[0];
  for(unsigned long i = 1; i < 9; i++) {
    count += fishes[i];
    fishes[i-1]=fishes[i];
  }
  fishes[8] = new_fishes;
  fishes[6] += new_fishes;

  return count;
}

int main(int argc, char* argv[]){
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <filename> <num-of-days>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  auto nums = read_csv_line(s);
  std::array<unsigned long, 9> fishes_b;
  fishes_b.fill(0);
  for(auto n:nums) fishes_b[n]++; // binning
  const int ndays = atoi(argv[2]);
  unsigned long count;

  for(int i = 0; i < ndays; i++) count = step_day_b(fishes_b);
  std::cout << count << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "make day06"
// End:
