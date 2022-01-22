#include<iostream>
#include<fstream>
#include<tuple>
#include<list>
#include<map>

typedef unsigned int uint;
typedef std::tuple<int, int, int> point;
template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& os, const point& p){
  os << std::get<0>(p) << ',' << std::get<1>(p) << ',' << std::get<2>(p);
  return os;
}

struct axis{
  enum {x=1, y=2, z=4} alignment;
  bool direction;              // false is +ve, true is -ve
  int get_co(const point& p);
  void set_co(point& p, int val);
};

int axis::get_co(const point& p){
  switch(alignment){
  case x:
    return std::get<0>(p);
  case y:
    return std::get<1>(p);
  case z:
    return std::get<2>(p);
  }
}

void axis::set_co(point &p, int val){
  switch (alignment){
  case x:
    std::get<0>(p) = (direction ? -1 : 1) * val;
    break;
  case y:
    std::get<1>(p) = (direction ? -1 : 1) * val;
    break;
  case z:
    std::get<2>(p) = (direction ? -1 : 1) * val;
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
  orientation(axis, axis, axis);
  point convert(const point&);
  point invert(const point&);

};

orientation::orientation(axis a, axis b, axis c) : axes({a, b, c}){
  if(a.alignment + b.alignment + c.alignment != 7)
    throw std::runtime_error("Set of axis alignments must be {x, y, z}");
  if(c != third_axis(a, b))
    throw std::runtime_error("These axes do not form Right handed system");
}

point orientation::convert(const point &p){
  point r;
  axis x, y, z; std::tie(x, y, z) = axes;
  std::get<0>(r) = (x.direction ? -1 : 1) * x.get_co(p);
  std::get<1>(r) = (y.direction ? -1 : 1) * y.get_co(p);
  std::get<2>(r) = (z.direction ? -1 : 1) * z.get_co(p);
  return r;
}

point orientation::invert(const point &p){
  point r;
  axis x, y, z; std::tie(x, y, z) = axes;
  x.set_co(r, std::get<0>(p));
  y.set_co(r, std::get<1>(p));
  z.set_co(r, std::get<2>(p));
  return r;
}

bool operator==(const struct orientation& a, const struct orientation& b){
  return a == b;
}

void test_orientation(){
  std::cout << "Testing Orientation\n";
  struct orientation o = {{axis::z, true}, {axis::x, false}, {axis::y, true}};
  point a = {1, 2, 3};
  point b = o.convert(a);
  point c = o.invert(b);
  std::cout << a << ' ' << b << ' ' << c << std::endl;
  if(a != c) throw "invert of convert is not id.";
}

struct scanner{
  uint id;
  std::list<point> obs;
private:
  point pos;
};

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
    std::cout << sc;
    if(s.get() != '\n') break;
  }
  return is;
}

int main(int argc, char* argv[0]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  auto is = read_input(s);
  test_orientation();
  return 0;
}

// Local Variables:
// compile-command: "gcc -ggdb -lstdc++ -std=c++17 -Wall -Wextra -o day19 day19.cc"
// End:
