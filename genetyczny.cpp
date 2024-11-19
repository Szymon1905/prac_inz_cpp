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
extern int roulette_ver;
extern int liczba_operacji;

// random number generator
random_device rd;
//mt19937 gen(rd());
uint32_t seed = 12345;
mt19937 gen(seed);

auto start= chrono::high_resolution_clock::now();;

vector<Solution> population;

Solution best_solution = Solution(vector<int>(), INT32_MAX);

void fisher_yates_shuffle(vector<int>& vec, mt19937& gen) {
    int len = vec.size();
    for (int i = len - 1; i > 0; i--) {
        //cout<<"przed range i = "<<i<<endl;
        uniform_int_distribution<int> dist(0, i);
        int j = dist(gen);
        //cout<<"po range j = "<<j<<endl;
        swap(vec[i], vec[j]);
    }
}

void print_best() {
    cout << endl;
    cout << endl;
    cout << "Best Solution: ";
    for (int elem: best_solution.cities) {
        cout << elem << " ";
    }
    cout << best_solution.cities.front() << endl;
    cout << endl;
    cout << "Cost of solution: " << best_solution.path_length;
    cout << endl;
    cout << endl;
}

void timer(int seconds) {
    auto start = chrono::high_resolution_clock::now();
    auto end = start + chrono::seconds(seconds);


    while (chrono::high_resolution_clock::now() < end) {
        //this_thread::sleep_for(chrono::milliseconds(10));
    }

    cout << "\rRemaining time: "
            << chrono::duration_cast<chrono::seconds>(end - chrono::high_resolution_clock::now()).count()
            << " seconds";
    cout.clear();

    cout << "Out of time" << endl;
}

void generate_starting_population() {

    // generate cities / cities
    vector<int> cities;
    for (int i = 0; i < global_city_count; i++) {
        cities.push_back(i);
    }

    // clear population on restart
    population.clear();

    // generating random permutations of cities by the shuffle method and inserting them into the starting population
    for (int i = 0; i < starting_population_size; i++) {
        //shuffle(cities.begin(), cities.end(), gen);
        fisher_yates_shuffle(cities,gen);
        Solution solution = Solution(cities, INT32_MAX);
        population.push_back(solution);
    }

    for (Solution &solution: population) {
        solution.path_length = calculate_path_length(cities, global_matrix);
    }

}

int calculate_path_length(const vector<int> &solution, vector<vector<int>> matrix) {
    int sum_path = 0;
    for (int i = 0; i < solution.size() - 1; i++) {
        sum_path += matrix[solution[i]][solution[i + 1]];
    }

    // add missing path to starting city
    sum_path += matrix[solution.back()][solution.front()];
    return sum_path;
}

void evaluate_population() {

    for (Solution &solution: population) {


        // calcaltion of path length
        solution.path_length = calculate_path_length(solution.cities, global_matrix);

        // compare with current best solution
        if (solution.path_length < best_solution.path_length) {

            // update best solution for survivability of the best
            best_solution = solution;

            /*
             for (int a: best_solution.cities) {
                cout << a << " ";
            }
             */
            //auto t = chrono::high_resolution_clock::now();
            //auto minelo_czasu = chrono::duration_cast<chrono::milliseconds>(t - start).count();
            //cout << "Nowe najlepsze rozwiazanie znalezione po " << minelo_czasu << " milisekundach." << endl;
            // cout << best_solution.cities[0] << endl;
            cout << "New best: " << solution.path_length << endl;
        }
    }
}

vector<Solution> custom_parent_choosing_method() {
    // Calculate the sum of the values of the objective function (i.e., path length) for all solutions
    int path_sum = 0;
    for (const Solution &solution : population) {
        path_sum += solution.path_length;
    }

    // Initialization of an empty list of selected parents
    vector<Solution> chosen_ones;

    // Checking that the population is not empty
    if (population.empty()) {
        return chosen_ones;
    }

    sort(population.begin(), population.end(), [](const Solution &a, const Solution &b) {
        return a.path_length < b.path_length;
    });

    // Chooosing parents
    for (int i = 0; i < int(population.size()) / 2; i++) {
        int sum = 0;
        int los = rand() % path_sum;

        for (const Solution &solution : population) {
            sum += solution.path_length;
            if (sum >= los) {
                chosen_ones.push_back(solution);
                break;
            }
        }
    }

    return chosen_ones;
}


vector<Solution> choosing_parent_book_method(){
    // Calculate the sum of the values of the objective function (i.e., path) for all individuals
    float path_sum = 0.0;
    for (const Solution &solution : population) {
        path_sum += 1.0f / (float)solution.path_length;
    }

    // Initialization of an empty list of selected parents
    vector<Solution> chosen_ones;

    uniform_real_distribution<float> distribution(0.0, path_sum);

    // Choosing parents
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


bool check_if_contains(vector<int> vector, int liczba) {
    for (int pole: vector) {
        if (pole == liczba) {
            return true;
        }
    }
    return false;
}

Solution OX_crossover(Solution parent1, Solution parent2) {
    Solution successor;
    int path_length = int(population[0].cities.size());

    uniform_int_distribution<> distribution(0, path_length - 2);
    int point1 = distribution(gen);
    int point2 = distribution(gen);


    if (point1 > point2) {
        swap(point1, point2);
    }

    // wypełninie potomka -1 (puste pole)
    for (int i = 0; i < path_length; i++) {
        successor.cities.push_back(-1);
    }

    // wstawienie miast do potomka od rodzica pomiędzy punktami cięcia,  takie zielone na slajdzie 17
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


void crossover() {
    Solution succesor1, succesor2;

    uniform_int_distribution<> distribution(0, int(population.size()) - 1);
    uniform_real_distribution<float> crossover_chance(0.0f, 1.0f);

    vector<Solution> new_ones;

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

void genetic(int duration) {
    vector<int> a;
    liczba_operacji = 0;

    // generacja populacji startowej
    generate_starting_population(); // etap 1


    //reset najlepszego osobnika
    best_solution.reset();

    // obliczanie czasu dla wątku który odpowiada za warunek stopu
    start = chrono::high_resolution_clock::now();
    auto stop = start + chrono::seconds(duration);

    // start wątku odliczajacego duration do zatrzymania alogrytmu ( warunek stopu )
    thread thread_timer(timer, duration);

    // ocena populacji startowej
    evaluate_population();                // etap 2 tylko raz na początku

    while (chrono::high_resolution_clock::now() < stop) {  // etap 3

        evaluate_population();       // etap 2 każdy kolejny

        if (roulette_ver == 0){
            population = custom_parent_choosing_method();
        } else{
            population = choosing_parent_book_method();
        }


        crossover();


        evaluate_population(); // todo delete

        mutation();

        liczba_operacji++;

    }


    thread_timer.join();

    // wypisanie najlepszego
    print_best();   // etap 7

    cout<<"Liczba operacji: "<<liczba_operacji<<endl;
}


void genetic_with_timer(int liczba_op) {
    vector<int> a;
    liczba_operacji = liczba_op;

    // generacja populacji startowej
    generate_starting_population(); // etap 1


    //reset najlepszego osobnika
    best_solution.reset();

    // obliczanie czasu dla wątku który odpowiada za warunek stopu
    start = chrono::high_resolution_clock::now();
    //auto stop = start + chrono::seconds(duration);

    // start wątku odliczajacego duration do zatrzymania alogrytmu ( warunek stopu )
    //thread thread_timer(timer, duration);

    // ocena populacji startowej
    evaluate_population();                // etap 2 tylko raz na początku

    while (liczba_operacji > 0) {  // etap 3

        evaluate_population();       // etap 2 każdy kolejny

        if (roulette_ver == 0){
            population = custom_parent_choosing_method();
        } else{
            population = choosing_parent_book_method();
        }


        crossover();


        evaluate_population(); // todo delete

        mutation();

        liczba_operacji--;

    }
    auto stop = chrono::high_resolution_clock::now();


    //thread_timer.join();

    auto time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();

    // wypisanie najlepszego
    print_best();   // etap 7

    cout<<"Zadana liczba operacji ukończona "<<endl;
    cout<<"Czas w milisekundach: "<< time<<endl;
}