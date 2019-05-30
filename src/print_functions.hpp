//
//  print_functions.hpp
//  finite_case
//
//  Created by Stéphan on 02/10/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//

#ifndef print_functions_hpp
#define print_functions_hpp

#include "simulation_policyApplication.hpp"

// Affichage des résultats en % avec les intervalles de confiance
vector<double> int_conf(vector<vector<double> > sortie_gen_DP,double somme,double sommeEcartType);
vector<double> Affichage_resultat(vector<vector<double> > sortie_gen_DP, vector<vector<double> > sortie_gen_VI, vector<vector<double> > sortie_gen_Qlearning, int sample, int T, int affichage, bool int_conf_index, int nbre_simulation_Qlearning);

int Affichage_VIvsQl(vector<int> & Etatvisiter,vector<vector<vector<double> > > sortie_VI, vector<int> & vitesse_gen_gen, vector<vector<int> > & espace_detat_bis);
int Affiche_resultat_Qlearning(vector<vector<double> > & Energie,vector<vector<double> > & Energie_res,vector<int> & vitesse_gen,vector<int> & VitesseMin,vector<vector<int> > & Espace_detat_reduit_appris,vector<int> & nbre_passage, vector<vector<double> > & VitesseEnregistrer, vector<int> & Etatvisiter);
int Affichage_VIvsQl_Construction(vector<int> & Etatvisiter,vector<vector<vector<double> > > & sortie_VI, vector<int> & vitesse_gen_gen, vector<vector<int> > & espace_detat_bis);

#endif /* print_functions_hpp */
