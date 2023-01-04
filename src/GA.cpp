# include "../include/GA.h"

GA *GA::inst_ = nullptr;

GA *GA::GetGA()
{
    if (inst_ == nullptr)
        inst_ = new GA();
    return inst_;
}

GA::GA(){
    srand(time(NULL));
}

void GA::PreProcessing(KJob job){
    Scheduler::PreProcessing(job);
}

uint16_t GA::GetSolution(){
    return Scheduler::GetSolution();
}

double GA::GetCalculateTime(){
    return Scheduler::GetCalculateTime();
}

void GA::RunAlgorithm() {
    auto start = high_resolution_clock::now();

    if (PRINT)cout << "[INFO]: GA algorithm start." << endl;
    // Parameter setting
    vector<uint16_t> fitness;
    vector<vector<uint16_t>> old_pop;
    uint16_t best_fitness = UINT16_MAX;

    // Generate initial population
    for (int i = 0; i < POP_SIZE; i++) {
        avaliable_node.clear();
        
        vector<uint16_t> gene = Sequence_Generate();
        old_pop.push_back(gene);
        fitness.push_back(Fitness(gene));

        // PRINT_SEQUENCE(old_pop[i]);
        // cout << Fitness(gene) << endl;
    }

    auto rng = default_random_engine {};


    // Algorithm Start
    for (int it = 0; it < Max_iter; it++) {
        
        // Used bubble sort to sort array
        for(int i = POP_SIZE - 1; i > 0; i--)
        {
            for(int j = 0; j <= i-1; j++)
            {
                if( fitness[j] > fitness[j+1])
                {
                    uint16_t tmp_f = fitness[j];
                    vector<uint16_t> tmp_p = old_pop[j];
                    fitness[j] = fitness[j+1];
                    fitness[j+1] = tmp_f;
                    old_pop[j] = old_pop[j+1];
                    old_pop[j+1] = tmp_p;
                }
            }
        }
        if (PRINT)cout << "[INFO]: Sort by fitness." << endl;
        // PRINT_POP(old_pop);
        // PRINT_SEQUENCE(fitness);
        
        // if we find repeat gene, we will 
        if (PRINT)cout << "[INFO]: Repeat gene back." << endl;
        int top = (int)((1 - CR) * POP_SIZE);
        for (int i = 0; i < top; i++) {
            for (int j = i + 1; j < top; j++) {
                bool identicle = true;
                for (int k = 0; k < kjob.taskNum; k++) {
                    if (old_pop[i][k] != old_pop[j][k]){
                        identicle = false;
                        break;
                    }
                }
                if (identicle){
                    vector<uint16_t> tmp = old_pop[j];
                    old_pop.erase(old_pop.begin() + j);
                    old_pop.push_back(tmp);
                    j--;
                }
            }
        }
        if (PRINT)PRINT_POP(old_pop);


        if (PRINT)cout << "[INFO]: Shuffle." << endl;
        shuffle(old_pop.begin() + top, old_pop.end(), rng);
        if (PRINT)PRINT_POP(old_pop);


        if (PRINT)cout << "[INFO]: Crossover." << endl;
        for (int i = old_pop.size() - 1; i >= top; i -= 2){
            vector<uint16_t> mask;
            vector<char> appeared;

            vector<uint16_t> children_1;
            vector<uint16_t> children_2;
            
            int valid_cnt = 0;
            bool success = false;
            while (valid_cnt < 10) {
                valid_cnt++;
                mask = Generate_mask();
                for (int j = 0; j < kjob.taskNum; j++) {
                    appeared.push_back('o');
                }
                for (int j = 0; j < kjob.taskNum; j++) {
                    if (appeared[old_pop[i][j] - 1] == 'x' && appeared[old_pop[i - 1][j] - 1] == 'x') {
                        for (int k = 0; k < kjob.taskNum; k++){
                            if (appeared[k] == 'p') {
                                children_1.push_back(k + 1);
                                appeared[k] = 'x';
                                break;
                            }
                        }
                        continue;
                    }
                    if (mask[j] == 1){
                        if (appeared[old_pop[i - 1][j] - 1] == 'o' || appeared[old_pop[i - 1][j] - 1] == 'p'){
                            children_1.push_back(old_pop[i - 1][j]);
                            if (appeared[old_pop[i][j] - 1] == 'o')
                                appeared[old_pop[i][j] - 1] = 'p';
                            appeared[old_pop[i - 1][j] - 1] = 'x';
                        }
                        else{
                            children_1.push_back(old_pop[i][j]);
                            if (appeared[old_pop[i - 1][j] - 1] == 'o')
                                appeared[old_pop[i - 1][j] - 1] = 'p';
                            appeared[old_pop[i][j] - 1] = 'x';
                        }
                    }
                    else {
                        if (appeared[old_pop[i][j] - 1] == 'o' || appeared[old_pop[i][j] - 1] == 'p'){
                            children_1.push_back(old_pop[i][j]);
                            if (appeared[old_pop[i - 1][j] - 1] == 'o')
                                appeared[old_pop[i - 1][j] - 1] = 'p';
                            appeared[old_pop[i][j] - 1] = 'x';
                        }
                        else{
                            children_1.push_back(old_pop[i - 1][j]);
                            if (appeared[old_pop[i][j] - 1] == 'o')
                                appeared[old_pop[i][j] - 1] = 'p';
                            appeared[old_pop[i - 1][j] - 1] = 'x';
                        }
                    }
                }
                if (Valid(children_1)){
                    if (PRINT)cout << "[INFO]: Children 1 pass." << endl;
                    old_pop[i] = children_1;
                    success = true;
                    break;
                }
                else{
                    children_1.clear();
                    appeared.clear();
                }
            }
            if (!success) {
                if (PRINT)cout << "[INFO]: Children 1 fail." << endl;
                old_pop[i] = Sequence_Generate();
            }

            
            valid_cnt = 0;
            success = false;
            appeared.clear();
            while (valid_cnt < 10){
                valid_cnt++;
                for (int j = 0; j < kjob.taskNum; j++) {
                    appeared.push_back('o');
                }
                mask = Generate_mask(); 
                for (int j = 0; j < kjob.taskNum; j++) {
                    if (appeared[old_pop[i][j] - 1] == 'x' && appeared[old_pop[i - 1][j] - 1] == 'x') {
                        for (int k = 0; k < kjob.taskNum; k++){
                            if (appeared[k] == 'p') {
                                children_2.push_back(k + 1);
                                appeared[k] = 'x';
                                break;
                            }
                        }
                        continue;
                    }
                    if (mask[j] == 1){
                        if (appeared[old_pop[i - 1][j] - 1] == 'o' || appeared[old_pop[i - 1][j] - 1] == 'p'){
                            children_2.push_back(old_pop[i - 1][j]);
                            if (appeared[old_pop[i][j] - 1] == 'o')
                                appeared[old_pop[i][j] - 1] = 'p';
                            appeared[old_pop[i - 1][j] - 1] = 'x';
                        }
                        else{
                            children_2.push_back(old_pop[i][j]);
                            if (appeared[old_pop[i - 1][j] - 1] == 'o')
                                appeared[old_pop[i - 1][j] - 1] = 'p';
                            appeared[old_pop[i][j] - 1] = 'x';
                        }
                    }
                    else {
                        if (appeared[old_pop[i][j] - 1] == 'o' || appeared[old_pop[i][j] - 1] == 'p'){
                            children_2.push_back(old_pop[i][j]);
                            if (appeared[old_pop[i - 1][j] - 1] == 'o')
                                appeared[old_pop[i - 1][j] - 1] = 'p';
                            appeared[old_pop[i][j] - 1] = 'x';
                        }
                        else{
                            children_2.push_back(old_pop[i - 1][j]);
                            if (appeared[old_pop[i][j] - 1] == 'o')
                                appeared[old_pop[i][j] - 1] = 'p';
                            appeared[old_pop[i - 1][j] - 1] = 'x';
                        }
                    }
                }
                if (Valid(children_2)){
                    if (PRINT)cout << "[INFO]: Children 2 pass." << endl;
                    old_pop[i - 1] = children_2;
                    success = true;
                    break;
                }
                else{
                    children_2.clear();
                    appeared.clear();
                }
            }
            if (!success) {
                if (PRINT)cout << "[INFO]: Children 2 fail." << endl;
                old_pop[i - 1] = Sequence_Generate();
            }


            // if elements in children1 is equal to parent1 or parent2, then substitute it with a random sequence
            bool identical = true;
            for (int j = 0; j < kjob.taskNum; j++){
                if (children_1[j] != old_pop[i][j]){
                    identical = false;
                    break;
                }
            }
            for (int j = 0; j < kjob.taskNum; j++){
                if (children_1[j] != old_pop[i - 1][j]){
                    identical = false;
                    break;
                }
            }
            if (identical){
                old_pop[i] = Sequence_Generate();
            }
            // if elements in children2 is equal to parent1 or parent2, then substitute it with a random sequence
            identical = true;
            for (int j = 0; j < kjob.taskNum; j++){
                if (children_2[j] != old_pop[i][j]){
                    identical = false;
                    break;
                }
            }
            for (int j = 0; j < kjob.taskNum; j++){
                if (children_2[j] != old_pop[i - 1][j]){
                    identical = false;
                    break;
                }
            }
            if (identical){
                old_pop[i -1] = Sequence_Generate();
            }
        }
        if (PRINT)PRINT_POP(old_pop);


        if (PRINT)cout << "[INFO]: Mutation." << endl;
        for (int i = old_pop.size() - 1; i >= top; i --){
            // Addopt swap polocy
            double rand_mutation = NOR_RAND();
            
            if (rand_mutation < MR){
                int try_cnt = 0;
                vector<uint16_t> new_pop;
                bool find = false;
                while(try_cnt < 10){
                    new_pop.clear();
                    for (int j = 0; j < kjob.taskNum; j++){
                        new_pop.push_back(old_pop[i][j]);
                    }
                    int rand_pick = RAND(0, kjob.taskNum);
                    int rand_swap = RAND(0, kjob.taskNum);
                    uint16_t tmp = new_pop[rand_pick];
                    new_pop[rand_pick] = new_pop[rand_swap];
                    new_pop[rand_swap] = tmp;
                    if (Valid(new_pop)){
                        find = true;
                        break;
                    }
                    else{
                        try_cnt++;
                    }
                }
                if (find){
                    if (PRINT)cout << "[INFO]: Find new mutate solution!!" << endl;
                    old_pop[i] = new_pop;
                }
                else {
                    if (PRINT)cout << "\033[1;33m[WARNING]: Cant' find mutate solution!\033[0m" << endl;
                }
            }
        }
        if (PRINT)PRINT_POP(old_pop);

        // calculate fitness again
        float total_prop = 0;
        if (PRINT)cout << "[INFO]: Calculate fitness again." << endl;
        for (int i = 0; i < POP_SIZE; i++){
            fitness[i] = Fitness(old_pop[i]);
            total_prop += 1 / (float)fitness[i];
            if (fitness[i] < best_fitness){
                best_fitness = fitness[i];
            }
        }

        // calculate probability
        if (PRINT)cout << "[INFO]: Calculate probability." << endl;
        vector<float> probability;
        for (int i = 0; i < POP_SIZE; i++){
            probability.push_back((1 / (float)fitness[i]) / total_prop);
        }
        
        // select
        if (PRINT)cout << "[INFO]: Select." << endl;
        vector<vector<uint16_t>> new_pop;
        for (int i = 0; i < POP_SIZE; i++){
            int rand_pick = RAND(0, POP_SIZE);
            if (probability[rand_pick] > NOR_RAND()){
                new_pop.push_back(old_pop[rand_pick]);
            }
            else{
                new_pop.push_back(old_pop[i]);
            }
        }

        // update
        if (PRINT)cout << "[INFO]: Update." << endl;
        old_pop = new_pop;
        if (PRINT)PRINT_POP(old_pop);

    }

    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if (PRINT) cout << "[INFO]: GA function spand " << (double)duration.count() / (double)1000000 << " s." << endl;
    calculate_time = (double)duration.count() / (double)1000000;
    station = best_fitness;
}

vector<uint16_t> GA::Generate_mask(){
    vector<uint16_t> mask;
    for (int i = 0; i < kjob.taskNum; i++){
        mask.push_back(RAND(1, 3));
    }
    return mask;
}

void GA::PRINT_POP(vector<vector<uint16_t>>pop){
    for(int i = 0; i < POP_SIZE; i++){
        PRINT_SEQUENCE(pop[i]);
    }
}