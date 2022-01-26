#include<iostream>
#include<utility>
#include<map>
#include<iomanip>

uint clamp(uint num, uint max){ // num should be in [1..max]
  uint r = num % max;
  if(r == 0) return max;
  else return r;
}

uint det_dice(const uint nsides){
  static uint start_from = 1;
  uint r = clamp(start_from, nsides) +
    clamp(start_from+1, nsides) +
    clamp(start_from+2, nsides);
  start_from = clamp(start_from+3, nsides);
  return r;
}

uint simulate_game(const uint dice_sides, const uint max_score, uint p1, uint p2)
{
  uint nturns = 0, p1score = 0, p2score = 0;
  while(true){
    p1 = clamp(p1+det_dice(dice_sides), 10);
    nturns+=3;
    p1score += p1;
    if(p1score >= max_score) break;
    
    p2 = clamp(p2+det_dice(dice_sides), 10);
    nturns+=3;
    p2score += p2;
    if(p2score >= max_score) break;
  }
  return nturns * (p1score < p2score ? p1score : p2score);
}

typedef unsigned long long ull;

std::map<uint, ull> dirac_dice(const ull nsides){
  std::map<uint, ull> m;
  for(ull i = 1; i <= nsides; i++)
    for(ull j = 1; j <= nsides; j++)
      for(ull k = 1; k <= nsides; k++)
	try { m[i+j+k] = m.at(i+j+k) + 1; }
	catch (std::out_of_range& e) {m[i+j+k] = 1;}
  return m;
}

void dump_state(const std::map<std::tuple<uint, uint, uint, uint>, ull> &state){
  for(const auto &s : state){
      uint p1p, p1s, p2p, p2s;
      ull onuniv = s.second;
      std::tie(p1p, p1s, p2p, p2s) = s.first;
      fprintf(stdout, "%d[%d] : %d[%d] - %lld\n", p1p, p1s, p2p, p2s, onuniv);
  }
}

ull count_univ(const std::map<std::tuple<uint, uint, uint, uint>, ull> &state){
  ull n = 0;
  for(const auto &s : state) n += s.second;
  return n;
}

void update_mv(std::map<std::tuple<uint, uint, uint, uint>, ull> &state,
	       const std::map<uint, ull>& new_split, bool player)
{
  ull init_univ_count = count_univ(state);
  std::map<std::tuple<uint, uint, uint, uint>, ull> new_state;
  for(auto s_n_p : new_split){
    uint step = s_n_p.first;
    uint nuniv = s_n_p.second;
    for(const auto &s : state){
      uint p1p, p1s, p2p, p2s;
      ull onuniv = s.second;
      std::tie(p1p, p1s, p2p, p2s) = s.first;
      if(player){ // second
	p2p = clamp(p2p+step, 10);
	p2s += p2p;
      } else {    // first
	p1p = clamp(p1p+step, 10);
	p1s += p1p;
      }
      std::tuple<uint, uint, uint, uint> uni_state = {p1p, p1s, p2p, p2s};
      auto uni_count = onuniv * nuniv;
      try {
	new_state[uni_state] = new_state.at(uni_state) + uni_count;
      } catch (std::out_of_range& e) {new_state[uni_state] = uni_count;}
    }
  }
  ull new_univ_count = count_univ(new_state);
  if(new_univ_count != 27 * init_univ_count)
    throw std::runtime_error("New universes have not expanded properly.");
  state = new_state;
}

ull reduce_mv(std::map<std::tuple<uint, uint, uint, uint>, ull> &state,
	      uint max_score)
{
  uint wins = 0;
  auto i = state.begin();
  while(i != state.end()){
    auto in = std::next(i);
    uint p1p, p1s, p2p, p2s;
    ull nuniv = i->second;
    std::tie(p1p, p1s, p2p, p2s) = i->first;
    if(p1s >= max_score || p2s >= max_score) {
      if(p1s >= max_score && p2s >= max_score)
	throw std::runtime_error("Max scores of both players reached.");
      wins += nuniv;
      state.erase(i);
    }
    i = in;
  }
  return wins;
}

ull simulate_game_mv(const uint dice_sides, const uint max_score, uint p1, uint p2){
  const std::map<uint, ull> dd_mv_split = dirac_dice(dice_sides);
  // {position1, score1, position2, score2} -> num_of_universes
  std::map<std::tuple<uint, uint, uint, uint>, ull> mv_state = {{{p1, 0, p2, 0}, 1}};
  ull p1_wins = 0, p2_wins = 0;
  uint nturns = 0;
  while(!mv_state.empty()){
    update_mv(mv_state, dd_mv_split, false);
    p1_wins += reduce_mv(mv_state, max_score);
    update_mv(mv_state, dd_mv_split, true);
    p2_wins += reduce_mv(mv_state, max_score);
    // dump_state(mv_state);
    nturns++;
    std::cout << std::setw(2) << nturns << ' '
	      << std::setw(15) << p1_wins << ' '
	      << std::setw(15) << p2_wins << ' '
	      << std::setw(15) << count_univ(mv_state) << ' '
	      << std::setw(15) << mv_state.size() << std::endl;
  }
  return p1_wins > p2_wins ? p1_wins : p2_wins;
}

int main(int argc, char* argv[0]){
  if(argc != 3){
    fprintf(stderr, "Usage: %s <player1> <player2>\n", argv[0]);
  }
  uint p1 = atoi(argv[1]);
  uint p2 = atoi(argv[2]);
  std::cout << simulate_game_mv(3, 21, p1, p2) << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day21 day21.cc"
// End:
