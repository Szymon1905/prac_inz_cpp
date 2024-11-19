#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>
#include <unordered_set>


using namespace std;

void genetic(int);

void evaluate_population();

void generate_starting_population();

int calculate_path_length(const vector<int> &solution, vector<vector<int>> matrix);

void genetic_with_timer(int liczba_op);



