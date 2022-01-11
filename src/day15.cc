#include<iostream>
#include<fstream>
#include<vector>
#include<list>
#include<unordered_map>
#include<unordered_set>

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
      // std::cout << std::unitbuf << crisk << ':'
      // 		<< '(' << p.first << ',' << p.second << ')' << ',';
      std::list<point> n = find_neighbours(p);
      const uint ns = n.size();
      n.remove_if([this, m, crisk, p, cnodes](auto &e){
	if(cnodes.find(e) != cnodes.end()) return true;
	try {return data[e.first][e.second]+m.at(p) > crisk;}
	catch (const std::out_of_range& e){ return true;}});
      for(auto ne : n) {
	nnodes.insert(ne);
	uint via_p_risk = m[p] + data[ne.first][ne.second];
	if(m.find(ne) == m.end()) m[ne] = via_p_risk;
      }
      if(ns == n.size()) dnodes.insert(p);
    }
    for(auto p : dnodes) {onodes.erase(p); cnodes.insert(p);}
    for(auto p : nnodes) onodes.insert(p);
    // for(auto p : m)
    //   std:: cout << '[' << p.first.first << ',' << p.first.second
    // 		 << ':' << p.second << ']';
    std::cout << crisk << ' ' << onodes.size() << std::endl;
  }
  return crisk;
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

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  struct cave c = read_cave(s);
  // std::cout << c.min_risk_path({c.height-1, c.width-1}, cache) << std::endl;
  std::cout << c.iterative_flow() << std::endl;
  return 0;
}


// Local Variables:
// compile-command: "gcc -O2 -lstdc++ -Wall -Wextra -o day15 day15.cc"
// End:
