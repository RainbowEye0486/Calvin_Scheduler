# include "../include/GP.h"


GP *GP::inst_ = nullptr;

GP *GP::GetGP()
{
    if (inst_ == nullptr)
        inst_ = new GP();
    return inst_;
}

GP::GP(){
    srand(time(NULL));
}

void GP::PreProcessing(KJob job){
    Scheduler::PreProcessing(job);
}

uint16_t GP::GetSolution(){
    return Scheduler::GetSolution();
}

double GP::GetCalculateTime(){
    return Scheduler::GetCalculateTime();
}

/**
 * @brief
 * A Genetic Programming algorithm to solve workstation scheduling problem.
*/
void GP::RunAlgorithm(){
    auto start = high_resolution_clock::now();

    if (PRINT)cout << "[INFO]: GP algorithm start." << endl;
    // Parameter setting
    vector<Node *> population;
    vector<uint16_t> fitness;
    for (int i = 0; i < POP_SIZE; i++) {
        population.push_back(Tree_init(2, Init_deep, 1));
    }

    // Best fitness
    uint16_t best_fitness = UINT16_MAX;

    // Start Genetic Programming Algorithm
    for (int it = 0; it < Max_iter; it++) {
        
        fitness.clear();
        // Calculate fitness
        for (int i = 0; i < POP_SIZE; i++) {
            fitness.push_back(GP_Fitness(population[i]));
            if (fitness[i] < best_fitness) {
                best_fitness = fitness[i];
            }
        }

        // cout << "[INFO]: Best fitness is " << best_fitness << endl;
        // bubble sort population by fitness
        for (int i = 0; i < POP_SIZE; i++) {
            for (int j = 0; j < POP_SIZE - i - 1; j++) {
                if (fitness[j] > fitness[j + 1]) {
                    swap(fitness[j], fitness[j + 1]);
                    swap(population[j], population[j + 1]);
                }
            }
        }

        for (int i = 0; i < POP_SIZE; i++) {
            fitness[i] = (GP_Fitness(population[i]));
        }

        // Select Elite and erase them from the population
        vector<Node *> elite;
        for (int i = 0; i < ELITE_SIZE; i++) {
            if (GP_Fitness(population[0]) != fitness[0]) {
                cout << "[ERROR]: Fitness is not equal." << endl;    
            }
            elite.push_back(population[0]);
            population.erase(population.begin());
            fitness.erase(fitness.begin());
        }

        // Rescale Fitness
        vector<double> possibility = Rescale(fitness);

        // Select parents
        vector<Node *> parents;
        for (int i = 0; i < POP_SIZE - ELITE_SIZE; i++) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            for (int j = 0; j < possibility.size(); j++) {
                if (rand_num < possibility[j]) {
                    parents.push_back(CloneTree(population[j]));
                    break;
                }
            }
        }

        // Free population
        for (int i = 0; i < POP_SIZE - ELITE_SIZE; i++) {
            FreeTree(population[i]);
        }

        // CrossOver with crossover rate CR
        for (int i = 0; i < POP_SIZE - ELITE_SIZE; i += 2) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < CR) {
                CrossOver(parents[i], parents[i + 1]);
            }
        }

        // Mutation with mutation rate MR
        for (int i = 0; i < POP_SIZE - ELITE_SIZE; i++) {
            double rand_num = (double)rand() / (double)RAND_MAX;
            if (rand_num < MR) {
                Mutation(parents[i]);
            }
        } 

        // Add elite to population
        for (int i = 0; i < ELITE_SIZE; i++) {
            // print fitness of elite
            if (PRINT)cout << "[INFO]: Elite fitness is " << GP_Fitness(elite[i]) << endl;
            parents.push_back(elite[i]);
        }
        population = parents;
    }
    

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    if (PRINT) cout << "[INFO]: GP function spand " << (double)duration.count() / (double)1000000 << " s." << endl;
    calculate_time = (double)duration.count() / (double)1000000;
    station = best_fitness;
}

Node *GP::Node_init(int depth){
    Node *newNode = new Node;
    newNode->Operand = -1;
    newNode->lnode = NULL;
    newNode->rnode = NULL; 
    newNode->depth = depth;
    return newNode;
}

/**
 * @brief
 * free all node in tree
*/
void GP::FreeTree(Node *root){
    if (root == NULL) return;
    FreeTree(root->lnode);
    FreeTree(root->rnode);
    delete root;
}



bool GP::Validate_Depth(Node *root, int depth) {
    if (root == NULL) return true;
    if (root->depth != depth) {
        cout << "\033[1;31m[ERROR]: depth error, level not match\033[0m" << endl;
        return false;
    }
    if (root->depth > Max_deep) {
        cout << "\033[1;31m[ERROR]: depth error, overflow\033[0m" << endl;
        return false;
    }
    if (Validate_Depth(root->lnode, depth + 1) && Validate_Depth(root->rnode, depth + 1)) {
        return true;
    }
    return false; 
}

/**
 * @brief
 * Init a tree with random node.
 * 
 * @param min_deep: min depth of tree
 * @param max_deep: max depth of tree
 * @param init_deep: if is a sub tree, init depth
 * @return Node*
*/
Node *GP::Tree_init(int min_deep, int max_deep, int init_deep){
    int total_node = RAND(pow(2, min_deep) - 1, pow(2, max_deep) - 1);
    if(PRINT) cout << "[INFO]: total node " << total_node << endl;
   
    vector<Node *> tree;
    tree.resize((int)pow(2, max_deep) - 1);
    vector<uint16_t> select_queue;
    select_queue.push_back(1);

    int deep = init_deep;
    tree[0] = Node_init(deep);

    for (int i = 1; i < total_node; i += 2) {

        int rand_idx = RAND(0, select_queue.size());
        uint16_t task_num = select_queue[rand_idx];
        if(PRINT) cout << "[INFO]: Select node " << task_num << endl;
             
        deep = init_deep + log2(task_num);

        tree[2 * task_num] = Node_init(deep + 1);
        tree[2 * task_num - 1] = Node_init(deep + 1);
        tree[task_num - 1]->lnode = tree[2 * task_num - 1];
        tree[task_num - 1]->rnode = tree[2 * task_num];

        select_queue.erase(select_queue.begin() + rand_idx);
        
        if (4 * task_num + 1 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num);
        }
            
        if (4 * task_num + 3 <= pow(2, max_deep) - 1) {
            select_queue.push_back(2 * task_num + 1);
        }  
        
    }

    AssertNode(tree[0]);

    return tree[0];

}

void GP::AssertNode(Node *root) {
    if (!root) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
        return;
    }
    if (!root->lnode && !root->rnode){
        int terminate = RAND(Operand::PT, Operand::RCT + 1);
        root->Operand = terminate;
        return;
    }

    int non_terminate = RAND(Operand::ADD, Operand::DIV + 1);
    root->Operand = non_terminate;

    if (root->rnode) {
        AssertNode(root->rnode);
    }

    if (root->lnode) {
        AssertNode(root->lnode);
    }

}

void GP::Postorder(Node *node) {
    if (!node) return;

    Postorder(node->lnode);
    Postorder(node->rnode);
    stack_.push_back(node->Operand);

}

/**
 * @brief : Clone the tree, and return the root of new tree
*/
Node *GP::CloneTree(Node *root) {
    if (!root) return NULL;
    Node *newNode = new Node;
    newNode->Operand = root->Operand;
    newNode->depth = root->depth;
    newNode->lnode = CloneTree(root->lnode);
    newNode->rnode = CloneTree(root->rnode);
    return newNode;
}




/**
 * @brief : Rescale the fitness value
*/
vector<double> GP::Rescale(vector<uint16_t> fitness) {
    vector<double> rescale_fitness;
    double total = 0;
    uint16_t rank_val = fitness[0];
    int rank = 1;
    for (int i = 0; i < fitness.size(); i++) {
        if (fitness[i] > rank_val) {
            rank_val = fitness[i];
            rank ++;
        }
        rescale_fitness.push_back(sqrt( 1 / (double)rank));
        total += rescale_fitness[i];
    }

    double accum = 0;
    for (int i = 0; i < fitness.size(); i++) {
        rescale_fitness[i] /= total;
        accum += rescale_fitness[i];
        rescale_fitness[i] = accum;
    }

    return rescale_fitness;
}

/**
 * @brief
 * Calculate postfix stack stack_ and output the result.
*/
double GP::Priority() {
    stack<double> prosses_s;

    for (int i = 0; i < stack_.size(); i++) {
        uint16_t element = stack_[i];
        if (element < Operand::PT) { // terminel set
            double _operator = element;
            double _operand2 = prosses_s.top();
            prosses_s.pop();
            double _operand1 = prosses_s.top();
            prosses_s.pop();
            switch (element)
            {
            case ADD:
                prosses_s.push(_operand1 + _operand2);
                break;
            case SUB:
                prosses_s.push(_operand1 - _operand2);
                break;
            case MUL:
                prosses_s.push(_operand1 * _operand2);
                break;
            case DIV:
                if (_operand2 == 0) // protect division
                    prosses_s.push(1);
                else 
                    prosses_s.push(_operand1 / _operand2);
                break;
            default:
                break;
            }

        }
        else { 
            switch (element)
            {
            case Operand::PT:
                prosses_s.push(terminate[0]);
                break;
            case Operand::FS:
                prosses_s.push(terminate[1]);
                break;
            case Operand::BS:
                prosses_s.push(terminate[2]);
                break;
            case Operand::SPR:
                prosses_s.push(terminate[3]);
                break;
            case Operand::TJ:
                prosses_s.push(terminate[4]);
                break;
            case Operand::RJ:
                prosses_s.push(terminate[5]);
                break;
            case Operand::CJ:
                prosses_s.push(terminate[6]);
                break;
            case Operand::RCT:
                prosses_s.push(terminate[7]);
                break;
            default:
                break;
            }
        }
    }
    if (prosses_s.size() != 1){
        cout << "\033[1;33m[WARNING]: Priority illegal.\033[0m" << endl;
    }
    if(PRINT) cout << "[INFO]: priority " << prosses_s.top() << endl;
    return prosses_s.top();

}


/**
 * @brief: This function find a random node in the tree, this node is other than the root node,
 *        and the node can be a non-terminel node or a terminel node. Than, replace the node with a new subtree,
 *       the new subtree is generated by the function Tree_init(deep). deep is (1, Max_deep - current_deep)
*/
void GP::Mutation(Node *root) {
    if (!root) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
    }
        if (!Validate_Depth(root, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
    
    // random select a node from the tree
    vector<Node *> node_queue;
    node_queue.push_back(root);
    int node_idx = 0;
    while (node_idx < node_queue.size()) {
        if (node_queue[node_idx]->lnode) {
            node_queue.push_back(node_queue[node_idx]->lnode);
        }
        if (node_queue[node_idx]->rnode) {
            node_queue.push_back(node_queue[node_idx]->rnode);
        }
        node_idx++;
    }

    int random_idx = RAND(1, node_queue.size());
    Node *random_node = node_queue[random_idx];

    // record random_node's parent 
    Node *parent = NULL;
    for (int i = 0; i < node_queue.size(); i++) {
        if (node_queue[i]->lnode == random_node || node_queue[i]->rnode == random_node) {
            parent = node_queue[i];
            break;
        }
    }

    int parent_deep = parent->depth;

    if (parent_deep > Max_deep) {
        cout << "\033[1;33m[WARNING]: parent_deep > Max_deep\033[0m" << endl;
    }
    
    // create a new subtree
    int new_deep = RAND(1, Max_deep - parent_deep);
    Node *new_subtree = Tree_init(1, new_deep, parent_deep + 1);

    // replace random_node with new_subtree
    if (parent->lnode == random_node) { // random_node is lnode of parent
        Node *tmp = parent->lnode;
        parent->lnode = new_subtree; 
        FreeTree(tmp);
    }
    else { // random_node is rnode of parent
        Node *tmp = parent->rnode;
        parent->rnode = new_subtree;
        FreeTree(tmp);
    }

    if (!Validate_Depth(root, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
}


/**
 * @brief: ramdomly select two subtree from two tree, and exchange them. after exchange
 *          , the depth of two tree won't exceed Max_deep
*/
void GP::CrossOver(Node *root1, Node *root2) {
    bool node1_is_lnode = false;
    bool node2_is_lnode = false;
    if (!root1 || !root2) {
        cout << "\033[1;33m[WARNING]: root not found!\033[0m" << endl;
    }

    // random select a node from the tree
    vector<Node *> node_queue1;
    node_queue1.push_back(root1);
    int node_idx1 = 0;
    while (node_idx1 < node_queue1.size()) {
        if (node_queue1[node_idx1]->lnode) {
            node_queue1.push_back(node_queue1[node_idx1]->lnode);
        }
        if (node_queue1[node_idx1]->rnode) {
            node_queue1.push_back(node_queue1[node_idx1]->rnode);
        }
        node_idx1++;
    }

    int random_idx1 = RAND(1, node_queue1.size());
    Node *random_node1 = node_queue1[random_idx1];

    // record random_node's parent 
    Node *parent1 = NULL;
    for (int i = 0; i < node_queue1.size(); i++) {
        if (node_queue1[i]->lnode == random_node1 || node_queue1[i]->rnode == random_node1) {
            parent1 = node_queue1[i];
            break;
        }
    }
    
    int parent_deep = parent1->depth;

    int random1_height = Tree_Height(random_node1);

    // random select a node from the tree
    vector<Node *> node_queue2;
    node_queue2.push_back(root2);
    int node_idx2 = 0;
    while (node_idx2 < node_queue2.size()) {
        if (node_queue2[node_idx2]->lnode) {
            node_queue2.push_back(node_queue2[node_idx2]->lnode);
        }
        if (node_queue2[node_idx2]->rnode) {
            node_queue2.push_back(node_queue2[node_idx2]->rnode);
        }
        node_idx2++;
    }

    Node *random_node2 = nullptr;
    // clone  node_queue2 to candaite_queue
    vector<Node *> candaite_queue;
    for (int i = 0; i < node_queue2.size(); i++) {
        candaite_queue.push_back(node_queue2[i]);
    }

    while (candaite_queue.size() > 1) {
        int random_idx2 = RAND(1, candaite_queue.size());
        int random2_height = Tree_Height(candaite_queue[random_idx2]);
        if ((random2_height > random1_height) || 
            (candaite_queue[random_idx2]->depth + random1_height > Max_deep)) {
            candaite_queue.erase(candaite_queue.begin() + random_idx2);
        }
        else {
            random_node2 = candaite_queue[random_idx2];
            break;
        }
    }


         
    if (!random_node2) { // only head to choose
        Node *tmp = CloneTree(root2);
        FreeTree(root2);
        if (parent1->lnode == random_node1) { // random_node1 is lnode of parent1
            parent1->lnode = tmp;
            root2 = random_node1;
        }
        else { // random_node1 is rnode of parent1  
            parent1->rnode = tmp;
            root2 = random_node1;
        }
        vector<Node *> tmp_queue1;
        tmp_queue1.push_back(random_node1);
        int tmp_idx1 = 0;
        vector<Node *> tmp_queue2;
        tmp_queue2.push_back(root2);
        int tmp_idx2 = 0;
        while (tmp_idx1 < tmp_queue1.size()) {
            if (tmp_queue1[tmp_idx1]->lnode) {
                tmp_queue1.push_back(tmp_queue1[tmp_idx1]->lnode);
            }
            if (tmp_queue1[tmp_idx1]->rnode) {
                tmp_queue1.push_back(tmp_queue1[tmp_idx1]->rnode);
            }
            tmp_idx1++;
        }
        while (tmp_idx2 < tmp_queue2.size()) {
            if (tmp_queue2[tmp_idx2]->lnode) {
                tmp_queue2.push_back(tmp_queue2[tmp_idx2]->lnode);
            }
            if (tmp_queue2[tmp_idx2]->rnode) {
                tmp_queue2.push_back(tmp_queue2[tmp_idx2]->rnode);
            }
            tmp_idx2++;
        }
        for (int i = 0; i < tmp_queue1.size(); i++) {
            tmp_queue1[i]->depth = tmp_queue1[i]->depth + parent1->depth - 1;
        }
        for (int i = 0; i < tmp_queue2.size(); i++) {
            tmp_queue2[i]->depth = tmp_queue2[i]->depth - parent1->depth + 1;
        }
        Validate_Depth(root2, 1);
        Validate_Depth(root1, 1);
        cout << "only head to choose" << endl;
        return;
    }
        


    // record random_node's parent
    Node *parent2 = NULL;
    for (int i = 0; i < node_queue2.size(); i++) {
        if (node_queue2[i]->lnode == random_node2 || node_queue2[i]->rnode == random_node2) {
            parent2 = node_queue2[i];
            break;
        }
    }

    // exchange random_node1 and random_node2
    if (parent1->lnode == random_node1) { // random_node1 is lnode of parent1
        node1_is_lnode = true;
    }
    if (parent2->lnode == random_node2) { // random_node2 is lnode of parent2
        node2_is_lnode = true;
    }
    if (node1_is_lnode) {
        parent1->lnode = random_node2;
    }
    else {
        parent1->rnode = random_node2;
    }

    if (node2_is_lnode) {
        parent2->lnode = random_node1;
    }
    else {
        parent2->rnode = random_node1;
    }
    // traverse subtree of random_node1 and random_node2
    Resize_deep(random_node1, parent1, parent2);
    Resize_deep(random_node2, parent2, parent1);

    // Validate the tree
    if (!Validate_Depth(root1, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }
    if (!Validate_Depth(root2, 1)) {
        cout << "\033[1;31m[ERROR]: Validate failed!\033[0m" << endl;
    }

    // free memory
    node_queue1.clear();
    node_queue2.clear();


    return;
}


int GP::Tree_Height(Node *root) {
    if (!root) {
        return 0;
    }
    int left_height = Tree_Height(root->lnode);
    int right_height = Tree_Height(root->rnode);
    return max(left_height, right_height) + 1;
}


void GP::Resize_deep(Node *root, Node *parent_old, Node *parent_new) {
    vector<Node *> tmp_queue;
    tmp_queue.push_back(root);
    int tmp_idx = 0;
    while (tmp_idx < tmp_queue.size()) {
        if (tmp_queue[tmp_idx]->lnode) {
            tmp_queue.push_back(tmp_queue[tmp_idx]->lnode);
        }
        if (tmp_queue[tmp_idx]->rnode) {
            tmp_queue.push_back(tmp_queue[tmp_idx]->rnode);
        }
        tmp_idx++;
    }
    for (int i = 0; i < tmp_queue.size(); i++) {
        tmp_queue[i]->depth = tmp_queue[i]->depth + parent_new->depth - parent_old->depth;
    }
}

/**
 * @brief: This function combine generate sequence and evaluate fitness
*/
uint16_t GP::GP_Fitness(Node *root){
    // Calculate total used station in the sequence
    if(PRINT) cout << "[INFO]: evaluate Fitness, cycletime = " << (int)kjob.cycleTime << endl;
    
    vector<uint16_t> sequence;
    stack_.clear();
    Postorder(root);
    
    avaliable_node.clear();
    for (int i = 0; i < kjob.taskNum; i++) {
        if (leaf_node[i]) {
            avaliable_node.push_back(i + 1);
        }
        finish_node[i] = false;
    }

    uint16_t cycle = 1; // stations used in the task
    uint16_t current_time = 0;
    uint16_t first_node = 0;

    // Input parameter

    uint16_t SPR = 0;
    for (int i = 0; i < kjob.taskNum; i++)
    {
        SPR += kjob.task_list[i].time;
    }
    uint16_t TJ = kjob.taskNum;
    uint16_t RJ = kjob.taskNum;
    uint16_t CJ = 0;
    uint16_t RCT = kjob.cycleTime;

    // Calculate sequence and fitness
    while (avaliable_node.size() > 0) {
        double best_prio = __DBL_MAX__;
        uint16_t chose_node = -1;
        uint16_t last_node = 0;
        vector<double> prio_list;
        vector<uint16_t> candidate_node;

        for (int i = 0; i < avaliable_node.size(); i++) {
            // Input parameter
            uint16_t PT = kjob.task_list[avaliable_node[i] - 1].time;
            uint16_t FS = 0;
            if (first_node)
                FS = kjob.setup_matrix[avaliable_node[i] - 1][first_node];
            uint16_t BS = 0;
            if (last_node)
                BS = kjob.setup_matrix[last_node][avaliable_node[i] - 1];
            
            // set terminate aray
            terminate[0] = PT;
            terminate[1] = FS;
            terminate[2] = BS;
            terminate[3] = SPR;
            terminate[4] = TJ;
            terminate[5] = RJ;
            terminate[6] = CJ;
            terminate[7] = RCT;

            double prio = Priority();
            prio_list.push_back(prio);
            if (prio < best_prio) {
                best_prio = prio;
            }

        }
        for (int i = 0; i < prio_list.size();i++) {
            if (prio_list[i] == best_prio)
                candidate_node.push_back(i);
        }

        // stop random strategy
        //int idx = candidate_node[RAND(0, candidate_node.size())];
        int idx = 0;
        

        chose_node = avaliable_node[candidate_node[idx]] - 1;
        // print chose node
        if (PRINT)cout << "[INFO]: chose node " << (int)chose_node + 1 << endl;
        
        if (chose_node == -1)
            cout << "\033[1;33m[WARNING]: No avaliable node return.\033[0m" << endl;

        uint16_t backward_setup = kjob.setup_matrix[last_node][chose_node];
        uint16_t forward_setup = kjob.setup_matrix[chose_node][first_node];
        // Change condition
        SPR -= kjob.task_list[chose_node].time;
        RJ --;

        if (current_time + backward_setup + forward_setup + kjob.task_list[chose_node].time 
                > cycle * kjob.cycleTime){
            // Need to open a new workstation
            first_node = chose_node;
            if(PRINT) cout << "[INFO]: new first " << (int)chose_node << endl;
            if(PRINT) cout << "[INFO]: cycle to " << (int)(cycle * kjob.cycleTime) << endl;
            current_time = (cycle * kjob.cycleTime) + kjob.task_list[chose_node].time;
            if(PRINT) cout << "[INFO]: current time " << (int)current_time << endl;
            CJ = 1;
            RCT = kjob.cycleTime;
            cycle++;
        }
        else {
            CJ++;
            RCT -= kjob.task_list[chose_node].time;
            if (RCT < 0) 
                cout << "\033[1;33m[WARNING]: RCT error.\033[0m" << endl;
            current_time += backward_setup + forward_setup + kjob.task_list[chose_node].time;
            if(PRINT) cout << "[INFO]: current time " << (int)current_time << endl;
        }

        
        // remove executed node and add next avaliable node
        finish_node[chose_node] = true;
        
        // Add next posible node to the list
        for (int i = 0; i < kjob.task_list[chose_node].or_next.size(); i++){
            uint16_t search_node = kjob.task_list[chose_node].or_next[i];
            if (find(avaliable_node.begin(), avaliable_node.end(), search_node) == avaliable_node.end()
                && !finish_node[search_node - 1]){
                avaliable_node.push_back(search_node);
            }
        }
        for (int i = 0; i < kjob.task_list[chose_node].and_next.size(); i++){
            bool and_done = true;
            uint16_t search_node = kjob.task_list[chose_node].and_next[i];
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

        avaliable_node.erase(avaliable_node.begin() + candidate_node[idx]);  
        sequence.push_back(chose_node + 1);

    }
    if (PRINT)cout << "[INFO]: station[" << (int)cycle << "] ";
    // Output fitness
    if (PRINT)PRINT_SEQUENCE(sequence);

    return cycle;
}


