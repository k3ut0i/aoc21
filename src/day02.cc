#include<iostream>
#include<fstream>
#include<list>

enum command_type {
  forward,
  up,
  down
};

struct command {
  command_type type;
  unsigned int magnitude;
};

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>> (std::basic_istream<CharT, Traits>& stream, struct command& c)
{
  std::string type_name;
  command_type type;
  unsigned int magnitude;
  stream >> std::ws >> type_name >> magnitude;
  if (type_name == "forward") {
    type = forward;
  } else if (type_name == "up") {
    type = up;
  } else if (type_name == "down") {
    type = down;
  } else {
    throw std::runtime_error("Unkown direction encountered when reading command: " + type_name);
  }
  c.type = type;
  c.magnitude = magnitude;
  return stream;
}

std::list<struct command> read_commands(std::string filename) {
  std::list<struct command> l;
  std::ifstream s(filename);
  while(!s.eof()){
    struct command c;
    s >> c;
    l.push_back(c);
    while(isspace(s.peek())) { // So that >> doesn't choke on whitespace
    s.get();
    }
  }

  return l;
}

std::pair<int, int> find_pos(std::list<struct command> cl) {
  int x = 0;
  int y = 0;
  for (auto c : cl) {
    switch (c.type) {
    case forward:
      x += c.magnitude;
      break;
    case up:
      y += c.magnitude;
      break;
    case down:
      y -= c.magnitude;
      break;
    }
  }
  return std::pair<int, int>(x, y);
}

std::pair<int, int> find_pos2(std::list<struct command> cl) {
  int x = 0;
  int y = 0;
  int aim = 0;
    for (auto c : cl) {
    switch (c.type) {
    case forward:
      x += c.magnitude;
      y -= aim * c.magnitude;
      break;
    case up:
      aim -= c.magnitude;
      break;
    case down:
      aim += c.magnitude;
      break;
    }
  }
  return std::pair<int, int>(x, y);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage %s <filename>", argv[0]);
    return -1;
  }
  std::list<struct command> cl = read_commands(argv[1]);
  // for (auto c: cl) {
  //   std::cout << c.type << " " << c.magnitude << std::endl;
  // }
  auto pos = find_pos(cl);
  std::cout << pos.first << "," << pos.second << " : "
	    << pos.first * (- pos.second)<< std::endl;
  auto pos2 = find_pos2(cl);
  std::cout << pos2.first << "," << pos2.second << " : "
	    << pos2.first * (- pos2.second)<< std::endl;
  return 0;
}


// Local Variables:
// compile-command: "gcc -lstdc++ day02.cc -o day02"
// End:
