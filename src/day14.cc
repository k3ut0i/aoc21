#include<iostream>
#include<fstream>
#include<sstream>

#include<list>
#include<map>


template<class C, class T> std::basic_istream<C, T>&
read_rules(std::basic_istream<C, T>& s, std::map<std::string, char>& r){
  std::string line, match_pair, filler;
  char inchar;
  while(!getline(s, line).eof()){
    std::istringstream l(line);
    l >> std::ws >> match_pair >> filler >> inchar;
    if(filler != "->") throw std::runtime_error("Expecting -> b/w rules");
    r[match_pair] = inchar;
  }
  return s;
}

struct polymer{
  std::list<char> s;
  polymer(std::string s) : s(s.begin(), s.end()){}
  void iterate(std::map<std::string, char>);
  ulong char_diff();
};

void polymer::iterate(std::map<std::string, char> rules){
  std::list<char>::iterator i = next(s.begin());
  while(i != s.end()){
    std::string p = {*prev(i), *i};
    if(rules.find(p) != rules.end())
      s.insert(i, rules[p]);
    i++;
  }
}

ulong polymer::char_diff(){
  std::map<char, ulong> m;
  for(auto c : s){
    if(m.find(c) == m.end()) m[c] = 1;
    else m[c]++;
  }
  ulong max = 0, min = s.size();
  for(auto &p: m)
    if (p.second > max) max = p.second;
    else if(p.second < min) min = p.second;
  std::cout << max << ' ' << min << std::endl;
  return max - min;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& o, const struct polymer& p){
  for(char c : p.s) o << c;
  return o;
}

struct polymer_c{ // condensed representation of the polymer
  std::map<std::string, ulong> data;
  std::array<char, 2> ends; // ends are invariant under iteration 
  polymer_c(std::string);
  void iterate(const std::map<std::string, char> &rules);
  ulong char_diff() const;
};

polymer_c::polymer_c(std::string s){
  ends[0] = s[0];
  ends[1] = s[s.size()-1];
  auto prev = s.begin();
  auto i = std::next(prev);
  while(i != s.end()) {
    std::string p({*prev, *i});
    if (data.find(p) == data.end()) data[p] = 1;
    else data[p]++;
    prev = i; i++;
  }
}

void polymer_c::iterate(const std::map<std::string, char> & rules){
  std::map<std::string, ulong> ndata;
  for(auto &p : data){
    char middle_char = rules.at(p.first);
    std::string e1 = {p.first[0], middle_char};
    std::string e2 = {middle_char, p.first[1]};
    ndata[e1] = ndata.find(e1) == ndata.end() ? p.second : p.second + ndata.at(e1);
    ndata[e2] = ndata.find(e2) == ndata.end() ? p.second : p.second + ndata.at(e2);
  }
  data = ndata;
}

ulong polymer_c::char_diff() const{
  std::map<char, ulong> m;
  for(auto  &p: data){
    for(auto c : p.first)
      if(m.find(c) == m.end()) m[c] = p.second;
      else m[c]+=p.second;
  }
  // ends are the only chars counted once.
  for(auto &e: ends) m[e] = (m[e] + 1)/2;
  for(auto &p: m)
    if(p.first != ends[0] && p.first != ends[1])
      m[p.first] = p.second/2;
  
  ulong max = m.at(ends[0]), min = m.at(ends[0]);
  for(auto &p: m)
    if (p.second > max) max = p.second;
    else if(p.second < min) min = p.second;
  std::cout << max << ' ' << min << std::endl;
  return max - min;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& s, const struct polymer_c& pc){
  for(auto &p: pc.data) s << p.first << ':' << p.second << std::endl;
}

int main(int argc, char* argv[]){
  if(argc != 3){
    fprintf(stderr, "Usage: %s <filename> <iter-count>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  ulong count;
  std::istringstream cs(argv[2]); cs >> count;
  std::string initstring; getline(s, initstring);
  if(s.get() != '\n') throw std::runtime_error("Expecting newline after init string");
  std::map<std::string, char> rules;
  read_rules(s, rules);

  struct polymer_c p(initstring);
  for(ulong i = 0; i < count; i++) p.iterate(rules);
  std::cout << p.char_diff() << std::endl;

  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day14 day14.cc"
// End:
