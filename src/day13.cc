#include<iostream>
#include<cstdio>
#include<fstream>
#include<vector>
#include<list>
#include<sstream>
#include<utility>

typedef unsigned int uint;

struct tog{
  uint width;                          // no. of columns - x-axis
  uint height;                         // no. of rows - y-axis
  std::vector<std::vector<bool>> paper;      // vector of rows
  tog(const std::list<std::pair<uint, uint>>& points);
  void fold_along(const bool along_yaxis, const uint val);// false x-axis vertical; to topleft 
  uint count_dots() const;
};

tog::tog(const std::list<std::pair<uint, uint>>& points){
  uint max_x = 0, max_y = 0;
  for(auto &p : points) {
    if(max_x < p.first) max_x = p.first;
    if(max_y < p.second) max_y = p.second;
  }
  width = max_x+1;
  height = max_y+1;
  paper.clear();
  paper = std::vector<std::vector<bool>>(height);
  for(uint i = 0; i < height; i++)
    paper[i] = std::vector<bool>(width, false);
  for(auto &p : points) paper[p.second][p.first] = true;

}

uint tog::count_dots() const{
  uint count = 0;
  for(uint y = 0; y < height; y++)
    for(uint x = 0; x < width; x++)
      if(paper[y][x]) count++;
  return count;
}

void tog::fold_along(const bool along_yaxis, const uint val){
  const uint final_ = along_yaxis ? height : width;
  const uint other_ = along_yaxis ? width : height;
  for(uint i = val; i < final_; i++)
    for(uint j = 0; j < other_; j++)
      if(2*val >= i){
	if(along_yaxis) paper[2*val-i][j] = paper[2*val-i][j] || paper[i][j];
	else paper[j][2*val-i] = paper[j][2*val-i] || paper[j][i];
      }
  if(along_yaxis) height = val;
  else width = val;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct tog& p){
  for(uint y = 0; y < p.height; y++){
    for(uint x = 0; x < p.width; x++){
      stream << (p.paper[y][x] ? '#' : '.');
    }
    stream << std::endl;
  }
  return stream;
}

template<class C, class T> std::basic_istream<C, T>&
read_input(std::basic_istream<C, T>& stream,
	   std::list<std::pair<uint, uint>> & points,
	   std::list<std::pair<bool, uint>> & folds)
{
  std::string line;
  while(!getline(stream, line).eof() && line != "") {
    std::istringstream s(line);
    uint x, y;
    s >> x;
    if(s.get() != ',') throw std::runtime_error("Expecting comma b/w numbers");
    s >> y;
    points.push_back(std::make_pair(x, y));
  }
  while(!getline(stream, line).eof() && line != ""){
    std::istringstream s(line);
    char axis; uint val; std::string filler; char eq;
    s >> filler >> filler >> axis >> eq >> val;
    folds.push_back(std::make_pair(axis == 'y', val));
  }
  return stream;
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<std::pair<uint, uint>> points;
  std::list<std::pair<bool, uint>> folds;
  read_input(s, points, folds);
  struct tog o(points);
  o.fold_along(folds.front().first, folds.front().second);
  uint ans = o.count_dots();
  for(auto p = folds.begin()++; p != folds.end(); p++)
    o.fold_along(p->first, p->second);
  std::cout << ans << std::endl << o;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb day13.cc -o day13"
// End:



