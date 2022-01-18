#include<iostream>
#include<fstream>
#include<bitset>
#include<list>
#include<sstream>
#include<map>
#include<functional>

typedef unsigned int uint;
typedef unsigned long long ull; // How big is this exactly? 128 bits?

template<typename R, typename T, class BiF, class UnF>
R reduce(const std::list<T>& l, BiF f, UnF u){
  auto i = l.begin();
  R v = u(*i);
  i++;
  for(;i != l.end(); i++)v = f(v, u(*i));
  return v;
}

const std::map<uint, std::function<ull(ull, ull)>> m = {
  {0, [](ull a, ull b){return a + b;}},
  {1, [](ull a, ull b){return a * b;}},
  {2, [](ull a, ull b){return a < b ? a : b;}},
  {3, [](ull a, ull b){return a > b ? a : b;}},
  {5,  [](ull a, ull b){return a > b;}},
  {6,  [](ull a, ull b){return a < b;}},
  {7,  [](ull a, ull b){return a == b;}}
};

struct pkt{
  uint version;
  uint type_id;
  std::list<bool> num;
  std::list<struct pkt> subp;
  pkt(uint v, uint t, std::list<struct pkt> s)
    : version(v), type_id(t), subp(s){}
  pkt(uint v, std::list<bool> num) : version(v), type_id(4), num(num){}
  ull reduce_pkt() const;
};

std::bitset<4> from_hex(const char c){
  std::istringstream s ({c});
  uint i;
  s >> std::hex >> i;
  std::bitset<4> b(i);
  return b;
}

unsigned long to_numeric(const std::list<bool> &bl){
  unsigned long n = 0;
  for(auto b : bl) n = 2*n + b;
  return n;
}

template<class C, class T> void
write_pkt_summary(std::basic_ostream<C, T>& s, const struct pkt& p,
		  const uint ilevel)
{
  std::string istring(ilevel, ' ');
  if(p.type_id == 4)
    s << istring << p.version << " [" << to_numeric(p.num) << "]\n";
  else {
    s << istring << p.version << ' ' << p.type_id << std::endl;
    for(const auto & ps : p.subp)
      write_pkt_summary(s, ps, ilevel+1);
  }
}

template<class C, class T> struct pkt
read_pkt(std::basic_istream<C, T>& s, std::list<bool>& bl, uint& bused){
  char c;
  std::bitset<4> b;
  while(bl.size() < 7){
    s.get(c);
    b = from_hex(c);
    bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
  }
  uint version = 0;
  for(uint i = 0; i < 3; i++) {
    version = 2*version+bl.front();
    bl.pop_front();
    bused++;
  }
  uint type_id = 0;
  for(uint i = 0; i < 3; i++) {
    type_id = 2*type_id+bl.front();
    bl.pop_front();
    bused++;
  }
  if(type_id == 4) { // literal values
    bool last_group = false;
    std::list<bool> num;
    while(!last_group){
      while(bl.size() < 5){
	s.get(c);
	b = from_hex(c);
	bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
      }
      last_group = !bl.front();
      bl.pop_front();
      bused++;
      for(uint i = 0; i < 4; i++) {
	num.push_back(bl.front());
	bl.pop_front();
	bused++;
      }
    }
    return pkt(version, num);
  } else { // compound values
    bool length_type = bl.front();
    bl.pop_front();
    bused++;
    if(length_type){
      while(bl.size() < 11){
	s.get(c);
	b = from_hex(c);
	bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
      }
      uint n = 0;
      for(uint i = 0; i < 11; i++) {
	n = 2*n+bl.front();
	bl.pop_front();
	bused++;
      }
      std::list<struct pkt> subp;
      for(uint i = 0; i < n; i++)
	subp.push_back(read_pkt(s, bl, bused));
      return pkt(version, type_id, subp);
    } else {
      while(bl.size() < 15){
	s.get(c);
	b = from_hex(c);
	bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
      }
      uint n = 0;
      for(uint i = 0; i < 15; i++) {
	n = 2*n+bl.front();
	bl.pop_front();
	bused++;
      }
      uint new_bused = 0;
      std::list<struct pkt> subp;
      while(new_bused < n)
	subp.push_back(read_pkt(s, bl, new_bused));
      bused += new_bused;
      return pkt(version, type_id, subp);
    }
  }
}

uint version_sum(const struct pkt& p){
  uint vsum = p.version;
  if(p.type_id != 4){
    for(const auto &sp : p.subp)
      vsum += version_sum(sp);
  }
  return vsum;
}

ull pkt::reduce_pkt() const{
  ull v = 0;
  if(type_id == 4) v = to_numeric(num);
  else {
    v = reduce<ull>(subp, m.at(type_id), [](const struct pkt &p){return p.reduce_pkt();});
  }
  return v;
}

void run_test(std::string s, ull expected){
  std::istringstream ss(s);
  std::list<bool> bl;
  uint bused;
  struct pkt p(read_pkt(ss, bl, bused));
  std::cout << "Expected: " << expected
	    << " Got: " << p.reduce_pkt() << '\t' << s << std::endl;
}

void run_tests(){
  const std::map<std::string, uint> t = {
    {"C200B40A82", 3},
    {"04005AC33890", 54},
    {"880086C3E88112", 7},
    {"CE00C43D881120", 9},
    {"D8005AC2A8F0", 1},
    {"F600BC2D8F", 0},
    {"9C005AC2F8F0", 0},
    {"9C0141080250320F1802104A08", 1}
  };
  for(const auto &p: t) run_test(p.first, p.second);
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\nRunning some tests:\n", argv[0]);
    run_tests();
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<bool> bl;
  uint bused;
  struct pkt p(read_pkt(s, bl, bused));
  std::cout << version_sum(p) << ' ' << p.reduce_pkt() << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day16 day16.cc"
// End:
