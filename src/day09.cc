#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <list>
#include <algorithm>
#include <map>
#include <stack>
#include <set>

typedef unsigned int uint;

template<class CharT, class Traits> std::pair<uint, uint>
read_height_map(std::basic_istream<CharT, Traits>& stream,
		std::vector<std::vector<uint>>& m)
{
  uint nrows = 0, ncount = 0;
  do{
    std::vector<uint> row;
    char c = stream.get();
    while (!isspace(c)){
      row.push_back(c-48);
      ncount++;
      c = stream.get();
    }
    m.push_back(row);
    while(isspace(stream.peek())) stream.get();
    nrows++;
  }while(!stream.eof());
  return std::pair<uint, uint>(nrows, ncount/nrows);  
}

template<class CharT, class Traits> void
print_height_map(std::basic_ostream<CharT, Traits>& ostream,
		 const std::vector<std::vector<uint>>& m)
{
  for(auto &row: m){
    for(auto n: row) ostream << n;
    ostream << std::endl;
  }
}

uint sum_lows(const std::vector<std::vector<uint>>& m,
		const uint nrows, const uint ncols){
  uint sum = 0;
  for(uint i = 0; i < nrows; i++)
    for(uint j = 0; j < ncols; j++)
      {
	// get neighbours and check if all of them are greater than current
	std::list<std::pair<int, int>> indices = {{i-1, j}, {i, j-1}, {i+1, j}, {i, j+1}};
	uint ch = m[i][j];
	indices.remove_if([nrows, ncols](std::pair<int, int> p){
	  return p.first < 0 || p.first >= (int)nrows
	    || p.second < 0 || p.second >= (int)ncols;}
	  );
	std::list<uint> neighbours;
	for(auto &p: indices) neighbours.push_back(m[p.first][p.second]);
	bool low = all_of(neighbours.begin(), neighbours.end(),
			  [ch](uint h){return h > ch;});
	// if(low) std::cout << ch << std::endl;
	sum +=  low ? (ch+1) : 0;
      }
  return sum;
}

struct coloring{
  std::map<uint, uint> m;
  const uint nrows, ncols;
  uint ncolors;
  coloring(std::vector<std::vector<uint>> height_map,
	   uint nrows, uint ncols) : nrows(nrows), ncols(ncols)
  {
    std::set<uint> uncolored; // all uncolored
    uncolored.clear();
    for(uint i = 0; i < nrows; i++)
      for(uint j = 0; j < ncols; j++)
	{
	  uint n = i*ncols+j;
	  m[n] = 0; // zeros is uncolored
	  if(height_map[i][j] != 9) uncolored.insert(n);
	}
    auto neighbours = [nrows, ncols, height_map](uint num){ // TODO: debug this
      uint i = int(num / ncols);
      uint j = num % ncols;
      std::list<std::pair<int, int>> indices = {{i-1, j}, {i, j-1}, {i+1, j}, {i, j+1}};
      indices.remove_if([nrows, ncols, height_map](std::pair<int, int> p){
	bool outbounds =  p.first < 0 || p.first >= (int)nrows
	  || p.second < 0 || p.second >= (int)ncols;
	return outbounds || height_map[p.first][p.second] == 9;
      }
	);
      std::list<uint> inums;
      for(auto &ip: indices) inums.push_back(ip.first*ncols+ip.second);
      return inums;
    };
    uint current_color = 0;
    while(!uncolored.empty()){
      current_color++;
      std::stack<uint> s; // current stack
      s.push(*uncolored.begin()); // get the first accessible element

      while(!uncolored.empty() && !s.empty()){
	uint current_num = s.top();
	auto connected = neighbours(current_num);
	if(m[current_num] == 0){
	  m[current_num] = current_color;
	  uncolored.erase(current_num);
	  s.pop();
	  for(auto n : connected) if(m[n] == 0) s.push(n);
	} else s.pop();
      }
    }
    ncolors = current_color;
  }
  template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
  print_colors(std::basic_ostream<CharT, Traits>& s){
    for(uint i = 0; i < nrows; i++)
      {
	for(uint j = 0; j < ncols; j++)
	  s << m[i*ncols+j] << ' ';
	s << std::endl;
      }
    return s;
  }
  template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
  write_pgm(std::basic_ostream<CharT, Traits>& stream){
    stream << "P2" << std::endl;
    stream << nrows << ' ' << ncols << std::endl;
    stream << ncolors << std::endl;
    for(uint i = 0; i < nrows; i++)
      {
	for(uint j = 0; j < ncols; j++)
	  stream << m[i*ncols+j] << ' ';
	stream << std::endl;
      }
    return stream;
  }
};

template<typename K, typename V>
std::map<V, uint> count_vals(std::map<K, V> m){
  std::map<V, uint> counts;
  for(auto &kv: m) counts[kv.second] = 0;
  for(auto &kv: m) counts[kv.second]++;
  return counts;
}

uint top_three_mult(std::map<uint, uint> color_areas){
  std::vector<uint> areas;
  for(auto &kv: color_areas) if(kv.first != 0) areas.push_back(kv.second);
  sort(areas.begin(), areas.end());
  uint first = *areas.rbegin();
  uint second = *++areas.rbegin();
  uint third = *++ ++ areas.rbegin();
  return first*second*third;
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::vector<std::vector<uint>> hm;
  uint nrows, ncols;
  std::tie(nrows, ncols) = read_height_map(s, hm);
  std::cout << sum_lows(hm, nrows, ncols) << std::endl;
  struct coloring c(hm, nrows, ncols);
  std::ofstream o("gen/day09_basins.pgm");
  c.write_pgm(o);
  std::cout << top_three_mult(count_vals(c.m)) << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -Wall -Wextra -ggdb -lstdc++ day09.cc -o day09"
// End:
