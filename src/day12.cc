#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <list>
#include <stack>
#include <utility>
#include <queue>
#include <algorithm>
using std::string;
struct Graph {
  std::multimap<string, string> m;
  std::set<string> nodes;
  template<class C, class T> Graph(std::basic_istream<C,T> &s){
    while(!s.eof()){
      string a; string b;
      char c;
      while(true){
	c = s.get();
	if(c == '-') break;
	a += c;
      }
      while(true){
	c = s.get();
	if (c == '\n') break;
	b += c;
      } 
      while(std::isspace(s.peek())) s.get(); // slurp preceding whitespace
      m.insert({{a, b}, {b, a}});
      nodes.insert({a, b});
    }
  }
};

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& s, struct Graph g){
  for(auto &p: g.m) s << p.first << '-' << p.second << std::endl;
  return s;
}

inline bool cave_small_p(std::string name) {return name[0] <= 122 && name[0] >= 97;}

template<typename K, typename V> std::set<V>
get_vals(std::multimap<K, V> m, K k) {
  std::set<V> vals; vals.clear();
  auto ip = m.equal_range(k);
  for(auto i = ip.first; i != ip.second; i++) vals.insert(i->second);
  return vals;
}

template<typename E, class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const std::set<E> &s) {
  for(auto &e : s) stream << e << ',';
  return stream;
}

template<typename K, typename V> void dump_map(const std::map<K, V> &m){
  for(auto &p : m) std::cout << p.first << ':' << p.second << std::endl;
}

std::set<string>
small_caves(const std::list<std::pair<std::string, std::set<std::string>>>& c){
  std::set<string> s; s.clear();
  for(const auto &p : c) if(cave_small_p(p.first)) s.insert(p.first);
  return s;
}

uint count_paths(Graph g){
  using std::string; using std::pair; using std::list;
  using std::set;
  // state variables
  uint count = 0;
  list<pair<string,set<string>>> choices = {{"start", get_vals(g.m, string("start"))}};
  // keep counting while choice points exist
  while(!choices.empty()) {
    auto c = choices.back();
    string c_node = c.first;
    set<string> c_choices = c.second;
    if(c_node == "end") {
      count++;
      choices.erase(--choices.end());
      // for(auto &p : choices)
      // 	std::cout << p.first << " ";
      // std::cout << std::endl;
    } else {
      if(c_choices.empty()) {

	if (c_node == "start") break; // get out of while loop
	else {
	  choices.erase(--choices.end());
	}
      } else {
	c_node = *c_choices.begin(); // get a choice
	choices.back().second.erase(c_node); // and remove that
	auto new_choices = get_vals(g.m, c_node);
	list<set<string>::iterator> rm_nodes;
	for (auto i = new_choices.begin(); i != new_choices.end(); i++)
	  if(small_caves(choices).count(*i) > 0) rm_nodes.push_back(i);
	for(auto &i : rm_nodes) new_choices.erase(i);
	choices.push_back({c_node, new_choices});
      }
    }
  }
  return count;
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  struct Graph g(s);
  std::cout << count_paths(g) << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day12 day12.cc"
// End:
