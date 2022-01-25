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
bool operator<(const struct axis& a, const struct axis& b){
  return a.alignment < b.alignment
    || (a.alignment == b.alignment && a.direction < b.direction);
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

std::optional<struct orientation> find_orientation(const point &p, const point&pi){
  for(const auto &o : all_o){
    if (p == o.invert(pi)) return o;
  }
  return {};
}
bool operator==(const struct orientation& a, const struct orientation& b){
  return a.axes == b.axes;
}

struct orientation operator*(const struct orientation& o1,
			     const struct orientation& o2){
  point pi = {1, 2, 3};
  point p = o1.invert(o2.invert(pi));
  auto omatch = find_orientation(p, pi);
  if(omatch)
    return omatch.value();
  else
    throw std::runtime_error("Could not find the composition of orientations.");
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct orientation& o){
  axis x, y, z; std::tie(x, y, z) = o.axes;
  return stream << '[' << x << ',' << y << ',' << z << ']';
}

bool operator<(const struct orientation& o1, const struct orientation o2){
  return o1.axes < o2.axes;
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
  if(ad != a)
    throw std::runtime_error("Composition of orientations is not working.");
  struct orientation s1 = {{axis::x, true}, {axis::y, false}, {axis::z, true}};
  struct orientation s4f1 = {{axis::z, true}, {axis::x, false}, {axis::y, true}};
  struct orientation s4 = {{axis::z, false}, {axis::x, true}, {axis::y, true}};
  if(!(s4 == s1 * s4f1))
    throw std::runtime_error("Composition of s4 relative to s1 is not s4 direct");
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
match_scanner(const struct scanner& as, const struct scanner& bs){
  const obs_digest a = process_obs(as);
  const obs_digest b = process_obs(bs);
  point a1, b1, a2, b2;
  uint da, db;
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
  std::set<std::pair<point, struct orientation>> ol;
  struct orientation o_p;
  point origin;
  for(auto x : possible_matches){
    std::tie(a1, a2) = x.first, std::tie(b1, b2) = x.second;
    // a1 = b1 and a2 = b2
    for(const auto &o : all_o){
      bool found_possible = false;
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
	std::set<point> common1, common2;
	for(auto y : possible_matches){
	  std::tie(a1, a2) = y.first; std::tie(b1, b2) = y.second;
	  if((a1 - o_p.invert(b1) == a2 - o_p.invert(b2)) ||
	     (a1 - o_p.invert(b2) == a2 - o_p.invert(b1))){
	    common1.insert(a1);
	    common1.insert(a2);
	    common2.insert(b1);
	    common2.insert(b2);
	    count++;
	  }
	  if(count >= 12) {
	    // std::cout << "Common points b/w scanners: " << as.id << ',' << bs.id << std::endl;
	    // std::cout << "Viewed from : " << as.id << std::endl;
	    // for(auto p : common1) std::cout << p << std::endl;
	    // std::cout << "Viewed from : " << bs.id << std::endl;
	    // for(auto p : common2) std::cout << p << std::endl;
	    ol.insert({origin, o_p});
	    goto FOUND;
	  }
	}
	
	// for(auto y : bs.obs){
	//   if(as.obs.find(origin+o_p.invert(y)) != as.obs.end()) count++;
	// }
	// if(count >= 12) ol.insert({origin, o_p});
      }
    }
  }
 FOUND:
  if(ol.size()==1) return {{origin, o_p}};
  else if(ol.size() > 1){
    std::cout << "Matching: " << as.id << ',' << bs.id
	      << "Possible Origins: ";
    for(const auto& p : ol) std::cout << p.first << '[' << p.second << "], ";
    std::cout << std::endl;
  return {};
  }
  else return {};
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
  for(const auto& s1: is)
    for(const auto& s2: is)
      if(s1.id != s2.id)
	if(match_scanner(s1, s2))
	  std::cout << "Matched: " << s1.id << ' ' << s2.id << std::endl;
    
  auto s0 = *is.begin();
  auto s1 = *std::next(is.begin());

  std::cout << "Trying to match\n";
  auto match = match_scanner(s0, s1);
  if(match){
    point origin = match.value().first;
    struct orientation o = match.value().second;
    std::cout << "Origin: " << origin << " Orientation: " << o << std::endl;
  } else throw std::runtime_error("Scanners 0 and 1 do not match");

  auto s2 = *std::next(is.begin(), 2);
  auto s4 = *std::next(is.begin(), 4);
  auto match2 = match_scanner(s4, s2);
  if(match2){
    point origin = match2.value().first;
    struct orientation o = match2.value().second;
    std::cout << "Matched 4 and 2: Origin: " << origin << " Orientation: "
	      << o << std::endl;
  } else throw std::runtime_error("Scanners 0 and 1 do not match");
}

// Return the number of beacons and populate the connectivity of scanners in graph
std::pair<uint, uint> analyze_scanners(const std::list<struct scanner>& ss,
				       std::map<uint, std::list<uint>>& graph)
{
  typedef struct scanner const* s_t;
  graph.clear();
  std::set<s_t> unreached;
  for(auto i = std::next(ss.begin()); i != ss.end(); i++) unreached.insert(&*i);
  std::map<s_t, std::pair<point, struct orientation>> m =
    {{&*ss.begin(), {{0, 0, 0}, orientation({axis::x, false},
					    {axis::y, false},
					    {axis::z, false})}}};
  std::list<s_t> known = {&*ss.begin()};
  auto i = known.begin();
  while(!unreached.empty()){
    if(i == known.end()) break;
      // throw std::runtime_error("Ran out of known before reaching all nodes ");
    s_t s = *i;
    std::list<s_t> in_range;
    auto j = unreached.begin();
    while(j != unreached.end()){
      auto match = match_scanner(*s, **j);
      auto next_j = std::next(j);
      if(match) {
	point r_origin = match.value().first;
	struct orientation r_o = match.value().second;
	point s_origin = m[s].first;
	struct orientation s_o = m[s].second;
	m[*j] = {s_origin + s_o.invert(r_origin),s_o*r_o};
	known.push_back(*j);
	unreached.erase(j);
      }
      j = next_j;
    }
    i++;
  }
  // std::set<std::pair<uint, point>> beacon;
  std::set<point> beacon;
  for(const auto & s : ss){
    for(auto ob : s.obs){
      point s_origin = m[&s].first;
      struct orientation s_o = m[&s].second;
      point p = s_o.invert(ob)+s_origin;
      beacon.insert(p);
      // beacon.insert({s.id, p});
    }
  }
  uint max_dis = 0;
  constexpr static auto abs_f = [](int a)->uint{return a > 0 ? a : -a;};
  constexpr static auto man_dis_f = [](point a, point b)->uint {
    return 
      abs_f(std::get<0>(a)-std::get<0>(b))+
      abs_f(std::get<1>(a)-std::get<1>(b))+
      abs_f(std::get<2>(a)-std::get<2>(b))
      ;
  };
  for(auto i = ss.begin(); i != ss.end(); i++)
    for(auto j = std::next(i); j != ss.end(); j++){
      point oi = m[&*i].first;
      point oj = m[&*j].first;
      uint d = man_dis_f(oi, oj);
      if(d > max_dis) max_dis = d;
    }
  // for(const auto &pi : beacon)
  //   std::cout << pi.first << ' ' << pi.second << std::endl;
  return {beacon.size(), max_dis};
}

void test_o_com(){
  struct orientation s1 = {{axis::x, true}, {axis::y, false}, {axis::z, true}};
  struct orientation s4f1 = {{axis::z, true}, {axis::x, false}, {axis::y, true}};
  struct orientation s4 = {{axis::z, false}, {axis::x, true}, {axis::y, true}};
  point a = {1, 2, 3};
  point b = s4f1.invert(a);
  point c = s1.invert(b);
  point d = (s1 * s4f1).invert(a);
  std::cout << '[' << a << "] s4f1-> [" << b << "] s1-> [" << c
	    << "] direct s4: [" << d << ']' << std::endl;
  if(!(s4 == s1 * s4f1))
    throw std::runtime_error("Composition of orientations not working.");
}

int main(int argc, char* argv[0]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  const std::list<struct scanner> is = read_input(s);
  std::map<uint, std::list<uint>> graph;
  // test_matching(is);
  // test_o_com();
  // test_orientation();
  auto ans = analyze_scanners(is, graph);
  std::cout << "Beacons: " << ans.first << " Max dis: " << ans.second << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -ggdb -lstdc++ -std=c++17 -Wall -Wextra -o day19 day19.cc"
// End:
