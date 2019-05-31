//
//  WCET_Lorch_MDP.cpp
//  finite_case
//
//  Created by *** on 05/02/2019.
//  Copyright © 2019 ***. All rights reserved.
//

//#include "backward_resolution.hpp"
#include "parser.hpp"
#include "WCET_Lorch_MDP.hpp"
#include "Simulation_WCET_LorchMDP.hpp"
#include "print_functions.hpp"
#include "output_R.hpp"
#include <algorithm>
#include <iterator>
#include <list>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <fstream>
#include <iomanip>

using namespace std;
bool affiche = false;

int factorielle(int n) {
        return (n == 1 || n == 0) ? 1 : factorielle(n - 1) * n;
}

int Compute_borne_inf(int bornesup, int l, int debutAnalyse, vector<vector<int> > & state_prev, int max_size, int nbJobASupprimer);

bool NbDeadline(vector<double> & nb_deadline, int max_speeds) {
        bool sortie = true;
        for (int i=0;i<nb_deadline.size(); i++) {
                if (nb_deadline[i]>max_speeds) {
                        sortie=false;
                        break;
                }
        }
        return sortie;
}

int create_state_space(int l,int max_size,int max_deadline, int bound_M, int max_speeds,vector<vector<vector<int> > > & state_space) {
        int size_wm1=0, size_w=(int)state_space.size();
        vector<vector<int> > int_state;
        int sum_exc_work;
        vector<int> new_int_state;
        vector<vector<int> > new2_int_state;
        // strict car sinon le job est terminé
        for (int ei=0; ei<max_size; ei++) {
                for (int di=1; di<=max_deadline;di++) {
                        new_int_state.push_back(ei);new_int_state.push_back(di);
                        new2_int_state.push_back(new_int_state);
                        state_space.push_back(new2_int_state);
                        new_int_state.erase(new_int_state.begin(), new_int_state.end());
                        new2_int_state.erase(new2_int_state.begin(), new2_int_state.end());
                }
        }
        l--;
        size_w =(int)(state_space.size());
        while (l>0) {
                for (int i=size_wm1; i<size_w;i++) {
                        int_state=state_space[i];
                        sum_exc_work=0;
                        for (int j=0; j<int_state.size();j++) {
                                sum_exc_work+=int_state[j][0];
                        }
                        for (int di=state_space[i][(int)(state_space[i].size())-1][1]; di <=max_deadline; di++) {
                                // @DONE change max_size en max_size-sum_exc_work
                                for (int ei=0; ei<max_size; ei++) {
                                        int_state=state_space[i];
                                        new_int_state.push_back(ei);new_int_state.push_back(di);
                                        int_state.push_back(new_int_state);
                                        state_space.push_back(int_state);
                                        new_int_state.erase(new_int_state.begin(), new_int_state.end());
                                        int_state.erase(int_state.begin(), int_state.end());
                                }
                        }
                }
                size_wm1=size_w;
                size_w=(int)(state_space.size());
                l--;
        }
        
        int deadline_jobprec,execworkPrec;
        for (int i=0; i<(int)(state_space.size()); i++) {
                deadline_jobprec=state_space[i][0][1];
                execworkPrec=state_space[i][0][0];
                
                for (int j=1; j<state_space[i].size(); j++) {
                        if (deadline_jobprec==state_space[i][j][1]) {
                                if ((state_space[i][j][0]!=0 && execworkPrec==0) || (state_space[i][j][0]!=0 && execworkPrec!=0)) {
                                        state_space.erase(state_space.begin()+i);
                                        i--;
                                        break;
                                }
                        }
                        deadline_jobprec=state_space[i][j][1];
                        execworkPrec=state_space[i][j][0];
                }
        }
        return 0;
}

// Proba considéré pour l'arrivée des taches
double proba_function(int NewExecWork, int ExecWork, int type_loi, int max_size, vector<double> & size_proba) {
        double value_proba = 0;
        // @DONE Cas loi uniforme
        if (type_loi==0) {
                // Loi uniforme, indep de NewExecWork et ExecWork
                // @TODO prendre en compte que la distribution de lois peut être autre que uniforme pour deadline ET pour taille
                if (ExecWork+NewExecWork<=max_size) {
                        value_proba=1/(double)(max_size);
                } else {
                        value_proba=0;
                }
        }
        // @TODO Cas loi discrete
        if (type_loi==1) {
                value_proba=size_proba[NewExecWork+ExecWork-1];
        }
        return value_proba;
}

// return the value of the repartition function in point x
double repartition_function(int x, int type_loi, int max_size, vector<double> & size_proba) {
        double value=0;
        if (type_loi==0) {
                // Loi uniforme U(0,max_size)
                value=(double)x/(double)(max_size);
        }
        // @TODO Cas loi discrete
        if (type_loi==1) {
                for (int i=x; i<max_size; i++) {
                        value+=size_proba[i];
                }
                value=1-value;
        }
        return value;
}

double recursif_somme(int k1, int nbre_jobs, double & res, double  value_gen_rec, int max_size, vector<vector<int> > & state_prev, vector<vector<int> > & state_suiv, int nbre_jobs_tot, double & value_tot, double  value_gen_prec, vector<double> & size_proba,int typeloi_uniforme) {
        // Calcul de proba est fait à état fixé => x,y,s
        int ExecWork;
        if (nbre_jobs>0) {
                int plafond = min(k1,max_size); // plafond ici correspond à la quantité maximal éxécuté <= WCET - e^x_l
                plafond = min(plafond, max_size-state_prev[nbre_jobs_tot-nbre_jobs][0]);
                
                value_gen_prec = value_gen_rec;
                for (int kj=1; kj <= plafond; kj++) {
                        
                        ExecWork = state_prev[nbre_jobs_tot-nbre_jobs][0];
                        value_gen_rec=value_gen_prec*proba_function(kj,ExecWork,typeloi_uniforme,max_size,size_proba)/(1-repartition_function(state_prev[nbre_jobs_tot-nbre_jobs][0], typeloi_uniforme, max_size,size_proba));
                        recursif_somme(k1-kj+1,nbre_jobs-1,res,value_gen_rec, max_size,state_prev,state_suiv,nbre_jobs_tot,value_tot, value_gen_prec,size_proba,typeloi_uniforme);
                        if (value_gen_rec!=0) {
                                value_gen_rec=value_gen_rec*(1-repartition_function(state_prev[nbre_jobs_tot-nbre_jobs][0], typeloi_uniforme, max_size,size_proba))/proba_function(kj,ExecWork,typeloi_uniforme,max_size,size_proba);
                        } else {
                                value_gen_rec=value_gen_prec;
                        }
                }
        } else {
                // value_tot sauvegarde le réultat de l'ensemble des sommes des produits.
                value_tot+=value_gen_rec;
                res++;
                return res;
        }
        return res;
}
double recursif_somme_general(int pos_job, bool & proba_nulle, int k1, int nbre_jobs, double & res, double value_gen_rec, int max_size, vector<vector<int> > & state_prev, vector<vector<int> > & state_suiv, int nbre_jobs_tot, double & value_tot, double value_gen_prec, vector<double> & size_proba, int typeloi_uniforme) {
        // Calcul de proba est fait à état fixé => x,y,s
        int ExecWork;
        if (nbre_jobs>0) {
                int plafond = min(k1,max_size); // plafond ici correspond à la quantité maximal éxécuté <= WCET - e^x_l
                plafond = min(plafond, max_size-state_prev[pos_job][0]);
                
                int test = Compute_borne_inf(k1,nbre_jobs,pos_job,state_prev,max_size,nbre_jobs);
                int borneinf = max(k1,max_size-state_prev[pos_job][0]);
                borneinf = min(test,borneinf);
                borneinf = min(borneinf,plafond);
                borneinf = max(borneinf,1);
                
                value_gen_prec = value_gen_rec;
                for (int kj=borneinf; kj <= plafond; kj++) {
                        ExecWork = state_prev[pos_job][0];
                        value_gen_rec=value_gen_prec*proba_function(kj,ExecWork,typeloi_uniforme,max_size,size_proba)/(1-repartition_function(state_prev[pos_job][0], typeloi_uniforme, max_size,size_proba));
                        recursif_somme_general(pos_job+1,proba_nulle,k1-kj+1,nbre_jobs-1,res,value_gen_rec, max_size,state_prev,state_suiv,nbre_jobs_tot,value_tot, value_gen_prec,size_proba,typeloi_uniforme);
                        if (value_gen_rec!=0) {
                                value_gen_rec=value_gen_rec*(1-repartition_function(state_prev[pos_job][0], typeloi_uniforme, max_size,size_proba))/proba_function(kj,ExecWork,typeloi_uniforme,max_size,size_proba);
                        } else {
                                value_gen_rec=value_gen_prec;
                        }
                }
        } else {
                // value_tot sauvegarde le réultat de l'ensemble des sommes des produits.
                if (k1<=1)
                        value_tot+=value_gen_rec;
                res++;
                return res;
        }
        return res;
}

/*************** Analyse changement d'etat ***************/
// Entrée en in     : x, y, nb_job_exec
// Sortie en in out : calcul_proba, ExecWork_x_nonEnded, ExecWork_y_nonEnded, Nouvel_Arrive
// Return False si:
// 1) Pas de l+1ième job présent
// 2) Le l+1ième job ne respecte pas e_y^l > e_x^{l+1}
// 3) Les jobs différents entre Shift_l(x) et y ont des executed work > 0
//
// Calcul de e_{l+1}^x ET de e_{j}^y (determine j) ET de la forme des jobs arrivées en t+1
// Recuperation de ExecWork(x,l+1) et ExecWork(y,i), où i est le premier job de deadline = d_{l+1} dans la liste de y
/****************                          ***************/
bool Analyse_changement_detat(vector<vector<int> > & state_prev,vector<vector<int> > & state_suiv, int nb_job_exec, int speed, int type_loi, int & ExecWork_x_nonEnded, int & ExecWork_y_nonEnded, vector<vector<int> > & Nouvel_Arrive, int & tailleShiftEtatPrec,vector<vector<int> > & shift_prev) {
        vector<vector<int> > Shift_x; vector<vector<int> >::iterator it;
        int Deadline_x_nonEnded, index_y_nonEnded_x_nonEnded=INFINITY;
        bool calcul_proba=true, noJobInShiftx =false;
        
        if (nb_job_exec == (int)(state_prev.size())) {
                // CAS pas de l+1 ième job car tous les jobs de x sont executés
                index_y_nonEnded_x_nonEnded=0;
                ExecWork_x_nonEnded=0; ExecWork_y_nonEnded=0;
                noJobInShiftx=true;
        } else {
                // l+1ième job EXISTE, Input: nb_job_exec jobs executés => regarde le l+1 ième job en position nb_job_exec
                // => ExecWork_x_nonEnded = e_{l+1}^x
                ExecWork_x_nonEnded = state_prev[nb_job_exec][0];
                Deadline_x_nonEnded = state_prev[nb_job_exec][1];
                // position du l+1ieme job
                it = state_prev.begin()+nb_job_exec;
                
                // Enleve l premiers jobs et Recuperation des autres stocké dans Shift_x
                Shift_x.assign(it, state_prev.end());
                // Ajuste la deadline des jobs restants pour leur enlever 1
                for (int i=0; i<Shift_x.size(); i++) {
                        Shift_x[i][1]-=1;
                }
                Deadline_x_nonEnded--;
                
                tailleShiftEtatPrec = (int)(Shift_x.size());
                
                // Recherche de l'indice dans y du job x_nonEnded = correspond au premier job de y de Deadline_x_nonEnded, dont l'exec work != 0
                if (!(state_suiv[0].empty())) {
                        int position=0;
                        while (state_suiv[position][1] != Deadline_x_nonEnded) {
                                position++;
                                // Prend en compte la fin de lecture du vecteur
                                if (position>=state_suiv.size())
                                        break;
                        }
                        
                        // Cas l+1ième job non trouvé et e^l_y-e^{l+1}_x < 0
                        if (position == (int)(state_suiv.size())) {
                                calcul_proba=false; // job non trouvé dans y
                                if (affiche)
                                        cout << "IMP!" << endl;
                        } else { // job trouvé dans y
                                index_y_nonEnded_x_nonEnded = position;
                                if (state_suiv[position][0]<ExecWork_x_nonEnded) {
                                        calcul_proba=false; // cas ou e_y < e_x
                                        if (affiche)
                                                cout << "IMP!" << endl;
                                } else {
                                        ExecWork_y_nonEnded=state_suiv[position][0];
                                        index_y_nonEnded_x_nonEnded=position;
                                }
                        }
                } else {
                        // CAS ou letat y est un etat vide
                        ExecWork_x_nonEnded=0; ExecWork_y_nonEnded=0;
                        // Aucun job n'est arrivé, donc vérifie que la différence est possible
                        if ((int)(state_suiv[0].size())-(int)(state_prev[0].size()+nb_job_exec)<0) {
                                calcul_proba=false;
                        }
                        
                }
        }
        
        shift_prev=Shift_x;
        
        // Elimination du cas ou le l+1ième job n'est pas présent dans x(ex,dx) et y(ey,dy) et ou ex>ey => ExecWork_y_nonEnded = e_{j}^y ou j = premiere_Deadline(y | deadline(x,l+1) == deadline(y))
        // TO CONLUDE => a = ExecWork_y_nonEnded - ExecWork_x_nonEnded, remplacera dans la borne de la somme le deuxième push_back de calcul min => push_back(max_size - ExecWork_y_nonEnded + ExecWork_x_nonEnded);
        
        // Enleve les cas ou Shift_x n'est pas compris dans state_suiv verifiaction que des deadlines
        if (!(state_suiv[0].empty())) {
                vector<vector<int> > test =state_suiv;
                bool Nechec;
                for (int i=0; i<Shift_x.size();i++) {
                        Nechec=false;
                        for (int j=0; j<test.size();j++) {
                                if (Shift_x[i][1]==test[j][1] && Shift_x[i][0]<=test[j][0]) {
                                        Nechec=true;
                                        test.erase(test.begin()+j);
                                        break;
                                }
                        }
                        if (!Nechec)
                                return false;
                }
        }
        
        // On a index_y_nonEnded_x_nonEnded qui correspond à la position du job l+1 dans y 
        if (calcul_proba) {
                // Détermination du nombre de job différent entre y = state_suiv et Shift_x, qui correspond aux arrivées de jobs
                vector<vector<int> > Remaining_y = state_suiv;
                vector<int> pos_identical_jobs;
                vector<int>::iterator it2;
                
                int evol=1;
                
                // démarre de 1 car le premier job de Shift_x correspond au job l+1 déjà analysé et ne pouvant pas être nouveau
                for (int i=1; i<Shift_x.size(); i++) {
                        for (int j=evol; j<state_suiv.size(); j++) {
                                // j!=index_y_nonEnded_x_nonEnded, car ce job correspond au job particulier l+1
                                if (Shift_x[i] == state_suiv[j] && j!=index_y_nonEnded_x_nonEnded) {
                                        pos_identical_jobs.push_back(j);
                                        evol=j+1;
                                        //                                        Remaining_y.erase(Remaining_y.begin()+j);
                                        //                                        j--;
                                        break;
                                }
                        }
                }
                // rajout du l+1ième job en commun.
                if(!noJobInShiftx) {
                        pos_identical_jobs.push_back(index_y_nonEnded_x_nonEnded);
                }
                
                // Sur les indices non pos_identical_jobs and non index_y_nonEnded_x_nonEnded, vérifier que tous les execWork sont à zéro
                for (int j=0; j<state_suiv.size(); j++) {
                        it2 = find(pos_identical_jobs.begin(),pos_identical_jobs.end(),j);
                        if (it2==pos_identical_jobs.end()) {
                                if (!(state_suiv[0].empty())) {
                                        // On regarde si les Execwork sont nuls
                                        if (state_suiv[j][0]!=0) {
                                                //@TODO Rajouter le cas state_suiv vaut l'ensemble vide
                                                calcul_proba=false;
                                                if (affiche)
                                                        cout << "IMP!" << endl;
                                                break;
                                        } else {
                                                Nouvel_Arrive.push_back(state_suiv[j]);
                                        }
                                }
                        }
                }
        }
        
        // @DONE max des \ell
        if (ExecWork_y_nonEnded-ExecWork_x_nonEnded+nb_job_exec>speed)
                return false;
        
        // CAS plus de jobs après qu'avant avec une vitesse trop petite
        int nbre_job_avt=(int)(state_prev.size());
        int nbre_job_apr=(int)(state_suiv.size());
        int nbreJobArrive=(int)(Nouvel_Arrive.size());
        if (nbre_job_apr-nbre_job_avt+nb_job_exec+nbreJobArrive<0) {
                calcul_proba=false;
        }
        return calcul_proba;
}

// @DONE IMPLEMENTATION DU CAS Jobs iid ET deadline INDEP Arrival Time
// Step 1: compter le nombre de job
// Step 2: determiner les jobs qui ont le même deadline
// Step 3: lister les deadlines différentes à prendre en compte
// Step 4: prendre en compte l'inter-arrival time
double probability_computation_Arrival_Jobs(int lx, int TailleMaxBuffer, int nb_job_exec,vector<vector<int> > & state_prev, vector<vector<int> > & Nouvel_Arrive, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, int tailleShiftEtatPrec) {
        // Chaque case du vecteur correpsond au nombre de jobs de deadline indice de la case
        vector<int> indice_nombre_deadline((int)(deadline_proba.size()),0);
        double proba_arrival=1;
        
        int tailleStatePrev=(int)(state_prev.size());
        if (!(state_prev.empty()))
                if (state_prev[0].empty())
                        tailleStatePrev=0;
        
        int maximal_jobs_number = TailleMaxBuffer - tailleStatePrev;
        
        // Compte la somme des probas inter-arrival time de lx à L
        double sum_compteur=0;
        for (int i=lx; i<inter_arrival_time_proba.size(); i++) {
                sum_compteur+=inter_arrival_time_proba[i];
        }
        
        if (!Nouvel_Arrive.empty() && !Nouvel_Arrive[0].empty() ) {
                for (int i=0; i<Nouvel_Arrive.size(); i++) {
                        // Calcul du nombre de jobs de même deadline l'indice du tableau correspond à la deadline et la case le nombre de jobs de même deadline
                        indice_nombre_deadline[(int)(Nouvel_Arrive[i][1]-1)]++;
                        if (i==0) {
                                proba_arrival=inter_arrival_time_proba[lx]/sum_compteur;
                        } else {
                                proba_arrival*=inter_arrival_time_proba[0];
                        }
                        // Proba d'arrivée d'un job de deadline spécifique
                        // Le -1 est du au fait que la deadline est strictement superieur à 1
                        proba_arrival*=deadline_proba[Nouvel_Arrive[i][1]-1];
                }
                
                // 1er cas   : trop de job dans le buffer
                if (((int)(Nouvel_Arrive.size()) < maximal_jobs_number) ) {
                        proba_arrival*=(1-inter_arrival_time_proba[0]);
                }
                
                // Calcul de la fraction de factorielles
                for (int i=0; i<indice_nombre_deadline.size(); i++) {
                        proba_arrival/=factorielle(indice_nombre_deadline[i]);
                }
                proba_arrival*=factorielle((int)(Nouvel_Arrive.size()));
        } else {
                // CAS spécifique où a(t) est l'ensemble vide
                // 2ieme cas : inter-arrival time trop grand si aucun job arrivé, i.e. lx = maxInterarrivaltime
                if (lx!=(int)(inter_arrival_time_proba.size())-1) {
                        proba_arrival*=1-inter_arrival_time_proba[lx]/sum_compteur;
                }
        }
        return proba_arrival;
}

// BorneInf pour la somme du calcul de P1, probabilité des jobs executés
int Compute_borne_inf(int bornesup, int l, int debutAnalyse, vector<vector<int> > & state_prev, int max_size, int nbJobASupprimer) {
        int sommeJobsSuivantPireCas=0;
        // debutAnalyse=nbJobASupprimer-l+1;
        for (int i=debutAnalyse; i<l; i++) {
                sommeJobsSuivantPireCas+=max_size-state_prev[i][0]-1; // le -1 est du au fait que dans le calcul de bornesup on a déjà pris en compte la presence du job pour une unité
        }
        int borneinf = bornesup-sommeJobsSuivantPireCas;
        return borneinf;
}
// Calcul recursif de proba p(x,s,y)
double Matrix_proba_computation(int lx, int ly, int TailleMaxBuffer, double & somme_probal1, double & somme_probal2, bool & calcul_proba_int, int ind_state_prev, int ind_state_suiv, int speed, vector<vector<vector<int> > > & state_space, int max_size, int max_speeds, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, int typeloi_uniforme) {
        
        vector<vector<int> > state_prev = state_space[ind_state_prev], state_suiv = state_space[ind_state_suiv];
        
        // Determine le nombre de job minimal a executer (l) ===> min_nbre_exec_job
        int somme=0, min_nbre_exec_job=0;
        
        // @DONE cas particulier state_prev = ensemble vie
        if ((state_prev[0].empty())) {
                bool exist=true;
                if (!(state_suiv[0].empty())) {
                        for (int i=0; i<state_suiv.size(); i++) {
                                if (state_suiv[i][0]!=0) {
                                        exist=false; break;
                                }
                        }
                } else {
                        exist=true;
                }
                if (exist) {
                        double P3_vide = probability_computation_Arrival_Jobs(lx, TailleMaxBuffer, 1,state_prev, state_suiv, inter_arrival_time_proba, deadline_proba, size_proba,0);
                        // @DONE Verification que les inter-arrival time sont bons
                        if (state_suiv[0].empty() && ly!=lx+1) {
                                P3_vide=0;
                        }
                        if (!(state_suiv[0].empty()) && ly!=1) {
                                P3_vide=0;
                        }
                        return P3_vide;
                } else {
                        return 0;
                }
        }
        
        // Egalité ci-dessous non respecté => 0 job executé => min_nbre_exec_job == 0
        if (speed >= max_size-state_prev[0][0]) {
                for (int x=0; x<state_prev.size();x++) {
                        somme += max_size-state_prev[x][0];
                        if (speed>=somme) {
                                min_nbre_exec_job=x+1;
                                break;
                        }
                }
        }
        
        int nb_jobAeliminer=0;
        for (int x=0; x<state_prev.size(); x++) {
                if (state_prev[x][1]==1)
                        nb_jobAeliminer+=1;
        }
        min_nbre_exec_job=max(min_nbre_exec_job,nb_jobAeliminer);
        
        // Pour calculer p(x,s,y) on utilise: ExecWork_y_nonEnded, ExecWork_x_nonEnded, a(t) = Nouvel_Arrive = liste de job arrivée en t
        // ExecWork = Quantite de travail executé pour un job = ExecWork_y_nonEnded - ExecWork_x_nonEnded
        vector<vector<int> > Nouvel_Arrive;
        int ExecWork, ExecWork_x_nonEnded,ExecWork_y_nonEnded;
        
        // SORTIE: Quantité de travail du l+1 ième job en x et en y => ExecWork_x_nonEnded, ExecWork_y_nonEnded et aussi les nouveaux jobs arrivées => Nouvel_Arrive
        bool calcul_proba = false;
        
        // compteur permet de savoir dans quelle somme on se situe et donc dans quel job de l'état (au début on est dans le job [0])
        int nbre_jobs_tot=(int)state_prev.size(), bornesup=0;
        
        double proba_gen=0, value_gen_rec=1, res=0;
        bool passage_calcul_proba = false;
        // P1 = proba de \prod[p(C=k_i)]
        // P2 = proba de lajout de travail executé sur le l+1ième job
        // P3 = proba de l'arrivee des nouveaux jobs en t
        double P1=0,P2=0,P3=0;
        // value_tot = P1 * P2 * P3
        double value_tot=0;
        int tailleShiftEtatPrec=0;
        
        // Parcours l'ensemble du nombre de jobs executé par une vitesse speed (sachant qu'un job est d'au moins une unité)
        vector<int> list_borne_exec_job; list_borne_exec_job.push_back(speed); list_borne_exec_job.push_back((int)(state_prev.size()));
        int borne_exec_job = *min_element(list_borne_exec_job.begin(),list_borne_exec_job.end());
        
        proba_gen=0;
        bool premierPassage=true;
        bool proba_nulle=false;
        int nbre_jobASupprimer=0;
        vector<vector<int> > shiftprev;
        
        for (int l=min_nbre_exec_job; l<=borne_exec_job; l++) {
                nbre_jobASupprimer=l;
                value_gen_rec=1; P1=0;
                // A l fixé, vérification dexistence de (x,s,y) Et si oui calcul de ExecWork_x_nonEnded, ExecWork_y_nonEnded and Nouvel_Arrive
                Nouvel_Arrive.erase(Nouvel_Arrive.begin(),Nouvel_Arrive.end());
                calcul_proba = Analyse_changement_detat(state_prev,state_suiv,l,speed,typeloi_uniforme,ExecWork_x_nonEnded,ExecWork_y_nonEnded,Nouvel_Arrive,tailleShiftEtatPrec, shiftprev);
                
                // @DONE Prendre en compte le cas ou la vitesse est assez grande pour executer l'execwork de fdifférence
                if (ExecWork_y_nonEnded-ExecWork_x_nonEnded > speed)
                        calcul_proba=false;
                
                if (l==0 && ExecWork_y_nonEnded-ExecWork_x_nonEnded != speed)
                        calcul_proba=false;
                
                if (calcul_proba) {
                        bornesup = speed - ExecWork_y_nonEnded + ExecWork_x_nonEnded-(l-1);
                        if (l>0) {
                                if (l!=(int)(state_prev.size())) {
                                        int pos_job = 0;
                                        int plafond = min(bornesup, max_size-state_prev[pos_job][0]);
                                        
                                        int debutAnalyse=1;
                                        int test = Compute_borne_inf(bornesup,l,debutAnalyse,state_prev,max_size, nbre_jobASupprimer);
                                        int borneinf = max(bornesup,max_size-state_prev[pos_job][0]);
                                        borneinf = min(test,borneinf);
                                        borneinf = min(borneinf,plafond);
                                        borneinf = max(borneinf,1);
                                        
                                        for (int k1=borneinf; k1<=plafond; k1++) { // bornesup avant
                                                if (affiche)
                                                        cout << "k1 = " << k1 << endl;
                                                value_gen_rec=1;
                                                ExecWork=state_prev[pos_job][0];
                                                // value_gen_rec permet de cumuler le produit des probas (1 valeur des sommes)
                                                value_gen_rec *= proba_function(k1, ExecWork, typeloi_uniforme, max_size, size_proba)/(1-repartition_function(state_prev[pos_job][0], typeloi_uniforme, max_size,size_proba));
                                                
                                                recursif_somme_general(pos_job+1,proba_nulle, bornesup+1-k1,l-1,res,value_gen_rec,max_size,state_prev,state_suiv,nbre_jobs_tot,P1, value_gen_rec,size_proba,typeloi_uniforme);
                                                if (value_gen_rec!=0) {
                                                        value_gen_rec=value_gen_rec*(1-repartition_function(state_prev[pos_job][0], typeloi_uniforme, max_size,size_proba))/proba_function(k1,ExecWork,typeloi_uniforme,max_size,size_proba);
                                                } else {
                                                        value_gen_rec=0;
                                                }
                                        }
                                        if (proba_nulle)
                                                P1=0;
                                } else {
                                        int plafond = min(bornesup,max_size); // plafond ici correspond à la quantité maximal éxécuté <= WCET - e^x_l
                                        plafond = min(plafond, max_size-state_prev[nbre_jobs_tot-l][0]);
                                        
                                        for (int k1=1; k1<=plafond; k1++) { // bornesup avant
                                                value_gen_rec=1;
                                                ExecWork=state_prev[nbre_jobs_tot-l][0];
                                                // value_gen_rec permet de cumuler le produit des probas (1 valeur des sommes)
                                                value_gen_rec *= proba_function(k1, ExecWork, typeloi_uniforme, max_size,size_proba)/(1-repartition_function(state_prev[nbre_jobs_tot-l][0], typeloi_uniforme, max_size,size_proba));
                                                recursif_somme(bornesup+1-k1,l-1,res,value_gen_rec,max_size,state_prev,state_suiv,nbre_jobs_tot,P1, value_gen_rec,size_proba,typeloi_uniforme);
                                                if (value_gen_rec!=0) {
                                                        value_gen_rec=value_gen_rec*(1-repartition_function(state_prev[nbre_jobs_tot-l][0], typeloi_uniforme, max_size,size_proba))/proba_function(k1,ExecWork,typeloi_uniforme,max_size,size_proba);
                                                } else {
                                                        value_gen_rec=0;
                                                }
                                        }
                                }
                        } else {
                                P1=1;
                        }
                        value_tot = P1;
                        // Calcul P_2 = proba arrivée des jobs
                        P2 = (1-repartition_function(ExecWork_y_nonEnded,typeloi_uniforme,max_size,size_proba))/(1-repartition_function(ExecWork_x_nonEnded,typeloi_uniforme,max_size,size_proba));
                        value_tot *= P2;
                        // Calcul P3 = proba des jobs arrivée ===> fonction probability_computation_Arrival_Jobs() [arrivée de chaque job suit la même loi]
                        P3 = probability_computation_Arrival_Jobs(lx,TailleMaxBuffer,l,shiftprev, Nouvel_Arrive, inter_arrival_time_proba, deadline_proba, size_proba,tailleShiftEtatPrec);
                        if ((int)shiftprev.size()==TailleMaxBuffer) { // cas n==0 pour l'arrivée des jobs avec TailleMaxBuffer =3
                                P3=1;
                        }
                        // @DONE Verification que les inter-arrival time sont bons
                        if (!(Nouvel_Arrive.empty())) {
                                if (Nouvel_Arrive[0].empty() && ly!=lx+1) {
                                        P3=0;
                                }
                        }
                        if (Nouvel_Arrive.empty() && ly!=lx+1)
                                P3=0;
                        if (!(Nouvel_Arrive.empty())) {
                                if (!(Nouvel_Arrive[0].empty()) && ly!=1) {
                                        P3=0;
                                }
                        }
                        if (!(Nouvel_Arrive.empty()) && ly!=1)
                                P3=0;
                        
                        // @DONE Cas \ell == L && tailleMaxBuffer == nb_jobs(x après application de la vitesse sans arrivée de jobs)
                        // @TODO changer lhypo2
                        if ( lx==(int)(inter_arrival_time_proba.size())-1 && TailleMaxBuffer==(int)(shiftprev.size())) {
                                if (ly==1) {
                                        P3=1;
                                }
                        }
                        value_tot *= P3;
                        passage_calcul_proba=true;
                        // Somme l'ensemble des probas pour chaque couple (x,l,y)
                        proba_gen+=value_tot;
                }
        }
        calcul_proba_int = calcul_proba;
        return proba_gen;
}

//@DONE Dans cette fonction, on détermine la vitesse minimale possible pour x:
// 1) On doit avoir executer l'ensemble des deadlines 1 de x, speed > \sum (C-e_i)
// 2) vitesse_maximale = max_speeds fixé par le processeur.
// 1) => 3) dans @TODO Matrix_Proba_computation: On doit supprimer au moins les jobs de deadline 1, \ie l > nb_job(d_i=1).
int Vitesses_possible(vector<double> & ens_speeds, int x, vector<vector<vector<int> > > & state_space, int max_speeds, int max_size) {
        vector<vector<int> > state_x = state_space[x];
        int somme=0;
        if (!(state_x[0].empty())) {
                for (int i=0; i<state_x.size();i++) {
                        if (state_x[i][1]==1) {
                                somme+=max_size-state_x[i][0];
                        }
                }
        } else {
                somme=0;
        }
        for (int i=somme; i<=max_speeds; i++) {
                ens_speeds.push_back(i);
        }
        // @TODO prend en compte la max_speeds
        if (somme>max_speeds)
                ens_speeds.push_back(max_speeds);
        return 0;
}

int Backward_resolution(int max_deadline,int TailleMaxBuffer, int Duration, vector<vector<vector<int> > > & state_space, vector<int> & Vector_State_inter_arrival_time, int max_size, int max_speeds, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, vector<double> & Speeds_min, vector<double> & Nrj_min, int tailleListemin, int typeloi_uniforme) {
        vector<double> Nrj_prec((int)state_space.size(),0),Nrj_suiv((int)state_space.size(),0),Speeds_suiv((int)state_space.size(),0);
        
        vector<double> ens_speeds, ens_possible_nrj;
        double energie_prov=0;
        vector<double>::iterator it;
        double proba;
        bool calcul_proba;
        
        vector<double> stockage_prov, Proba_res,state_res;
        
        /*** PARAMETRE ***/
        bool affiche2=true;
        
        for (int t=Duration-1;t>=0; t--) {
                cout << "t===== " << t << endl;
                for (int x=0; x<state_space.size(); x++) {
                        // lx inter_arrival time
                        int lx=Vector_State_inter_arrival_time[x];
                        ens_speeds.erase(ens_speeds.begin(), ens_speeds.end());
                        ens_possible_nrj.erase(ens_possible_nrj.begin(), ens_possible_nrj.end());
                        // @DONE Vitesses_possible() renvoie l'ensemble des vitesses possibles
                        Vitesses_possible(ens_speeds,x,state_space,max_speeds, max_size);
                        // min sur l'ensemble des vitesses possibles
                        for (int s=0; s<ens_speeds.size();s++) {
                                energie_prov=cout_nrj(ens_speeds[s],1000); // max_speeds à la place de 1000
                                // Somme sur tous les y (on pourrait eventuellement sélectionner les plus pertinents)
                                double sum_proba=0, somme_probal1=0, somme_probal2=0;
                                Proba_res.erase(Proba_res.begin(), Proba_res.end());
                                state_res.erase(state_res.begin(), state_res.end());
                                for (int y=0; y<state_space.size(); y++) {
                                        // lx et ly sont les inter-arrival time de x et de y pour letat considéré
                                        int ly = Vector_State_inter_arrival_time[y];
                                        proba = Matrix_proba_computation(lx,ly,TailleMaxBuffer,somme_probal1, somme_probal2,calcul_proba,x,y,ens_speeds[s],state_space, max_size, max_speeds,inter_arrival_time_proba,deadline_proba,size_proba,typeloi_uniforme);
                                        if (proba!=0) {
                                                Proba_res.push_back(proba);
                                                state_res.push_back(y);
                                        }
                                        sum_proba+=proba;
                                        energie_prov+=Nrj_prec[y]*proba;
                                        if (Nrj_prec[y]*proba!=0) {
                                                stockage_prov.push_back(Nrj_prec[y]*proba);
                                        }
                                }
                                if (round(sum_proba*100000)/100000<1 || round(sum_proba*100000)/100000>1) {
                                        cout << "OUT OF PROBAS > 1 ou < 1" << endl;
                                }
                                ens_possible_nrj.push_back(energie_prov);
                                stockage_prov.erase(stockage_prov.begin(), stockage_prov.end());
                        }
                        // min des energies pour l'esnemble des vitesses possibles
                        it = min_element(ens_possible_nrj.begin(), ens_possible_nrj.end());
                        Nrj_suiv[x]=*it;
                        Speeds_suiv[x]=ens_speeds[distance(ens_possible_nrj.begin(),it)];
                        Speeds_min[t*(int)state_space.size()+x]=ens_speeds[distance(ens_possible_nrj.begin(),it)];
                        Nrj_min[t*(int)state_space.size()+x] = Nrj_suiv[x];
                }
                cout << "Energie" << endl;
                cout << Nrj_suiv << endl;
                cout << "Vitesse associé" << endl;
                cout << Speeds_suiv << endl;
                Nrj_prec = Nrj_suiv;
        }
        cout << "FIN BACKARD ITERATION" << endl;
        return 0;
}

int Value_iteration(int max_deadline,int TailleMaxBuffer, vector<vector<vector<int> > > & state_space, vector<int> & Vector_State_inter_arrival_time, int max_size, int max_speeds, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba, vector<double> & Speeds_min, vector<double> & Nrj_min, int tailleListemin, int typeloi_uniforme) {
        vector<double> Nrj_prec((int)state_space.size(),0),Nrj_suiv((int)state_space.size(),0),Speeds_suiv((int)state_space.size(),0),Speeds_prec((int)state_space.size(),0);
        
        vector<double> ens_speeds, ens_possible_nrj;
        double energie_prov=0;
        vector<double>::iterator it;
        double proba;
        bool calcul_proba;
        int iter = 0;
        int EgaliterVitesses=0;
        
        vector<double> stockage_prov,Proba_res,state_res;
        
        /*** PARAMETRE ***/
        double epsilon = 0.1;
        bool afficheVI=false;
        int lx,ly;
        double sum_proba, somme_probal1, somme_probal2;
        //        && span(Speeds_suiv-Speeds_prec)<epsilon
        while ((iter==0 || !(span(Nrj_suiv-Nrj_prec)<epsilon)) && iter<100 ) {
                //                cout << "iter===== " << iter << endl;
                iter++;
                Nrj_prec=Nrj_suiv;
                Speeds_prec=Speeds_suiv;
                for (int x=0; x<state_space.size(); x++) {
                        // lx inter_arrival time
                        lx=Vector_State_inter_arrival_time[x];
                        ens_speeds.erase(ens_speeds.begin(), ens_speeds.end());
                        ens_possible_nrj.erase(ens_possible_nrj.begin(), ens_possible_nrj.end());
                        // @DONE Vitesses_possible() renvoie l'ensemble des vitesses possibles
                        Vitesses_possible(ens_speeds,x,state_space,max_speeds, max_size);
                        // @DONE min sur l'ensemble des vitesses possibles
                        for (int s=0; s<ens_speeds.size();s++) {
                                energie_prov=cout_nrj(ens_speeds[s],1000); // max_speeds à la place de 1000
                                // Somme sur tous les y (on pourrait eventuellement sélectionner les plus pertinents)
                                sum_proba=0; somme_probal1=0; somme_probal2=0;
                                Proba_res.erase(Proba_res.begin(), Proba_res.end());
                                state_res.erase(state_res.begin(), state_res.end());
                                
                                for (int y=0; y<state_space.size(); y++) {
                                        ly = Vector_State_inter_arrival_time[y];
                                        proba = Matrix_proba_computation(lx,ly,TailleMaxBuffer,somme_probal1, somme_probal2,calcul_proba,x,y,ens_speeds[s],state_space, max_size, max_speeds,inter_arrival_time_proba,deadline_proba,size_proba,typeloi_uniforme);
                                        if (proba!=0) {
                                                Proba_res.push_back(proba);
                                                state_res.push_back(y);
                                        }
                                        sum_proba+=proba;
                                        // @TODO ATTEBNTION rjaout de 0.9 pour lambda
                                        energie_prov+=0.9*Nrj_prec[y]*proba;
                                        if (Nrj_prec[y]*proba!=0) {
                                                stockage_prov.push_back(Nrj_prec[y]*proba);
                                        }
                                }
                                // @TODO TOADD
                                //                                if (round(sum_proba*100000)/100000<1 || round(sum_proba*100000)/100000>1) { // assert que la somme des probas vaut bien 1
                                //                                        cout << "OUT OF PROBA > 1 ou < 1 " << endl;
                                //                                }
                                ens_possible_nrj.push_back(energie_prov);
                                stockage_prov.erase(stockage_prov.begin(), stockage_prov.end());
                        }
                        // min des energies pour l'esnemble des vitesses possibles
                        it = min_element(ens_possible_nrj.begin(), ens_possible_nrj.end());
                        Nrj_suiv[x]=*it;
                        Speeds_suiv[x]=ens_speeds[distance(ens_possible_nrj.begin(),it)];
                }
        }
        Nrj_min = Nrj_suiv;
        Speeds_min = Speeds_suiv;
        cout << "FIN VALUE ITERATION" << endl;
        return 0;
}


/*** TODO EN COURS TEST ***/
// Calcul Proba Taille vecteur rassemblant tous les cas
int CalculProbaTaille (double & somme, vector<double> & vector_proba_courant, vector<vector<double> > & ens_possible_proba_size, int & pos, int & compt, int discretisation_pas) {
        if (somme<1 && compt<3) {
                for (int ci=0; ci<=discretisation_pas-somme*discretisation_pas; ci++) {
                        
                        compt++;
                        somme+=(double)ci/discretisation_pas;
                        vector_proba_courant.push_back((double)ci/discretisation_pas);
                        CalculProbaTaille(somme, vector_proba_courant, ens_possible_proba_size, pos, compt,discretisation_pas);
                        vector_proba_courant.erase(vector_proba_courant.end()-1);
                        somme-=(double)ci/discretisation_pas;
                        compt--;
                }
        } else {
                vector_proba_courant.push_back(1-somme);
                int suppr=0;
                if ((int)vector_proba_courant.size()<4) {
                        while ((int)vector_proba_courant.size()!=4){
                                vector_proba_courant.push_back(0);
                                suppr++;
                        }
                }
                // si la valeur de c = WCET a une proba > 0 alors on l'enregistre
                // si p(WCET)=0 alors ce n'est plus le WCET
                if (vector_proba_courant[vector_proba_courant.size()-1]>0)
                        ens_possible_proba_size.push_back(vector_proba_courant);
                
                if (suppr>0) {
                        while (suppr>0){
                                vector_proba_courant.erase(vector_proba_courant.end()-1);
                                suppr--;
                        }
                }
                vector_proba_courant.erase(vector_proba_courant.end()-1);
        }
        return 0;
}


// Proba considéré pour l'arrivée des taches
double calcul_probabilite(int value,int max_value,string type_loi) {
        double value_proba = 0;
        if (type_loi.compare("uniforme")==0) {
                // Loi uniforme, indep de NewExecWork et ExecWork
                value_proba=1/(double)max_value;
        }
        if (type_loi.compare("max")==0) {
                if (value==max_value) {
                        value_proba=1;
                } else {
                        value_proba=0;
                }
        }
        return value_proba;
}


// Fonction de creation du vecteur de proba de taille, deadline et inter-arrival-time
// @DONE cas uniforme
// @TODO Rajouter d'autre fonction de répartition

/****************** VALEUR PRISE EN ENTREE 1 *********************/
int Creation_proba(vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba, vector<double> & size_proba, int max_inter_arrival_time, int max_size, int max_deadline) {
        for (int i=0; i<= max_inter_arrival_time; i++) {
                inter_arrival_time_proba.push_back(calcul_probabilite(i,max_inter_arrival_time+1,"uniforme"));
        }
        for (int d=1; d<= max_deadline; d++) {
                //                deadline_proba.push_back(calcul_probabilite(d,max_deadline,"uniforme"));
                deadline_proba.push_back(calcul_probabilite(d,max_deadline,"max"));
        }
        for (int c=1; c<= max_size; c++) {
                size_proba.push_back(calcul_probabilite(c,max_size,"uniforme"));
        }
        
        //@DONE Test Changement pour la proba de la taille
        size_proba[0]=(double)1/4;
        size_proba[1]=(double)1/4;
        size_proba[2]=(double)1/4;
        size_proba[3]=(double)1/4;
        
        //        // @DONE test changement inter-arrival-time
        inter_arrival_time_proba[0]=(double)0;
        inter_arrival_time_proba[1]=(double)1;
        //        inter_arrival_time_proba[2]=(double)0;
        //        inter_arrival_time_proba[3]=(double)1;
        
        //        // CAS WCET=4
        //        size_proba[0]=(double)0.325674326;
        //        size_proba[1]=(double)0.645354645;
        //        size_proba[2]=(double)0.025974026;
        //        size_proba[3]=(double)0.002997003;
        
        //        // CAS WCET=6
        //        size_proba[0]=(double)0.000000000;
        //        size_proba[1]=(double)0.830169830;
        //        size_proba[2]=(double)0.150849151;
        //        size_proba[3]=(double)0.012987013;
        //        size_proba[4]=(double)0.003996004;
        //        size_proba[5]=(double)0.001998002;
        
        //        // CAS WCET=10
        //        size_proba[0]=(double)0.000000000;
        //        size_proba[1]=(double)0.000000000;
        //        size_proba[2]=(double)0.757242757;
        //        size_proba[3]=(double)0.165834166;
        //        size_proba[4]=(double)0.057942058;
        //        size_proba[5]=(double)0.012987013;
        //        size_proba[6]=(double)0.000000000;
        //        size_proba[7]=(double)0.003996004;
        //        size_proba[8]=(double)0.000999001;
        //        size_proba[9]=(double)0.000999001;
        
        return 0;
}

int Comparaison_Lorch_MDP(int max_size,int max_deadline, int bound_M, int max_speeds) {
        vector<vector<vector<int> > > state_space;
        
        /*** PARAMETRES ***/
        int max_buffer= 3;                          // nombre de job maximal
        // @TODO Attention Modifier L en fonction de inter_arrival_time_proba
        int max_inter_arrival_time = 1;         // Nombre d'inter-arrival-time maximal
        max_size=4;                             // Taille max du job
        max_speeds=max_buffer*max_size;         // Vitesse Maximale
        max_deadline=3;                         // Deadline maximale
        // @TODO Attention Modifier typeloi_uniforme en fonction de size_proba
        int typeloi_uniforme = 1;               // Choix de la loi pour la taille 1=> loi discrete
        
        create_state_space(max_buffer,max_size,max_deadline,bound_M,max_speeds,state_space);
        
        // Ajout de l'etat vide
        vector<int> EmptyState; vector<vector<int> > EmptyStateVector; EmptyStateVector.push_back(EmptyState);
        state_space.push_back(EmptyStateVector);
        
        // @DONE définir les probas d'entrée
        // vecteur de taille (1, max_value) et proba pour chaque case
        
        vector<double> inter_arrival_time_proba,deadline_proba,size_proba;
        
        Creation_proba(inter_arrival_time_proba,deadline_proba,size_proba, max_inter_arrival_time, max_size, max_deadline);
        
        // En sortie, on a (1,D), (1,C) et (0,L)
        max_inter_arrival_time=(int)(inter_arrival_time_proba.size())-1;
        
        int tailleListemin= (int)(state_space.size());
        vector<vector<vector<int> > > Construct_state_space = state_space;
        vector<int> Vector_State_inter_arrival_time;
        for (int i=0; i<max_inter_arrival_time-1;i++) {
                state_space.insert(state_space.end(),Construct_state_space.begin(),Construct_state_space.end());
        }
        for (int i=0; i<max_inter_arrival_time;i++) {
                for (int j=0; j<Construct_state_space.size();j++) {
                        Vector_State_inter_arrival_time.push_back(i+1);
                }
        }
        // Ensemble des combinaisons possibles en 10ième pour la distrib des WCETs
        int compt=0, pos=-1;
        vector<vector<double> > ens_possible_proba_size;
        vector<double> vector_proba_courant;
        double somme;
        int discretisation_pas=10;
        for (int c1=0; c1<=discretisation_pas; c1++) {
                compt=1;
                somme=(double)c1/discretisation_pas;
                vector_proba_courant.push_back((double)c1/discretisation_pas);
                CalculProbaTaille(somme, vector_proba_courant, ens_possible_proba_size, pos, compt, discretisation_pas);
                vector_proba_courant.erase(vector_proba_courant.end()-1);
                compt--;
        }
        
        /***********/
        
        vector<vector<double> > EtudeTaille_sortie;
        
        ens_possible_proba_size.erase(ens_possible_proba_size.begin(), ens_possible_proba_size.end());
        ens_possible_proba_size.push_back(size_proba);
        
        for (int configSize=0; configSize<ens_possible_proba_size.size();configSize++) {
                size_proba=ens_possible_proba_size[configSize];
                //                cout << "Iter n° " << configSize << "/" << ens_possible_proba_size.size() << " Configuration taille: " << size_proba << endl;
                cout << "Size distribution: " << size_proba << endl;
                cout << "Deadline distribution: " << deadline_proba << endl;
                cout << "Inter-arrival time distribution: " << inter_arrival_time_proba << endl;
                
                /********************* VALEUR PRISE EN ENTREE 2 ************************/
                int T = 1000;                  // Maturity
                int TailleMaxBuffer = max_buffer; // TailleMax du buffer
                
                vector<double> Speeds_min((T)*(int)state_space.size(),0), Nrj_min((T)*(int)state_space.size(),0);
                vector<double> Speeds_min_VI((int)state_space.size(),0), Nrj_min_VI((int)state_space.size(),0);
                
                //                float temps_VI; clock_t t1_VI, t2_VI; t1_VI= clock(); // gestion du temps
                
                bool Test_VI = true; // Sinon c'est test Backward
                if (Test_VI) {
                        Value_iteration(max_deadline,TailleMaxBuffer,state_space,Vector_State_inter_arrival_time,max_size, max_speeds,inter_arrival_time_proba,deadline_proba, size_proba, Speeds_min_VI, Nrj_min_VI,tailleListemin,typeloi_uniforme);
                } else {
                        Backward_resolution(max_deadline,TailleMaxBuffer, T, state_space, Vector_State_inter_arrival_time, max_size, max_speeds,inter_arrival_time_proba,deadline_proba, size_proba, Speeds_min, Nrj_min,tailleListemin,typeloi_uniforme);
                }
                
                //                t2_VI = clock(); temps_VI = (float)(t2_VI-t1_VI)/CLOCKS_PER_SEC; cout << "temps VI (min): " << temps_VI/60 << endl;// gestion du temps
                /**** PARAMETRES ****/
                int nbre_simu = 1000;
                
                vector<vector<vector<int> > > TestStateSpace;
                for (int i=0; i<T;i++) {
                        TestStateSpace.insert(TestStateSpace.end(),state_space.begin(),state_space.end());
                }
                
                vector<double> Matrice_vitesse = Speeds_min;
                vector<int> valueDiffSortie; // Comprend
                int sommeLorchvsMDP=0, sommesquaredLorchvsMDP=0, sommeOAvsMDP=0, sommesquaredOAvsMDP=0;
                int compteur=nbre_simu;
                vector<int> Valeur_obtenue_diffLorchMDP, Valeur_obtenue_diffOAMDP;
                vector<vector<double> >  Sortie_cost_MDP_Lorch,Sortie_cost_MDP_OA;
                int sommeMoyenneMDP=0,sommeMoyenneLorch=0, sommeMoyenneOA=0;
                
                /*** PARTIE MONITORING sur ArrivalTime et Deadline ***/
                vector<vector<double> > Monitoring_Vector;
                //        for (int i=0; i<4; i++)
                //                Monitoring_Vector.push_back(Nbre_job_arrivParXMinB);
                //        
                vector<vector<double> > typeDeadline;
                //        for (int long_buffer=0; long_buffer<=max_buffer; long_buffer++) {
                //                for (int i=0; i<=max_deadline; i++) {
                //                        for (int j=i; j<=max_deadline; j++) {
                //                                for (int z=j; z<=max_deadline; z++) {
                //                                        vector<double> DeadlineRes;DeadlineRes.push_back(long_buffer);DeadlineRes.push_back(i*100+j*10+z);DeadlineRes.push_back(0);
                //                                        typeDeadline.push_back(DeadlineRes);
                //                                }
                //                        }
                //                }
                //        }
                /******/
                
                while (compteur>0) {
                        //                        if (compteur%100==0)
                        //                                cout << "============================================================================ SIMU n° " << compteur << " / " << nbre_simu << endl;
                        if (Test_VI) {
                                valueDiffSortie = simulationVI(size_proba, deadline_proba,inter_arrival_time_proba,typeDeadline,Monitoring_Vector,max_speeds,TailleMaxBuffer, Speeds_min_VI, max_size, max_deadline, max_inter_arrival_time, state_space,T,Sortie_cost_MDP_Lorch,Sortie_cost_MDP_OA,Vector_State_inter_arrival_time,tailleListemin);
                        } else {
                                valueDiffSortie = simulation(TailleMaxBuffer, Speeds_min, max_size, max_deadline, max_inter_arrival_time, state_space,T,Sortie_cost_MDP_Lorch,Sortie_cost_MDP_OA,Vector_State_inter_arrival_time,tailleListemin);
                        }
                        
                        Valeur_obtenue_diffLorchMDP.push_back(valueDiffSortie[0]);
                        Valeur_obtenue_diffOAMDP.push_back(valueDiffSortie[1]);
                        
                        sommeLorchvsMDP+=valueDiffSortie[0];
                        sommesquaredLorchvsMDP+=pow(valueDiffSortie[0],2);
                        
                        sommeOAvsMDP+=valueDiffSortie[1];
                        sommesquaredOAvsMDP+=pow(valueDiffSortie[1],2);
                        
                        sommeMoyenneMDP+=Sortie_cost_MDP_Lorch[nbre_simu-compteur][0];
                        sommeMoyenneLorch+=Sortie_cost_MDP_Lorch[nbre_simu-compteur][1];
                        sommeMoyenneOA+=Sortie_cost_MDP_OA[nbre_simu-compteur][1];
                        
                        compteur--;
                }
                cout << " Val moyenne de Cout MDP         : " << (double)sommeMoyenneMDP/(double)nbre_simu << endl;
                cout << " Val moyenne de Cout LORCH       : " << (double)sommeMoyenneLorch/(double)nbre_simu << endl;
                cout << " Val moyenne de Cout OA          : " << (double)sommeMoyenneOA/(double)nbre_simu << endl;
                cout << " La diff moyenne entre Lorch-MDP : " << (double)sommeLorchvsMDP/(double)nbre_simu << endl;
                cout << " La diff moyenne entre OA-MDP    : " << (double)sommeOAvsMDP/(double)nbre_simu << endl;
                
                // Affichage mean + interval de confiance
                cout << "*********** LORCH vs MDP ***********" << endl;
                vector<double> sortieLorch = Affichage_resultat(Sortie_cost_MDP_Lorch, Sortie_cost_MDP_Lorch, Sortie_cost_MDP_Lorch, nbre_simu, T, 1, 1, 0);
                cout << "*********** OA vs MDP ***********" << endl;
                vector<double> sortieOA = Affichage_resultat(Sortie_cost_MDP_OA, Sortie_cost_MDP_OA, Sortie_cost_MDP_OA, nbre_simu, T, 1, 1, 0);
                
                //        /*** @DONE Monitoring Analysis sur ArrivalTime and Deadline ***/
                //        vector<vector<double> > Proba_monitoring=Monitoring_Vector;
                //        for (int i=0; i< Monitoring_Vector.size(); i++) {
                //                int sum =0;
                //                for (int j=0; j<Monitoring_Vector[i].size(); j++) {
                //                        sum+=Monitoring_Vector[i][j];
                //                }
                //                for (int j=0; j<Monitoring_Vector[i].size(); j++) {
                //                        Proba_monitoring[i][j] = Monitoring_Vector[i][j]/sum;
                //                }
                //        }
                //        vector<vector<double> > Proba_deadline=typeDeadline;
                //        int sum =0;
                //        for (int i=0; i<=max_buffer; i++) {
                //                sum=0;
                //                for (int j=0; j<typeDeadline.size(); j++) {
                //                        if (i==typeDeadline[j][0])
                //                                sum+=typeDeadline[j][2];
                //                }
                //                for (int j=0; j<typeDeadline.size(); j++) {
                //                        if (i==typeDeadline[j][0])
                //                                Proba_deadline[j][2]=(double)typeDeadline[j][2]/sum;
                //                }
                //                
                //        }
                //        Affiche(Proba_deadline);
                
                // @TODO tracer de fonction
                // @TODO fonction qui prend un vector<double> et le transforme en fichier R que l'on execute
                
                vector<double> Sortie_int;
                // Ajout de la moyenne des tailles des jobs
                double moyenne_proba_size=0;
                double squaredValue=0;
                for (int i=0; i<size_proba.size();i++) {
                        moyenne_proba_size+=size_proba[i]*(i+1);
                        squaredValue+=pow(size_proba[i]*(i+1)*discretisation_pas,2);
                }
                Sortie_int.push_back(moyenne_proba_size);
                Sortie_int.push_back(sortieLorch[0]*100);
                Sortie_int.push_back(sortieOA[0]*100);
                Sortie_int.push_back(squaredValue/(double)discretisation_pas - pow(moyenne_proba_size,2));
                if (sortieOA[0]*100<0)
                        cout << "INFERIEUR à 0" << endl;
                EtudeTaille_sortie.push_back(Sortie_int);
                
                //                // @TODO tracer les sorties de fonctions 
                //                Enregistrer(Sortie_cost_MDP_Lorch);
                //                cout << "Configuration taille: " << size_proba << endl;
                //                cout << endl;
                
        }
        //        // @TODO Recuperation du vecteur EtudeTaille_sortie dans un fichier
        //        // fichier dataTailleOAvsMDPvsLorch.txt
        //        Enregistrer(EtudeTaille_sortie);
        return 0;
}
