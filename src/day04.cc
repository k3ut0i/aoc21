#include <iostream>
#include <fstream>
#include <list>
#include <unordered_set>
#include <array>
#include <algorithm>

struct bingoC {
  unsigned int array[5][5];
};

template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream, std::unordered_set<unsigned int>& s){
  auto ei = s.begin();
  if (ei == s.end()) return stream;
  stream << *ei;
  ei++;
  while(ei != s.end()) {
    stream << ", " << *ei;
    ei++;
  }
  return stream;
}

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, struct bingoC& b){
  for(int i = 0; i < 5; i++)
    for(int j = 0; j < 5; j++){
      stream >> b.array[i][j];
    }
  return stream;
}

// Just for debugging
template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream, struct bingoC& b){
  for(int i = 0; i < 5; i++){
    for(int j = 0; j < 5; j++){
      stream << b.array[i][j] << " ";
    }
    stream << std::endl;
  }
  return stream;
}

template<class CharT, class Traits> std::list<unsigned int>
read_csv_line(std::basic_istream<CharT, Traits>& stream){
  std::list<unsigned int> nums;
  while(true){
    unsigned int i;
    stream >> i;
    // std::cout << i << std::endl;
    nums.push_back(i);
    char delim = stream.get();
    if (delim == '\n') break;
    else if (delim != ',') throw std::runtime_error("Not a csv line.");
  }
  return nums;
}

template<class CharT, class Traits> std::list<struct bingoC>
read_bingo_cards(std::basic_istream<CharT, Traits>& stream){
  std::list<struct bingoC> cards;
  while(!stream.eof()){
    struct bingoC c;
    stream >> c;
    cards.push_back(c);
    while(isspace(stream.peek())) stream.get();
  }
  return cards;
}

bool mark_card(const struct bingoC& card, const unsigned int num,
	       std::unordered_set<unsigned int>& marked)
{
  for (unsigned int i = 0; i < 5; i++)
    for (unsigned int j = 0; j < 5; j++){
      if (card.array[i][j] == num) {
	marked.insert(5*i+j);
	return true;
      }
    }
  return false;
}

bool isbingo(const std::unordered_set<unsigned int>& marked){
  for (unsigned int i = 0; i < 5; i++){ // columns
    std::array<unsigned int, 5> col = {i, i+5, i+10, i+15, i+20};
    if(std::all_of(col.begin(), col.end(),
		   [marked](int i){return marked.count(i)==1;}))
      return true;
  }
  for (unsigned int i = 0; i < 5; i++){ // rows
    unsigned int r = 5*i;
    std::array<unsigned int, 5> row = {r, r+1, r+2, r+3, r+4};
    if(std::all_of(row.begin(), row.end(),
		   [marked](int i){return marked.count(i)==1;}))
      return true;
  }
  return false;
}
unsigned int score_card(const struct bingoC& card,
			const std::unordered_set<unsigned int>& marked,
			const unsigned int won_num)
{
  unsigned int unmarked = 0;
  for(unsigned int i = 0; i < 5; i++)
    for(unsigned int j = 0; j < 5; j++){
      if(marked.count(5*i+j)!=1) unmarked+=card.array[i][j];
    }
  return unmarked * won_num;
}
std::pair<unsigned int, unsigned int>
playbingo(const std::list<struct bingoC>& cards,
	  const std::list<unsigned int>& nums)
{
  std::list<std::pair<struct bingoC, std::unordered_set<unsigned int>>> state;
  bool first_won = false;
  unsigned int first_score = 0;
  unsigned int last_score = 0;
  for(auto c : cards) {
    std::unordered_set<unsigned int> s; s.clear();
    auto p = std::pair<struct bingoC, std::unordered_set<unsigned int>>(c, s);
    state.push_back(p);
  }
  for(auto n : nums) {
    // for (auto &cm: state){ // If I do not use &, cm will be a copy of the node, not a reference to it.
    for(auto cm = state.begin(); cm != state.end();){
      auto csafe = cm; // erase(cm) destroys way to get to next node;
      cm++; // get to the next node and deal with the saved current iterator csafe
      
      if(mark_card(csafe->first, n, csafe->second) && isbingo(csafe->second)){
	if(!first_won)
	  {
	    first_score = score_card(csafe->first, csafe->second, n);
	    first_won = true;
	  }
	if(++state.begin() == state.end()) {
	  // only one card is left and it just won
	  last_score = score_card(state.front().first, state.front().second, n);
	  goto END; // game over
	}
	state.erase(csafe);
      }

      // std::cout << cm.first << "marked: " << cm.second << std::endl;
    }
  }
 END:
  return std::pair<unsigned int, unsigned int>(first_score, last_score);
}

int main(int argc, char* argv[]){
  if (argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  auto rand_nums = read_csv_line(s);
  auto cards = read_bingo_cards(s);
  // for(auto c : cards) std::cout << c << std::endl;
  auto scores = playbingo(cards, rand_nums);
  std::cout << "Winning: " << scores.first << "\t"
	    << "Losing: " << scores.second<< std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -ggdb -Wall -o day04 day04.cc"
// End:
