#include <iostream>
#include <string>
#include <fstream>
#include <list>

using namespace std;

list<int> read_nums(const string filename) {
  list<int> num_list = {};
  ifstream s(filename);
  while(!s.eof()){
    int num;
    s >> num;
    num_list.push_back(num);
  }
  return num_list;
}

int count_inc(list<int> depths) {
  int count = 0;
  auto cursor = depths.begin();
  int prev = *cursor;
  cursor++;
  while(cursor != depths.end()){
    if (*cursor > prev) count++;
    prev = *cursor;
    cursor++;
  }
  return count;
}

int count_three_sliding(list<int> depths) {
  int count = 0;
  auto cursor = depths.begin();
  int first  = *cursor;
  int second = *(++cursor);
  int third  = *(++cursor);
  int prev_sum = first+second+third;
  cursor++;
  while(cursor != --depths.end()){ // XXX: But Why does this require -- ?
    first = second;
    second = third;
    third = *cursor;
    int sum = first+second+third;
    // fprintf(stdout, "%d - %d[%d,%d,%d]\n", prev_sum, sum,
    // 	    first, second, third);
    if(sum > prev_sum) count++;
    prev_sum = sum;
    cursor++;
  }
  return count;
}

int main(int argc, char* argv[]){
  if (argc != 2){
    fprintf(stderr, "Usage %s <filename>\n", argv[0]);
    return -1;
  }
  string filename(argv[1]);
  list<int> nums = read_nums(filename);
  fprintf(stdout, "Increasing instances: %d\n", count_inc(nums));
  fprintf(stdout, "Increasing triples: %d\n", count_three_sliding(nums));
  return 0;
}


// Local Variables:
// compile-command: "gcc -lstdc++ day01.cc -o day01"
// End:
