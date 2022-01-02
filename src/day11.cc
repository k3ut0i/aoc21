#include <iostream>
#include <fstream>
#include <list>
#include <utility> // pair
#include <cstdlib> // strtoui
#include <stack>

static const uint nrows = 10;
static const uint ncols = 10;

struct octostate {
  uint energies[nrows][ncols];
};

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, struct octostate& s){
  for(uint i = 0; i < nrows; i++){
    for(uint j = 0; j < ncols; j++){
      if(stream.eof()) throw std::runtime_error("Unexpexted end of file");
      const char c = stream.get();
      if(c > 57 || c < 48) throw std::runtime_error("Expecting a digit char.");
      s.energies[i][j] = c - 48; // char to digit conversion
    }
    if(stream.get() != '\n') // slurps the newline char
      throw std::runtime_error("Line not terminated with newline");
  }
  return stream;
}

template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& s, struct octostate& o){
  for(uint i = 0; i < nrows; i++){
    for(uint j = 0; j < ncols; j++){
      s << char(o.energies[i][j]+48);
    }
    s << std::endl;
  }
  return s;
}

std::list<std::pair<uint, uint>> neighbours(uint i, uint j){
  std::list<std::pair<uint, uint>> n = {{i-1, j-1}, {i-1, j}, {i-1, j+1},
					{i, j-1}, {i, j+1},
					{i+1, j-1}, {i+1, j}, {i+1, j+1}};
  n.remove_if([](auto p){ // remove out of bounds
    return p.first < 0 || p.second < 0 ||
      p.first >= nrows || p.second >= ncols;
  });
  return n;
}

uint step (struct octostate& s){
  uint flashes = 0;
  std::stack<std::pair<uint, uint>> will_flash;
  for(uint i = 0; i < nrows; i++)
    for(uint j = 0; j < ncols; j++)
      {
	s.energies[i][j]++;
	if(s.energies[i][j] > 9)
	  will_flash.push(std::pair<uint, uint>(i, j));
      }
  while(!will_flash.empty()){
    const uint i = will_flash.top().first;
    const uint j = will_flash.top().second;
    if(s.energies[i][j] == 0) will_flash.pop(); // some on stack would have aleady flashed
    else {
      // flash top now
      flashes++;
      will_flash.pop();
      s.energies[i][j] = 0;
      // handle neighbours
      auto nl = neighbours(i, j);
      for(auto n : nl) {
	uint e = s.energies[n.first][n.second];
	if(e != 0) s.energies[n.first][n.second]++; // if e is 0 it flashed now
	if(s.energies[n.first][n.second] > 9)
	  will_flash.push(n);
      }
    }
  }
  return flashes;
}

int main(int argc, char* argv[]){
  if(argc != 3){
    fprintf(stderr, "Usage: %s <filename> <count>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  const ulong nsteps = std::stoul(argv[2]);
  uint flashes = 0;
  struct octostate e;
  s >> e;
  for(ulong i = 0; i < nsteps; i++){
    flashes += step(e);
  }
  std::cout << flashes << std::endl;
  ulong current_step = nsteps; // already done nsteps
  while(step(e) != 100) current_step++;
  std::cout << ++current_step << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -ggdb -lstdc++ -Wall -Wextra -o day11 day11.cc"
// End:
