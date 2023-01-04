# include <random>
# include "Scheduler.h"

# define MR         0.2 // mutation rate
# define CR         0.8  // crossover rate
# define POP_SIZE   100


class GA: private Scheduler{
    public:
        static GA *GetGA();
        GA();
        ~GA();
        void PreProcessing(KJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();



    private:
        static GA *inst_;
        void PRINT_POP(vector<vector<uint16_t>>);
        vector<uint16_t> Generate_mask();
};