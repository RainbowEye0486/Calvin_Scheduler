# include "projectConfig.h"
# include "nlohmann/json.hpp"
# include <stdio.h>
# include <stdlib.h>
# include <cstring>
# include <fstream>
# include <unistd.h>
# include <iostream>
# include "../include/SA.h"
# include "../include/GA.h"
# include "../include/GP.h"



string ALGORITHM = "SA";
# define DATA_NUM    200


using namespace std;
using json = nlohmann::json;

// Function declaration
KJob KData_Resolver(string);

int main (int argc, char *argv[])
{
    
    string algorithm = ALGORITHM;
    
    //just print version of the project
    cout << "\n==============Calvin Scheduler Simulation Start==============" << endl;
    cout << "\033[1;32m[SYSTEM]:Project Version Major: " << PROJECT_VERSION_MAJOR << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]:Projevt Version Minor: " << PROJECT_VERSION_MINOR << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]:Project Version Patch: " << PROJECT_VERSION_PATCH << "\033[0m" << endl;
    cout << "\033[1;32m[SYSTEM]: Calvin Scheduler Start\033[0m" << endl;
    // Problem in Damla Kizilay's Problem
    
    SA *CSA = SA::GetSA();
    GA *CGA = GA::GetGA();
    GP *CGP = GP::GetGP();
    
    int dataset_cnt = 1;

    vector<uint16_t> solution;
    vector<uint16_t> feasible; // minimum feasible solution in dataset
    vector<double> calculate_time; 

    int feasible_cnt = 0;
    int optimal_cnt = 0;
    double CPU_time = 0;
    ofstream station_file;
    ofstream timelog_file;



    for (int i = 1332; i < 1361; i++) {
        string data_path = "/home/airobots/Calvin_Scheduler/data/KizilayDataset/KData" + to_string(i) + ".json";
        KJob job = KData_Resolver(data_path);
        cout << "[INFO]: Proceccing Data " << i << endl;
        if (algorithm.compare("SA") == 0){
            CSA->PreProcessing(job);
        }
        else if (algorithm.compare("GA") == 0){
            CGA->PreProcessing(job);
        }
        else if (algorithm.compare("GP") == 0){
            CGP->PreProcessing(job);
        }
        uint16_t best_solution = UINT16_MAX;
        double bestTime = 0;
        for (int j = 0; j < 3; j++) {
            
            if (algorithm.compare("SA") == 0){
                CSA->RunAlgorithm();
                if (CSA->GetSolution() < best_solution){
                    best_solution = CSA->GetSolution();
                    bestTime = CSA->GetCalculateTime();
                }
            }
            else if (algorithm.compare("GA") == 0){
                CGA->RunAlgorithm();
                if (CGA->GetSolution() < best_solution){
                    best_solution = CGA->GetSolution();
                    bestTime = CGA->GetCalculateTime();
                }
            }
            else if (algorithm.compare("GP") == 0){
                CGP->RunAlgorithm();
                if (CGP->GetSolution() < best_solution){
                    best_solution = CGP->GetSolution();
                    bestTime = CGP->GetCalculateTime();
                }
            }
            
        }
        solution.push_back(best_solution);
        calculate_time.push_back(bestTime);
        feasible.push_back(job.station);
        // print best time
        cout << "Best time: " << bestTime << endl;
        cout << "station " << (int)job.station << ", solution " << best_solution << endl;
        
        station_file.open("/home/airobots/Calvin_Scheduler/data/results/station_" + ALGORITHM + "_1.csv", ios::app);
        timelog_file.open("/home/airobots/Calvin_Scheduler/data/results/timelog_" + ALGORITHM + "_1.csv", ios::app);
        station_file << best_solution << "," << endl;
        timelog_file << bestTime << "," << endl;
        station_file.close();
        timelog_file.close();

        if (best_solution <= job.station){
            feasible_cnt++;
            
            if (best_solution < job.station){
                optimal_cnt++;
                cout << "\033[1;32mBeat it! \033[0m" << feasible_cnt << ", win " << optimal_cnt << endl;
            }
            else {
                cout << "\033[1;33mJust feasible!\033[0m" << feasible_cnt << endl;
            }
        }
        else {
            cout << "\033[1;31mBite the dust!\033[0m" << endl;
        }
        CPU_time += bestTime;
    }

    cout << "[INFO]: Avarage CPU time: " << CPU_time/60 << endl;
    cout << "[INFO]: Feasible count: " << feasible_cnt << endl;
    cout << "[INFO]: Optimal count: " << optimal_cnt << endl;




    // result << algorithm << "," << DATA_NUM << "," << CPU_time/100 << "," << feasible_cnt << "," << optimal_cnt << endl;
    // result.close();


    cout << "\033[1;32m[SYSTEM]: Simulation over.\033[0m" << endl;
        
    return 0;
}


/*
Read json file as dataset input
*/
KJob KData_Resolver(string dataName){
    std::ifstream f(dataName);
    if (!f)
        cout << "\033[1;31m[ERROR]:File not found!\033[0m" << endl;   
    json data = json::parse(f);

    uint16_t _taskNum = (uint16_t)data["taskNum"];

    vector<Ktask> _task_list;
    for(int i = 0; i < _taskNum; i++) {
        vector<uint16_t> _and_prev;
        vector<uint16_t> _and_next;
        vector<uint16_t> _or_prev;
        vector<uint16_t> _or_next;
        for(int j = 0; data["task"][i]["and"][j] != nullptr; j++) 
            _and_next.push_back((uint16_t)data["task"][i]["and"][j]);
        for(int j = 0; data["task"][i]["or"][j] != nullptr; j++) 
            _or_next.push_back((uint16_t)data["task"][i]["or"][j]);
        Ktask tmp_task = {
            .num = (uint16_t)data["task"][i]["number"],
            .time = (uint16_t)data["task"][i]["time"],
            .and_prev = _and_prev,
            .and_next = _and_next,
            .or_prev = _or_prev,
            .or_next = _or_next
        };
        _task_list.push_back(tmp_task);
    }
    uint16_t **_setup_matrix = (uint16_t **)malloc(_taskNum * sizeof(uint16_t *));
    
    for(int i = 0; i < _taskNum; i++){
        _setup_matrix[i] = (uint16_t *)malloc(_taskNum * sizeof(uint16_t));
        for(int j = 0; j < _taskNum; j++){
            _setup_matrix[i][j] = (uint16_t)data["setup"][i][j];
        }
    }

    KJob job = {
        .taskNum = _taskNum,
        .station = data["station"],
        .cycleTime = data["cycleTime"],
        .task_list = _task_list,
        .setup_matrix = _setup_matrix
    };
    if (&job == nullptr) 
        cout << "\033[1;31m[ERROR]:Data resolve failed!\033[0m" << endl; 
    

    // cout << "[INFO]: Data resolve over." << endl;
    return job;
}
