# include <queue>
# include <stack>
# include "Scheduler.h"
# include <assert.h>

# define Init_deep  2
# define Max_deep   6
# define ELITE_SIZE    2 // POP_SIZE * 10%
# define POP_SIZE    20
# define CR         0.6 // Crossover rate
# define MR         0.1 // Mutation rate

typedef struct node {
    int Operand;
    int depth;
    struct node *lnode;
    struct node *rnode;
} Node; 



/**
 * Nonterminate node: 
 * ADD: a + b
 * SUB: a - b
 * MUL: a * b
 * DIV: protect a / b
 * 
 * Terminate node: determined by some parameters of whole 
 * scheduling plan
 * 
 * PT: processing time
 * FS: forward setup time
 * BS: backward setup time
 * SPR: some of all remain processing time
 * TJ: total job number
 * RJ: remaining job number
 * CJ: job in this cycle
 * RCT: remain cycle time
*/
enum Operand {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    PT = 5,
    FS = 6,
    BS = 7,
    SPR = 8,
    TJ = 9,
    RJ = 10,
    CJ = 11,
    RCT = 12
};


class GP: private Scheduler{
    public:
        static GP *GetGP();
        ~GP();
        void PreProcessing(KJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();

    private:
        static GP *inst_;
        vector<uint16_t> stack_; // store postorder sequence
        uint16_t terminate[8];

        GP(); 
        Node *Node_init(int);
        Node *Tree_init(int, int, int);
        void AssertNode(Node *);
        void Postorder(Node *);
        double Priority();
        uint16_t GP_Fitness(Node *);
        void CrossOver(Node *, Node *);
        void Mutation(Node *);
        vector<double> Rescale(vector<uint16_t>);
        void FreeTree(Node *);
        Node *CloneTree(Node *);
        bool Validate_Depth(Node *, int);
        int Tree_Height(Node *);
        void Resize_deep(Node *, Node *, Node *);
};      