//
//  Simulation_WCET_LorchMDP.hpp
//  finite_case
//
//  Created by *** on 05/03/2019.
//  Copyright © 2019 ***. All rights reserved.
//

#ifndef Simulation_WCET_LorchMDP_hpp
#define Simulation_WCET_LorchMDP_hpp

#include <stdio.h>
#include "WCET_Lorch_MDP.hpp"

vector<int> simulation(int TailleMaxBuffer, vector<double> & Matrice_vitesse, int max_size, int max_deadline, int max_inter_arrival_time, vector<vector<vector<int> > > & state_space, int T, vector<vector<double> > & Sortie_cost_MDP_Lorch, vector<vector<double> > & Stock_MDP_OA, vector<int> & Vector_State_inter_arrival_time,int tailleListemin);

vector<int> simulationVI(vector<double> & size_proba, vector<double> & deadline_proba, vector<double> inter_arrival_time_proba, vector<vector<double> > & typeDeadline, vector<vector<double> > & Monitoring_vector, int max_speeds, int TailleMaxBuffer, vector<double> & Matrice_vitesse, int max_size, int max_deadline, int max_inter_arrival_time, vector<vector<vector<int> > > & state_space, int T, vector<vector<double> > & Stock_MDP_Lorch, vector<vector<double> > & Stock_MDP_OA, vector<int> & Vector_State_inter_arrival_time, int tailleListemin);

int Calcul_Vitesse_OA_WCET(vector<vector<int> > & etat_suivant_OA, int & vitesse_OA, int max_size);
int Calcul_Vitesse_Lorch_WCET(vector<vector<int> > & etat_courant, vector<double> & LorchSpeedParJob, int max_size, vector<double> & size_proba);
#endif /* Simulation_WCET_LorchMDP_hpp */
