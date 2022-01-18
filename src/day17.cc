#include<iostream>
#include<optional>
#include<cmath>

struct target{
  int xmin, xmax, ymin, ymax;
};
typedef std::pair<int, int> point;

inline int pos_at(const int init_v, const uint time, const bool neg_vel_p){
  if(neg_vel_p){
    return init_v*time - time*(time-1)/2;
  }else{
    if(time > init_v) // xvelocity is always positive
      return init_v*(init_v+1)/2;
    else
      return init_v*time - time*(time-1)/2;
  }
}

bool in_range(const struct target& t, point p){
  return t.xmin <= p.first && p.first <= t.xmax
    && t.ymin <= p.second && p.second <= t.ymax;
};

std::optional<uint> max_height(const int xv0, const int yv0, const struct target& t){
  const uint itime = 2*yv0+1;
  const uint maxh = yv0*(yv0+1)/2;
  point p = {pos_at(xv0, itime, false), 0}; // y-pos is zero at itime
  int xv = xv0 - itime;
  if(xv < 0) xv = 0;
  int yv = -yv0-1;
  uint time = itime;
  // std::cout << "Start time: " << itime << " MaxHeight: " << maxh
  // 	    << " Velocity: " << xv << ',' << yv
  // 	    << " Position: " << p.first << ',' << p.second << std::endl;
  static constexpr auto xv_update = [](int v){return v > 0 ? v - 1 : 0;};
  static constexpr auto pos_update = [](point p, int xv, int yv) -> point {
    return {p.first+xv, p.second+yv};
  };

  while(p.first <= t.xmax && p.second >= t.ymin){
    if(in_range(t, p)) return maxh;
    p = pos_update(p, xv, yv);
    xv = xv_update(xv);
    yv--;
    time++;
  }
  return {};
}

std::pair<uint, uint> search_positive(const struct target &t){
  const uint xopt = floor(sqrt(2*t.xmin+0.5) - 0.25);
  const uint yopt = -1*t.ymin - 1;
  uint mh = 0;
  uint count = 0;
  for(int x0 = xopt; x0 <= t.xmax; x0++)
    for(int y0 = yopt; y0 > 0; y0--)
      {
	std::optional<uint> oh = max_height(x0, y0, t);
	if(oh){
	  count++;
	  // std::cout << x0 <<',' << y0 << std::endl;
	  if(oh.value() > mh)  mh = oh.value();
	}
      }
  return {mh, count};
}

bool will_strike(uint xv, int yv, const struct target& t){
  static constexpr auto pos = [](int vel, int time) -> int{
    return vel*time - time*(time-1)*0.5;
  };
  auto ira = [xv, yv, t](int time) -> bool{ // in range at time
    int xpos = pos(xv, time), ypos = pos(yv, time);
    return t.xmin <= xpos && xpos <= t.xmax &&
      t.ymin <= ypos && ypos <= t.ymax;
  };
  // ceil, floor are decided apropriately as we enter x+ve and y-ve

  uint txmin = ceil(xv+0.5-sqrt((xv+0.5)*(xv+0.5)-2*t.xmin));
  if(errno == EDOM) { txmin = -1; errno = 0;}
  uint txmax = floor(xv+0.5-sqrt((xv+0.5)*(xv+0.5)-2*t.xmax));
  if(errno == EDOM) { txmax = -1; errno = 0;}
  uint tymin = ceil(yv+0.5+sqrt((yv+0.5)*(yv+0.5)-2*t.ymax));
  uint tymax = floor(yv+0.5+sqrt((yv+0.5)*(yv+0.5)-2*t.ymin));
  // std::cout << txmin << ',' << txmax << ' '
  // 	    << tymin << ',' << tymax << ' '
  // 	    << std::boolalpha << ira(txmin) << ' ' << ira(txmax) << std::endl;

  return(ira(txmin) && txmin >= tymin && txmin <= tymax)
    || (ira(txmax) && txmax >= tymin && txmax <= tymax);
}
uint count_negative(const struct target &t){
  uint count = 0;
  const uint xv_min = floor(sqrt(2*t.xmin+0.5) - 0.25);

  for(uint xv = t.xmax; xv >= xv_min; xv--)
    for(int yv = t.ymin; yv <= 0; yv++)
      if(will_strike(xv, yv, t)){
	// std::cout << xv << ',' << yv << std::endl;
	count++;
      }
  return count;
}

int main(int argc, char* argv[]){
  if(argc != 5){
    fprintf(stderr, "Usage: %s xmin xmax ymin ymax\n", argv[0]);
    return -1;
  }
  int xmin = atoi(argv[1]), xmax = atoi(argv[2]);
  int ymin = atoi(argv[3]), ymax = atoi(argv[4]);
  const struct target t {xmin, xmax, ymin, ymax};
  auto sr = search_positive(t);
  std::cout <<  sr.first << ' ' << sr.second + count_negative(t) << std::endl;
  // std::cout << std::boolalpha << will_strike(9, 0, t) << std::endl;
  return 0;
}

// Local Variables:
// compile-command: "gcc -lm -lstdc++ -std=c++17 -o day17 -Wall -Wextra day17.cc"
// End:
