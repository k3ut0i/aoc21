#include <iostream>
#include <bitset>
#include <list>
#include <fstream>
#include <array>
#include <vector>

template<std::size_t N>
std::pair<std::list<std::bitset<N>>, unsigned long>
read_seqs(std::ifstream& stream)
{
  std::list<std::bitset<N>> seq_list;
  unsigned long num = 0;
  while(!stream.eof()) {
    std::bitset<N> bs;
    stream >> bs;
    if(isspace(stream.peek())) {
      do {
	stream.get();
      } while(isspace(stream.peek()));
    } else {
      throw std::runtime_error("binary sequence must be delimied with whitespace");
    }
    seq_list.push_back(bs);
    num++;
  }
  return
    std::pair<std::list<std::bitset<N>>, unsigned long>(seq_list, num);
}

template<std::size_t N> std::bitset<N>
find_average(const std::list<std::bitset<N>> seq_list,
	     const std::size_t len) {
  std::bitset<N> average;
  std::array<unsigned long, N> bit_count;
  bit_count.fill(0); // Code goes wonky without this

  for (auto seq : seq_list) {
    for(unsigned long i = 0; i < N; i++) {
      bit_count[i] += seq[i];
    }
  }
  for(unsigned long i = 0; i < N; i++) {
    average[i] = (2*bit_count[i] > len);
  }
  return average;
}

template<std::size_t N> std::pair<unsigned long, unsigned long>
search_ratings(const std::list<std::bitset<N>> seq_list,
	       const std::size_t len)
{
  typedef std::list<std::bitset<N>> bseq;
  bseq ogr = seq_list;
  bseq csr = seq_list;
  auto ci = csr.begin();
  // std::cout << "Oxygen generator rating: deletions" << std::endl;
  for(std::size_t i = N-1; i >= 0; i--){
    // std::list::erase_if(ogr, [](auto r){/**/}); // c++20 required
    int ones = 0; int zeroes = 0;
    for(auto oi = ogr.begin(); oi != ogr.end(); oi++){
      if ((*oi)[i]) {
	ones++;
      } else {
	zeroes++;
      }
    }
    // switch std::strong_order(ones, zeroes) {} // c++20
    bool filter_bit;
    if (ones >= zeroes) { // for ogr if equal prefer one
      filter_bit = true;
    } else {
      filter_bit = false;
    }
    auto oi = ogr.begin();
    while(oi != ogr.end()){
      auto o = oi; oi++;
      if ((*o)[i] != filter_bit) {
	// std::cout << *o << std::endl;
	ogr.erase(o);
      }
    }
    if(++ogr.begin() == ogr.end()) break;
  }
  std::cout << "Oxygen generator rating: " << ogr.front() << std::endl;
  for(std::size_t i = N-1; i >= 0; i--){
    int ones = 0; int zeroes = 0;
    for(auto ci = csr.begin(); ci != csr.end(); ci++){
      if ((*ci)[i]) {
	ones++;
      } else {
	zeroes++;
      }
    }
    bool filter_bit;
    if (ones >= zeroes) { // for csr if equal zero
      filter_bit = false;
    } else {
      filter_bit = true;
    }
    auto ci = csr.begin();
    while(ci != csr.end()){
      auto c = ci; ci++;
      if ((*c)[i] != filter_bit) {
	// std::cout << *c << std::endl;
	csr.erase(c);
      }
    }
    if(++csr.begin() == csr.end()) break;
  }

  if (ogr.size() != 1 || csr.size() != 1) {
    fprintf(stderr, "Filtered sizes are OGR: %ld\tCSR: %ld\n",
	    ogr.size(), csr.size());
    throw std::runtime_error("Could not find ratings.");
  }

  std::cout << "CO2 scrubber rating: " << csr.front() << std::endl;
  return std::pair<unsigned long, unsigned long>
    (ogr.front().to_ulong(), csr.front().to_ulong());
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream stream(argv[1]);
  int count = 0;
  while(!isspace(stream.get())) count++;
  constexpr std::size_t seq_len = 12;

  if(seq_len != count) {
    fprintf(stderr, "Recompile with the appropriate seq_len: got %d, defined %ld.\n", count, seq_len);
    return -1;
  }
  stream.seekg(std::ios_base::beg); // go to the start

  auto seq_list_count = read_seqs<seq_len>(stream);
  auto seq_list = seq_list_count.first;
  unsigned long seq_count = seq_list_count.second;
  // for (auto seq : seq_list) std::cout << seq << std::endl;
  std::bitset<seq_len> average = find_average(seq_list, seq_count);
  std::cout << "Power consumption: "
	    << average.to_ulong() * average.flip().to_ulong()
	    << std::endl;
  auto ratings = search_ratings(seq_list, seq_count);
  std::cout << "Rating: " << ratings.first * ratings.second << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -ggdb day03.cc -o day03"
// End:
