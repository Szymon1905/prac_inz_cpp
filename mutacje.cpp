#include "mutacje.h"
#include "genetyczny.h"
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
extern float mutation_rate;
extern int mutation_method;
extern vector<Solution> population;
extern mt19937 gen;

// Metoda implementująca mutację poprzez inwersję
void invert(Solution &osobnik) {

    // Wybieram losowy fragment drogi
    uniform_int_distribution<int> distribution(0, osobnik.cities.size() - 1);
    int punkt1 = distribution(gen);
    int punkt2 = distribution(gen);

    // Upewnienie się, że punkt1 < punkt2
    if (punkt1 > punkt2) {
        swap(punkt1, punkt2);
    }

    // Odwraca kolejność miast w wybranym fragmencie drogi
    reverse(osobnik.cities.begin() + punkt1, osobnik.cities.begin() + punkt2 + 1);

}

// Metoda przygotowująca parametry (wspołczynnik mutacji) do mutacji poprzez inwersję
void invertion_method() {
    auto population_size = population.size();
    int mutation_count = static_cast<int>(mutation_rate * population_size);

    uniform_int_distribution<int> distribution(0, population_size - 1);
    for (int i = 0; i < mutation_count; i++) {
        int random = distribution(gen);
        invert(population[random]);
    }
}

// Metoda mutacji poprzez zamianę (swap)
void swapping_method() {
    uniform_int_distribution<int> distribution(0, population.size() - 1);
    int point1 = distribution(gen);
    int point2 = distribution(gen);
    swap(population[point1], population[point2]);
}

//Metoda wyboru metody mutacji
void mutation() {
    if (mutation_method == 0) {
        invertion_method();
    }
    if (mutation_method == 1) {
        swapping_method();
    }
}