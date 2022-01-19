#include<iostream>
#include<fstream>
#include<memory>
#include<list>
#include<sstream>
#include<map>

typedef std::shared_ptr<struct snailnum> snp;
struct snailnum{
  union {
    uint num;
    std::pair<snp, snp> snum;
  };  
  enum { regular, snail } type;
  struct snailnum* parent = nullptr; // non-owning pointer to allow reverse travel
  bool order;              // Is this node left(false) or right(true) on the parent
  snailnum(const struct snailnum& s) : type(s.type), parent(s.parent), order(s.order)
  {
    if (s.type == snail) {  // not a deep copy
      snum = s.snum;
      snum.first->parent = this;  // change the parent address in children
      snum.second->parent = this;
    }
    else num = s.num;
  }
  snailnum(uint n) : num(n), type(regular){}
  snailnum(snp a, snp b) : snum({a, b}), type(snail){
    a->parent = this; a->order = false;
    b->parent = this; b->order = true;
  }
  ~snailnum(){
    if (type == snail) snum.~pair();
  }
  bool is_regular() const {return type == regular;}
  bool is_regular_pair() const ; // need it to check explosion
  bool reduce();
};
bool snailnum::is_regular_pair() const{
  return type == snail && snum.first->type == regular && snum.second->type == regular;
}

std::list<struct snailnum*> depth_first(struct snailnum& sn){
  std::list<struct snailnum*> s ({&sn});
  std::list<struct snailnum*> dfo;
  while (!s.empty()){
    struct snailnum* c = s.back();
    s.pop_back();
    dfo.push_back(c);
    if(!c->is_regular()) 
      s.insert(s.end(), {c->snum.second.get(), c->snum.first.get()});
  }
  return dfo;
}

template<class C, class T> std::basic_ostream<C, T>&
operator<<(std::basic_ostream<C, T>& s, const struct snailnum & n){
  if(n.type == snailnum::regular){
    s << n.num;
  } else {
    s << '[' << *n.snum.first << ',' << *n.snum.second << ']';
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
    uint left = exploding_pair->snum.first->num;
    uint right = exploding_pair->snum.second->num;
    struct snailnum* left_regular = nullptr;
    struct snailnum* right_regular = nullptr;
    auto i = dfo.begin();
    while(*i != exploding_pair->snum.first.get()){
      if((*i)->is_regular()) left_regular = *i;
      ++i;
    }
    while(*i != exploding_pair->snum.second.get()) ++i;
    ++i;
    while(i != dfo.end()){
      if((*i)->is_regular()) {right_regular = *i; break;}
      ++i;
    }
    struct snailnum* parent = exploding_pair->parent;
    bool pos_on_parent = exploding_pair->order;
    if(left_regular != nullptr) {
      left_regular->num += left;
    }
    if(right_regular != nullptr) {
      right_regular->num += right;
    }
    auto zero = std::make_shared<struct snailnum>(snailnum(0));
    zero->parent = parent;
    zero->order = pos_on_parent;
    // std::cout << *parent << std::endl;
    if(pos_on_parent) { // right
      parent->snum.second = zero;
    } else { // left
      parent->snum.first = zero;
    }
    // std::cout << *parent << std::endl;
    return true;
  } else { // No exploding pair
    for(auto n : dfo)
      if (n->is_regular() && n->num >= 10){
	snp left = std::make_shared<struct snailnum>(snailnum(n->num/2));
	snp right = std::make_shared<struct snailnum>(snailnum(n->num - n->num/2));
	snp new_node = std::make_shared<struct snailnum>(left, right);
	auto parent = n->parent;
	new_node->parent = parent;
	new_node->order = n->order;
	if(n->order) // on the right
	  parent->snum.second = new_node;
	else
	  parent->snum.first = new_node;
	return true; // Do this once and get out the loop
      }
  }
  return false;
}

snp add(snp a, snp b){
  snp sum = std::make_shared<struct snailnum>(snailnum(a, b));
  std::cout << *sum << std::endl;
  while(sum->reduce());
  std::cout << *sum << std::endl;
  return sum;
}

void test(){
  std::istringstream as("[[[[6,6],[7,7]],[[0,7],[7,7]]],[[[5,5],[5,6]],9]]");
  std::istringstream bs("[1,[[[9,3],9],[[9,0],[0,7]]]]") ;
  snp a = read_snailnum(as);
  snp b = read_snailnum(bs);
  std::cout << *add(a, b) << std::endl;
}

int main(int argc, char* argv[]){
  if(argc != 2){
    fprintf(stderr, "Usage: %s <filename>", argv[0]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<snp> nums;
  while(!s.eof()){
    std::string line;
    getline(s, line);
    if(line == "") break; // stop at empty line
    std::istringstream ss(line);
    snp sn = read_snailnum(ss);
    nums.push_back(sn);
  }
  snp sum = nums.front();
  for(auto i = ++nums.begin(); i != nums.end(); i++)
    sum = add(sum, *i);
  std::cout << *sum << std::endl;
  // test();
  return 0;
}

// Local Variables:
// compile-command: "gcc -lstdc++ -Wall -Wextra -ggdb -o day18 day18.cc"
// End:
