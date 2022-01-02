#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <stack>
#include <vector>
#include <algorithm>

enum char_t{
  lparen, rparen,
  lbracket, rbracket,
  lbrace, rbrace,
  langle, rangle
};

static const std::map<char, char_t> cm = { {'(', lparen},{')', rparen},
					   {'[', lbracket},{']', rbracket},
					   {'{', lbrace},{'}', rbrace},
					   {'<', langle},{'>', rangle}};

static const std::map<char_t, char_t> close_char = {{lparen, rparen},
						    {lbracket, rbracket},
						    {lbrace, rbrace},
						    {langle, rangle}};
inline bool is_open(char_t c){
  return c == lparen || c == lbracket || c == lbrace || c == langle;
}

const std::map<char_t, uint> char_score = {{rparen, 3},
					   {rbracket, 57},
					   {rbrace, 1197},
					   {rangle, 25137}};

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, std::list<char_t>& chars){
  while(std::isspace(stream.peek())) stream.get(); // skip initial whitespace
  while(!std::isspace(stream.peek())) chars.push_back(cm.at(stream.get()));
  return stream;
}

const std::map<char_t, uint> completion_score = {{rparen, 1},
						 {rbracket, 2},
						 {rbrace, 3},
						 {rangle, 4}};
enum line_status_t{
  legal, corrupted, incomplete
};

line_status_t status(const std::list<char_t>& line,
		     char_t& corrupted_char,
		     uint& iscore)
{
  std::stack<char_t> s;
  for(auto c : line){
    if(is_open(c)) s.push(c);
    else { // what is top in an empty stack? Does it throw an error?
      // i.e., what happens if the line or chunk starts with a closing character?
      if(c == close_char.at(s.top())) s.pop();
      else {
	corrupted_char = c;
	return corrupted;
      }
    }
  }
  if (s.empty()) return legal;
  else{
    iscore = 0; // start with zero
    while(!s.empty()) {
      iscore = 5*iscore + completion_score.at(close_char.at(s.top()));
      s.pop();
    }
    return incomplete;
  }
};

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<std::list<char_t>> cs;
  while(!s.eof()){
    std::list<char_t> line;
    s >> line;
    cs.push_back(line);
    while(std::isspace(s.peek())) s.get();
  }
  uint score = 0, iscore = 0;
  char_t c;
  std::vector<uint> iscores;
  for(auto &line: cs)
    switch (status(line, c, iscore)) {
    case corrupted:
      score += char_score.at(c);
      break;
    case incomplete:
      iscores.push_back(iscore);
      break;
    case legal:
      break;
    }
  sort(iscores.begin(), iscores.end());
  for(auto i : iscores) std::cout << i << std::endl;
  std::cout << score << ' ' << iscores[(iscores.size()-1)/2] << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day10 day10.cc"
// End:
