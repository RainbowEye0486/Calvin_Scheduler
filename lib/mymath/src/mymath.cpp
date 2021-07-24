/*
Define some simple arithmetic logic without dealing with bounded condition .
There are three function which calculate add, minus and sqare root.
*/


# include "mymath.h"
# include <iostream>
# include <stdexcept>

using namespace std;
namespace mymath
{

    double mysqrt(double number){
        static constexpr double precision = 1e-6;
        static auto abs = [](double n) ->double{ return n > 0? n:-n;};

        if(number < 0){
            throw invalid_argument("Cannot calculate the sqare root of a negative number!");
        }
        double guess = number;
        while (abs(guess * guess - number) > precision) {
            guess = (guess + number / guess) / 2;
        } 
        return guess;
    }
        double myadd(double num1, double num2){
        double sum;
        sum = num1 + num2;
        return sum;
    }

    double mysub(double num1, double num2){
        double remain;
        remain = num1 - num2;
        return remain;
    }


}
