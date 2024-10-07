#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <fstream>
#include <chrono>
#include <Windows.h>
#include <thread>
#include "genetyczny.h"

using namespace std;



int global_city_count;
vector<vector<int>> global_matrix;

int stop_time=10;
int starting_population_size = 500;
float mutation_rate = 0.01;
float crossover_factor = 0.8;
int mutation_method = 0;
int roulette_ver = 0;
string name_of_matrix;


vector<vector<int>> read_matrix(const string& file_name) {

    ifstream input_file;

    input_file.open(file_name);
    if (input_file.is_open()) {
        cout << "File opened " << file_name << endl;
        name_of_matrix = file_name;
    } else {
        cout << "Unable ot open file" << endl;
        return {};
    }
    int city_count;
    input_file >> city_count;

    vector<vector<int> > matrix(city_count, vector<int>(city_count));

    for (int i = 0; i < city_count; ++i) {
        for (int j = 0; j < city_count; ++j) {
            input_file >> matrix[i][j];
        }
    }
    input_file.close();

    global_city_count = city_count;
    return matrix;
}




int main() {
    SetConsoleOutputCP(CP_UTF8);
    cout<<"Author: Szymon Borzdyński"<<endl;
    int option;

    string name;


    while(true){
        cout << "Options:  [] <- current param value" << endl;
        cout << "0 - read matrix[" << name_of_matrix << "]" << endl;
        cout << "1 - stop criteria [" << stop_time << "]" << endl;
        cout << "2 - starting population size[" << starting_population_size << "]" << endl;
        cout << "3 - mutation rate[" << mutation_rate << "]" << endl;
        if(mutation_method == 0){
            cout << "4 - typ mutacji[invert]"<< endl;
        } else{
            cout << "4 - typ mutacji[swapowanie]"<< endl;
        }
        cout << "5 - crossover factor[" << crossover_factor << "]" << endl;
        cout << "6 - Start the genetic algorithm" << endl;
        cout << "7 - Roulette method[";
        if(roulette_ver == 0){
            cout << "Custom (Recommended)]"<< endl;
        } else{
            cout << "Book]"<< endl;
        }
        cin >> option;

        switch (option) {
            default:
                system("CLS");
                cout << "Invalid option" << endl << endl;
                cin >> option;
                break;
            case 0:
                cout<<"Enter the file name: "<<endl;
                cin >> name;
                global_matrix = read_matrix(name);
                system("CLS");
                break;
            case 1:
                cout<<"Podaj kryterium stopu w sekundach: "<<endl;
                cin >> stop_time;
                break;
            case 2:
                cout<<"Specify the stop criterion in seconds: "<<endl;
                cin >> starting_population_size;
                system("CLS");
                break;
            case 3:
                cout<<"Enter the value of the mutation rate: "<<endl;
                cin >> mutation_rate;
                system("CLS");
                break;
            case 4:
                cout<<"Specify the type of mutation: "<<endl;
                cout<<"0 - invert"<<endl;  // (Inversion Mutation):
                cout<<"1 - swapping"<<endl;  //  (Swap Mutation):
                cin >> mutation_method;
                while(mutation_method != 0 and mutation_method != 1){
                    cout<<"invlaid param, "<<endl;
                    cin >> mutation_method;
                }
                system("CLS");
                break;
            case 5:
                cout<<"Enter the value of the crossover factor: "<<endl;
                cin >> crossover_factor;
                break;
            case 6:
                if(global_matrix.empty()){
                    cout<<"empty matrix"<<endl;
                    break;
                }
                cout<<"Start of algorithm"<<endl;
                genetic(stop_time);
                break;
            case 7:
                cout<<"Enter the roulette version: "<<endl;
                cout<<" 0 - Custom (recommended)"<<endl;
                cout<<" 1 - Book"<<endl;
                cin >> roulette_ver;
                break;
            case 8:
                return 0;
        }
    }
}
