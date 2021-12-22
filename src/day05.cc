#include <iostream>
#include <list>
#include <fstream>

struct line{
  int x1, x2, y1, y2;
};

enum orientation{
  vertical, horizontal, diagonal, oblique
};

bool on_line_p(const struct line& l, const int x, const int y){
  return (l.x1-x) *(l.y1-l.y2) == (l.y1-y)*(l.x1-l.x2);
}

orientation line_type(const struct line& l){
  orientation type;
  if(l.x1 == l.x2) type = vertical;
  else if(l.y1 == l.y2) type = horizontal;
  else if ((l.x1-l.x2==l.y1-l.y2) || (l.x1+l.y1==l.x2+l.y2))
    type = diagonal;
  else type = oblique;
  return type;
}

template<class CharT, class Traits> std::basic_istream<CharT, Traits>&
operator>>(std::basic_istream<CharT, Traits>& stream, struct line& l){
  int x1, x2, y1, y2;
  stream >> x1;
  if(stream.get() != ',') throw std::runtime_error("Expecting comma after number.");
  stream >> y1;
  while(std::isspace(stream.peek())) stream.get();
  if(stream.get() != '-' || stream.get() != '>')
    throw std::runtime_error("Expecting -> after point.");
  stream >> x2;
  if(stream.get() != ',') throw std::runtime_error("Expecting comma after number.");
  stream >> y2;
  // while(std::isspace(stream.peek())) stream.get();
  l = line({x1, x2, y1, y2});
  return stream;
}

template<class CharT, class Traits> std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits>& stream, const struct line& l){
  stream << l.x1 << "," << l.y1 << " -> " << l.x2 << "," << l.y2;
  return stream;
}

template<class CharT, class Traits> std::list<struct line>
read_lines(std::basic_istream<CharT, Traits>& stream){
  std::list<struct line> lines;
  while(!stream.eof()){
    struct line l;
    stream >> l;
    lines.push_back(l);
    while(std::isspace(stream.peek())) stream.get();
  }
  return lines;
}

std::pair<int, int> bounds(std::list<struct line>& lines){
  int max_x = 0;
  int max_y = 0;
  for(auto &l: lines){
    if(l.x1 > max_x) max_x = l.x1;
    if(l.x2 > max_x) max_x = l.x2;
    if(l.y1 > max_y) max_y = l.y1;
    if(l.y2 > max_y) max_y = l.y2;
  }
  return std::pair<int, int>(max_x, max_y);
}

unsigned int density_sat(std::list<struct line>& lines, bool diagonalp){
  auto max_b = bounds(lines);
  const unsigned int max_x = max_b.first;
  const unsigned int max_y = max_b.second;
  unsigned int data[max_x+1][max_y+1];
  // initialization
  for(unsigned int x = 0; x <= max_x; x++)
    for(unsigned int y = 0; y <= max_y; y++)
      data[x][y] = 0;

  // populate with lines
  int xsign, ysign;
  for(auto &l: lines){
    switch (line_type(l)){
    case horizontal:
      xsign = l.x1 > l.x2 ? -1 : 1;
      for(int x = l.x1; xsign*(x-l.x2) <= 0; x+=xsign){
	data[x][l.y1]++;
      }
      break;
    case vertical:
      ysign = l.y1 > l.y2 ? -1 : 1;
      for(int y = l.y1; ysign*(y-l.y2) <= 0; y+=ysign){
	data[l.x1][y]++;
      }
      break;
    case diagonal:
      if(diagonalp){
	xsign = l.x1 > l.x2 ? -1 : 1;
	ysign = l.y1 > l.y2 ? -1 : 1;
	for(int x = l.x1, y = l.y1;
	    xsign*(x-l.x2) <= 0 && ysign*(y-l.y2) <= 0;
	    x+=xsign, y+=ysign)
	  data[x][y]++;
      }
      break;
    case oblique:
      break;
    }
  }
  /* Print the density map
  for(unsigned int y = 0; y <= max_y; y++){
    for(unsigned int x = 0; x <= max_x; x++){
      if(data[x][y]==0){
	std::cout << ". ";
      } else {
      std::cout << data[x][y] << " ";
      }
    }
    std::cout << std::endl;
  }
  */
  unsigned int count = 0;
  for(unsigned int x = 0; x <= max_x; x++)
    for(unsigned int y = 0; y <= max_y; y++){
      count += data[x][y] > 1 ? 1 : 0;
    }
  return count;
}

int main(int argc, char* argv[]){
  if(argc !=2){
    fprintf(stderr, "Usage: %s <filename>", argv[1]);
    return -1;
  }
  std::ifstream s(argv[1]);
  std::list<struct line> lines = read_lines(s);
  // for(auto &l: lines) std::cout << l << std::endl;
  std::cout << density_sat(lines, false) << std::endl;
  std::cout << density_sat(lines, true) << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -Wall -ggdb -lstdc++ day05.cc -o day05"
// End:
