//
//  simulation_policyApplication.hpp
//  finite_case
//
//  Created by Stéphan on 02/10/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//

#ifndef simulation_policyApplication_hpp
#define simulation_policyApplication_hpp

#include "backward_resolution.hpp"


int Calcul_vitesse_OA_ETaches(map<int, vector<int> >  & ETaches_courant,vector<int> & vitesse_OA);

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_restreint(vector<vector<double> > & espace_detat_init,int T,int max_deadline,int max_period,vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res);
vector<double> simulation_restreint(int T,int max_deadline,int max_size,int max_vitesse,int max_period, vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int>  > & jobs_T, bool tracer_courbe, int max_deadline_res);

vector<double> simulation_esp_restreint(int T,vector<double> p_size,int max_reduc,int max_deadline, int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<vector<double> > > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T);
vector<double> simulationVI_esp_restreint(int T,vector<double> p_size,int max_reduc, int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<double> > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T);

vector<double> simulation(int T,vector<double> p,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_offline, vector<vector<vector<double> > > sortie_choix, int iteration, bool dp, vector<vector<double>  > & jobs_T, bool tracer_courbe);
vector<double> simulationVI(int T,vector<double> p,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_offline, vector<vector<double> > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T,bool tracer_courbe);

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_general(vector<vector<double> > & espace_detat_init,int T,int max_deadline,int max_period,vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res);
vector<double> simulation_general(bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period, vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int>  > & jobs_T, bool tracer_courbe, int max_deadline_res);
vector<double> simulation_VI(double gamma,bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int>  > & jobs_T, bool tracer_courbe, int res_max_deadline);


vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_restreint_ktaches(vector<vector<int> > & espace_detat_init,int T,int max_deadline,int max_period, vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res);
vector<double> simulation_restreint_ktaches(int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res);


vector<double> simulation_restreint_ktachesQlearning(int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<int> & sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res);
vector<double> simulation_Qlearning(double gamma, bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<int> vitesse_gen, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int res_max_deadline,vector<int> & Etatvisiter);

vector<vector<vector<double> > > LORSCH_backward_resolution(double gamma,int T, map<int,vector<Job> > & matrix_job, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init);

#endif /* simulation_policyApplication_hpp */
