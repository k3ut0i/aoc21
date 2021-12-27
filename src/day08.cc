#include <iostream>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <list>
#include <algorithm>
#include <sstream>
struct obs{ // an observation
  std::array<std::bitset<7>, 10> signals;
  std::array<std::bitset<7>, 4> output; // 'a' is the biset[0]
};

std::bitset<7> from_char_ls(std::string s){
  std::bitset<7> b; // all bits set to zero
  for(auto c: s){
    b[c - 'a'] = 1; // for input this should never throw an out of bounds exception
  }
  return b;
};

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, struct obs& o){
  for(unsigned int i = 0; i < 10; i++){
    std::string es_str;
    stream >> es_str;
    o.signals[i] = from_char_ls(es_str);
  }
  while(isspace(stream.peek())) stream.get();
  if(stream.get() != '|') throw std::runtime_error("Expecting | after signals.");
  for(unsigned int i = 0; i < 4; i++){
    std::string digit_str;
    stream >> digit_str;
    std::bitset<7> digit = from_char_ls(digit_str);
    o.output[i] = digit;
  }
  return stream;
}

template <class CharT, class Traits> std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream, const struct obs& o){
  for(unsigned int i = 0; i < 10; i++){
    stream << o.signals[i] << ' ';
  }
  stream << " | ";
  for(unsigned int i = 0; i < 4; i++){
    stream << o.output[i] << ' ';
  }
  return stream;
}

template<class CharT, class Traits> std::list<struct obs>
read_obs_list(std::basic_istream<CharT, Traits>& stream){
  std::list<struct obs> l;
  while(!stream.eof()){
    struct obs s;
    stream >> s;
    l.push_back(s);
    while(isspace(stream.peek())) stream.get();
  }
  return l;
}
//  0
// 1 2
//  3
// 4 5
//  6
//Order of segments: 0123456
static std::unordered_map<std::string, unsigned int> m
  {{"1110111", 0},{"0010010", 1},{"1011101", 2},
   {"1011011", 3},{"0111010", 4},{"1101011", 5},
   {"1101111", 6},{"1010010", 7},{"1111111", 8},{"1111011", 9}};

bool is_unique(const std::bitset<7>& b){
      switch(b.count()){
      case 2: // digit 1
      case 3: // digit 7
      case 4: // digit 4
      case 7: // digit 8
	return true;
      default:
	return false;
      }
}

unsigned int count_unique_bits(const std::list<struct obs>& l){
  unsigned int count = 0;
  for(auto &o : l)
    for(const std::bitset<7>& n: o.output)
      count += is_unique(n) ? 1 : 0;
  return count;
}
// is the given bitset a digit in the standard ordering
bool is_digit(const std::bitset<7>& n){
  if(m.find(n.to_string()) != m.end()) return true;
  return false;
};

unsigned int decoded_output_sum(const struct obs& o){
  std::array<unsigned int, 7> p = {0, 1, 2, 3, 4 ,5 ,6};
  do{
    bool valid = true;
    unsigned int output_num = 0;
    for(unsigned int i = 0; i < 10; i++){
      std::bitset<7> pi;
      for(unsigned int j = 0; j < 7; j++)
	pi[j] = o.signals[i][p[j]];
      if(!is_digit(pi)) {valid = false; break;}
    }
    if (valid)
      for(unsigned int i = 0; i < 4; i++){
	std::bitset<7> pi;
	for(unsigned int j = 0; j < 7; j++)
	  pi[j] = o.output[i][p[j]];
	if(!is_digit(pi)) {valid = false; break;}
	else output_num = output_num*10+m[pi.to_string()];
      }
    if(valid) return output_num;
  } while(std::next_permutation(p.begin(), p.end()));
  throw std::runtime_error("Unable to decode the observation.");
}

void test_decoding(){
  std::istringstream test_d("acedgfb cdfbe gcdfa fbcad dab cefabd cdfgeb eafb cagedb ab | cdfeb fcadb cdfeb cdbaf");
  struct obs test_o;
  test_d >> test_o;
  std::cout << decoded_output_sum(test_o) << std::endl;
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  auto l = read_obs_list(s);
  std::cout << count_unique_bits(l) << std::endl;

  unsigned int output_sum = 0;
  for(auto &o: l){output_sum += decoded_output_sum(o);}
  std::cout << output_sum << std::endl;
  return 0;
}
// Local Variables:
// compile-command: "gcc -Wall -Wextra -ggdb -lstdc++ -o day08 day08.cc"
// End:
