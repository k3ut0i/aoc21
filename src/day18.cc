#include<iostream>
#include<fstream>
#include<memory>
#include<list>
#include<sstream>
#include<map>
#include<variant>

typedef std::shared_ptr<struct snailnum> snp;
struct snailnum{
  std::variant<uint, std::pair<snp, snp>> num;
  enum { regular, snail } type;
  struct snailnum* parent = nullptr; // non-owning pointer to allow reverse travel
  bool order;              // Is this node left(false) or right(true) on the parent
  snailnum(struct snailnum&& s)
    : num(s.num), type(s.type), parent(s.parent), order(s.order)
  {
    if(s.type == snail){
      auto ch = s.children();
      ch.first->parent = this;
      ch.second->parent = this;
    }
  }
  snailnum(uint n) : num(n), type(regular) {}
  snailnum(snp a, snp b) : num(std::make_pair(a, b)), type(snail){
    a->parent = this; a->order = false;
    b->parent = this; b->order = true;
  }
  std::pair<snp, snp>& children() {return std::get<1>(num);}
  uint& num_value() {return std::get<0>(num);}
  
  bool is_regular() const {return type == regular;}
  bool is_regular_pair() ; // need it to check explosion
  bool reduce();
  uint magnitude();
};

bool snailnum::is_regular_pair() {
  if(type == regular) return false;
  else {
    auto ch = children();
    return ch.first->is_regular() && ch.second->is_regular();
  }
}

uint snailnum::magnitude(){
  if(type == regular) return num_value();
  else {
    auto ch = children();
    return 3*ch.first->magnitude() + 2*ch.second->magnitude();
  } 
}

bool replace(struct snailnum& a, snp b){
  struct snailnum* parent = a.parent;
  if(parent == nullptr) return false;
  bool order = a.order;
  b->parent = parent;
  b->order = order;
  if(order) // right
    std::get<1>(parent->num).second = b;
  else
    std::get<1>(parent->num).first = b;
  return true;
}


std::list<struct snailnum*> depth_first(struct snailnum& sn){
  std::list<struct snailnum*> s ({&sn});
  std::list<struct snailnum*> dfo;
  while (!s.empty()){
    struct snailnum* c = s.back();
    s.pop_back();
    dfo.push_back(c);
    if(!c->is_regular()){
      auto children = std::get<1>(c->num);
      s.insert(s.end(), {children.second.get(), children.first.get()});
    }
  }
  return dfo;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& s, const struct snailnum & n){
  if(n.type == snailnum::regular){
    s << std::get<0>(n.num);
  } else {
    s << '[' << *std::get<1>(n.num).first << ','
      << *std::get<1>(n.num).second << ']';
  }
  return s;
}

template<class C, class T> std::shared_ptr<struct snailnum>
read_snailnum(std::basic_istream<C,T>& s){
  char c;
  s.get(c);
  if(c == '['){
    snp first = read_snailnum(s);
    s.get(c);
    if(c != ',') throw std::runtime_error("Expecting Comma after first snailnum.");
    snp second = read_snailnum(s);
    s.get(c);
    if(c != ']') throw std::runtime_error("Expecting ] after two snailnums.");
    return std::make_shared<struct snailnum>(snailnum(first, second));
  } else {
    if (c < 48 || c > 57) throw std::runtime_error("Expecting a digit.");
    else return std::make_shared<struct snailnum>(snailnum(c-48));
  }
}

bool snailnum::reduce(){
    // std::cout << *this << std::endl;
  std::map<struct snailnum*, uint> depth({{nullptr, -1}, {this, 0}});
  std::list<struct snailnum*> dfo = depth_first(*this);
  struct snailnum* exploding_pair = nullptr;

  for(auto n : dfo){ // find the exploding pair
    depth[n] = depth[n->parent]+1;
    if(depth[n] >= 4 && n->is_regular_pair()) {
      exploding_pair = n;
      break;
    }
  }
  if(exploding_pair){ // if one exists
    // std::cout << "Found exploding pair: " << *exploding_pair << std::endl;
    auto children = exploding_pair->children();
    uint left = children.first->num_value();
    uint right = children.second->num_value();
    struct snailnum* left_regular = nullptr;
    struct snailnum* right_regular = nullptr;
    auto i = dfo.begin();
    while(*i != children.first.get()){
      if((*i)->is_regular()) left_regular = *i;
      ++i;
    }
    while(*i != children.second.get()) ++i;
    ++i;
    while(i != dfo.end()){
      if((*i)->is_regular()) {right_regular = *i; break;}
      ++i;
    }
    struct snailnum* parent = exploding_pair->parent;
    bool pos_on_parent = exploding_pair->order;
    if(left_regular != nullptr) {
      left_regular->num_value() += left;
    }
    if(right_regular != nullptr) {
      right_regular->num_value() += right;
    }
    auto zero = std::make_shared<struct snailnum>(snailnum(0));
    zero->parent = parent;
    zero->order = pos_on_parent;
    // std::cout << *parent << std::endl;
    if(pos_on_parent) { // right
      parent->children().second = zero;
    } else { // left
      parent->children().first = zero;
    }
    // std::cout << *parent << std::endl;
    return true;
  } else { // No exploding pair
    for(auto n : dfo)
      if (n->is_regular() && n->num_value() >= 10){
	const uint nv = n->num_value();
	snp left = std::make_shared<struct snailnum>(snailnum(nv/2));
	snp right = std::make_shared<struct snailnum>(snailnum(nv - nv/2));
	snp new_node = std::make_shared<struct snailnum>(left, right);
	replace(*n, new_node);
	return true; // Do this once and get out the loop
      }
  }
  return false;
}

snp add(snp a, snp b){
  snp sum = std::make_shared<struct snailnum>(snailnum(a, b));
  while(sum->reduce());
  return sum;
}
#if true
void test(){
  std::istringstream as("[[2,[[7,7],7]],[[5,8],[[9,3],[0,2]]]]");
  std::istringstream bs("[[[0,[5,8]],[[1,7],[9,6]]],[[4,[1,2]],[[1,4],2]]]") ;
  snp a = read_snailnum(as);
  snp b = read_snailnum(bs);
  snp c = add(a, b);
  std::cout << *c << ' ' << c->magnitude() << std::endl;
}

void test_dfo() {
  std::istringstream as("[[[[6,6],[7,7]],[[0,7],[7,7]]],[[[5,5],[5,6]],9]]");
  snp a = read_snailnum(as);
  auto dfo = depth_first(*a);
  std::cout << *a << std::endl;
  for(auto e : dfo) {
    std::cout << e << ' ' << e->parent << ' ';
    if(e->is_regular()) std::cout << e->num_value();
    std::cout << std::endl;
  }
}
#endif // DEBUG
snp read_snailnum(std::string s){
  std::istringstream ss(s);
  return read_snailnum(ss);
}
int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<snp> nums;
  std::list<std::string> snums;
  while(!s.eof()){
    std::string line;
    getline(s, line);
    if(line == "") break; // stop at empty line
    std::istringstream ss(line);
    snp sn = read_snailnum(ss);
    snums.push_back(line);
    nums.push_back(sn);
  }
  // part1
  snp sum = nums.front();
  for(auto i = ++nums.begin(); i != nums.end(); i++)
    sum = add(sum, *i);
  std::cout << *sum << ' ' << sum->magnitude() << std::endl;

  // part2
  uint max_magnitude = 0; snp a, b;
  for(auto sn1 : snums)
    for(auto sn2 : snums)
      if(sn1 != sn2){
	auto n1 = read_snailnum(sn1);
	auto n2 = read_snailnum(sn2);
	auto n = add(n1, n2);
	uint m = n->magnitude();
	// std::cout << *n1 << '+' << *n2 << '=' << *n << ' ' << m << std::endl;
	if( m > max_magnitude) { max_magnitude = m; a = n1; b = n2;}
      }
  std::cout << *a << ' ' << *b << ' ' << max_magnitude << std::endl;
  // test();
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day18 day18.cc"
// End:
