#include<iostream>
#include<fstream>
#include<bitset>
#include<list>
#include<sstream>

struct pkt{
  uint version;
  uint type_id;
  std::list<bool> num;
  std::list<struct pkt> subp;
  pkt(uint v, uint t, std::list<struct pkt> s)
    : version(v), type_id(t), subp(s){}
  pkt(uint v, std::list<bool> num) : version(v), type_id(4), num(num){}
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
    s << istring << p.version << ' ' << to_numeric(p.num) << std::endl;
  else {
    s << istring << p.version << std::endl;
    for(const auto & ps : p.subp)
      write_pkt_summary(s, ps, ilevel+1);
  }
}

template<class C, class T> struct pkt
read_pkt(std::basic_istream<C, T>& s, std::list<bool>& bl, uint& bused){
  if(s.eof()) throw std::runtime_error("unexpected EOF");
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

    if(length_type){
      while(bl.size() < 11){
	if(s.get(c).eof()) throw std::runtime_error("Unexpected EOF");
	b = from_hex(c);
	bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
      }
      uint n = 0;
      for(uint i = 0; i < 11; i++) {
	n = 2*n+bl.front();
	bl.pop_front();
	bused++;
      }
      std::cout << "Reading " << n << " packets\n";
      std::list<struct pkt> subp;
      for(uint i = 0; i < n; i++)
	subp.push_back(read_pkt(s, bl, bused));
      for(const auto& sp: subp) write_pkt_summary(std::cout, sp, 0);
      return pkt(version, type_id, subp);
    } else {
      while(bl.size() < 15){
	if(s.get(c).eof()) throw std::runtime_error("Unexpected EOF");
	b = from_hex(c);
	bl.insert(bl.end(), {b[3], b[2], b[1], b[0]});
      }
      uint n = 0;
      for(uint i = 0; i < 15; i++) {
	n = 2*n+bl.front();
	bl.pop_front();
      }
      uint new_bused = 0;
      std::list<struct pkt> subp;
      std::cout << "Reading packets in [" << n << "] bits\n";
      while(new_bused < n)
	subp.push_back(read_pkt(s, bl, new_bused));
      for(const auto& sp: subp) write_pkt_summary(std::cout, sp, 0);
      return pkt(version, type_id, subp);
    }
  }
}


int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<bool> bl;
  uint bused;
  struct pkt p(read_pkt(s, bl, bused));
  write_pkt_summary(std::cout, p, 0);
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day16 day16.cc"
// End:
