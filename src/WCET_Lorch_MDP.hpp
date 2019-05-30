//
//  WCET_Lorch_MDP.hpp
//  finite_case
//
//  Created by Stéphan on 05/02/2019.
//  Copyright © 2019 Stéphan. All rights reserved.
//

#ifndef WCET_Lorch_MDP_hpp
#define WCET_Lorch_MDP_hpp

#include <stdio.h>
#include "backward_resolution.hpp"
#include "parser.hpp"

//int state_space_creation(int max_size, int max_deadline, int bound_M, int max_speeds, vector<vector<double> > & state_space);
int create_state_space(int l,int max_size,int max_deadline, int bound_M, int max_speeds,vector<vector<vector<double> > > & state_space);
int search_next_state(int ind_prev_state, int deadline_new_task, int size_new_task, int prev_step_speed, int nb_job_executed, vector<vector<vector<double> > > & state_space);
vector<vector<double> > & probability_matrix(int speed, int & ind_prev_state, vector<vector<double> > & ind_next_state);
int Creation_proba(vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba, vector<double> & size_proba, int max_inter_arrival_time, int max_size, int max_deadline);

int Matrix_proba_computation(int lx, int ly, double & somme_probal1, double & somme_probal2,int ind_state_prev, int ind_state_suiv, int speed, vector<vector<vector<double> > > & state_space, vector<double> & inter_arrival_time_proba, vector<double> & deadline_proba,vector<double> & size_proba, vector<double> & Speeds_min, vector<double> & Nrj_min, int typeloi_uniforme);

int Comparaison_Lorch_MDP(int max_size,int max_deadline, int bound_M, int max_speeds);
double probability_computation_Arrival_Jobs(int lx, int TailleMaxBuffer, int nb_job_exec,vector<vector<int> > & state_prev, vector<vector<int> > & Nouvel_Arrive, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, int tailleShiftEtatPrec);

int Value_iteration(int max_deadline,int TailleMaxBuffer, vector<vector<vector<int> > > & state_space, vector<int> & Vector_State_inter_arrival_time, int max_size, int max_speeds, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, vector<double> & Speeds_min, vector<double> & Nrj_min, int tailleListemin,int typeloi_uniforme);

#endif /* WCET_Lorch_MDP_hpp */
