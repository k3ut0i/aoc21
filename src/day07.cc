#include <iostream>
#include <fstream>
#include <list>
#include <tuple>
#include <utility>

typedef unsigned int pos_t;
std::tuple<std::list<pos_t>, pos_t, pos_t> read_f(std::string fname){
  std::list<pos_t> l;
  pos_t count = 0, sum = 0;
  std::ifstream s(fname);
  while(true){
    pos_t n;
    s >> n;
    l.push_back(n);
    count++;
    sum += n;
    char c = s.get();
    if(c == '\n') break;
    else if (c != ',') throw std::runtime_error("Expecting comma after number.");
  }
  return std::make_tuple(l, count, sum);
}

std::list<std::pair<pos_t, unsigned int>> sort_and_bin(std::list<pos_t>& posl){
  posl.sort();
  std::list<std::pair<pos_t, unsigned int>> bin;
  bin.push_back(std::pair<pos_t, unsigned int>(posl.front(), 1));
  pos_t prev = bin.front().first;
  for(auto i = ++posl.begin(); i != posl.end(); i++){
    if(*i == prev) {
      bin.back().second++;
    } else {
      bin.push_back(std::pair<pos_t, unsigned int>(*i, 1));
      prev = *i;
    }
  }
  return bin;
}
// size and sum can be found while constructing the list itself.
// unsigned int min_fuel(const std::list<std::pair<pos_t, unsigned int>>& bin,
// 		      unsigned int size, unsigned int sum)
// {
//   unsigned int left_sum, right_sum, left_count, right_count, min_fuel;
//   typedef std::list<std::pair<pos_t, unsigned int>>::const_iterator bin_it;
  
//   return min_fuel;
// }
std::pair<unsigned int, unsigned int>
min_fuel_brute(const std::list<std::pair<pos_t, unsigned int>>& bin,
	       unsigned int sum)
{
  unsigned int min_fuel = sum;
  unsigned int min_fuel_2 = sum*(sum+1)/2;
  for(const auto &pc : bin){
    unsigned int current_fuel = 0;
    for(const auto &pc2 : bin){
      current_fuel += pc2.second*abs(int(pc.first) - int(pc2.first));
    }
      if(current_fuel < min_fuel) min_fuel = current_fuel;
  }
  for(pos_t i = bin.front().first; i <= bin.back().first; i++){
    unsigned int current_fuel = 0;
    for(const auto &pc2 : bin){
      unsigned int dis = abs(int(i) - int(pc2.first));
      current_fuel += pc2.second*dis*(dis+1)/2;
    }
      if(current_fuel < min_fuel_2) min_fuel_2 = current_fuel;
  }
  return std::pair<unsigned int, unsigned int>(min_fuel, min_fuel_2);
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::list<pos_t> l;
  pos_t c, s;
  std::tie(l, c, s) = read_f(argv[1]);
  auto bin = sort_and_bin(l);
  auto ans = min_fuel_brute(bin, s);
  std::cout <<  ans.first << " " << ans.second << std::endl;
  // for(auto &pc : bin)std::cout << pc.first << " " << pc.second << std::endl;

  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -ggdb -o day07 day07.cc"
// End:
