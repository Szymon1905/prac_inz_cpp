#include "genetyczny.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <random>
#include <unordered_set>
#include "mutacje.h"


extern int global_city_count;
extern int starting_population_size;
extern float crossover_factor;
extern float mutation_rate;
extern vector<vector<int>> global_matrix;
extern float mutation_method;
extern int liczba_operacji;


uint32_t seed = 1; // ziarno generatora liczb losowych
mt19937 gen(seed); // inicjalizacja generatora liczb losowych

auto start= chrono::high_resolution_clock::now();;

vector<Solution> population;

Solution best_solution = Solution(vector<int>(), INT32_MAX);

void fisher_yates_shuffle(vector<int>& vec, mt19937& gen) {
    int len = vec.size();
    for (int i = len - 1; i > 0; i--) {
        uniform_int_distribution<int> dist(0, i);
        int j = dist(gen);
        swap(vec[i], vec[j]);
    }
}

void print_best() {
    cout << endl;
    cout << endl;
    cout << "Najlepsze rozwiązanie: ";
    for (int elem: best_solution.cities) {
        cout << elem << " ";
    }
    cout << best_solution.cities.front() << endl;
    cout << endl;
    cout << "Koszt rozwiązania: " << best_solution.path_length;
    cout << endl;
    cout << endl;
}


void generate_starting_population() {

    // Generowanie miast do vectora
    vector<int> cities;
    for (int i = 0; i < global_city_count; i++) {
        cities.push_back(i);
    }

    // reset populacji po restarcie algorytmu
    population.clear();

    // generowanie losowych permutacji miast metodą fisher yates shuffle i wstawianie ich do populacji początkowej
    for (int i = 0; i < starting_population_size; i++) {
        fisher_yates_shuffle(cities,gen);
        Solution solution = Solution(cities, INT32_MAX);
        population.push_back(solution);
    }

    for (Solution &solution: population) {
        solution.path_length = calculate_path_length(cities, global_matrix); // obliczanie długości dróg każdego osobnika w populacji
    }

}

int calculate_path_length(const vector<int> &solution, vector<vector<int>> matrix) {
    int sum_path = 0;
    for (int i = 0; i < solution.size() - 1; i++) {
        sum_path += matrix[solution[i]][solution[i + 1]];
    }

    // dodanie drogi powrotnej do ostatniego miasta
    sum_path += matrix[solution.back()][solution.front()];
    return sum_path;
}


// Metoda oceniajaca całą populację
void evaluate_population() {

    for (Solution &solution: population) {

        // Obliczanie długości ścieżki
        solution.path_length = calculate_path_length(solution.cities, global_matrix);

        // porównanie z aktualnym najlepszym rozwiązaniem
        if (solution.path_length < best_solution.path_length) {

            // aktualizacja najlepszego rozwiązania dla przeżywalności najlepszego osobnika
            best_solution = solution;

            cout << "Nowy najlepszy: " << solution.path_length << endl;
        }
    }
}

// Metoda implementująca ruletkę, wybierjąca osobniki do krzyżówania
vector<Solution> choosing_parent_book_method(){
    // Obliczamy sumę wartości funkcji celu (tj. ścieżki) dla wszystkich osobników.
    float path_sum = 0.0;
    for (const Solution &solution : population) {
        path_sum += 1.0f / (float)solution.path_length;
    }

    // Inicjalizacja pustego vectora na wybranych rodziców
    vector<Solution> chosen_ones;

    uniform_real_distribution<float> distribution(0.0, path_sum);

    // Wybieramy rodziców
    float los;
    float sum;
    for (int i = 0; i < int(population.size()) / 2; i++) {
        sum = 0;
        los = distribution(gen);

        for (const Solution &solution : population) {
            sum += 1.0f / (float)solution.path_length;
            if (sum >= los) {
                chosen_ones.push_back(solution);
                break;
            }
        }
    }

    return chosen_ones;
}

// funkcja sprawdzająca czy dane miasto jest w vectorze
bool check_if_contains(vector<int> vector, int liczba) {
    for (int pole: vector) {
        if (pole == liczba) {
            return true;
        }
    }
    return false;
}

// Metoda implementująca krzyżównaie OX 2 rodziców w celu stworzenia potomka
Solution OX_crossover(Solution parent1, Solution parent2) {
    Solution successor;
    int path_length = int(population[0].cities.size());

    uniform_int_distribution<> distribution(0, path_length - 2);
    int point1 = distribution(gen);
    int point2 = distribution(gen);

    // zamiana punktów jeśli trzeba
    if (point1 > point2) {
        swap(point1, point2);
    }

    // wypełninie potomka -1 (puste pole)
    for (int i = 0; i < path_length; i++) {
        successor.cities.push_back(-1);
    }

    // wstawienie miast do potomka od rodzica pomiędzy punktami cięcia
    for (int i = point1; i <= point2; i++) {
        successor.cities[i] = parent1.cities[i];
    }

    //wybranie do osobnego vectora miast ktore mogę wziąść z rodzica2 (te miasta co nie zostały pobrane z rodzica 1) z prawej storny
    vector<int> available_cities;
    for (int i = point2 + 1; i < path_length; i++) {
        if (not check_if_contains(successor.cities, parent2.cities[i])) {
            if (not check_if_contains(available_cities, parent2.cities[i])) {

                // jeśli miasta nie ma w cześci zielonej (slajd 17), dodajemy miasto do dostepnych miast
                available_cities.push_back(parent2.cities[i]);
            }
        }
    }

    //wybranie do osobnego vectora miast ktore mogę wziąść z rodzica2 z lewej storny
    for (int i = 0; i <= point2; i++) {
        if (not check_if_contains(successor.cities, parent2.cities[i])) {
            if (not check_if_contains(available_cities, parent2.cities[i])) {

                // jeśli miasta nie ma w cześci zielonej (slajd 17), dodajemy miasto do dostepnych miast
                available_cities.push_back(parent2.cities[i]);
            }
        }
    }


    // Wypełnienie potomka miastami z rodzica 2 częsci prawej
    for (int i = point2 + 1; i < path_length; i++) {
        if (successor.cities[i] == -1) {

            // jeśli pole jest puste (-1) oraz miasto się nie powtarza, wpsiujemy miasto
            successor.cities[i] = available_cities.front();
            available_cities.erase(available_cities.begin());
        }
    }

    // Wypełnienie potomka miastami z rodzica 2 częsci lewej
    for (int i = 0; i < point1; i++) {

        if (successor.cities[i] == -1) {
            // jeśli pole jest puste (-1) oraz miasto się nie powtarza, wpsiujemy miasto
            successor.cities[i] = available_cities.front();
            available_cities.erase(available_cities.begin());
        }
    }

    return successor;
}

// Funkcja implmentująca krzyżówanie, zwracająca nową populację
void crossover() {
    Solution succesor1, succesor2;

    uniform_int_distribution<> distribution(0, int(population.size()) - 1);
    uniform_real_distribution<float> crossover_chance(0.0f, 1.0f);

    vector<Solution> new_ones;

    // Generacja pierwszego potomka
    for (Solution solution: population) {
        float chance = crossover_chance(gen);

        if (chance < crossover_factor) {
            int rodzic1 = distribution(gen);
            int rodzic2 = distribution(gen);
            succesor1 = OX_crossover(population[rodzic1], population[rodzic2]);
            new_ones.push_back(succesor1);
        } else{
            new_ones.push_back(solution);
        }

    }

    // Generacja drugiego potomka
    for (Solution solution: population) {
        float szansa = crossover_chance(gen);

        if (szansa < crossover_factor) {
            int parent1 = distribution(gen);
            int parent2 = distribution(gen);
            succesor2 = OX_crossover(population[parent2], population[parent1]);
            new_ones.push_back(succesor2);
        } else{
            new_ones.push_back(solution);
        }

    }

    population = new_ones;
}

// Główna funkcja alogrytmu genetycznego używająca określonego czasu jako warunku stopu
void genetic(int duration) {
    vector<int> a;
    liczba_operacji = 0;

    // generacja populacji startowej
    generate_starting_population();

    //reset najlepszego osobnika
    best_solution.reset();

    // start pomiaru czasu
    start = chrono::high_resolution_clock::now();
    auto stop = start + chrono::seconds(duration);

    // ocena populacji startowej
    evaluate_population();


    while (chrono::high_resolution_clock::now() < stop) {  // etap 3

        evaluate_population();

        population = choosing_parent_book_method();

        crossover();

        mutation();

        liczba_operacji++;

    }

    // wypisanie najlepszego
    print_best();

    cout<<"Liczba operacji: "<<liczba_operacji<<endl;
}

// Główna funkcja alogrytmu genetycznego używająca określonej liczby pokoleń jako warunku stopu uzyta do testów wydajności
void genetic_with_timer(int liczba_op) {
    vector<int> a;
    liczba_operacji = liczba_op;

    // generacja populacji startowej
    generate_starting_population();

    //reset najlepszego osobnika
    best_solution.reset();

    // start pomiaru czasu
    start = chrono::high_resolution_clock::now();

    // ocena populacji startowej
    evaluate_population();

    while (liczba_operacji > 0) {

        evaluate_population();

        population = choosing_parent_book_method();

        crossover();

        mutation();

        liczba_operacji--;

    }
    auto stop = chrono::high_resolution_clock::now(); // koniec pomiaru czasu

    auto time = chrono::duration_cast<chrono::milliseconds>(stop - start).count(); // zliczenie czasu wykonania

    // wypisanie najlepszego osobnika / rozwiązania
    print_best();

    cout<<"Zadana liczba operacji ukończona "<<endl;
    cout<<"Czas w milisekundach: "<< time<<endl;
}