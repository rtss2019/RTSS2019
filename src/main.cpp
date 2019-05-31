//
//  main.cpp
//  finite_case
//
//  Created by *** on 02/05/2017.
//  Copyright © 2017 ***. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <map>
#include <random>
#include <fstream>
#include <sstream>
#include "parser.hpp"
#include "backward_resolution.hpp"
#include "output_R.hpp"
#include "print_functions.hpp"
#include "WCET_Lorch_MDP.hpp"

int main(int argc, const char * argv[]) {
        int max_deadline=0;
        int max_size=0;
        int max_speeds=0;
        int bound_M = 2*max_deadline;
        
        Comparaison_Lorch_MDP(max_size,max_deadline,bound_M,max_speeds);
        return 0;
}

