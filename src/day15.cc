#include<iostream>
#include<fstream>
#include<vector>
#include<list>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<queue>
typedef unsigned int uint;
typedef std::pair<uint, uint> point;

struct cave{
  const uint width;
  const uint height;
  std::vector<std::vector<uint>> data;
  cave(const uint w, const uint h, std::vector<std::vector<uint>>d)
    : width(w), height(h), data(d){}
  std::list<point> find_neighbours(const point &p) const;
  uint iterative_flow();
  uint dijsktra() const;
};

std::list<point> cave::find_neighbours(const point& p) const{
  const int y = p.first;
  const int x = p.second;
  std::list<std::pair<int, int>> n = {{y, x-1}, {y, x+1}, {y-1, x}, {y+1, x}};
  n.remove_if([this, x, y](std::pair<int, int> e){
    return e.first < 0 || e.second < 0 || e.first >= height || e.second >= width;
  });
  std::list<point> nu;
  for(auto e : n) nu.push_back(e);
  return nu;
}

std::vector<uint> explode_string(std::string s) noexcept{
  std::vector<uint> v;
  for(char c : s) v.push_back(c-48);
  return v;
}

template<> struct std::hash<point>{
  std::size_t operator()(const point& p) const noexcept{
    std::size_t h1 = std::hash<uint>{}(p.first);
    std::size_t h2 = std::hash<uint>{}(p.second);
    return h1 ^ (h2 << 1); // Why ?? cpp/utility/hash example
  }
};

uint cave::iterative_flow(){
  uint crisk = 0; // starting risk is zero
  std::unordered_map<point, uint> m = {{{0, 0}, crisk}};
  std::unordered_set<point> onodes = {{0, 0}};
  std::unordered_set<point> cnodes;
  while(m.find({height-1, width-1}) == m.end()){
    crisk++;
    std::unordered_set<point> nnodes; // new
    std::unordered_set<point> dnodes; // delete
    for(auto &p : onodes){
      std::list<point> n = find_neighbours(p);
      const uint ns = n.size();
      n.remove_if([this, m, crisk, p, cnodes](auto &e){
	if(cnodes.find(e) != cnodes.end()) return true;
	try {return data[e.first][e.second]+m.at(p) > crisk;}
	catch (const std::out_of_range& e){ return true;}});
      for(auto ne : n) {
	nnodes.insert(ne);
	uint via_p_risk = m[p] + data[ne.first][ne.second];
	if(m.find(ne) == m.end()) m[ne] = via_p_risk; // XXX:can use insert_or_assign
      }
      if(ns == n.size()) dnodes.insert(p);
    }
    for(auto p : dnodes) {onodes.erase(p); cnodes.insert(p);}
    for(auto p : nnodes) onodes.insert(p);
  }
  return crisk;
}

uint dis(const point &p){return p.first*p.first+p.second*p.second;}

template <> struct std::less<std::pair<point, uint>> {
  bool operator()(const std::pair<point, uint> & p1,
		  const std::pair<point, uint> & p2) const{
    if(p1.second == p2.second)
      if(p1.first.first == p2.first.first)
	return p1.first.second < p2.first.second;
      else return p1.first.first < p2.first.first;
    else return p1.second < p2.second;
  };
};

template<typename T, class C = std::less<T>> struct pqueue : std::set<T, C>{
  T top() const { return *this->begin();}
  bool update(const T prev, const T current){
    auto i = this->find(prev);
    if(i == this->end()) return false;
    else {
      this->erase(i);
      this->insert(current);
      return true;
    }
  }
};

uint cave::dijsktra() const{
  std::unordered_set<point> visited;
  pqueue<std::pair<point, uint>> unvisited;
  std::unordered_map<point, uint> m;
  for(uint h = 0; h < height; h++)
    for(uint w = 0; w < width; w++)
      {
	const std::pair<point, uint> entry = {{h, w}, -1};
	unvisited.insert(entry);
	m[{h, w}] = -1;
      }
  unvisited.update({{0, 0}, -1}, {{0, 0}, 0}); // set 0,0 risk to 0
  m[{0, 0}] = 0; // initial position and risk
  const point fi_p = {height-1, width-1};
  while(!unvisited.empty()){ // I can also check if neighbours of fi_p are visited
    point p = unvisited.top().first;
    uint risk = unvisited.top().second;
    std::list<point> n = find_neighbours(p);
    for(const auto &np : n){
      uint cell_risk = data[np.first][np.second];
      uint nrisk = risk + cell_risk;
      if(m[np] > nrisk) {
	unvisited.update({np, m[np]}, {np, nrisk});
	m[np] = nrisk;
      }
    }
    unvisited.erase(unvisited.top());
    visited.insert(p);
  }
  return m.at(fi_p);
}

template<class C, class T> struct cave read_cave(std::basic_istream<C, T>& s){
  uint nrows = 1; // initalize with a row to get the number of columns
  std::string line;
  getline(s, line);
  const uint ncols = line.size();
  std::vector<std::vector<uint>> data = {explode_string(line)};
  while(!getline(s, line).eof()){
    nrows++;
    if(line.size() != ncols)
      throw std::runtime_error("Different number of columns");
    else
      data.push_back(explode_string(line));
  }
  return cave(ncols, nrows, data);
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& s, const struct cave &c){
  for(uint i = 0; i < c.height; i++){
    for(uint j = 0; j < c.width; j++)
      s << c.data[i][j];
    s << std::endl;
  }
  return s;
}

struct cave expand_tile_cave(const struct cave &c, const uint scale){
  const uint width = scale * c.width;
  const uint height = scale * c.height;
  std::vector<std::vector<uint>> data(height);
  for(uint h = 0; h < height; h++)
    data[h] = std::vector<uint>(width);
  for(uint h = 0; h < height; h++)
    for(uint w = 0; w < width; w++){
      const uint offset = (h / c.height) + (w / c.width);
      data[h][w] = c.data[h % c.height][w % c.width] + offset;
      if(data[h][w] > 9) data[h][w] -= 9; 
    }
  return cave(width, height, data);
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  struct cave c = read_cave(s);
  // std::cout << c.min_risk_path({c.height-1, c.width-1}, cache) << std::endl;
  std::cout << c.dijsktra() << std::endl;
  struct cave cw = expand_tile_cave(c, 5);
  std::cout << cw.dijsktra() << std::endl;
  return 0;
}


// Local Variables:
// compile-command: "gcc -ggdb -lstdc++ -Wall -Wextra -o day15 day15.cc"
// End:
