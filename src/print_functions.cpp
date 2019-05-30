//
//  print_functions.cpp
//  finite_case
//
//  Created by Stéphan on 02/10/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//

#include "print_functions.hpp"
#include <cmath>

vector<double> int_conf(vector<vector<double> > sortie_gen_DP, double somme, double sommeEcartType) {
        vector<double> sortie;
        double mean = somme/(sortie_gen_DP.size());
        cout <<"moyenne " <<  somme/(sortie_gen_DP.size()) << endl;
        cout << "value_test " << sommeEcartType/(double)(sortie_gen_DP.size()) - pow(mean,2) << endl;
        double ecart_type = sqrt(sommeEcartType/(double)(sortie_gen_DP.size()) - pow(mean,2));
        cout << "ecart type " <<  ecart_type << endl;
        double intconfmin = mean - 1.96*ecart_type/sqrt((int)(sortie_gen_DP.size()));
        double intconfmax = mean + 1.96*ecart_type/sqrt((int)(sortie_gen_DP.size()));
        cout << "confidence interval 95% " << intconfmin << "    " << intconfmax << endl;
        sortie.push_back(mean);sortie.push_back(intconfmin); sortie.push_back(intconfmax);
        return sortie;
};

vector<double> Affichage_resultat(vector<vector<double> > sortie_gen_DP, vector<vector<double> > sortie_gen_VI, vector<vector<double> > sortie_gen_Qlearning, int sample, int T, int affichage, bool int_conf_index, int nbre_simulation_Qlearning) {
        vector<double> moyenne_sortie;
        cout << "LE NOMBRE DITERATION EST DE " << T << endl;
        cout << "nbre simulations utilisant les politiques apprises " << sample << endl;
        // cout << "PAS d'apprentissage du Qlearning " << nbre_simulation_Qlearning << endl;
        double somme = 0, sommeEcartType = 0;
        int nbreAEnlever = 0;
        vector<double> diff;
        double moyenne_OAvsDP=0,moyenne_OAvsVI=0,moyenne_VIvsDP=0,valTotalDP=0,valTotalVI=0,valOA=0,moyenne_OAvsQlearning=0,valTotalQlearning=0;
        
        if (affichage==0 || affichage==1) {
                // OAvsDP
                for (int j=0; j<sortie_gen_DP.size(); j++){
                        if (sortie_gen_DP[j][0] != 0) {
                                diff.push_back((sortie_gen_DP[j][1]-sortie_gen_DP[j][0])/sortie_gen_DP[j][0]);
                        } else {
                                nbreAEnlever++;
                        }
                }
                for (int j=0; j<diff.size(); j++){
                        somme+=diff[j];
                        sommeEcartType+=pow(diff[j],2);
                        valTotalDP+=sortie_gen_DP[j][0];
                        valOA+=sortie_gen_DP[j][1];
                }
                if (int_conf_index) {
                        int_conf(sortie_gen_DP,somme, sommeEcartType);
                }
                moyenne_OAvsDP = somme/(double)diff.size();
        }
        if (affichage==0 || affichage==2) {
                // OAvsVI
                somme=0; diff.erase(diff.begin(), diff.end());sommeEcartType=0;
                for (int j=0; j<sortie_gen_VI.size(); j++){
                        if (sortie_gen_VI[j][0] != 0) {
                                diff.push_back((sortie_gen_VI[j][1]-sortie_gen_VI[j][0])/sortie_gen_VI[j][0]);
                        } else {
                                nbreAEnlever++;
                        }
                }
                for (int j=0; j<diff.size(); j++){
                        somme+=diff[j];
                        sommeEcartType+=pow(diff[j],2);
                        valTotalVI+=sortie_gen_VI[j][0];
                }
                if (int_conf_index) {
                        int_conf(sortie_gen_VI,somme,sommeEcartType);
                }
                moyenne_OAvsVI = somme/(double)diff.size();
        }
        if (affichage==0 || affichage==3) {
                // VIvsDP
                somme=0; diff.erase(diff.begin(), diff.end());sommeEcartType=0;
                for (int j=0; j<sortie_gen_DP.size(); j++){
                        if (sortie_gen_DP[j][0] != 0) {
                                diff.push_back((sortie_gen_VI[j][0]-sortie_gen_DP[j][0])/sortie_gen_DP[j][0]);
                        } else {
                                nbreAEnlever++;
                        }
                }
                for (int j=0; j<diff.size(); j++){
                        somme+=diff[j];
                        sommeEcartType+=pow(diff[j],2);
                }
                int_conf_index=false;
                if (int_conf_index) {
                        int_conf(sortie_gen_DP,somme, sommeEcartType);
                }
                moyenne_VIvsDP = somme/(double)diff.size();
        }
        if (affichage==0 || affichage==4) {
                // OAvsVI
                somme=0; diff.erase(diff.begin(), diff.end());sommeEcartType=0;
                for (int j=0; j<sortie_gen_Qlearning.size(); j++){
                        if (sortie_gen_Qlearning[j][0] != 0) {
                                diff.push_back((sortie_gen_Qlearning[j][1]-sortie_gen_Qlearning[j][0])/sortie_gen_Qlearning[j][0]);
                        } else {
                                nbreAEnlever++;
                        }
                }
                for (int j=0; j<diff.size(); j++){
                        somme+=diff[j];
                        sommeEcartType+=pow(diff[j],2);
                        valTotalQlearning+=sortie_gen_Qlearning[j][0];
                }
                if (int_conf_index) {
                        int_conf(sortie_gen_Qlearning,somme,sommeEcartType);
                }
                moyenne_OAvsQlearning = somme/(double)diff.size();
        }
        cout << "<<<<<======================" << endl;
        if (affichage==0 || affichage==1) {
                cout << "Overconsumption vs DP (%) : " << moyenne_OAvsDP*100 << endl;
                //                cout << "valTotalDP: " << valTotalDP/sortie_gen_DP.size() << endl;
        }
        if (affichage==0 || affichage==2) {
                cout << "Overconsumption vs VI (%) : " << moyenne_OAvsVI*100 << endl;
                //                cout << "valTotalVI: " << valTotalVI/sortie_gen_VI.size() << endl;
        }
        if (affichage==0 || affichage==4) {
                cout << "Overconsumption vs Qlearning (%) : " << moyenne_OAvsQlearning*100 << endl;
        }
        if (affichage==3) {
                cout << "VI vs DP (%) : " << moyenne_VIvsDP*100 << endl;
                //                cout << "valOA: " << valOA/sortie_gen_DP.size() << endl;
        }
        if (affichage==0)
                cout << "VI vs Qlearning (%) : " << (moyenne_OAvsQlearning-moyenne_OAvsVI)*100 << endl;
        cout << "<<<<<======================" << endl;
        moyenne_sortie.push_back(moyenne_OAvsDP);moyenne_sortie.push_back(moyenne_OAvsVI);moyenne_sortie.push_back(moyenne_VIvsDP);moyenne_sortie.push_back(moyenne_OAvsQlearning);
        //        // STOCKAGE DES RESULTATS
        //        ofstream data_sortie;
        //        data_sortie.open("/Users/stephan/WORKSPACE/these/finite_case/output_files/resultat_stocker.txt", ios::app);
        //        data_sortie << "OAvsDP: " ;data_sortie << moyenne_OAvsDP; data_sortie << ";";data_sortie << "OAvsVI: ";data_sortie << moyenne_OAvsVI;data_sortie << ";" ;data_sortie <<  "VIvsDP: ";data_sortie << moyenne_VIvsDP <<endl;
        //        data_sortie << "============" << endl;
        //        data_sortie.close();
        
        return moyenne_sortie;
}

int Affichage_VIvsQl(vector<int> & Etatvisiter,vector<vector<vector<double> > > sortie_VI, vector<int> & vitesse_gen_gen, vector<vector<int> > & espace_detat_bis) {
        vector<int> etat_diff;
        for (int jk=0; jk<espace_detat_bis.size();jk++) {
                //                if (vitesse_gen_gen[Etatvisiter[jk]]  > 100000000) {
                //                        value = +INFINITY;
                //                } else {
                //                        value=(double)vitesse_gen_gen[Etatvisiter[jk]];
                //                }                
                if (sortie_VI[1][0][jk]!=(double)vitesse_gen_gen[jk]) {
                        cout << "DIFFERENCE" << endl;
                        cout << "============> " << jk << " = (" << espace_detat_bis[jk][0] << "," << espace_detat_bis[jk][1] << ","<< espace_detat_bis[jk][2] << ")"<< endl;
                        cout << "VI: " << sortie_VI[1][0][jk] << " vs Ql " <<  vitesse_gen_gen[jk] <<  endl;
                        etat_diff.push_back(jk);
                }
        }
        cout << "Etat diff: " << etat_diff.size() << " => " << etat_diff << endl;
        
        return 0;
}

int Affichage_VIvsQl_Construction(vector<int> & Etatvisiter,vector<vector<vector<double> > > & sortie_VI, vector<int> & vitesse_gen_gen, vector<vector<int> > & espace_detat_bis) {
        vector<int> etat_diff;
        for (int jk=0; jk<Etatvisiter.size();jk++) {
                if (sortie_VI[1][0][Etatvisiter[jk]]!=(double)vitesse_gen_gen[jk]) {
                        cout << "DIFFERENCE" << endl;
                        cout << "NOK ========> " << Etatvisiter[jk] << " = (" << espace_detat_bis[Etatvisiter[jk]][0] << "," << espace_detat_bis[Etatvisiter[jk]][1] << ","<< espace_detat_bis[Etatvisiter[jk]][2] << ")"<< endl;
                        cout << "VI: " << sortie_VI[1][0][Etatvisiter[jk]] << " vs Ql " <<  vitesse_gen_gen[jk] <<  endl;
                        etat_diff.push_back(Etatvisiter[jk]);
                } else {
                        cout << "OK   ==> " << Etatvisiter[jk] << " = (" << espace_detat_bis[Etatvisiter[jk]][0] << "," << espace_detat_bis[Etatvisiter[jk]][1] << ","<< espace_detat_bis[Etatvisiter[jk]][2] << ")"<< endl;
                        cout << "vitesse: " <<  vitesse_gen_gen[jk] <<  endl;
                }
        }
        cout << "Etat diff: " << etat_diff.size() << " => " << etat_diff << endl;
        
        return 0;
}

// CALCUL VITESSE A PARTIR Q-ENERGIE
int Affiche_resultat_Qlearning(vector<vector<double> > & Energie,vector<vector<double> > & Energie_res,vector<int> & vitesse_gen,vector<int> & VitesseMin,vector<vector<int> > & Espace_detat_reduit_appris,vector<int> & nbre_passage, vector<vector<double> > & VitesseEnregistrer, vector<int> & Etatvisiter) {
        for (int i=0; i<Espace_detat_reduit_appris.size();i++) {
                if (Energie[i][0]!=0) {
                        cout << "===================== " << i << "  " << VitesseMin[i] << endl;
                        cout << Espace_detat_reduit_appris[i];
                        cout << Energie[i];
                        cout << vitesse_gen[i] << endl;
                        //                        cout << VitesseEnregistrer[i] << endl;
                        Etatvisiter.push_back(i);
                }
        }
        bool affiche_test_et_prop=false;
        if (affiche_test_et_prop) {
                vector<vector<double> > test =Energie-Energie_res; cout << "Diff entre les deux derniers vecteurs d'énergie: " << test << endl;
                vector<int>::iterator it0 = min_element(nbre_passage.begin(), nbre_passage.end());
                cout << " ==================== PROPORTION DE PASSAGE ============================ " << endl;
                cout << "proportion nbre etat max vs nbre etat min :  " << (double)*max_element(nbre_passage.begin(), nbre_passage.end())/ (double)*min_element(nbre_passage.begin(), it0) << endl;
                cout << "nbre dans l'etat minimal : " << *min_element(nbre_passage.begin(), it0)  << " --- et dans l'etat maximal : " << *max_element(nbre_passage.begin(), nbre_passage.end()) << endl;
        }
        cout << endl;
        //        // @AUTILISER pour gérer la précision
        //        for (int ill=0; ill<Energie.size(); ill++) {
        //                for (int kll=0; kll<Energie[ill].size(); kll++) {
        //                        //précision à 10^-5
        //                        Energie[ill][kll] = floor(Energie[ill][kll]*100000);
        //                }
        //        }
        return 0;
}