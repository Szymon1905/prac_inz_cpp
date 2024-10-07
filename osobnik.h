#include <iostream>
#include <vector>


using namespace std;

class Solution {
public:
    vector<int> cities;
    int path_length;

    Solution(vector<int> path, int path_length);

    Solution();


    void reset();
};