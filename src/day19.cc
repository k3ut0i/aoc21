#include<iostream>
#include<fstream>
#include<tuple>
#include<list>
#include<set>
#include<map>
#include<optional>
#include<algorithm>

typedef unsigned int uint;
typedef std::tuple<int, int, int> point;
template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& os, const point& p){
  os << std::get<0>(p) << ',' << std::get<1>(p) << ',' << std::get<2>(p);
  return os;
}

point operator+(const point& p1, const point& p2){
  int x1, y1, z1, x2, y2, z2;
  std::tie(x1, y1, z1) = p1;
  std::tie(x2, y2, z2) = p2;
  return {x1+x2, y1+y2, z1+z2};
}
point operator-(const point& p1, const point& p2){
  int x1, y1, z1, x2, y2, z2;
  std::tie(x1, y1, z1) = p1;
  std::tie(x2, y2, z2) = p2;
  return {x1-x2, y1-y2, z1-z2};
}

uint square(int x) { return x * x;}
uint dis_m(const point& a, const point& b){
  int ax, ay, az, bx, by, bz;
  std::tie(ax, ay, az) = a;
  std::tie(bx, by, bz) = b;
  return square(ax-bx) + square(ay-by) + square(az-bz);
}

struct axis{
  enum {x=1, y=2, z=4} alignment;
  bool direction;              // false is +ve, true is -ve
  template<typename T> T get_co(const std::tuple<T, T, T>& p) const;
  template<typename T> void set_co(std::tuple<T, T, T>& p, T val) const;
};

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct axis& a){
  if(a.direction) stream << '-';
  switch(a.alignment){
  case axis::x:
    stream << 'x';
    break;
  case axis::y:
    stream << 'y';
    break;
  case axis::z:
    stream << 'z';
    break;
  }
  return stream;
}

struct axis operator-(const struct axis a){
  struct axis r(a);
  r.direction = !r.direction;
  return r;
}

template<typename T> T axis::get_co(const std::tuple<T, T, T>& p) const{
  T r;
  switch(alignment){
  case x:
    r = std::get<0>(p);
    break;
  case y:
    r = std::get<1>(p);
    break;
  case z:
    r = std::get<2>(p);
    break;
  }
  return r;
}

template<typename T> void axis::set_co(std::tuple<T, T, T> &p, T val) const{
  switch (alignment){
  case x:
    std::get<0>(p) = (direction ? -val : val);
    break;
  case y:
    std::get<1>(p) = (direction ? -val : val);
    break;
  case z:
    std::get<2>(p) = (direction ? -val : val);
    break;
  }
}

bool operator==(const struct axis& a, const struct axis& b){
  return (a.alignment == b.alignment) && (a.direction == b.direction);
}

bool operator!=(const struct axis& a, const struct axis& b){
  return !(a == b);
}

struct axis third_axis(const struct axis& a, const struct axis& b){
  struct axis r;
  if(a.alignment == b.alignment)
    throw "Both axis are parallel.";
  
  switch (a.alignment){
  case axis::x:
    if(b.alignment == axis::y) r = {axis::z, false};
    else r = {axis::y, true};
    break;
  case axis::y:
    if(b.alignment == axis::z) r = {axis::x, false};
    else r = {axis::z, true};
    break;
  case axis::z:
    if(b.alignment == axis::x) r = {axis::y, false};
    else r = {axis::x, true};
    break;
  }
  if(a.direction != b.direction)
    r.direction = !r.direction;
  return r;
}

struct orientation{
  std::tuple<axis, axis, axis> axes;
  orientation() : axes({{axis::x, false}, {axis::y, false}, {axis::z, false}}){}
  orientation(axis, axis, axis);
  template<typename T> std::tuple<T, T, T> convert(const std::tuple<T, T, T>&) const;
  template<typename T> std::tuple<T, T, T> invert(const std::tuple<T, T, T>&) const;

};
const std::list<struct orientation> all_o = {
  {{axis::x, false}, {axis::y, false}, {axis::z, false}},
  {{axis::x, false}, {axis::y, true}, {axis::z, true}},
  {{axis::x, true}, {axis::y, false}, {axis::z, true}},
  {{axis::x, true}, {axis::y, true}, {axis::z, false}},
  {{axis::x, false}, {axis::z, false}, {axis::y, true}},
  {{axis::x, true}, {axis::z, true}, {axis::y, true}},
  {{axis::x, false}, {axis::z, true}, {axis::y, false}},
  {{axis::x, true}, {axis::z, false}, {axis::y, false}},
  {{axis::y, false}, {axis::z, false}, {axis::x, false}},
  {{axis::y, false}, {axis::z, true}, {axis::x, true}},
  {{axis::y, true}, {axis::z, false}, {axis::x, true}},
  {{axis::y, true}, {axis::z, true}, {axis::x, false}},
  {{axis::y, false}, {axis::x, false}, {axis::z, true}},
  {{axis::y, true}, {axis::x, true}, {axis::z, true}},
  {{axis::y, false}, {axis::x, true}, {axis::z, false}},
  {{axis::y, true}, {axis::x, false}, {axis::z, false}},
  {{axis::z, false}, {axis::x, false}, {axis::y, false}},
  {{axis::z, false}, {axis::x, true}, {axis::y, true}},
  {{axis::z, true}, {axis::x, false}, {axis::y, true}},
  {{axis::z, true}, {axis::x, true}, {axis::y, false}},
  {{axis::z, false}, {axis::y, false}, {axis::x, true}},
  {{axis::z, true}, {axis::y, true}, {axis::x, true}},
  {{axis::z, false}, {axis::y, true}, {axis::x, false}},
  {{axis::z, true}, {axis::y, false}, {axis::x, false}},
};

bool valid_axes(const std::tuple<axis, axis, axis>& ax){
  struct axis a, b, c;
  std::tie(a, b, c) = ax;
  if(a.alignment + b.alignment + c.alignment != 7)
    throw std::runtime_error("Set of axis alignments must be {x, y, z}");
  return (c == third_axis(a, b));
}

orientation::orientation(axis a, axis b, axis c) : axes({a, b, c}){
  if(!valid_axes({a, b, c}))
    throw std::runtime_error("These axes do not form Right handed system");  
}

template<typename T> std::tuple<T, T, T>
orientation::convert(const std::tuple<T, T, T> &p) const{
  std::tuple<T, T, T> r;
  axis x, y, z; std::tie(x, y, z) = axes;
  std::get<0>(r) = (x.direction ? -1 : 1) * x.get_co(p);
  std::get<1>(r) = (y.direction ? -1 : 1) * y.get_co(p);
  std::get<2>(r) = (z.direction ? -1 : 1) * z.get_co(p);
  return r;
}

template<typename T> std::tuple<T, T, T>
orientation::invert(const std::tuple<T, T, T> &p) const{
  std::tuple<T, T, T> r;
  axis x, y, z; std::tie(x, y, z) = axes;
  x.set_co(r, std::get<0>(p));
  y.set_co(r, std::get<1>(p));
  z.set_co(r, std::get<2>(p));
  return r;
}

bool operator==(const struct orientation& a, const struct orientation& b){
  return a == b;
}

struct orientation operator*(const struct orientation& o1,
			     const struct orientation& o2){
  axis x, y, z;
  std::tie(x, y, z) = o1.invert<axis>(o2.axes);
  return orientation(x, y, z);
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct orientation& o){
  axis x, y, z; std::tie(x, y, z) = o.axes;
  return stream << '[' << x << ',' << y << ',' << z << ']';
}

void test_orientation(){
  std::cout << "Testing all possible Orientations\n";
  if(!std::all_of(all_o.begin(),all_o.end(), [](auto a){return valid_axes(a.axes);}))
    throw std::runtime_error("all_o does has a invalid orientation.");
  std::cout << "Testing Orientation\n";
  struct orientation o = {{axis::z, true}, {axis::x, false}, {axis::y, true}};
  const point a = {1, 2, 3};
  const point b = o.convert(a);
  struct orientation o1 = {{axis::y, false}, {axis::z, false}, {axis::x, false}};
  const point c = o1.convert(b);
  const point b1 = o1.invert(c);
  const point a1 = o.invert(b1);
  struct orientation om = o * o1;
  std::cout << "Composing Orientations:\t"
	    << o << " * " << o1 << " = " << om << std::endl;
  point d = om.convert(a);
  point ad = om.invert(d);
  std::cout << "Converting points:\t" << a << " --> "
	    << b << " --> " << c << " also directly to: " << d << std::endl;
  std::cout << "Inverting points:\t" << a1 << " <-- "
	    << b1 << " <-- " << c << " also directly to: " << ad << std::endl;
  if(ad != a) throw "Composition of orientations is not working.";
}

struct scanner{
  uint id;
  std::list<point> obs;
private:
  point pos; // always set w.r.t origin
};

typedef std::set<std::tuple<point, point, uint>> obs_digest;

template<> struct std::less<std::tuple<point, point, uint>>{
  bool operator()(const std::tuple<point, point, uint>& a,
			    const std::tuple<point, point, uint>& b) const
  {
    point pa1, pa2, pb1, pb2; uint da = 0, db = 0;
    std::tie(pa1, pa2, da) = a;
    std::tie(pb1, pb2, db) = b;
    return
      da < db || (da == db && pa1 < pa2) || (da == db && pa1 == pa2 && pb1 < pb2);
  }
};

obs_digest process_obs(const struct scanner& s){
  obs_digest m;
  for(auto i = s.obs.begin(); i != s.obs.end(); i++)
    for(auto j = std::next(i); j != s.obs.end(); j++)
      m.insert({*i, *j, dis_m(*i, *j)});
  return m;
}

std::optional<std::pair<point, struct orientation>>
match_scanner(const obs_digest& a, const obs_digest& b){
  point a1, b1, a2, b2;
  uint da, db;
  bool found = false;
  constexpr point dummy = {INTMAX_MAX, INTMAX_MAX, INTMAX_MAX}; // for searching
  std::list<std::pair<std::pair<point, point>,
		      std::pair<point, point>>> possible_matches;
  for(const auto & ppd : a){
    std::tie(a1, a2, da) = ppd;
    auto i = b.lower_bound({dummy, dummy, da});
    if(i != b.end()){ // If a matching distance is found
      std::tie(b1, b2, db) = *i;
      while(db == da){
	possible_matches.push_back({{a1, a2}, {b1, b2}});
	i++;
	std::tie(b1, b2, db) = *i;
      }
    }
  }
  // For each possible match create an orientation and check for points that
  // satisfy the orientation and do not violate the constraints
  // If twelve points are common b/w a and b return True, else False.
  struct orientation o_p;
  point origin;
  for(auto x : possible_matches){
    std::tie(a1, a2) = x.first, std::tie(b1, b2) = x.second;

    bool found_possible = false;
    // a1 = b1 and a2 = b2
    for(const auto o : all_o){
      if(a1 - o.invert(b1) == a2 - o.invert(b2)){
	origin = a1 - o.invert(b1);
	o_p = o;
	found_possible = true;
      } else if(a1 - o.invert(b2) == a2 - o.invert(b1)){
	origin = a1 - o.invert(b2);
	o_p = o;
	found_possible = true;
      }
      if(found_possible){
	uint count = 0;
	for(auto y : possible_matches){
	  if(count >= 12) goto FOUND_MATCH;
	  std::tie(a1, a2) = y.first; std::tie(b1, b2) = y.second;
	  if((a1 - o.invert(b1) == a2 - o.invert(b2)) ||
	     (a1 - o.invert(b2) == a2 - o.invert(b1)))
	    count++;
	}
      }
    }
  }
  return {};

 FOUND_MATCH:
  return {{origin, o_p}};
}

template<class C, class T> std::basic_istream<C, T>&
operator>>(std::basic_istream<C, T>& stream, const std::string&& s){
  for(auto c : s){
    char c_s = stream.get();
    if(c != c_s)
      throw std::runtime_error("Could not read: " + s + " from the stream, "
			       + "Expecting:[" + std::string(1, c)
			       + "]Got:[" + std::string(1, c_s) + "]");
  }
  return stream;
}

template<class C, class T> std::basic_istream<C, T>&
operator>>(std::basic_istream<C, T>& stream, struct scanner& sc){
  uint id;
  stream >> std::noskipws >> "--- scanner " >> id >> " ---\n";
  sc.id = id;
  while(!stream.eof()){
    int a, b, c;
    stream >> std::noskipws >> a >> "," >> b >> "," >> c >> "\n";
    sc.obs.push_back({a, b, c});
    if(stream.peek() == '\n') break;
  }  
  return stream;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct scanner& sc){
  using std::get;
  stream << "--- scanner " << sc.id << " ---\n";
  for(const auto & o : sc.obs)
    stream << get<0>(o) << ',' << get<1>(o) << ',' << get<2>(o) << std::endl;
  return stream;
}

template<class C, class T> std::list<struct scanner>
read_input(std::basic_istream<C, T>& s){
  std::list<struct scanner> is;
  while (!s.eof()) {
    struct scanner sc;
    s >> sc;
    is.push_back(sc);
    if(s.get() != '\n') break;
  }
  return is;
}

void test_matching(const std::list<struct scanner>& is){
  auto s1 = *is.begin();
  auto s2 = *std::next(is.begin());
  auto od1 = process_obs(s1);
  auto od2 = process_obs(s2);
  std::cout << "Trying to match\n";
  auto match = match_scanner(od1, od2);
  if(match){
    point origin = match.value().first;
    struct orientation o = match.value().second;
    std::cout << "Origin: " << origin << " Orientation: " << o << std::endl;
  }
  else
    throw std::runtime_error("Scanners 0 and 1 do not match");
}

// Return the number of beacons and populate the connectivity of scanners in graph
uint analyze_scanners(const std::list<struct scanner>& ss,
		      std::map<uint, std::list<uint>>& graph)
{
  typedef struct scanner const* s_t;
  graph.clear();
  std::set<s_t> unreached;
  for(auto i = std::next(ss.begin()); i != ss.end(); i++) unreached.insert(&*i);
  std::map<s_t, std::pair<point, struct orientation>> known =
    {{&*ss.begin(), {{0, 0, 0}, orientation({axis::x, false},
					    {axis::y, false},
					    {axis::z, false})}}};
  auto i = known.begin();
  while(!unreached.empty()){
    s_t s = i->first;
    point origin = i->second.first;
    struct orientation o = i->second.second;
    std::list<s_t> in_range;
    auto j = unreached.begin();
    while(j != unreached.end()){
      auto match = match_scanner(process_obs(*s), process_obs(**j));
      if(match) {
	known[*j] = match.value();
	unreached.erase(j);
      }
      j++;
    }
    i++;
  }
  std::set<point> beacon;
  for(const auto & s : ss){
    for(auto ob : s.obs){
      point s_origin = known[&s].first;
      struct orientation s_o = known[&s].second;
      beacon.insert(s_o.invert(ob)+s_origin);
    }
  }
}

int main(int argc, char* argv[0]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  const std::list<struct scanner> is = read_input(s);
  test_matching(is);
  // test_orientation();
  return 0;
}

// Local Variables:
// compile-command: "gcc -ggdb -lstdc++ -std=c++17 -Wall -Wextra -o day19 day19.cc"
// End:
