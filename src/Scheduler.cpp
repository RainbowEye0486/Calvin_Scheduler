#include "../include/Scheduler.h"

#define RAND(min, max)  ((int)((double) rand() / (RAND_MAX) * ((max) - (min))) + (int)(min))
#define NOR_RAND()      ((double)rand() / ((RAND_MAX)))




uint16_t Scheduler::GetSolution() {
    return station;
}

double Scheduler::GetCalculateTime() {
    return calculate_time;
}

void Scheduler::PreProcessing(KJob job) {
    kjob = job;
    // data structure init
    
    leaf_node = (bool *)malloc(kjob.taskNum * sizeof(bool));
    finish_node = (bool *)malloc(kjob.taskNum * sizeof(bool));
    root_node = (bool *)malloc(kjob.taskNum * sizeof(bool));
    for (int i = 0; i < kjob.taskNum; i++) {
        leaf_node[i] = true;
        finish_node[i] = false;
        root_node[i] = false;
    }
    // form tree structure
    for (int i = 0; i < kjob.taskNum; i++) {
        if (kjob.task_list[i].and_next.size() == 0 && kjob.task_list[i].or_next.size() == 0) {
            root_node[i] = true;
        }
        else {
            for(int j = 0; j < kjob.task_list[i].and_next.size(); j++){
                leaf_node[kjob.task_list[i].and_next[j] - 1] = false;
                kjob.task_list[kjob.task_list[i].and_next[j] - 1].and_prev.push_back(i + 1);
                if(PRINT) cout << "[INFO]: " << (int)kjob.task_list[kjob.task_list[i].and_next[j] - 1].num << " node and prev " << i+1 << endl;
            }
            for(int j = 0; j < kjob.task_list[i].or_next.size(); j++){
                leaf_node[kjob.task_list[i].or_next[j] - 1] = false;
                kjob.task_list[kjob.task_list[i].or_next[j] - 1].or_prev.push_back(i + 1);
                if(PRINT) cout << "[INFO]: " << (int)kjob.task_list[kjob.task_list[i].or_next[j] - 1].num << " node or prev " << i+1 << endl;
            }
        }
    }
    for (int i = 0; i < kjob.taskNum; i++) {
        if (leaf_node[i]) {
            if(PRINT) cout << "[INFO]: leaf node " << i + 1 << endl;
        }        
        if (root_node[i])
            if(PRINT) cout << "[INFO]: root node " << i + 1 << endl;  
            
    }
}


vector<uint16_t> Scheduler::Sequence_Generate(){
    vector<uint16_t> sequence;
    int rand_num;
    uint16_t cycleTime = kjob.cycleTime;
    vector<uint16_t>::iterator it;
    
    for (int i = 0; i < kjob.taskNum; i++) {
        if (leaf_node[i]) {
            avaliable_node.push_back(i + 1);
        }
        finish_node[i] = false;
    }

    // Start random generate sequence
    while (avaliable_node.size() > 0){

        rand_num = RAND(0, avaliable_node.size()); // Select a random node in avaliable list
        
        if (PRINT) cout << "[INFO]: Chosen node " << (int)avaliable_node[rand_num] << endl;
        sequence.push_back(avaliable_node[rand_num]);
        finish_node[avaliable_node[rand_num] - 1] = true;
        
        // Add next posible node to the list
        for (int i = 0; i < kjob.task_list[avaliable_node[rand_num] - 1].or_next.size(); i++){
            uint16_t search_node = kjob.task_list[avaliable_node[rand_num] - 1].or_next[i];
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1]){
                avaliable_node.push_back(search_node);
            }
        }
        for (int i = 0; i < kjob.task_list[avaliable_node[rand_num] - 1].and_next.size(); i++){
            bool and_done = true;
            uint16_t search_node = kjob.task_list[avaliable_node[rand_num] - 1].and_next[i];
            if(PRINT) cout << "[INFO]: prev node" << (int)kjob.task_list[search_node - 1].and_prev.size() << endl;
            for (int j = 0; j < kjob.task_list[search_node - 1].and_prev.size(); j++) {
                if(PRINT) cout << "[INFO]: prev and node[" << (int)kjob.task_list[search_node - 1].and_prev[j] << "]finished." << endl;
                if (!finish_node[kjob.task_list[search_node - 1].and_prev[j] - 1]) 
                    and_done = false;
            }   
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1] && and_done){
                avaliable_node.push_back(search_node);
                if (PRINT) cout << "[INFO]: " << (int)search_node << " added." << endl;
            }
        }
        avaliable_node.erase(avaliable_node.begin() + rand_num);      
    }

    if (PRINT) cout << "[INFO]: output sequence "; 
    if (PRINT) PRINT_SEQUENCE(sequence);
    
    return sequence;
}


uint16_t Scheduler::Fitness(vector<uint16_t> sequence){

    // Calculate total used station in the sequence
    if(PRINT) cout << "[INFO]: evaluate sequence, cycletime = " << (int)kjob.cycleTime << endl;
    uint16_t cycle = 1; // stations used in the 
    uint16_t first = sequence[0] - 1;
    uint16_t current_time = kjob.task_list[sequence[0] - 1].time;
    
    for (int i = 1; i < sequence.size(); i++) {
        if(PRINT) cout << "[INFO]: task " << (int)sequence[i] << endl;
        uint16_t backward_setup = kjob.setup_matrix[sequence[i - 1] - 1][sequence[i] - 1];
        uint16_t forward_setup = kjob.setup_matrix[sequence[i] - 1][first];
        if(PRINT) cout << "[INFO]: forward setup " << (int)forward_setup << endl;


        if (current_time + backward_setup + forward_setup + kjob.task_list[sequence[i] - 1].time 
                > cycle * kjob.cycleTime){
            // Need to open a new workstation
            first = sequence[i] - 1;
            if(PRINT) cout << "[INFO]: new first " << (int)sequence[i] << endl;
            if(PRINT) cout << "[INFO]: cycle to " << (int)(cycle * kjob.cycleTime) << endl;
            current_time = (cycle * kjob.cycleTime) + kjob.task_list[sequence[i] - 1].time;
            if(PRINT) cout << "[INFO]: current time " << (int)current_time << endl;
            cycle++;
        }
        else {
            current_time += backward_setup + forward_setup + kjob.task_list[sequence[i] - 1].time;
            if(PRINT) cout << "[INFO]: current time " << (int)current_time << endl;
        }
    }
    if(PRINT) cout << "[INFO]: station " << (int)cycle << endl;

    return cycle;
}


bool Scheduler::Valid(vector<uint16_t> sequence){
    if (PRINT) cout << "[INFO]: Varafied new sequence";
    for (int i = 0; i < sequence.size(); i++) {
        if (PRINT) cout << " " << (int)sequence[i];
        if (sequence[i] == 0)
            return false;
        finish_node[i] = false;
    }
    if (PRINT) cout << endl;
    
    finish_node[sequence[0] - 1] = true;
    bool legal_sequence = true;
    bool valid;

    if (!leaf_node[sequence[0] - 1]){
        if (PRINT) cout << "[INFO]: the sequence valid : 0" << endl;
        return false;
    }
    for (int i = 1; i < sequence.size(); i++) {
        // Check -or connection
        valid = false; 
        for (int j = 0; j < kjob.task_list[sequence[i] - 1].or_prev.size(); j++){
            if (finish_node[kjob.task_list[sequence[i] - 1].or_prev[j] - 1]) 
                valid = true; // Need at least one pre node finished
        }
        if (kjob.task_list[sequence[i] - 1].or_prev.size() != 0 && !valid) {
            legal_sequence = false;
            if(PRINT) cout << "[INFO]: invalid in node [or] " << (int)sequence[i] << endl;
            break;
        }
        // Check -and connection
        valid = true;
        for (int j = 0; j < kjob.task_list[sequence[i] - 1].and_prev.size(); j++){
            if (!finish_node[kjob.task_list[sequence[i] - 1].and_prev[j] - 1])
                valid = false; // Need all pre node finished
        }
        if (kjob.task_list[sequence[i] - 1].and_prev.size() != 0 && !valid) {
            legal_sequence = false;
            if(PRINT) cout << "[INFO]: invalid in node [and] " << (int)sequence[i] << endl;
            break;
        }
        finish_node[sequence[i] - 1] = true;
    }
    
    if (PRINT) cout << "[INFO]: the sequence valid :" << legal_sequence << endl;
    return legal_sequence;
}


void Scheduler::PRINT_SEQUENCE(vector<uint16_t> sequence) {
    for (int i = 0; i < sequence.size(); i++){
        cout << sequence[i] << " ";
    }
    cout << endl;
}
