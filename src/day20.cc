#include<iostream>
#include<fstream>
#include<vector>
#include<algorithm>
#include<string>
#include<optional>

void assert(bool b, std::string s = ""){
  if(!b) throw std::runtime_error(s);
}

uint from_bvec(const std::vector<bool> &v){
  uint num = 0;
  for(bool b : v) num = 2*num + b;
  return num;
}

struct image{ // infinite in size
  uint nrows;
  uint ncols;
  bool get(int i, int j) const;
  image(uint r, uint c) : nrows(r), ncols(c){
    data = std::vector<std::vector<bool>>(nrows, std::vector<bool>());
    for(uint i = 0; i < nrows; i++) data[i] = std::vector<bool>(ncols, false);
  }
  image(uint r, uint c, std::vector<std::vector<bool>> b)
    : nrows(r), ncols(c), data(b){}
  uint get_3x3(int i, int j) const;
  void set_ofr(bool b) {out_of_range=b;}
  void set(int i, int j, bool b);
  bool in_range(int i, int j) const;
  std::optional<uint> count_lit() const;
private:
  bool out_of_range = false;           // far away pixel value
  std::vector<std::vector<bool>> data; // a vector of rows
};

bool image::in_range(int i, int j) const{
  return !(i < 0 || i >= (int)nrows || j < 0 || j >= (int)ncols);
}

bool image::get(int i, int j) const{
  if(in_range(i, j)) return data[i][j];
  else return out_of_range;
}

void image::set(int i, int j, bool b){
  if(in_range(i, j)) data[i][j] = b;
  else throw std::runtime_error("Out of range, use set_ofr instead.");
}

uint image::get_3x3(int i, int j) const{
  std::vector<bool> bits;
  for(int x : {i-1, i, i+1})
    for(int y : {j-1, j, j+1})
      bits.push_back(get(x, y));
  return from_bvec(bits);
}

std::optional<uint> image::count_lit() const{
  if(out_of_range) return {}; // Infinite
  else {
    uint count = 0;
    for(const auto &r : data)
      for(auto b : r) if(b) count++;
    return count;
  }
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& stream, const struct image &im){
  for(uint i = 0; i < im.nrows; i++){
    for(uint j = 0; j < im.ncols; j++)
      stream << (im.get(i, j) ? '#' : '.');
    stream << '\n';
  }
  return stream;
}

struct image enhance(const struct image& im, std::vector<bool>alg){
  struct image n(im.nrows+2, im.ncols+2);
  for(int i = 0; i < (int)n.nrows; i++)
    for(int j = 0; j < (int)n.ncols; j++)
      n.set(i, j, alg[im.get_3x3(i-1, j-1)]);
  n.set_ofr(alg[im.get_3x3(-3, -3)]);
  return n;
}

template<class C, class T> std::pair<std::vector<bool>, struct image>
read_input(std::basic_istream<C, T>& stream){
  std::vector<bool> alg;
  bool prev_nl = false;
  while(!stream.eof()){
    char c = stream.get();
    if(c == '\n' && prev_nl) break;
    else if (c == '\n') prev_nl = true;
    else {
      prev_nl = false;
      alg.push_back(c == '#');

    }
  }
  std::vector<std::vector<bool>> image_data;
  while(!stream.eof()){
    std::vector<bool> row;
    std::string line;
    std::getline(stream, line);
    if(line == "") break;
    for(auto c : line) row.push_back(c == '#');
    // for(auto b : row) std::cout << std::boolalpha << b << ' ';
    image_data.push_back(row);
  }
  uint ncols = image_data[0].size();
  assert(std::all_of(image_data.begin(), image_data.end(), [ncols](auto i){return i.size()==ncols;}));
  uint nrows = image_data.size();
  return {alg, image(nrows, ncols, image_data)};
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  auto alg_im = read_input(s >> std::noskipws);
  std::vector<bool> alg = alg_im.first;
  struct image im = alg_im.second;
  struct image im1 = enhance(im, alg);
  struct image im2 = enhance(im1, alg);
  auto count_maybe = im2.count_lit();
  if(count_maybe) std::cout << count_maybe.value() << std::endl;
  else throw std::runtime_error("Image has lit out of range values");
  
  for(uint i = 0; i < 50 ; i++)
    im = enhance(im, alg);
  std::cout << im.count_lit().value() << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day20 day20.cc"
// End:
