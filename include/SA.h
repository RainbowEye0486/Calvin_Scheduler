
# include "Scheduler.h"


# define T_init      3
# define T_min       0.009
# define Alpha       0.95

class SA: private Scheduler{
    public:
        static SA *GetSA();
        SA();
        ~SA();
        void PreProcessing(KJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();

    private:
        static SA *inst_;
        vector<uint16_t> Neighbor(vector<uint16_t>);
        uint16_t Shift(vector<uint16_t>);
        
};