# ifndef SCHEDULER_H

# define SCHEDULER_H 

# include <iostream>
# include <cstring>
# include <vector>
# include <cstdlib>
# include <chrono>
# include <time.h>
# include <unistd.h>
# include <algorithm>
# include <math.h>

# define RAND(min, max)  ((int)((double) rand() / (RAND_MAX) * ((max) - (min))) + (int)(min))
# define NOR_RAND()      ((double)rand() / ((RAND_MAX)))

# define PRINT       0
# define Max_iter    300

using namespace std::chrono;
using namespace std;


typedef struct task{
    uint16_t num;
    uint16_t time;
    vector<uint16_t> and_prev;
    vector<uint16_t> and_next;
    vector<uint16_t> or_prev;
    vector<uint16_t> or_next;
}Ktask;


typedef struct job{
    uint16_t taskNum;
    uint16_t station;
    uint16_t cycleTime;
    vector<Ktask> task_list;
    uint16_t **setup_matrix;
}KJob;


class Scheduler
{
public:
    uint16_t GetSolution();
    double GetCalculateTime();

    KJob kjob;
    double calculate_time;

    bool *leaf_node;
    bool *finish_node;
    bool *root_node;
    vector<uint16_t> avaliable_node;
    // Members used in Kizilay's scheduler
    uint16_t taskNum; // How many task in the job
    uint16_t station; // Best value of used station
    uint16_t cycleTime; // Duration in a period
    void PreProcessing(KJob);
    vector<uint16_t> Sequence_Generate();
    uint16_t Fitness(vector<uint16_t>);
    bool Valid(vector<uint16_t>);
    void PRINT_SEQUENCE(vector<uint16_t>);

    private:
};


# endif