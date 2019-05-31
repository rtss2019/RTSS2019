//
//  output_R.hpp
//  finite_case
//
//  Created by *** on 02/10/2018.
//  Copyright © 2018 ***. All rights reserved.
//

#ifndef output_R_hpp
#define output_R_hpp

#include "backward_resolution.hpp"
#include <fstream>
#include <iomanip>

void Tracer_simulation_VI(vector<vector<double> > & sortie_bis);
void tracer_courbe_function(vector<vector<double> > & val_tracer);
int Fichier_EnregistrerCourbeQ(vector<vector<double> > & MinEnergie,vector<vector<double> > & vitesse_choisie,vector<vector<int> > & Espace_detat_reduit_appris,int nbre_simu_apprentissage, int PasDAnalyse);
int Fichier_EnregistrerCourbeVitesse(vector<vector<double> > & MinEnergie,vector<vector<double> > & vitesse_choisie,vector<vector<int> > & Espace_detat_reduit_appris,int nbre_simu_apprentissage, int PasDAnalyse);
int Fichier_EnregistrerEtat036(vector<vector<double> > & etat496, int PasDAnalyse);
int Fichier_Enregistrer_VI(vector<vector<vector<double> > > & sortie_VI);
int Fichier_Enregistrer1etat(vector<vector<double> > & etat37,int nb_simu);
void Enregistrer(vector<vector<double> > & EtudeTaille_sortie);

#endif /* output_R_hpp */
