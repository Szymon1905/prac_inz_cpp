#include <iostream>
#include <vector>
#include <fstream>
#include <Windows.h>
#include "genetyczny.h"


using namespace std;



int global_city_count;
vector<vector<int>> global_matrix;

int stop_time=10;
int starting_population_size = 500;
float mutation_rate = 0.01;
float crossover_factor = 0.8;
int mutation_method = 0;

int liczba_operacji = 0;
string name_of_matrix;


vector<vector<int>> read_matrix(const string& file_name) {

    ifstream input_file;

    input_file.open(file_name);
    if (input_file.is_open()) {
        cout << "Plik otwarty " << file_name << endl;
        name_of_matrix = file_name;
    } else {
        cout << "Nie udało się otworzyć pliku" << endl;
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
    cout << "Plik zamknięty " << file_name << endl;
    global_city_count = city_count;
    return matrix;
}



int main() {
    SetConsoleOutputCP(CP_UTF8);
    cout<<"Autor: Szymon Borzdyński"<<endl;
    int option;

    string name;


    while(true){
        cout << "Opcje:  [] <- obecna wartość parametru" << endl;
        cout << "0 - Wczytaj macierz sąsiedztwa [" << name_of_matrix << "]" << endl;
        cout << "1 - Określ kryterium stopu [" << stop_time << "]" << endl;
        cout << "2 - Wielkość populacji początkowej [" << starting_population_size << "]" << endl;
        cout << "3 - Współczynnik mutacji [" << mutation_rate << "]" << endl;
        if(mutation_method == 0){
            cout << "4 - typ mutacji[invert]"<< endl;
        } else{
            cout << "4 - typ mutacji[swapowanie]"<< endl;
        }
        cout << "5 - Współczynnik krzyżówania [" << crossover_factor << "]" << endl;
        cout << "6 - Start algorytmu genetycznego" << endl;
        cout << "7 - Start algorytmu genetycznego z określoną liczbą pokoleń" << endl;
        cin>>option;

        switch (option) {
            default:
                system("CLS");
                cout << "Nieprawidłowa opcja" << endl << endl;
                cin >> option;
                break;
            case 0:
                cout<<"Podaj ścieżke do pliku: "<<endl;
                cout<<"Na przykład: tsps/tsp_10.txt: "<<endl;
                cin >> name;
                global_matrix = read_matrix(name);
                break;
            case 1:
                cout<<"Podaj kryterium stopu w sekundach: "<<endl;
                cin >> stop_time;
                break;
            case 2:
                cout<<"Podaj wielkość populacji początkowej: "<<endl;
                cin >> starting_population_size;
                break;
            case 3:
                cout<<"Enter the value of the mutation rate: "<<endl;
                cin >> mutation_rate;
                break;
            case 4:
                cout<<"Określ typ mutacji : "<<endl;
                cout<<"0 - Inwersja"<<endl;
                cout<<"1 - Zamiana (Swap)"<<endl;
                cin >> mutation_method;
                while(mutation_method != 0 and mutation_method != 1){
                    cout<<"Nieprawidłowy parametr, "<<endl;
                    cin >> mutation_method;
                }
                break;
            case 5:
                cout<<"Podaj współczynnik krzyżowania (0..1): "<<endl;
                cin >> crossover_factor;
                break;
            case 6:
                if(global_matrix.empty()){
                    cout<<"Pusta macierz"<<endl;
                    break;
                }
                cout<<"Start alogrytmu"<<endl;
                genetic(stop_time);
                break;
            case 7:
                cout<<"Start algorytmu genetycznego z określoną liczbą pokoleń"<<endl;
                cout<<"Podaj liczbę pokoleń do wykonania: "<<endl;
                int liczba_op;
                cin>>liczba_op;
                genetic_with_timer(liczba_op);
                break;
        }
    }
}
