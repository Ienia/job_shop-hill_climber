#include <fstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sys/time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <iostream>
// #include <cstdlib>
#include <limits.h>

using namespace std;

int current_iteration_min_task_time(int nr_of_rows, int *tab) {
    int min_time = INT_MAX;
    
    for(int i = 0; i < nr_of_rows; i++) {
        if(tab[i] > 0 && tab[i] < min_time){
            min_time = tab[i];
        }
    }

    return min_time == INT_MAX ? 0 : min_time;
}

void write_out_table_1Db(int nr_of_rows, bool *tab) {

    int i;
    for(i = 0; i < nr_of_rows; i++)
    {
        
        cout << tab[i] << " ";
        
    }
    cout<<endl;
}
void write_out_table_1D(int nr_of_rows, int *tab)
{
    int i;
    for(i = 0; i < nr_of_rows; i++)
    {
        
        cout << tab[i] << " ";
        
    }
    cout<<endl;
}

void write_out_table_2D(int nr_of_rows, int nr_of_columns, int **tab)
{
    int i, j;
    for(i = 0; i < nr_of_rows; i++)
    {
        for(j = 0; j < nr_of_columns; j++)
            cout << tab[i][j] << " ";
        cout << endl;
    }
}

int main(int argc, char *argv[])
{
    // kod do czytania z linii polecen beasley
    fstream data;
    data.open(argv[1], fstream::in);
    if(data.is_open()){
        int nr_of_jobs, nr_of_machines;
        data >> nr_of_jobs >> nr_of_machines;
        //cout<<"DANE:"<<endl;
        //write_out_table_2D(nr_of_jobs, 2*nr_of_machines, table);
        if(argc == 3){
            if(stoi(argv[2]) > nr_of_jobs){
                cout<<"Za duza liczba zadan, max: "<<nr_of_jobs<<"."<<endl;
                return 1;
            }
            else
                nr_of_jobs = stoi(argv[2]);
        }
    // } 
    // string plik;
    // fstream data;
    // // cout<<"What is a data file *.txt"<<endl;
    // // cin>>plik;
    // plik ="przyklad.txt";
    // data.open(plik, fstream::in);
    // if(data.is_open()){
    //     int max_jobs, nr_of_jobs, nr_of_machines;
    //     data >> max_jobs >> nr_of_machines;
        // cout << "Insert number of jobs you wanna do. " << endl;
        // do
        // {
        //     cout << "Max is " << max_jobs << endl;
        //     cin >> nr_of_jobs;
        // } while (nr_of_jobs > max_jobs);
                        
        int i, j, k;
        //initialization of dynamic array (containing our data)
        
        int **jobshop_table = new int*[nr_of_jobs];
        for(i = 0; i < nr_of_jobs; i++)
            jobshop_table[i] = new int[nr_of_machines*2];
        for(i = 0; i < nr_of_jobs; i++)
            for(j = 0; j < 2*nr_of_machines; j++)
                data >> jobshop_table[i][j];

        data.close();
        int max_time = 0;

        int **solution = new int*[nr_of_jobs];
        for(i = 0; i < nr_of_jobs; i++)
        {
            solution[i] = new int[nr_of_machines];
            for(j = 0; j < nr_of_machines; j++)
            {
                solution[i][j] = -1;
            }
        }

        int **jobshop_table_cp = new int*[nr_of_jobs];

        for(i = 0; i < nr_of_jobs; i++){
            jobshop_table_cp[i] = new int[nr_of_machines*2];
        }
        
        for(i = 0; i < nr_of_jobs; i++){
            for(j = 0; j < nr_of_machines * 2; j++){
                jobshop_table_cp[i][j] = jobshop_table[i][j];
            }
        }

        bool *jobs_done = new bool[nr_of_jobs];

        for(i = 0; i < nr_of_jobs; i++){
            jobs_done[i] = 0;
        }

        bool all_jobs_done = false;

        int current_time = 0;

        int *current_machine_job = new int[nr_of_machines];
        int *current_machine_time = new int[nr_of_machines];
        int *current_machine_task = new int[nr_of_machines];
        bool *current_machine_occupied = new bool[nr_of_machines];

        for (i = 0; i < nr_of_machines; i++) {
            current_machine_job[i] = -1;
            current_machine_task[i] = -1;
            current_machine_time[i] = INT_MAX;
            current_machine_occupied[i] = false;
        }

        //start pomiaru czasu
        struct timeval begin, end;
        gettimeofday(&begin, 0);

        while(!all_jobs_done){
            for(int machine_number = 0; machine_number < nr_of_machines; machine_number++){
                //czy obecnie rozpatrywana maszyna ma juz przypisane jakies zadanie
                if(current_machine_occupied[machine_number])
                        continue;

                for(int job_number = 0; job_number < nr_of_jobs; job_number++){
                    //czy obecnie rozpatrywany job zostal juz wykonany
                    if(jobs_done[job_number])
                        continue;
                
                    int task_number = 0;

                    //dopoki task_number nie przekracza ilosci taskow w danym job
                    while(task_number < nr_of_machines * 2){
                        
                        //jesli czas potrzebny do rozwizania danego taska w job
                        if(jobshop_table[job_number][task_number + 1] == 0) {
                            
                            //jesli obecnie rozpatrywany task nie byl wczesniej rozpoczety
                            if (solution[job_number][task_number / 2] == -1)
                            {
                                //przypisanie czasu rozpoczecia zadania jako obecny timestamp
                                solution[job_number][task_number / 2] = current_time;
                            }
                            
                            //przejscie do nastepnego taska w job
                            task_number+=2;
                            continue;
                        }

                        //czy maszyna potrzebna do rozwiazania obecnego taska odpowiada rozpatrywanej maszynie
                        if(jobshop_table[job_number][task_number] != machine_number) {
                            //potrzebna maszyna niezgodna z rozpatrywana -> przechodzimy do kolejnego zadania
                            break;
                        }

                        //sprawdzenie czy obecnie rozpatrywany task posiada najmniejszy czas wykonania ze wszystkich dostępnych dla rozpatrywanej maszyny (greedy)
                        if(jobshop_table[job_number][task_number + 1] < current_machine_time[machine_number] && !current_machine_occupied[machine_number]){
                            //przypisanie numeru zadania, taska, oraz czasu potrzebnego do jego rozwiązania
                            current_machine_job[machine_number] = job_number;
                            current_machine_time[machine_number] = jobshop_table[job_number][task_number + 1];
                            current_machine_task[machine_number] = task_number / 2;
                        }
                        
                        break;
                    }

                    //czy task_number przekroczyl ilosc taskow w danym job (jesli tak, job uznany jest za wykonany)
                    if(task_number >= nr_of_machines * 2){
                        jobs_done[job_number] = true;
                    }
                }
                
            }
        // cout<<endl<<"w czasie: "<<current_time<<endl;
        // cout<<endl<<"tablica wejsciowa"<<endl;
        // write_out_table_2D(nr_of_jobs, nr_of_machines * 2, jobshop_table);
        // cout<<endl<<"tablica czasow"<<endl;
        // write_out_table_2D(nr_of_jobs, nr_of_machines, solution);
        // cout<<endl<<"zajeta"<<endl;
        // write_out_table_1Db(nr_of_jobs, current_machine_occupied);
        // cout<<endl<<"zadanie"<<endl;
        // write_out_table_1D(nr_of_jobs, current_machine_job);
        // cout<<endl<<"operacja"<<endl;
        // write_out_table_1D(nr_of_jobs, current_machine_task);
        // cout<<endl<<"pozostały czas"<<endl;
        // write_out_table_1D(nr_of_jobs, current_machine_time);
            int min_task_time = current_iteration_min_task_time(nr_of_machines, current_machine_time);

            for(int machine_number = 0; machine_number < nr_of_machines; machine_number++){

                //czy maszyna wykonuje obecnie jakies zadanie
                if(current_machine_job[machine_number] != -1 ){
                    //zmiejszenie czasu potrzebnego do wykonania taska w tabeli wejsciowej oraz w tabeli do konkretnej maszyny
                    jobshop_table[current_machine_job[machine_number]][current_machine_task[machine_number] * 2 + 1] -= min_task_time;
                    current_machine_time[machine_number] -= min_task_time;
                    // jobshop_table[current_machine_job[machine_number]][current_machine_task[machine_number] * 2 + 1]--;
                    // current_machine_time[machine_number]--;

                    //zmiana statusu maszyny na uzywana
                    current_machine_occupied[machine_number] = true;

                    //czy zadanie nie bylo wczesniej wykonywane
                    if(solution[current_machine_job[machine_number]][current_machine_task[machine_number]] == -1){
                        //przypisanie czasu rozpoczecia zadania jako obecny timestamp
                        solution[current_machine_job[machine_number]][current_machine_task[machine_number]] = current_time;                        
                    }
                    
                    //czy maszyna zakonczyla wykonywanie taska
                    if (!current_machine_time[machine_number])
                    {
                        //reset domyślnych wartości dla maszny
                        current_machine_job[machine_number] = -1;
                        current_machine_task[machine_number] = -1;
                        current_machine_time[machine_number] = INT_MAX;
                        current_machine_occupied[machine_number] = false;
                    }
                }
            }
            all_jobs_done = true;
            //czy wszystkie taski sie wykonaly
            for (int index = 0; index < nr_of_jobs; index++)
            {   
                if(!jobs_done[index]) {
                    all_jobs_done = false;
                }
            }
            //obecny czas +1
            current_time += min_task_time;
            // current_time++;
        }
        
        //max time to najwieksza wartosc, z czasow zakonczen maszyn    
        for(i = 0; i < nr_of_jobs; i++){
            for(j = 0; j < nr_of_machines; j++){
                max_time = (solution[i][j] + jobshop_table_cp[i][j*2 + 1]) > max_time ? (solution[i][j] + jobshop_table_cp[i][j*2 + 1]) : max_time;
            }
        }

        // cout<<max_time<<endl;
        // write_out_table_2D(nr_of_jobs, nr_of_machines, solution);
        //end of greedy
        
        //HILLCLIMBER
        //HILLCLIMBER{

        //zrobienie tabelki czasowej wypelnionej -1 (co oznacza ze zadne zadanie nie jest wykonywane)
        // int **times_table = new int*[nr_of_jobs];
        // for(i = 0; i < nr_of_jobs; i++){
        //     times_table[i] = new int[max_time];
        //     for(j = 0; j < max_time; j++)
        //         times_table[i][j] = -1;
        // }
        
        // //uzupelnianie tabelki zgodnie z rozwiazaniem powstalym przez greedy algorytm
        // for(i = 0; i < nr_of_jobs; i++){
        //     for(j = 0; j < nr_of_machines; j++){
        //         for(k = 0; k < jobshop_table_cp[i][j*2+1]; k++)
        //             times_table[i][solution[i][j]+k] = jobshop_table_cp[i][j*2];
        //     }
        // }
        
        // write_out_table_2D(nr_of_jobs, max_time, times_table);

        int new_max_time = 0;

        int **new_solution = new int*[nr_of_jobs];
        for(i = 0; i < nr_of_jobs; i++)
        {
            new_solution[i] = new int[nr_of_machines];
            for(j = 0; j < nr_of_machines; j++)
            {
                new_solution[i][j] = -1;
            }
        }

        //zdublowac tablice nizej zeby przechowywały wartosci poprzedniego rozwiazania
        //poprzenosic petle zerujace do srodka petli while
        
        int **sol_current_job_shifted_machines = new int*[nr_of_jobs];
        for(i = 0; i < nr_of_jobs; i++)
        {
            sol_current_job_shifted_machines[i] = new int[nr_of_machines];
            for(j = 0; j < nr_of_machines; j++)
            {
                sol_current_job_shifted_machines[i][j] = -1;
            }
        }
        
        int *sol_current_machine_shifted_job_number = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            sol_current_machine_shifted_job_number[i] = -1;
        }

        int *sol_current_machine_shifted_job_start = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            sol_current_machine_shifted_job_start[i] = -1;
        }
        
        int *sol_current_machine_shifted_task_number = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            sol_current_machine_shifted_task_number[i] = -1;
        }

        int **current_job_shifted_machines = new int*[nr_of_jobs];
        for(i = 0; i < nr_of_jobs; i++)
        {
            current_job_shifted_machines[i] = new int[nr_of_machines];
            for(j = 0; j < nr_of_machines; j++)
            {
                current_job_shifted_machines[i][j] = -1;
            }
        }
        
        bool *current_machine_increase_shifted_job_start = new bool[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            current_machine_increase_shifted_job_start[i] = false;
        }

        int *current_machine_shifted_job_number = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            current_machine_shifted_job_number[i] = -1;
        }

        int *current_machine_shifted_job_start = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            current_machine_shifted_job_start[i] = -1;
        }
        
        int *current_machine_shifted_task_number = new int[nr_of_machines];
        for(i = 0; i < nr_of_machines; i++)
        {
            current_machine_shifted_task_number[i] = -1;
        }

         //numer znalezionego job z mozliwoscia ulepszenia dzialania
        int new_found_job_number = -1;
        //numer taska w job ktory mozna przesunac
        int new_found_task_number = -1;
        //timestamp w ktorym znaleziony task ma sie rozpoczac
        int new_found_task_start_time = -1;
        //numer znalezionej maszyny
        int new_found_machine_number = -1;
        i = 0;
        j = 0;

        int reps = 0;

        int job_iterator = 0;
        int task_iterator = 0;
    
        //VVVVVV
        while(job_iterator < nr_of_jobs){
            for(; job_iterator < nr_of_jobs; job_iterator++){
                for(; task_iterator < nr_of_machines; task_iterator++){
                    if(task_iterator == 0 && solution[job_iterator][task_iterator] != 0){
                        new_found_job_number = job_iterator;
                        new_found_task_number = task_iterator;
                        new_found_task_start_time = 0;
                        new_found_machine_number = jobshop_table[job_iterator][task_iterator];
                        task_iterator++;
                        break;
                    }
                    if(task_iterator > 0 && solution[job_iterator][task_iterator] > (solution[job_iterator][task_iterator - 1] + jobshop_table_cp[job_iterator][task_iterator * 2 - 1])){
                        new_found_job_number = job_iterator;
                        new_found_task_number = task_iterator;
                        new_found_task_start_time = solution[job_iterator][task_iterator - 1] + jobshop_table_cp[job_iterator][task_iterator * 2 - 1];
                        new_found_machine_number = jobshop_table[job_iterator][task_iterator * 2];
                        task_iterator++;
                        break;
                    } 
                }
                if(new_found_job_number != -1) {
                    break;
                }
                task_iterator = 0;
            }

            if (job_iterator == nr_of_jobs)
            {
                break;
            }

            if(current_machine_shifted_job_number[new_found_machine_number] != -1){
                    int job_id = current_machine_shifted_job_number[new_found_machine_number];
                for(int iter = current_machine_shifted_task_number[new_found_machine_number]; iter < nr_of_machines; iter++){
                    int machine_id = current_job_shifted_machines[job_id][iter];
                    if(current_job_shifted_machines[job_id][iter] != -1){
                        current_machine_shifted_job_number[machine_id] = -1;
                        current_machine_shifted_job_start[machine_id] = -1;
                        current_job_shifted_machines[job_id][iter] = -1;
                        current_machine_shifted_task_number[machine_id] = -1;
                    }
                }
            }

            current_machine_shifted_job_number[new_found_machine_number] = new_found_job_number;
            current_machine_shifted_job_start[new_found_machine_number] = new_found_task_start_time;
            current_job_shifted_machines[new_found_job_number][new_found_task_number] = new_found_machine_number;
            current_machine_shifted_task_number[new_found_machine_number] = new_found_task_number;
            
            new_found_job_number = -1;
            new_found_task_number = -1;
            new_found_task_start_time = -1;
            new_found_machine_number = -1;

        for (i = 0; i < nr_of_machines; i++) {
            current_machine_job[i] = -1;
            current_machine_task[i] = -1;
            current_machine_time[i] = INT_MAX;
            current_machine_occupied[i] = false;
            current_machine_increase_shifted_job_start[i] = false;
        }

        for(i = 0; i < nr_of_jobs; i++){
            jobs_done[i] = 0;
        }

        all_jobs_done = false;

        current_time = 0;

        for(i = 0; i < nr_of_jobs; i++){
            for(j = 0; j < nr_of_machines * 2; j++){
                jobshop_table[i][j] = jobshop_table_cp[i][j];
            }
        }

        for(i = 0; i < nr_of_jobs; i++)
        {
            for(j = 0; j < nr_of_machines; j++)
            {
                new_solution[i][j] = -1;
            }
        }

        new_max_time = 0;

        i = 0;
        j = 0;

        // //start greedy
            // cout<<"ITERACJA"<<endl<<"nr przesunietego joba"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_job_number);
            // cout<<endl<<"czas przesunietego joba"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_job_start);
            // cout<<endl<<"nr przesunietego taska"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_task_number);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;
            // write_out_table_2D(nr_of_jobs, nr_of_machines, current_job_shifted_machines);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;

        while(!all_jobs_done){
            // cout<<endl<<endl<<"ITERACJA"<<endl<<"nr przesunietego joba"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_job_number);
            // cout<<endl<<"czas przesunietego joba"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_job_start);
            // cout<<endl<<"nr przesunietego taska"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_shifted_task_number);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;
            // write_out_table_2D(nr_of_jobs, nr_of_machines, current_job_shifted_machines);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;
            // write_out_table_1Db(nr_of_machines, current_machine_occupied);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;
            // write_out_table_2D(nr_of_jobs, nr_of_machines * 2, jobshop_table);
            for(int machine_number = 0; machine_number < nr_of_machines; machine_number++){
                //czy obecnie rozpatrywana maszyna ma juz przypisane jakies zadanie
                if(current_time == current_machine_shifted_job_start[machine_number]){
                    if(current_machine_shifted_task_number[machine_number] > 0 && jobshop_table[current_machine_shifted_job_number[machine_number]][current_machine_shifted_task_number[machine_number] * 2 - 1] > 0) {
                        current_machine_increase_shifted_job_start[machine_number] = true;
                        continue;
                    }
                    current_machine_job[machine_number] = current_machine_shifted_job_number[machine_number];
                    current_machine_time[machine_number] = jobshop_table[current_machine_job[machine_number]][current_machine_shifted_task_number[machine_number] * 2 + 1];
                    current_machine_task[machine_number] = current_machine_shifted_task_number[machine_number];
                    current_machine_occupied[machine_number] = true;
                }
                
                if(current_machine_occupied[machine_number])
                        continue;

                for(int job_number = 0; job_number < nr_of_jobs; job_number++){
                    //czy obecnie rozpatrywany job zostal juz wykonany
                    if(jobs_done[job_number])
                        continue;
                
                    int task_number = 0;

                    //dopoki task_number nie przekracza ilosci taskow w danym job
                    while(task_number < nr_of_machines * 2){
                        
                        //jesli czas potrzebny do rozwizania danego taska w job
                        if(jobshop_table[job_number][task_number + 1] == 0) {
                            
                            //jesli obecnie rozpatrywany task nie byl wczesniej rozpoczety
                            if (new_solution[job_number][task_number / 2] == -1)
                            {
                                //przypisanie czasu rozpoczecia zadania jako obecny timestamp
                                new_solution[job_number][task_number / 2] = current_time;
                            }
                            
                            //przejscie do nastepnego taska w job
                            task_number+=2;
                            continue;
                        }

                        //czy maszyna potrzebna do rozwiazania obecnego taska odpowiada rozpatrywanej maszynie
                        if(jobshop_table[job_number][task_number] != machine_number) {
                            //potrzebna maszyna niezgodna z rozpatrywana -> przechodzimy do kolejnego zadania
                            break;
                        }

                        //sprawdzenie czy obecnie rozpatrywany task posiada najmniejszy czas wykonania ze wszystkich dostępnych dla rozpatrywanej maszyny (greedy)
                        if(jobshop_table[job_number][task_number + 1] < current_machine_time[machine_number] && !current_machine_occupied[machine_number]){

                            if(current_machine_shifted_job_number[machine_number] == -1){
                                current_machine_job[machine_number] = job_number;
                                current_machine_time[machine_number] = jobshop_table[job_number][task_number + 1];
                                current_machine_task[machine_number] = task_number / 2;
                                // cout<<"job: "<<job_number<<" warunek: 1 maszyna: "<<machine_number<<" czas: "<<current_time<<endl;
                            }

                            if(current_machine_shifted_job_number[machine_number] != -1 && (current_machine_shifted_job_number[machine_number] != -1 && (current_time + jobshop_table[job_number][task_number + 1] <= current_machine_shifted_job_start[machine_number]))){
                                current_machine_job[machine_number] = job_number;
                                current_machine_time[machine_number] = jobshop_table[job_number][task_number + 1];
                                current_machine_task[machine_number] = task_number / 2;
                                // cout<<"job: "<<job_number<<" warunek: 2 maszyna: "<<machine_number<<" czas: "<<current_time<<endl;
                            }
                            
                            if(current_machine_shifted_job_number[machine_number] != -1 && current_time >= current_machine_shifted_job_start[machine_number]) {
                                current_machine_job[machine_number] = job_number;
                                current_machine_time[machine_number] = jobshop_table[job_number][task_number + 1];
                                current_machine_task[machine_number] = task_number / 2;
                                // cout<<"job: "<<job_number<<" warunek: 3 maszyna: "<<machine_number<<" czas: "<<current_time<<endl;
                            }

                            //przypisanie numeru zadania, taska, oraz czasu potrzebnego do jego rozwiązania
                            // current_machine_job[machine_number] = job_number;
                            // current_machine_time[machine_number] = jobshop_table[job_number][task_number + 1];
                            // current_machine_task[machine_number] = task_number / 2;
                        }
                        break;
                    }

                    //czy task_number przekroczyl ilosc taskow w danym job (jesli tak, job uznany jest za wykonany)
                    if(task_number >= nr_of_machines * 2){
                        jobs_done[job_number] = true;
                    }
                }
                
            }
    
            int min_task_time = current_iteration_min_task_time(nr_of_machines, current_machine_time);
            // cout<<endl<<endl<<current_time<<" "<<min_task_time<<endl;
            // cout<<endl<<"tabela"<<endl;
            // write_out_table_2D(nr_of_jobs, nr_of_machines * 2, jobshop_table);
            // cout<<endl<<"joby"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_job);
            // cout<<endl<<"taski"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_task);
            // cout<<endl<<"czasy"<<endl;
            // write_out_table_1D(nr_of_machines, current_machine_time);
            // cout<<endl<<"zajete"<<endl;
            // write_out_table_1Db(nr_of_machines, current_machine_occupied);
            // cout<<endl<<"tabela i jak poprzesuwane w jobach"<<endl;
    
            for(int machine_number = 0; machine_number < nr_of_machines; machine_number++){
                if(current_machine_increase_shifted_job_start[machine_number]){
                    current_machine_increase_shifted_job_start[machine_number] = false;
                    current_machine_shifted_job_start[machine_number] +=  min_task_time;
                    continue;
                }

                //czy maszyna wykonuje obecnie jakies zadanie
                if(current_machine_job[machine_number] != -1 ){
                    //zmiejszenie czasu potrzebnego do wykonania taska w tabeli wejsciowej oraz w tabeli do konkretnej maszyny
                    jobshop_table[current_machine_job[machine_number]][current_machine_task[machine_number] * 2 + 1] -= min_task_time;
                    current_machine_time[machine_number] -= min_task_time;

                    //zmiana statusu maszyny na uzywana
                    current_machine_occupied[machine_number] = true;
                    

                    // niepotrzebne v
                    if(current_machine_time[machine_number] <= 0){
                        jobshop_table[current_machine_job[machine_number]][current_machine_task[machine_number] * 2 + 1] = 0;
                        current_machine_time[machine_number] = 0;
                    }

                    //czy zadanie nie bylo wczesniej wykonywane
                    if(new_solution[current_machine_job[machine_number]][current_machine_task[machine_number]] == -1){
                        //przypisanie czasu rozpoczecia zadania jako obecny timestamp
                        new_solution[current_machine_job[machine_number]][current_machine_task[machine_number]] = current_time;                        
                    }
                    
                    //czy maszyna zakonczyla wykonywanie taska
                    if (!current_machine_time[machine_number])
                    {   
                        //reset domyślnych wartości dla maszny
                        current_machine_job[machine_number] = -1;
                        current_machine_task[machine_number] = -1;
                        current_machine_time[machine_number] = INT_MAX;
                        current_machine_occupied[machine_number] = false;
                    }
                }
            }
            all_jobs_done = true;
            //czy wszystkie taski sie wykonaly
            for (int index = 0; index < nr_of_jobs; index++)
            {   
                if(!jobs_done[index]) {
                    all_jobs_done = false;
                }
            }
            //obecny czas +1
            current_time += min_task_time;
            if(current_time >= max_time) {
                break;
            }
        }
        //max time to najwieksza wartosc, z czasow zakonczen maszyn    
        for(i = 0; i < nr_of_jobs; i++)
            for(j = 0; j < nr_of_machines; j++){
                new_max_time = (new_solution[i][j] + jobshop_table_cp[i][j*2 + 1]) > new_max_time ? (new_solution[i][j] + jobshop_table_cp[i][j*2 + 1]) : new_max_time;
            }


        // cout<<new_max_time<<endl;
        // write_out_table_2D(nr_of_jobs, nr_of_machines, new_solution);

        //sprawdzic czy new_max_time lepsze od starego - jezeli tak to usuwamy tablice times_table i tworzymy na nowo z nowymi wymiarami i wpisujemy wartosci
        //nie trzeba tworzyc wtedy new times table
        if(new_max_time < max_time) {
            // for(i = 0; i < nr_of_jobs; i++){
            //     delete[] times_table[i];
            // }
            // delete[] times_table;
            // times_table = new int*[nr_of_jobs];
            // for(i = 0; i < nr_of_jobs; i++){
            //     times_table[i] = new int[new_max_time];
            //     for(j = 0; j < new_max_time; j++){
            //         times_table[i][j] = -1;
            //     }
            // }
            // for(i = 0; i < nr_of_jobs; i++){
            //     for(j = 0; j < nr_of_machines; j++){
            //         for(k = 0; k < jobshop_table_cp[i][j*2+1]; k++)
            //             times_table[i][new_solution[i][j]+k] = jobshop_table_cp[i][j*2];
            //     }
            // }
        
            for(i = 0; i < nr_of_jobs; i++)
            {
                for(j = 0; j < nr_of_machines; j++)
                {
                    sol_current_job_shifted_machines[i][j] = current_job_shifted_machines[i][j];
                }
            }
        
            for(i = 0; i < nr_of_machines; i++)
            {
                sol_current_machine_shifted_job_number[i] = current_machine_shifted_job_number[i];
            }

            for(i = 0; i < nr_of_machines; i++)
            {
                sol_current_machine_shifted_job_start[i] = current_machine_shifted_job_start[i];
            }
            
            for(i = 0; i < nr_of_machines; i++)
            {
                sol_current_machine_shifted_task_number[i] = current_machine_shifted_task_number[i];
            }

            for(i = 0; i < nr_of_jobs; i++)
            {
                for(j = 0; j < nr_of_machines; j++)
                {
                    solution[i][j] = new_solution[i][j];
                }
            }

            max_time = new_max_time;
        }
        else {
            for(i = 0; i < nr_of_jobs; i++)
            {
                for(j = 0; j < nr_of_machines; j++)
                {
                    current_job_shifted_machines[i][j] = sol_current_job_shifted_machines[i][j];
                }
            }
        
            for(i = 0; i < nr_of_machines; i++)
            {
                current_machine_shifted_job_number[i] = sol_current_machine_shifted_job_number[i];
            }

            for(i = 0; i < nr_of_machines; i++)
            {
                current_machine_shifted_job_start[i] = sol_current_machine_shifted_job_start[i];
            }
            
            for(i = 0; i < nr_of_machines; i++)
            {
                current_machine_shifted_task_number[i] = sol_current_machine_shifted_task_number[i];
            }
        }
    
            reps++;
        // write_out_table_2D(nr_of_jobs, new_max_time, times_table);
        }

        cout<<max_time<<endl;
        write_out_table_2D(nr_of_jobs, nr_of_machines, solution);

        gettimeofday(&end, 0);
        long seconds = end.tv_sec - begin.tv_sec;
        long microseconds = end.tv_usec - begin.tv_usec;
        double elapsed = seconds + microseconds*1e-6;
        printf("Time: %.5f seconds.\n", elapsed);

        fstream ans;
        ans.open("solution.txt", fstream::out);

        ans<<max_time<<endl;
        for(i = 0; i < nr_of_jobs; i++)
        {
            for(j = 0; j < nr_of_machines; j++)
            {
                ans<<solution[i][j]<<" ";
            }
            ans<<endl;
        }

        ans.close();
        // }
        //END HILL*/
        //end of greedy

            /*
                    wykorzystac komentarz niepotrzbne v do rozwiazania bledu z ujemnymi czasami w table co sie odpierdalaja

                    drugi blad to ze jedna maszyna wykonuje jednoczesnie dwa zadania mozna zrobic ze jak dla shifted machine task start sie zgadza
                    ale maszyna jest zajeta to zwiekszyc o 1 az w koncu bedzie wolna
                    albo poprzedni task sie nie zkonczyl wiec zeby zarezerwowana maszyny nie zaczela wykonywac rownoczesnie z poprzednim
                    sprawdzamy czy poprzedni task skonczony, jesli nie to zwiekszamy start time o 1 i continue
                    albo w solution zmniejszyc czas startu poprzedniego o 1 ale to nie koniecznie zadziala xd
            */

            /* DO DOROBIENIA:
            te chyuba wyjebane
                ZEROWANIE PRZY SZUKANIU NOWEGO ROZWIAZANIA
                SPRAWDZIC CZY KOD WYPLUWA DOBRE PRZESUNIECIA W PETLI
            
            tego brakuje
                OGRANICZENIE DO 3 MIN
            */
            
            //te kody musza byc tutaj

            //numer znalezionej maszyny
            // new_found_machine_number = times_table[new_found_job_number][new_found_task_timestamp];

            // //dopisanie maszyny do listy przesunietych maszyn w danym job
            // current_job_shifted_machines[new_found_job_number][0] = new_found_machine_number;

            // //przyppisanie maszynie numeru job w ktorym jest przesunieta
            // current_machine_shifted_job_number[new_found_machine_number] = new_found_job_number;

            // //przypisanie maszynie czasu w ktorym ma zaczac wykonywanie sie przesunietego zadania
            // current_machine_shifted_job_start[new_found_machine_number] = new_found_task_start_time;

            // cout<<endl<<"maszyna: "<<new_found_machine_number<<", przesunieta jest w zadaniu: "<<current_machine_shifted_job_number[new_found_machine_number]<<", rozpocznie go wykonywac w czasie: "<<current_machine_shifted_job_start[new_found_machine_number]<<endl;
        // }
        /*
            musi byc jeszcze tabelka dla kazdego job ktora mowi w jakiej koljenosci sa w danym job poprzesuwane maszyny, bo jak jakies stoja za konkretna maszyna
            i ta maszyna sie zwolni wczesniej albo cos czasowo sie nie zgrywa to tamte sie wyjebia bo nie sa dostepne
        */

        /*
            po wykonaniu hill climber:
                jesli rozwizanie git to przepiszemy wszystko do tablic, a new_found_job_number = -1 i lecimy od nowa po jobach
                jak nie to new_found_job_number zostaje taki sam bo bedziemy lecieli od dalszego czasu i na poczatku znajdujemy nastepny dostepny time stamp
        */

        // cout<<endl<<"nowy job: "<<new_found_job_number<<endl<<"numer taska: "<<new_found_task_number<<endl<<"czas rozpoczecia: "<<new_found_task_timestamp<<endl;

    }
    else {
        cout<<"Problem z otwarciem pliku "<<argv[1]<<"."<<endl;
    }
}
