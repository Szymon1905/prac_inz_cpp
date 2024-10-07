#include "osobnik.h"
#include <vector>
#include <random>

using namespace std;


Solution::Solution(vector<int> path, int path_length) {
    this->cities = path;
    this->path_length = path_length;
}

Solution::Solution() {}

void Solution::reset() {
    this->cities.clear();
    this->path_length = INFINITY;
}
