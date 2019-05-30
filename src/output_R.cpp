//
//  output_R.cpp
//  finite_case
//
//  Created by Stéphan on 02/10/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//

#include "output_R.hpp"

void Tracer_simulation_VI(vector<vector<double> > & sortie_bis) {
        ofstream data_sortie;
        data_sortie.open("/Users/stephan/WORKSPACE/these/finite_case/output_files/data_sortie.txt", ios::app);
        data_sortie << "vitesseVI<-c(";
        for (int i=0; i<sortie_bis[3].size(); i++) {
                data_sortie << sortie_bis[3][i];
                if (i < sortie_bis[3].size()-1) {
                        data_sortie << ",";
                }
        }
        data_sortie << ")" << endl;       // On copie le source dans le temporaire
        data_sortie << "pointOffline<-list(a,d,taille,deadline)" << endl;
        data_sortie << "tracer_VI_DP_OA(pointOffline,vitesse,vitesseOA,vitesseVI,max_deadline,cas_multi)" << endl;
        data_sortie.close();
}

void tracer_courbe_function(vector<vector<double> > & val_tracer) {
        //        char *mot = " creer en c++";
        ofstream data_sortie ("data_sortie.txt");
        //        ofstream data_sortie;
        //        data_sortie.open("~/WORKSPACE/these/finite_case/finite_case/data_sortie.txt", ios::out);
        //        if (data_sortie.good()) {
        //                cout << "PAS OK " << endl;
        //        }
        //        data_sortie << "contenu du fichier " << endl;//*
        for (int j=0; j<val_tracer.size(); j++) {
                if (j==0) {
                        data_sortie << "a<-c(";
                        if (val_tracer[0][0] !=0)
                                data_sortie << "0,";
                } else if (j==1) {
                        data_sortie << "d<-c(";
                } else if (j==2) {
                        data_sortie << "deadline<-c(";
                } else if (j==3) {
                        data_sortie << "vitesse<-c(";
                } else if (j==4) {
                        data_sortie << "vitesseOA<-c(";
                } else if (j==5) {
                        data_sortie << "w_value<-c(";
                } else if (j==6) {
                        data_sortie << "taille<-c(";
                }
                for (int i=0; i<val_tracer[j].size(); i++) {
                        data_sortie << val_tracer[j][i];
                        if (i < val_tracer[j].size()-1) {
                                data_sortie << ",";
                        }
                }
                data_sortie << ")" << endl;
        }
        data_sortie << "vitesseVI<-c(";
        for (int i=0; i<val_tracer[3].size(); i++) {
                data_sortie << val_tracer[3][i];
                if (i < val_tracer[3].size()-1) {
                        data_sortie << ",";
                }
        }
        data_sortie << ")" << endl;
        data_sortie << "pointOffline<-list(a,d,taille,deadline)" << endl;
        data_sortie << "tracer_VI_DP_OA(pointOffline,vitesse,vitesseOA,vitesseVI,max_deadline,cas_multi)" << endl;
        data_sortie.close();
}

// Enregistre dans un fichier la matice de Qlearning
int Fichier_EnregistrerCourbeQ(vector<vector<double> > & MinEnergie,vector<vector<double> > & vitesse_choisie,vector<vector<int> > & Espace_detat_reduit_appris,int nbre_simu_apprentissage, int PasDAnalyse) {
        ofstream file("MatriceQlearning.dat");
        int nb_etat_etudier = (int)Espace_detat_reduit_appris.size();
        file << "nb_etat_etudier = " << nb_etat_etudier << endl;
        file << "nbre_simu_apprentissage = " << nbre_simu_apprentissage << endl;
        
        vector<double> position;
        for (int j=0; j<nb_etat_etudier ; j++) {
                //                if (MinEnergie[nbre_simu_apprentissage-1][j] !=0) {
                position.push_back(j);
                file << "etat" << j  << "<-c(";
                for (int i=0; i<nbre_simu_apprentissage; i++){
                        if (i<nbre_simu_apprentissage-1-PasDAnalyse) {
                                file << MinEnergie[i][j] << ",";
                        } else {
                                file << MinEnergie[i][j];
                        }
                        i+=PasDAnalyse;
                }
                file << ");" << endl;
                //                }
        }
        file << "position<-c(" ;
        for (int i=0; i<position.size();i++) {
                if (i<(position.size()-1)) {
                        file << position[i] << ",";
                } else {
                        file << position[i];
                }
        }
        file << ");" << endl;
        file.close();
        return 0;
}

int Fichier_Enregistrer1etat(vector<vector<double> > & etat37,int nb_simu) {
        ofstream file("MatriceEtat37.dat");
        int vitesse = (int)(etat37[0].size());
        for (int i=0; i<vitesse; i++) {
                file << "etat37_vitesse" << i  << "<-c(";
                for (int j=0; j<nb_simu; j++) {
                        if (j<(nb_simu-1)) {
                                file << etat37[j][i] << ",";
                        } else {
                                file << etat37[j][i];
                        }
                }
                file << ");" << endl;
        }
        file.close();
        return 1;
}


// Enregistre dans un fichier la matice de vitesses au cours du temps
int Fichier_EnregistrerCourbeVitesse(vector<vector<double> > & MinEnergie,vector<vector<double> > & vitesse_choisie,vector<vector<int> > & Espace_detat_reduit_appris,int nbre_simu_apprentissage, int PasDAnalyse) {
        ofstream filevitesse("VitessesQlearning.dat");
        
        int nb_etat_etudier = (int)Espace_detat_reduit_appris.size();
        filevitesse << "nb_etat_etudier = " << nb_etat_etudier << endl;
        filevitesse << "nbre_simu_apprentissage = " << nbre_simu_apprentissage << endl;
        vector<double> position;
        for (int j=0; j<nb_etat_etudier ; j++) {
                if (MinEnergie[nbre_simu_apprentissage-1][j] !=0) {
                        position.push_back(j);
                }
        }
        for (int j=0; j<position.size() ; j++) {
                filevitesse << "vitesse" << position[j]  << "<-c(";
                for (int i=0; i<nbre_simu_apprentissage; i++){
                        if (i<(nbre_simu_apprentissage-1-PasDAnalyse)) {
                                filevitesse << vitesse_choisie[i][position[j]] << ",";
                        } else {
                                filevitesse << vitesse_choisie[i][position[j]];
                        }
                        i+=PasDAnalyse;
                }
                filevitesse << ");" << endl;
        }
        filevitesse << "position<-c(" ;
        for (int i=0; i<position.size();i++) {
                if (i<(int)position.size()-1) {
                        filevitesse << position[i] << ",";
                } else {
                        filevitesse << position[i];
                }
        }
        filevitesse << ");" << endl;
        filevitesse.close();
        return 0;
}

// Visualisation de l'état (0,3,6) tous les PasDAnalyse pas 
int Fichier_EnregistrerEtat036(vector<vector<double> > & etat496, int PasDAnalyse) {
        vector<double> etat496_v0,etat496_v1,etat496_v2,etat496_v3;
        for (int i=0; i<etat496.size(); i++) {
                etat496_v0.push_back(etat496[i][0]);
                etat496_v1.push_back(etat496[i][1]);
                etat496_v2.push_back(etat496[i][2]);
                etat496_v3.push_back(etat496[i][3]);
                i+=PasDAnalyse;
        }
        ofstream file("Etat496.dat");
        file << "etat496_v0<-c(";
        for (int j=0; j<etat496_v0.size() ; j++) {
                if (j<(int)(etat496_v0.size())-1) {
                        file << etat496_v0[j] << ",";
                } else {
                        file << etat496_v0[j];
                }
        }
        file << ");" << endl;
        file << "etat496_v1<-c(";
        for (int j=0; j<etat496_v1.size() ; j++) {
                if (j<(int)(etat496_v1.size())-1) {
                        file << etat496_v1[j] << ",";
                } else {
                        file << etat496_v1[j];
                }
        }
        file << ");" << endl;
        file.close();
        return 0;
}

int Fichier_Enregistrer_VI(vector<vector<vector<double> > > & sortie_VI) {
        ofstream file("NRJVI.dat");
        file << "VIvalue<-c(";
        for (int j=0; j<sortie_VI[0][0].size(); j++) {
                if (j<(int)(sortie_VI[0][0].size())-1) {
                        file << sortie_VI[0][0][j] << ",";
                } else {
                        file << sortie_VI[0][0][j];
                }
        }
        file << ");" << endl;
        file.close();
        return 0;
}

// Enregistrer EtudeTaille_sortie
void Enregistrer(vector<vector<double> > & EtudeTaille_sortie) {
        ofstream dataTailleOAvsMDPvsLorch("dataTailleOAvsMDPvsLorch.txt");
        //        dataTailleOAvsMDPvsLorch.open("/Users/stephan/WORKSPACE/these/finite_case/output_files/dataTailleOAvsMDPvsLorch.txt", ios::app);
        dataTailleOAvsMDPvsLorch << "MoyenneTaille<-c(";
        for (int i=0; i<EtudeTaille_sortie.size(); i++) {
                dataTailleOAvsMDPvsLorch << EtudeTaille_sortie[i][0];
                if (i < EtudeTaille_sortie.size()-1) {
                        dataTailleOAvsMDPvsLorch << ",";
                }
        }
        dataTailleOAvsMDPvsLorch << ")" << endl;
        dataTailleOAvsMDPvsLorch << "LorchvsMDP<-c(";
        for (int i=0; i<EtudeTaille_sortie.size(); i++) {
                dataTailleOAvsMDPvsLorch << EtudeTaille_sortie[i][1];
                if (i < EtudeTaille_sortie.size()-1) {
                        dataTailleOAvsMDPvsLorch << ",";
                }
        }
        dataTailleOAvsMDPvsLorch << ")" << endl;
        dataTailleOAvsMDPvsLorch << "OAvsMDP<-c(";
        for (int i=0; i<EtudeTaille_sortie.size(); i++) {
                dataTailleOAvsMDPvsLorch << EtudeTaille_sortie[i][2];
                if (i < EtudeTaille_sortie.size()-1) {
                        dataTailleOAvsMDPvsLorch << ",";
                }
        }
        dataTailleOAvsMDPvsLorch << ")" << endl;
}

