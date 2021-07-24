#include <iostream>
#include "projectConfig.h"
#include "mymath/mymath.h"

using namespace std;
using namespace mymath;

int main (int argc, char *argv[])
{
    
    //just print version of project version
    cout << "Project Version: " << PROJECT_VERSION << endl;
    cout << "Project Version Major: " << PROJECT_VERSION_MAJOR << endl;
    cout << "Projevt Version Minor: " << PROJECT_VERSION_MINOR << endl;
    cout << "Project Version Patch: " << PROJECT_VERSION_PATCH << endl;


    if (argc < 2) {
        cerr << "Must have at least 2 command line argument." << endl;
        return 1;
    }

    if (argc == 2) {
        try {
        double inputValue = stof(argv[1]);
        double outputValue = mymath::mysqrt(inputValue);
        cout << "the sqare root of " << inputValue << " is " << outputValue << endl;
        }
        catch(const invalid_argument& e){
            cerr << e.what() << endl;
            return 1;
        }
    }
    if (argc == 3 ){
        double num1 = stof(argv[1]);
        double num2 = stof(argv[1]);
        try {
            if (argv[3] == "+") {
                double outputValue = mymath::myadd(num1, num2);
                cout << "the sum of two numbers " << num1 << " and " << num2 << " is " << outputValue << endl;
            }
            else {
                double outputValue = mymath::mysub(num1, num2);
                cout << "the sum of two numbers " << num1 << " and " << num2 << " is " << outputValue << endl;
            }
        }
        catch(const invalid_argument& e){
            cerr << e.what() << endl;
            return 1;
        }  
        
    }
    
    return 0;
}