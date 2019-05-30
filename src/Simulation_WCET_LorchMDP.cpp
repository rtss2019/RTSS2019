//
//  Simulation_WCET_LorchMDP.cpp
//  finite_case
//
//  Created by Stéphan on 05/03/2019.
//  Copyright © 2019 Stéphan. All rights reserved.
//

#include "Simulation_WCET_LorchMDP.hpp"
#include "backward_resolution.hpp"
#include "parser.hpp"
#include "WCET_Lorch_MDP.hpp"
#include <algorithm>
#include <iterator>
#include <list>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <fstream>
#include <iomanip>
using namespace std;

bool ceilMode = false, floorMode = false, midMode = true;

bool Affichage_sortie=false;

// Function de comparaison de deux vecteurs en fonctions des deadlines (ie le deuxième paramètre) => utile pour le tri (function sort())
bool cmp(const vector<int> &a, const vector<int> &b) {
        return a[1]<b[1];
}


// Retourne la position du nouvel etat dans l'espace detat
int calcul_etat_suivant(int nbre_job_suppr, int qte_travail_faite, int ind_etat_prec, int vitesse, vector<vector<int> > & taches_arrivees,vector<vector<vector<int> > > & state_space) {
        vector<vector<int> > etat_prec = state_space[ind_etat_prec];
        
        // Step 1: Décalage dans le temps des deadlines d'etat_prec
        for (int i=0; i<etat_prec.size(); i++) {
                etat_prec[i][1]--;
        }
        
        // Step 2: diminution et suppression des etats en fonction de la vitesses choisie => NE PAS FAIRE
        // Attention ici il faut prendre en compte l et la quantité de travail executé
        // => on connait seulement les jobs supprimés et la partie du l+1ième job executées
        etat_prec.erase(etat_prec.begin(),etat_prec.begin()+nbre_job_suppr);
        etat_prec[nbre_job_suppr+1][0]+=qte_travail_faite;
        
        // Step 3: Ajout des taches arrivees
        for (int i=0; i< taches_arrivees.size(); i++)
                etat_prec.push_back(taches_arrivees[i]);
        
        // Step 4: Determiner la position de ce nouvel etat
        std::vector<vector<vector<int> > >::iterator it = find(state_space.begin(), state_space.end(), etat_prec);
        int position = (int)distance(state_space.begin(), it);
        
        return position;
}

int Arrivee_job_simu_CreationProba(int DerniereArriveeJobs, int TailleMaxBuffer, vector<vector<int> > & etat_courant, vector<vector<int> > & job_arrivee, vector<double> & inter_arrival_time_proba,vector<double> & deadline_proba,vector<double> & size_proba) {
        // Nbre de job accepté en arrivée
        int NbMaxJobsArrivees = TailleMaxBuffer -(int)(etat_courant.size());
        
        // Espace detat des jobs arrivés
        vector<vector<vector<int> > > espace_etat_jobarrivee;
        // Le premier etat ajouté, c'est l'etat des nouveaux jobs arrivées NUL
        vector<int> JobInt;
        vector<vector<int> > EtatInt; EtatInt.push_back(JobInt);
        espace_etat_jobarrivee.push_back(EtatInt);
        
        // @DONE création de l'espace detat d'arrivée
        int deb,fin;
        deb = 0;
        fin = (int)(espace_etat_jobarrivee.size());
        for (int nbjob=0; nbjob<NbMaxJobsArrivees; nbjob++) {
                for (int i=1; i<=deadline_proba.size(); i++) {
                        JobInt.erase(JobInt.begin(), JobInt.end());
                        JobInt.push_back(0);JobInt.push_back(i);
                        //                        Affiche(JobInt); cout << endl;
                        for (int j=deb;j<fin; j++) {
                                EtatInt.erase(EtatInt.begin(),EtatInt.end());
                                EtatInt = espace_etat_jobarrivee[j];
                                if (!(EtatInt[0].empty())) {
                                        EtatInt.push_back(JobInt);
                                } else {
                                        EtatInt.erase(EtatInt.begin(), EtatInt.end());
                                        EtatInt.push_back(JobInt);
                                }
                                //                                Affiche(EtatInt);
                                
                                if (!(espace_etat_jobarrivee[j][0].empty())) {
                                        if (espace_etat_jobarrivee[j][espace_etat_jobarrivee[j].size()-1][1]<=i) {
                                                espace_etat_jobarrivee.push_back(EtatInt);
                                        }
                                } else if (nbjob==0) {
                                        espace_etat_jobarrivee.push_back(EtatInt);
                                }
                        }
                }
                deb=fin;
                fin=(int)(espace_etat_jobarrivee.size());
        }
        // @DONE calcul de la proba pour un etat donné => déjà fait normalement
        vector<int> test;
        vector<vector<int> > state_prev; state_prev.push_back(test);
        vector<double> Proba_etat_nouvelle_arrivee;
        // Attention le cas par defaut est linterarrival time 1 car on a des inter-arrival time de 0 ou de 1.
        Proba_etat_nouvelle_arrivee.push_back(1-inter_arrival_time_proba[1]);
        for (int i=1; i<espace_etat_jobarrivee.size();i++) {
                // @DONE \ell = DerniereArriveeJob
                Proba_etat_nouvelle_arrivee.push_back(probability_computation_Arrival_Jobs(DerniereArriveeJobs, TailleMaxBuffer,1,state_prev, espace_etat_jobarrivee[i], inter_arrival_time_proba, deadline_proba, size_proba,0));
        }
        // @DONE Verification somme
        //        double somme=0;
        //        for (int i=0; i<Proba_etat_nouvelle_arrivee.size(); i++) {
        //                somme += Proba_etat_nouvelle_arrivee[i];
        //        }
        //        cout << somme << endl;
        
        // @DONE création du vecteur_somme qui somme les probas au fur et à mesure
        double somme = 0; double valInt=0;
        vector<double> Somme_proba_etat_nouvelle_arrivee;
        for (int i=0; i<Proba_etat_nouvelle_arrivee.size(); i++) {
                valInt=(double)Proba_etat_nouvelle_arrivee[i];
                //                cout << somme << endl;
                somme+=valInt;
                //                cout << valInt << "  " << somme << endl;
                Somme_proba_etat_nouvelle_arrivee.push_back(somme);
        }
        Somme_proba_etat_nouvelle_arrivee[(int)(Somme_proba_etat_nouvelle_arrivee.size())-1]=1;
        
        // @DONE le l est pour le test du RAND()
        //        int l=100;
        int ind_proba=0;
        //        while (l>0) {
        // @DONE Appel à la fonction RAND()
        double random_value = rand()/(double)RAND_MAX;
        if (Affichage_sortie)
                cout << "Random_value " << random_value << endl;
        
        // @DONE Cherche l'indice qui correspond à la proba dans le vecteur
        if (random_value < Somme_proba_etat_nouvelle_arrivee[0]) {
                ind_proba=0;
        } else {
                for (int i=1; i<Somme_proba_etat_nouvelle_arrivee.size(); i++) {
                        if (random_value < Somme_proba_etat_nouvelle_arrivee[i] && random_value >= Somme_proba_etat_nouvelle_arrivee[i-1])
                                ind_proba=i;
                }
        }
        //                cout << random_value << "  " << Somme_proba_etat_nouvelle_arrivee[ind_proba] << endl;
        //                cout << espace_etat_jobarrivee[ind_proba] << endl;
        //                l--;
        //        }
        
        // @DONE Recupere les jobs arrivés à partir de cet indice
        job_arrivee=espace_etat_jobarrivee[ind_proba];
        cout << job_arrivee << endl;
        // @TODO Pour vérifier les cas ou il n'ya aucun job qui arrive
        if (!job_arrivee.empty()) {
                if ((job_arrivee[0].empty())) {
                        if (Affichage_sortie)
                                cout << "stop" << endl;
                }
        }
        
        cout << "fin " << endl;
        return 0;
}



/*** BUT    : A partir de la liste des jobs determine letat suivant                                                                  ***/
/*** ENTREE : liste des jobs (ci,di), liste des jobs arrives en t (ci^t,di^t), l'indice de l'etat courant (ei,di) et l'espace d'état ***/
// liste_job_arrivee = (ci,di)
// etat_suivant = (ei,di) (etat de index_etat_suivant
// Ct=liste_job_arrivee, At=job_arrivee, it=etat_suivant, vt=vitesse_MDP
/*** RETURN : Indice de l'etat suivant                                                                                               ***/
int calcul_liste_etat_suivant(int DerniereArriveeJob, vector<vector<int> > & liste_job_arrivee, vector<vector<int> > & job_arrivee, int & index_etat_suivant, vector<vector<vector<int> > > & state_space, int vitesse_MDP, vector<int> & Vector_State_inter_arrival_time) {
        // Position de l'etat_suivant
        int pos, reste_vitesse = vitesse_MDP;
        vector<vector<int> > etat_suivant;
        if (index_etat_suivant>=0)
                etat_suivant=state_space[index_etat_suivant];
        
        // SUPPRESS EXECUTED JOBS: Suppression des jobs déjà terminé avec les taches considérées et la vitesse choisie
        int borne = (int)etat_suivant.size();
        
        if (!(etat_suivant.empty())) {
                if (!(etat_suivant[0].empty())) {
                        for (int i=0; i<borne; i++) {
                                // comparer la taille du job ave la vitesse pour le cas MDP
                                if (- liste_job_arrivee[i][0] + etat_suivant[i][0] + reste_vitesse >= 0) {
                                        // on supprime un job
                                        reste_vitesse = reste_vitesse - liste_job_arrivee[i][0] + etat_suivant[i][0];
                                        // Suppression des jobs dans la liste des jobs executés et présent
                                        liste_job_arrivee.erase(liste_job_arrivee.begin()+i);
                                        etat_suivant.erase(etat_suivant.begin()+i);
                                        borne--; i--; // Du a la suppression des jobs et donc au décalage dans le vecteur
                                } else {
                                        // modification de l'executed work, rajout de la partie executée
                                        etat_suivant[i][0] = etat_suivant[i][0] + reste_vitesse;
                                        break;
                                        
                                }
                        }
                        
                        // TIME SHIFT: déclage des jobs présents dans la liste, deadline diminuée de 1 ET ENLEVE les jobs ac deadline terminés
                        for (int i=0; i<liste_job_arrivee.size(); i++) {
                                liste_job_arrivee[i][1]--;
                                if (liste_job_arrivee[i][1]==0) {
                                        liste_job_arrivee.erase(liste_job_arrivee.begin()+i);
                                        i--; // Du a la suppression des jobs et donc au décalage dans le vecteur
                                }
                        }
                        if (index_etat_suivant>=0) {
                                for (int i=0; i<etat_suivant.size(); i++) {
                                        etat_suivant[i][1]--;
                                        if (etat_suivant[i][1]==0) {
                                                etat_suivant.erase(etat_suivant.begin()+i);
                                                i--;  // Du a la suppression des jobs et donc au décalage dans le vecteur
                                        }
                                }
                        }
                        
                }
        }
        if (Affichage_sortie)
                cout << etat_suivant.size() << " ET  " << job_arrivee.size() << " <=3 " << endl;
        
        // AJOUT NEW JOBS: ajoute les nouveaux jobs à la liste des anciens jobs déjà présent
        vector<int> job_intermediaire;
        // Si pas de job_arrivée on ne le rajoute pas
        if (!(job_arrivee.empty())) {
                if (!(job_arrivee[0].empty())) {
                        if (!(etat_suivant.empty())) {
                                if (etat_suivant[0].empty()) {
                                        etat_suivant.erase(etat_suivant.begin(), etat_suivant.end());
                                }
                        }
                        
                        for (int i=0; i<job_arrivee.size(); i++) {
                                // ajout des nouveaux jobs aux deux listes présentes
                                liste_job_arrivee.push_back(job_arrivee[i]);
                                job_intermediaire = job_arrivee[i];
                                job_intermediaire[0] = 0; // transforme le ci en ei qui vaut 0
                                etat_suivant.push_back(job_intermediaire);
                        }
                        
                        // ORDONNE liste_job_arrivee et etat_suivant en fonction des deadlines (2nd param)
                        sort(liste_job_arrivee.begin(),liste_job_arrivee.end(),cmp);
                        if (!(etat_suivant.empty())) {
                                if (!(etat_suivant[0].empty())) {
                                        sort(etat_suivant.begin(),etat_suivant.end(),cmp);
                                }
                        }
                }
        }
        
        // @ADD Cas specifique plus de jobs
        if (etat_suivant.empty()) {
                vector<int> test;
                etat_suivant.push_back(test);
        }
        
        
        
        // calcul de la position de cet nouvel état dans l'espace d'état
        std::vector<vector<vector<int> > >::iterator it = find(state_space.begin(),state_space.end(), etat_suivant);
        // @TODO Rechercher la bonne valeur avec a la fois Vector_State_inter_arrival_time et state_space
        // DerniereArriveeJob
        pos = (int)(distance(state_space.begin(),it));
        index_etat_suivant = pos;
        if (index_etat_suivant>=state_space.size()) {
                cout << "stop" << endl;
                cout << etat_suivant << endl;
                cout << endl;
        }
        if (Affichage_sortie) {
                cout << "listejob:  " << liste_job_arrivee << endl;
                cout << "etatSuivant: " << etat_suivant << endl;
        }
        
        return index_etat_suivant;
        
}

//int calcul_liste_etat_suivant_Lorch(vector<vector<int> > & liste_job_arrivee, vector<vector<int> > & job_arrivee, vector<vector<int> > & Etat_suivant_lorch, vector<vector<vector<int> > > & state_space, int vitesse_MDP) {
//        // Position de l'etat_suivant
//        int reste_vitesse = vitesse_MDP;
//        vector<vector<int> > etat_suivant;
//        //        if (index_etat_suivant>=0)
//        //                etat_suivant=state_space[index_etat_suivant];
//        
//        // Analyse du cas etta_suiavnt non nul
//        //        if (index_etat_suivant>=0) { // Prise en compte du cas particulier ou la liste n'est pas nulle
//        
//        // SUPPRESS EXECUTED JOBS: Suppression des jobs déjà terminé avec les taches considérées et la vitesse choisie
//        int borne = (int)etat_suivant.size();
//        
//        if (!(etat_suivant.empty())) {
//                if (!(etat_suivant[0].empty())) {
//                        
//                        for (int i=0; i<borne; i++) {
//                                // comparer la taille du job ave la vitesse pour le cas MDP
//                                if (- liste_job_arrivee[i][0] + etat_suivant[i][0] + reste_vitesse >= 0) {
//                                        // on supprime un job
//                                        reste_vitesse = reste_vitesse - liste_job_arrivee[i][0] + etat_suivant[i][0];
//                                        // Suppression des jobs dans la liste des jobs executés et présent
//                                        liste_job_arrivee.erase(liste_job_arrivee.begin()+i);
//                                        etat_suivant.erase(etat_suivant.begin()+i);
//                                        borne--; i--; // Du a la suppression des jobs et donc au décalage dans le vecteur
//                                } else {
//                                        // modification de l'executed work, rajout de la partie executée
//                                        etat_suivant[i][0] = etat_suivant[i][0]+reste_vitesse;
//                                        break;
//                                        
//                                }
//                        }
//                        
//                        
//                        
//                        // TIME SHIFT: déclage des jobs présents dans la liste, deadline diminuée de 1 ET ENLEVE les jobs ac deadline terminés
//                        for (int i=0; i<liste_job_arrivee.size(); i++) {
//                                liste_job_arrivee[i][1]--;
//                                if (liste_job_arrivee[i][1]==0) {
//                                        liste_job_arrivee.erase(liste_job_arrivee.begin()+i);
//                                        i--; // Du a la suppression des jobs et donc au décalage dans le vecteur
//                                }
//                        }
//                        for (int i=0; i<etat_suivant.size(); i++) {
//                                etat_suivant[i][1]--;
//                                if (etat_suivant[i][1]==0) {
//                                        etat_suivant.erase(etat_suivant.begin()+i);
//                                        i--;  // Du a la suppression des jobs et donc au décalage dans le vecteur
//                                }
//                        }
//                        
//                }
//        }
//        
//        cout << "ajout job" << endl;
//        // AJOUT NEW JOBS: ajoute les nouveaux jobs à la liste des anciens jobs déjà présent
//        vector<int> job_intermediaire;
//        // Si pas de job_arrivée on ne le rajoute pas
//        if (!(job_arrivee[0].empty())) {
//                if (!(etat_suivant.empty())) {
//                        if (etat_suivant[0].empty()) {
//                                etat_suivant.erase(etat_suivant.begin(), etat_suivant.end()); // @DONE le pb est ICI => @DONE rajout d'un etat_suivant
//                        }
//                }
//                
//                for (int i=0; i<job_arrivee.size(); i++) {
//                        // ajout des nouveaux jobs aux deux listes présentes
//                        liste_job_arrivee.push_back(job_arrivee[i]);
//                        job_intermediaire = job_arrivee[i];
//                        job_intermediaire[0] = 0; // transforme le ci en ei qui vaut 0
//                        etat_suivant.push_back(job_intermediaire);
//                }
//                
//                // ORDONNE liste_job_arrivee et etat_suivant en fonction des deadlines (2nd param)
//                sort(liste_job_arrivee.begin(),liste_job_arrivee.end(),cmp);
//                if (!(etat_suivant.empty())) {
//                        if (!(etat_suivant[0].empty())) {
//                                sort(etat_suivant.begin(),etat_suivant.end(),cmp);
//                        }
//                }
//        }
//        
//        // @ADD Cas specifique plus de jobs
//        if (etat_suivant.empty()) {
//                vector<int> test;
//                etat_suivant.push_back(test);
//        }
//        
//        cout << "listejob:  " << liste_job_arrivee << endl;
//        cout << "etatSuivant: " << etat_suivant << endl;
//        
//        return 0;
//        
//}

int calcul_liste_etat_suivant_Lorch(vector<vector<int> > & liste_job_arrivee, vector<vector<int> > & job_arrivee, vector<vector<int> > & Etat_suivant_lorch, vector<double> LorchSpeedParJob) {
        
        //        cout << "liste job arrivee" << endl;
        //        Affiche(liste_job_arrivee);
        //        cout << "etat suivant lorch " << endl;
        //        Affiche(Etat_suivant_lorch);
        //        cout << endl;
        
        // @TODO changer Lorch pour la vitesse par jobs et non de manière générale
        int compteur = 0;
        if (!(liste_job_arrivee.empty())) {
                if (!(liste_job_arrivee[0].empty())) {
                        for (int i=0; i<Etat_suivant_lorch.size(); i++) {
                                // Shift des deadlines
                                liste_job_arrivee[i][1]--;
                                Etat_suivant_lorch[i][1]--;
                                // augmentation de e_i
                                Etat_suivant_lorch[i][0]+=LorchSpeedParJob[compteur];
                                //                                liste_job_arrivee[i][0]-=LorchSpeedParJob[i];
                                if (Etat_suivant_lorch[i][0]-liste_job_arrivee[i][0]>=0) {
                                        // Suppression des jobs si les jobs de liste_job valent 0
                                        liste_job_arrivee.erase(liste_job_arrivee.begin()+i);
                                        Etat_suivant_lorch.erase(Etat_suivant_lorch.begin()+i);
                                        i--;
                                }
                                compteur++;
                        }
                }
        }
        //        cout << "liste_job_arrivee" << endl;
        //        Affiche(liste_job_arrivee);
        //        cout << "etat suivant lorch " << endl;
        //        Affiche(Etat_suivant_lorch);
        //        cout << endl;
        
        
        // @DONE ANCIENNE VERSION OU JE SUPPRIMAIS LES JOBS DE MANIERE EDF AVEC LA VITESSE
        //        // @TODO Suppression des etats deja analysé en fonction de vitesseLorch
        //        // Dans liste_job_arrivee et dans la liste Etat_suivant_lorch
        //        int res=vitesseLorch;
        //        int compteur=0;
        //        cout << "speedLorch= " << vitesseLorch << endl;
        //        //        int tailleMax = (int)(liste_job_arrivee.size());
        //        while (res>0 && compteur<liste_job_arrivee.size()) {
        //                if (res-liste_job_arrivee[compteur][0]+Etat_suivant_lorch[compteur][0]>=0) {
        //                        // Suppression des jobs executés par la vitesse Lorch
        //                        res = res-liste_job_arrivee[compteur][0]+Etat_suivant_lorch[compteur][0];
        //                        liste_job_arrivee.erase(liste_job_arrivee.begin()+compteur);
        //                        Etat_suivant_lorch.erase(Etat_suivant_lorch.begin()+compteur);
        //                        compteur--;
        //                } else {
        //                        Etat_suivant_lorch[compteur][0]+=res;
        //                        break;
        //                }
        //                compteur++;
        //        }
        //        // @TODO Shift les deadlines pour prendre en compte lavancee dans le temps
        //        if (!(liste_job_arrivee.empty())) {
        //                if (!(liste_job_arrivee[0].empty())) {
        //                        for (int i=0; i<liste_job_arrivee.size(); i++) {
        //                                liste_job_arrivee[i][1]--;
        //                                Etat_suivant_lorch[i][1]--;
        //                        }
        //                }
        //        }
        
        vector<int> jobInt;
        
        // @TODO Ajoute les job_arrivee a liste_job_arrivee Et Etat_suivant_lorch
        for (int i=0; i<job_arrivee.size(); i++) {
                jobInt=job_arrivee[i];
                liste_job_arrivee.push_back(job_arrivee[i]);
                jobInt[0]=0;
                Etat_suivant_lorch.push_back(jobInt);
        }
        
        // @TODO Reordonne
        if (!(Etat_suivant_lorch.empty())) {
                if (!(Etat_suivant_lorch[0].empty())) {
                        sort(Etat_suivant_lorch.begin(),Etat_suivant_lorch.end(),cmp);
                }
        }
        if (!(liste_job_arrivee.empty())) {
                if (!(liste_job_arrivee[0].empty())) {
                        sort(liste_job_arrivee.begin(),liste_job_arrivee.end(),cmp);
                }
        }
        //        cout << "liste_job_arrivee" << endl;
        //        Affiche(liste_job_arrivee);
        //        cout << "etat suivant lorch " << endl;
        //        Affiche(Etat_suivant_lorch);
        //        cout << endl;
        return 0;
}


// @TODO Calcul vitesse Lorch en fonction du vecteur des tailles 
// @TODO Calcul somme des probas
// @TODO Calcul des a_i, b_i pour chaque changement de proba dnas la somme
/**** TODO ****/
int Calcul_fonction_repartition_Lorch(vector<double> & size_proba, int deadline_courant, int executed_work, double & vitesse) {
        // @DONE Calcul de la somme des probas en fonction de la taille
        vector<double> Somme_proba = size_proba;
        double somme=0;
        for (int i=0; i<size_proba.size(); i++) {
                somme+=size_proba[i];
                Somme_proba[i]= somme;
        }
        
        // @DONE Calcul des a_i et b_i pour chaque portion de droite considéré
        // Repartition_droite = [fin de la pente, coef. dir. pente (ai), ordonnée a lorigine (bi)]
        vector<vector<double> > Repartition_droite;
        double value_cour = 0,res = 0;
        double ordonnee_origine, pente;
        // Calcul des pentes 
        for (int i=0; i<size_proba.size(); i++) {
                value_cour+=size_proba[i];
                pente = (value_cour-res); // Pente vaut 0 si arrivée de job de taille i nulle
                ordonnee_origine = res - (double)(i)*pente;
                vector<double> Ajout; Ajout.push_back(i); Ajout.push_back(pente); Ajout.push_back(ordonnee_origine);
                Repartition_droite.push_back(Ajout);
                res=value_cour;
        }
        
        // @TODO Calcul de \sigma en fonction de l'intégrale de la fonction de répartition de la loi uniforme par morceaux
        // Parcours de l'ensemble des portions de droites considérées
        double sigma = 0;
        double ai,bi,deb,fin;
        for (int i=0; i<Repartition_droite.size(); i++) {
                // Ajout de chaque portion de droite
                ai = Repartition_droite[i][1];
                bi = Repartition_droite[i][2];
                deb = Repartition_droite[i][0];
                fin = deb + 1;
                // En fonction de la valeur de la pente
                if (ai!=0) {
                        // Cas ou on a une integration (pente de la fonction de repartition)
                        // valeur = 3/(4*ai) * [(1-ai*i-bi)^(4/3) - (1-ai*i'-bi)^(4/3)]
                        if (((1-ai*deb-bi)<0) || ((1-ai*fin-bi)<0)) {
                                //                                cout << "===+++===> " << 1-ai*deb-bi << "  " << 1-ai*fin-bi << endl;
                                sigma += (double)3*(pow(max(1-ai*deb-bi,(double)0),(double)4/3)-pow(max(1-ai*fin-bi,(double)0),(double)4/3))/((double)4*(double)ai);
                        } else {
                                sigma += (double)3*(pow(1-ai*deb-bi,(double)4/3)-pow(1-ai*fin-bi,(double)4/3))/((double)4*(double)ai);
                        }
                        if (Affichage_sortie)
                                cout << (double)3*(pow(1-ai*deb-bi,(double)4/3)-pow(1-ai*fin-bi,(double)4/3))/((double)4*(double)ai) << endl;
                } else {
                        // Cas ou on integre pas (plateau de la fonction de repartition)
                        // valeur = (1-bi)^(1/3)
                        sigma += pow((1-bi),(double)1/3);
                }
        }
        sigma = (double)sigma/deadline_courant;
        
        // @TODO calcul de la vitesse en fonction du job \ie J_i(e_i,d)
        // @TODO Parcours de l'ensemble des droites et choisir la bonne droite en fonction de e_i
        if (executed_work==0) {
                vitesse = sigma;
        } else {
                vitesse = sigma / pow((1-Somme_proba[executed_work-1]), (double)1/3);
        }
        // Car on ne prend que les valeures entières
        // vitesse = ceil(vitesse);
        return 0;
}



// Retourne un vecteur de vitesse correspondant à la vitesse à appliquer à chaque job present dans etat_suivant
int Calcul_Vitesse_Lorch_WCET(vector<vector<int> > & etat_courant, vector<double> & LorchSpeedParJob, int max_size, vector<double> & size_proba) {
        int deadline, executedwork;
        // Parcours des jobs presents
        if (!etat_courant.empty()) {
                if (!(etat_courant[0].empty())) {
                        for (int i=0; i<etat_courant.size(); i++) {
                                executedwork = etat_courant[i][0];
                                deadline = etat_courant[i][1];
                                double speed_Lorch_1job;
                                // @TODO Dans le cas générale d'une fonction à valeur discrète approximé par une fonction uniforme par morceaux
                                Calcul_fonction_repartition_Lorch(size_proba, deadline, executedwork, speed_Lorch_1job);
                                
                                // @TODO Dans le cas particulier d'une fonction de répaartition uniforme
                                //                cout << "Speed Lorch " << (double)3/(16*deadline*pow(1-executedwork/4, 1/3)) << endl;
                                //double speed_Lorch_1job = (double)3*(double)max_size/(4*(double)deadline*pow((1-(double)executedwork/max_size),(double)1/3));
                                if (deadline==1)
                                        speed_Lorch_1job=max_size-executedwork;
                                
                                if (ceilMode) {
                                        LorchSpeedParJob.push_back(ceil(speed_Lorch_1job));
                                } else if (floorMode) {
                                        LorchSpeedParJob.push_back(floor(speed_Lorch_1job));
                                } else if (midMode) {
                                        if (speed_Lorch_1job-floor(speed_Lorch_1job)<0.5) {
                                                LorchSpeedParJob.push_back(floor(speed_Lorch_1job));
                                        } else {
                                                LorchSpeedParJob.push_back(ceil(speed_Lorch_1job));
                                        }
                                }
                                if (Affichage_sortie)
                                        cout << "Speed Lorch " << speed_Lorch_1job << endl;
                        }
                }
        }
        return 1;
}

int unif_size(int max_size) {
        int size;
        double value_random,deb,fin;
        
        value_random = rand()/(double)RAND_MAX;
        for (int i=0; i<max_size; i++) {
                deb =(double)i/(double)max_size;
                fin = (double)(i+1)/(double)max_size;
                if ( value_random >= deb && fin > value_random) {
                        size = i+1;
                        break;
                }
        }
        return size;
}

int Simu_VectorProba(vector<double> Vector) {
        //        int size = (int)(Vector.size());
        int value;
        
        // Parcours de Vector et sommation
        double somme=0;
        vector<double> Somme_Vector;
        for (int i=0; i<Vector.size(); i++) {
                somme+=Vector[i];
                Somme_Vector.push_back(somme);
        }
        
        double value_random = rand()/(double)RAND_MAX;
        double deb,fin;
        deb=0;
        for (int i=0; i<Somme_Vector.size(); i++) {
                fin = Somme_Vector[i];
                if ( value_random >= deb && fin > value_random) {
                        value = i;
                        break;
                }
                deb = Somme_Vector[i];
        }
        return value;
}


// @TODO Determine liste_job_present_OA ET etat_suivant_OA en fonction de vitesse_OA et job_arrivee
int calcul_liste_etat_suivant_OA(vector<vector<int> > & liste_job_present_OA, vector<vector<int> > & job_arrivee, vector<vector<int> > & etat_suivant_OA, int vitesse_OA) {
        
        int SpeedInt = vitesse_OA;
        int compteur=0;
        
        // @TODO Mise ajour de liste_job_present_OA ET etat_suivant_OA => enleve vitesse_OA
        if (!(liste_job_present_OA.empty())) {
                if (!(liste_job_present_OA[0].empty())) {
                        while (!(liste_job_present_OA.empty()) && SpeedInt-liste_job_present_OA[compteur][0]+etat_suivant_OA[compteur][0]>=0) {
                                // TODO Nouvelle vitesse decrut de la qté déjà faite
                                SpeedInt=SpeedInt-liste_job_present_OA[compteur][0]+etat_suivant_OA[compteur][0];
                                liste_job_present_OA.erase(liste_job_present_OA.begin()+compteur);
                                etat_suivant_OA.erase(etat_suivant_OA.begin()+compteur);
                        }
                        // @TODO Augmentation des e_in pour le job critique
                        if (!(etat_suivant_OA.empty()))
                                if (!(etat_suivant_OA[0].empty()))
                                        etat_suivant_OA[compteur][0]+=SpeedInt;
                }
        }
        
        // @TODO Shift des deadlines
        if (!(liste_job_present_OA.empty())) {
                if (!(liste_job_present_OA[0].empty())) {
                        for (int i=0; i<liste_job_present_OA.size(); i++) {
                                if (liste_job_present_OA[i][1]==1 || etat_suivant_OA[i][1]==1) {
                                        cout << "ERREUR DANS OA" << endl;
                                }
                                //                                assert(liste_job_present_OA[i][1]!=1 || etat_suivant_OA[i][1]!=1);
                                liste_job_present_OA[i][1]--;
                                etat_suivant_OA[i][1]--;
                        }
                }
        }
        
        // @TODO Mise ajour de liste_job_present_OA Rajout job_arrivee
        // @TODO Mise a jour etat_suivant_OA Rajout job_arrivee
        vector<int> JobInt;
        for (int i=0; i<job_arrivee.size(); i++) {
                JobInt=job_arrivee[i];
                liste_job_present_OA.push_back(job_arrivee[i]);
                JobInt[0]=0;
                etat_suivant_OA.push_back(JobInt);
        }
        
        // @TODO tri des jobs dans les deux listes
        // @TODO Reordonne
        if (!(etat_suivant_OA.empty())) {
                if (!(etat_suivant_OA[0].empty())) {
                        sort(etat_suivant_OA.begin(),etat_suivant_OA.end(),cmp);
                        sort(liste_job_present_OA.begin(),liste_job_present_OA.end(),cmp);
                }
        }
        return 0;
}

// Choix de vitesse OA en fonction du WCET
int Calcul_Vitesse_OA_WCET(vector<vector<int> > & etat_suivant_OA, int & vitesse_OA, int max_size) {
        // @TODO Calcul du max des pentes sur le WCET
        vector<double> pente; pente.push_back(0);
        double somme=0;
        for (int i=0; i<etat_suivant_OA.size(); i++) {
                if (etat_suivant_OA[i][1]==0) {
                        cout << "ERREUR: " << etat_suivant_OA << endl;
                }
                somme += max_size - etat_suivant_OA[i][0];                          //  \sum (WCET - e_i)
                pente.push_back((double)somme/(double)(etat_suivant_OA[i][1]));     //  max( \sum (WCET - e_i) / d_i) ) \forall J_i
        }
        // @TODO Recupere la pente entière la plus grande
        vitesse_OA = ceil(*max_element(pente.begin(), pente.end()));
        return vitesse_OA;
}


// Fonction simulation finite case sur T itérations
// Simule les taches sur T : nouveau jobs ou pas selon la proba donner en parametre
// Determine letat courant et calcule letat suivant
// Entrée: Matrice_vitesse = matrice etat/vitesse de sortie de Backwadrd iteration
vector<int> simulation(int TailleMaxBuffer, vector<double> & Matrice_vitesse, int max_size, int max_deadline, int max_inter_arrival_time, vector<vector<vector<int> > > & state_space, int T, vector<vector<double> > & Stock_MDP_Lorch, vector<vector<double> > & Stock_MDP_OA, vector<int> & Vector_State_inter_arrival_time, int tailleListemin) {
        // Boucle sur les pas temps < T                                                                    => duration T
        // génére jobs selon l'aléa choisi au fur et à mesure du temps                                     => table de proba pour deadline, WCET et arrival time
        // détermine l'état suivant (vitesse, etat_prec, jobs arrivés)
        
        bool AffichageSimu = false;
        
        // DATAS statistique sur les jobs
        vector<double> inter_arrival_time_proba,deadline_proba,size_proba;
        Creation_proba(inter_arrival_time_proba,deadline_proba,size_proba, max_inter_arrival_time, max_size, max_deadline);
        
        vector<vector<int> > job_arrivee; // job_arrivee = ensemble des jobs qui arrivent en t.
        vector<vector<int> > liste_job_present_MDP, etat_suivant_MDP; int vitesse_MDP=0;
        vector<vector<int> > liste_job_present_Lorch, etat_suivant_Lorch;
        vector<vector<int> > liste_job_present_OA, etat_suivant_OA; int vitesse_OA=0;
        
        
        int index_etat_suivant_MDP =-1;
        vector<int> StockageVitesseMDP, StockageVitesseLorch,StockageVitesseOA;
        vector<vector<double> > StockageVitessesLorchParJobs;
        vector<vector<vector<int> > > Stockage_etat;
        vector<vector<vector<int> > > ListeJobArrivalSimulation, ListeEtatLorch,ListeEtatMDP, ListeEtatOA;
        vector<int> pasdeTemps;
        vector<double> LorchSpeedParJob;
        
        int t=0;
        int current_inter_arrival_time=0;
        int current_size, current_deadline;
        int nbre_job_executer = 0;
        int delai_k=0;
        
        /**** Partie simulation en fonction du temps ****/
        // @TODO Tirer l_i selon U(0,L)
        //        current_inter_arrival_time = unif_size(max_inter_arrival_time+1)-1; 
        //        current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba); // vecteur proba
        current_inter_arrival_time=0; // Pour débuter
        
        cout << "==> Inter-arrival-time courant: " << current_inter_arrival_time << endl;
        
        while (t<T-max_deadline) {
                cout << "========================================================== > TIME:   " << t << endl;
                cout << "==> InterArrivalTime courant Dois valoir 0: " << current_inter_arrival_time << endl;
                // @TODO création de de l'ensemble des jobs arrivée a(t)
                // @TODO Calcul du nombre de job executé
                int res=vitesse_MDP;
                nbre_job_executer=0;
                for (int i=0; i<liste_job_present_MDP.size(); i++) {
                        if (res-liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0]>=0) {
                                nbre_job_executer++;
                                res-=liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0];
                        } else {
                                break;
                        }
                }
                cout << "NBRE JOB EXEC ===> " << nbre_job_executer << endl;
                
                int nbre_jobs = (int)(etat_suivant_MDP.size()) - nbre_job_executer;
                cout << "NBRE de jobs " << nbre_jobs << endl;
                vector<vector<int> > Nouvel_Arrivee;
                bool JobEstArrive=false;
                if (nbre_jobs==TailleMaxBuffer) {
                        cout << "ERREUR" << endl;
                }
                while (current_inter_arrival_time==0 && nbre_jobs<TailleMaxBuffer) {
                        // @TODO Tirer c_i selon U(1,S)
                        //                        current_size = unif_size(max_size);
                        current_size = Simu_VectorProba(size_proba)+1; // vecteur proba
                        // @TODO Tirer d_i selon U(1,\Delta)
                        //                        current_deadline = unif_size(max_deadline);
                        current_deadline = Simu_VectorProba(deadline_proba)+1; // vecteur proba
                        // @TODO Rajouter le vecteur Nouvel Arrive
                        vector<int> Ajout; Ajout.push_back(current_size); Ajout.push_back(current_deadline);
                        job_arrivee.push_back(Ajout);
                        nbre_jobs++; // Rajout d'un job dans lespace detat
                        // @TODO Tirer l_i selon U(0,L)
                        //                        current_inter_arrival_time = unif_size(max_inter_arrival_time+1)-1;
                        current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba); // vecteur proba
                        
                        cout << "==> InterArrivalTime courant pour apres un arrivée: " << current_inter_arrival_time << endl;
                        if (!JobEstArrive) {
                                pasdeTemps.push_back(t);
                                JobEstArrive=true;
                        }
                        delai_k=0;
                }
                cout << "job_ arrivee: " << job_arrivee << endl;
                ListeJobArrivalSimulation.push_back(job_arrivee);
                cout << "VALEUR ECART: " << current_inter_arrival_time << endl;
                
                // @DONE choix de vitesse + Avancement de l'etat
                
                /******* MDP Case *******/
                // calcul_liste_etat_suivant() DETERMINE etat en fonction de la liste job_arrive => index etat suivant comprend it,at et vt-1
                index_etat_suivant_MDP = calcul_liste_etat_suivant(0,liste_job_present_MDP, job_arrivee, index_etat_suivant_MDP, state_space,  vitesse_MDP,Vector_State_inter_arrival_time);
                if (index_etat_suivant_MDP<state_space.size())
                        cout << "OUT OF RANGE " << endl;
                etat_suivant_MDP=state_space[index_etat_suivant_MDP];
                ListeEtatMDP.push_back(etat_suivant_MDP);
                // Pris en compte l'inter-arrival time mais seulement dans le calcul de vitesse, pas pour determiner letat suivant car non necessaire !
                cout << index_etat_suivant_MDP << " ET " << delai_k << " ET " << index_etat_suivant_MDP + delai_k*tailleListemin + t*(int)(state_space.size()) << endl;
                cout << "Taille MAx Matrice:  " << Matrice_vitesse.size() << endl;
                if (!(index_etat_suivant_MDP + delai_k*tailleListemin + t*(int)(state_space.size())<Matrice_vitesse.size())) {
                        cout << "OUT OF BOUND" << endl;
                }
                vitesse_MDP = Matrice_vitesse[index_etat_suivant_MDP + delai_k*tailleListemin + t*(int)(state_space.size())];
                cout << "===> VITESSE MDP: " << vitesse_MDP << endl;
                StockageVitesseMDP.push_back(vitesse_MDP);
                
                /******* LORCH Case *******/
                calcul_liste_etat_suivant_Lorch(liste_job_present_Lorch, job_arrivee, etat_suivant_Lorch, LorchSpeedParJob);
                ListeEtatLorch.push_back(etat_suivant_Lorch);
                LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
                Calcul_Vitesse_Lorch_WCET(etat_suivant_Lorch, LorchSpeedParJob, max_size, size_proba);
                StockageVitessesLorchParJobs.push_back(LorchSpeedParJob);
                
                /******* (OA) Case *******/
                // @TODO implementer Chnagement detat OA avec connaissance vitesse et job_arrive
                calcul_liste_etat_suivant_OA(liste_job_present_OA, job_arrivee, etat_suivant_OA, vitesse_OA);
                ListeEtatOA.push_back(etat_suivant_OA);
                // @TODO A parti de letat calcul la vitesse
                Calcul_Vitesse_OA_WCET(etat_suivant_OA, vitesse_OA, max_size);
                StockageVitesseOA.push_back(vitesse_OA);
                
                
                cout << "============ MDP ============" << endl;
                cout << liste_job_present_MDP << endl;
                cout << etat_suivant_MDP << endl;
                cout << "speed: " << vitesse_MDP << endl;
                cout << "============ Lorch ============" << endl;
                cout << liste_job_present_Lorch << endl;
                cout << etat_suivant_Lorch << endl;
                cout << "speed: " << LorchSpeedParJob << endl;
                cout << "============ OA ============" << endl;
                cout << liste_job_present_OA << endl;
                cout << etat_suivant_OA << endl;
                cout << "speed: " << vitesse_OA << endl;
                
                job_arrivee.erase(job_arrivee.begin(), job_arrivee.end());
                
                // @TODO Passe à l'état suivant si on atteint le nombre de jobs max
                // On tire l_i selon la loi restante adapté à U(1,L)
                // @TODO => Trouver une meilleure solution
                while (current_inter_arrival_time == 0) {
                        // @TODO Tirer l_i selon U(1,L)
                        //                        current_inter_arrival_time = unif_size(max_inter_arrival_time); // @TODO Faire la fonction simulation pour un vecteur de proba
                        current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba);
                        cout << "==> InterArrivalTime courant pour apres un arrivée bloqué: " << current_inter_arrival_time << endl;
                }
                
                // @TODO Descente du temps en fonction de l'inter-arrival time
                // PAS de temps ou on ne fait rien arriver
                // MAIS @DONE choix de vitesse + Avancement de l'etat
                delai_k=0;
                while (current_inter_arrival_time>1 && t<T-max_deadline) {
                        t++;
                        current_inter_arrival_time--;
                        delai_k++;
                        cout << "SAUT d'un ETAT " << endl;
                        cout << "========================================================== > TIME:   " << t << endl;
                        cout << "==> InterArrivalTime courant deduite du fait que l'InterArrivalTime est fort: " << current_inter_arrival_time << endl;
                        
                        /******* MDP Case *******/
                        // calcul_liste_etat_suivant() DETERMINE etat en fonction de la liste job_arrive => index etat suivant comprend it,at et vt-1
                        index_etat_suivant_MDP = calcul_liste_etat_suivant(0,liste_job_present_MDP, job_arrivee, index_etat_suivant_MDP, state_space,  vitesse_MDP,Vector_State_inter_arrival_time);
                        etat_suivant_MDP=state_space[index_etat_suivant_MDP];
                        ListeEtatMDP.push_back(etat_suivant_MDP);
                        cout << index_etat_suivant_MDP << " ET " << delai_k << " ET " <<index_etat_suivant_MDP + delai_k*tailleListemin +t*(int)(state_space.size()) << endl;
                        if (!(index_etat_suivant_MDP + delai_k*tailleListemin + t*(int)(state_space.size())<Matrice_vitesse.size()))
                                cout << "OUT OF BOUND" << endl;
                        // Pris en compte l'inter-arrival time mais seulement dans le calcul de vitesse, pas pour determiner letat suivant car non necessaire !
                        vitesse_MDP = Matrice_vitesse[index_etat_suivant_MDP + delai_k*tailleListemin + t*(int)(state_space.size())];
                        cout << "VITESSE MDP: " << vitesse_MDP << endl;
                        StockageVitesseMDP.push_back(vitesse_MDP);
                        
                        /******* LORCH Case *******/
                        calcul_liste_etat_suivant_Lorch(liste_job_present_Lorch, job_arrivee, etat_suivant_Lorch, LorchSpeedParJob);
                        ListeEtatLorch.push_back(etat_suivant_Lorch);
                        LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
                        Calcul_Vitesse_Lorch_WCET(etat_suivant_Lorch, LorchSpeedParJob, max_size, size_proba);
                        StockageVitessesLorchParJobs.push_back(LorchSpeedParJob);
                        
                        /******* OA Case *******/
                        calcul_liste_etat_suivant_OA(liste_job_present_OA, job_arrivee, etat_suivant_OA, vitesse_OA);
                        ListeEtatOA.push_back(etat_suivant_OA);
                        Calcul_Vitesse_OA_WCET(etat_suivant_OA, vitesse_OA, max_size);
                        StockageVitesseOA.push_back(vitesse_OA);
                        
                        cout << "============ MDP ============" << endl;
                        cout << liste_job_present_MDP << endl;
                        cout << etat_suivant_MDP << endl;
                        cout << "speed: " << vitesse_MDP << endl;
                        cout << "============ Lorch ============" << endl;
                        cout << liste_job_present_Lorch << endl;
                        cout << etat_suivant_Lorch << endl;
                        cout << "speed: " << LorchSpeedParJob << endl;
                        cout << "============ OA ============" << endl;
                        cout << liste_job_present_OA << endl;
                        cout << etat_suivant_OA << endl;
                        cout << "speed: " << vitesse_OA << endl;
                }
                t++;
                current_inter_arrival_time--;
                delai_k++;
        }
        vector<int> Somme_StockageVitessesLorchParJobs;
        for (int i=0; i<StockageVitessesLorchParJobs.size(); i++) {
                if (!(StockageVitessesLorchParJobs[i].empty())) {
                        int somme_int = 0;
                        for (int j=0; j<StockageVitessesLorchParJobs[i].size(); j++) {
                                somme_int+=StockageVitessesLorchParJobs[i][j];
                        }
                        Somme_StockageVitessesLorchParJobs.push_back(somme_int);
                } else {
                        Somme_StockageVitessesLorchParJobs.push_back(0);
                }
        }
        cout << "Somme_StockageVitessesLorchParJobs " << Somme_StockageVitessesLorchParJobs << endl;
        
        cout << "pasDeTemps: " << pasdeTemps << endl;
        /*** MDP ***/
        int somme=0;
        int value_energie_MDP=0;
        for (int i=0; i<StockageVitesseMDP.size(); i++) {
                somme+=StockageVitesseMDP[i];
                value_energie_MDP+=pow(StockageVitesseMDP[i],3);
        }
        cout << "tot: " << somme << "  Et energie: " << value_energie_MDP << "   Stockage vitesse MDP  : " << StockageVitesseMDP << endl;
        /*** LORCH ***/
        somme=0;int value_energie_Lorch=0;
        for (int i=0; i<Somme_StockageVitessesLorchParJobs.size(); i++) {
                somme+=Somme_StockageVitessesLorchParJobs[i];
                value_energie_Lorch+=pow(Somme_StockageVitessesLorchParJobs[i],3);
        }
        cout << "tot: " << somme << "  Et energie: " << value_energie_Lorch << "   Stockage vitesse LORCH: " << Somme_StockageVitessesLorchParJobs << endl;
        cout << "stockage vitesse Lorch par pas de tmeps" << endl;
        Affiche(StockageVitessesLorchParJobs); cout << endl;
        /*** OA ***/
        int somme_OA=0;
        int value_energie_OA=0;
        for (int i=0; i<StockageVitesseOA.size(); i++) {
                somme_OA+=StockageVitesseOA[i];
                value_energie_OA+=pow(StockageVitesseOA[i],3);
        }
        cout << "tot: " << somme_OA << "  Et energie: " << value_energie_OA << "   Stockage vitesse OA  : " << StockageVitesseOA << endl;
        /*** Qté de Job_Arrivée ***/
        somme=0;
        for (int i=0; i<ListeJobArrivalSimulation.size(); i++) {
                for (int j=0; j<ListeJobArrivalSimulation[i].size(); j++) {
                        somme+=ListeJobArrivalSimulation[i][j][0];
                }
        }
        cout << "Quantité de travail:  " << somme << "   Et les jobs vus sont: " << endl;
        /*** Affichage des jobs, etats LORCH, MDP, OA ***/
        if (AffichageSimu) {
                int iter=0;
                for (int i=0; i<ListeEtatLorch.size(); i++) {
                        /*** Liste Jobs ***/
                        cout << "++++++++++++++++++++++++++++++++++++++++++ " << i << endl;
                        if (pasdeTemps[iter]==i) {
                                cout << "ARRIVEE: ";
                                if (iter<ListeJobArrivalSimulation.size()) {
                                        Affiche(ListeJobArrivalSimulation[iter]);
                                        iter++;
                                } else {
                                        cout << "OUT" << endl;
                                }
                                cout << endl;
                        } else {
                                cout << "NONE = Rien d'arrivée à ce pas de temps" << endl;
                        }
                        /*** LORCH Etat + vitesse ***/
                        cout << "======== LORCH ========== " << StockageVitessesLorchParJobs[i] << endl;
                        if (i<ListeEtatLorch.size()) {
                                Affiche(ListeEtatLorch[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                        /*** MDP Etat + vitesse ***/
                        cout << "======== MDP ========== " << StockageVitesseMDP[i] << endl;
                        if (i<ListeEtatMDP.size()) {
                                cout << " Delai:  " << Vector_State_inter_arrival_time[i] << endl;;
                                Affiche(ListeEtatMDP[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                        /*** OA Etat + vitesse ***/
                        cout << "======== OA ========== " << StockageVitesseOA[i] << endl;
                        if (i<ListeEtatOA.size()) {
                                Affiche(ListeEtatOA[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                }
        }
        // @TODO Faire affichage du cout
        // @TODO apple à la fonction de dessin en R
        cout << "Energie Diff Lorch - MDP  :  " << value_energie_Lorch-value_energie_MDP << endl;
        cout << "Energie Diff OA - MDP     :  " << value_energie_OA-value_energie_MDP << endl;
        vector<double> MDP_Lorch,MDP_OA;
        MDP_Lorch.push_back(value_energie_MDP); MDP_Lorch.push_back(value_energie_Lorch);
        MDP_OA.push_back(value_energie_MDP); MDP_OA.push_back(value_energie_OA);
        Stock_MDP_Lorch.push_back(MDP_Lorch);
        Stock_MDP_OA.push_back(MDP_OA);
        vector <int> valueDiffSortie; valueDiffSortie.push_back(value_energie_Lorch-value_energie_MDP);valueDiffSortie.push_back(value_energie_OA-value_energie_MDP);
        return valueDiffSortie;
}




vector<int> simulationVI(vector<double> & size_proba, vector<double> & deadline_proba, vector<double> inter_arrival_time_proba, vector<vector<double> > & typeDeadline, vector<vector<double> > & Monitoring_vector,int max_speeds,int TailleMaxBuffer, vector<double> & Matrice_vitesse, int max_size, int max_deadline, int max_inter_arrival_time, vector<vector<vector<int> > > & state_space, int T, vector<vector<double> > & Stock_MDP_Lorch, vector<vector<double> > & Stock_MDP_OA, vector<int> & Vector_State_inter_arrival_time, int tailleListemin) {
        // Boucle sur les pas temps < T                                                                    => duration T
        // génére jobs selon l'aléa choisi au fur et à mesure du temps                                     => table de proba pour deadline, WCET et arrival time
        // détermine l'état suivant (vitesse, etat_prec, jobs arrivés)
        
        bool AffichageSimu = false;
        //        // DATAS statistique sur les jobs
        //        vector<double> inter_arrival_time_proba,deadline_proba,size_proba;
        //        Creation_proba(inter_arrival_time_proba,deadline_proba,size_proba, max_inter_arrival_time, max_size, max_deadline);
        
        vector<vector<int> > job_arrivee; // job_arrivee = ensemble des jobs qui arrivent en t.
        vector<vector<int> > liste_job_present_MDP, etat_suivant_MDP; int vitesse_MDP=0;
        vector<vector<int> > liste_job_present_LORCH, etat_suivant_LORCH;
        vector<vector<int> > liste_job_present_OA, etat_suivant_OA; int vitesse_OA=0;
        
        vector<int> StockageVitesseMDP, StockageVitesseLORCH,StockageVitesseOA,pasdeTemps;
        vector<vector<double> > StockageVitessesLorchParJobs;
        vector<vector<vector<int> > > ListeJobArrivalSimulation, ListeEtatLORCH,ListeEtatMDP, ListeEtatOA, Stockage_etat;
        vector<double> LorchSpeedParJob;
        
        int current_size, current_deadline,current_inter_arrival_time, Nbre_job;
        int nbre_job_executer = 0, delai_k=0;
        int t=0;
        int index_etat_suivant_MDP = tailleListemin-1;
        etat_suivant_MDP=state_space[index_etat_suivant_MDP];
        
        if (Affichage_sortie)
                cout << "etat de départ: " << state_space[index_etat_suivant_MDP] << endl;
        
        /**** Partie simulation en fonction du temps ****/
        // @DONE Simulation du premier Inter-arrival-time: inter_arrival_time_proba_Normaliser, distribution normalisé dans le cas particulier
        vector<double> inter_arrival_time_proba_Normaliser (inter_arrival_time_proba.begin()+1,inter_arrival_time_proba.end());
        inter_arrival_time_proba_Normaliser=inter_arrival_time_proba_Normaliser/(1-inter_arrival_time_proba[0]);
        current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba_Normaliser);
        if (Affichage_sortie)
                cout << "INTER_ARRIVAL_TIME Au Depart: " << current_inter_arrival_time << endl;
        
        vector<vector<int> > PremArriv;
        ListeJobArrivalSimulation.push_back(PremArriv);
        ListeEtatMDP.push_back(PremArriv); ListeEtatLORCH.push_back(PremArriv); ListeEtatOA.push_back(PremArriv);
        
        bool Monitoring=false;
        vector<double> Nbre_job_arrivParXMinB(4,0); // 0,1,2,3
        vector<vector<double> > Nbre_job_arriv;
        for (int i=0; i<4; i++)
                Nbre_job_arriv.push_back(Nbre_job_arrivParXMinB);
        
        int delai_k_MDP=0, current_inter_arrival_time_MDP, Nbre_job_MDP;
        current_inter_arrival_time_MDP = Simu_VectorProba(inter_arrival_time_proba_Normaliser);
        vector<vector<int> > job_arrivee_MDP;
        vector<vector<vector<int> > > ListeJobArrivalSimulation_MDP;
        
        int delai_k_OA=0, current_inter_arrival_time_OA, Nbre_job_OA;
        current_inter_arrival_time_OA = Simu_VectorProba(inter_arrival_time_proba_Normaliser);
        vector<vector<int> > job_arrivee_OA;
        vector<vector<vector<int> > > ListeJobArrivalSimulation_OA;
        
        int delai_k_LORCH=0, current_inter_arrival_time_LORCH, Nbre_job_LORCH;
        current_inter_arrival_time_LORCH = Simu_VectorProba(inter_arrival_time_proba_Normaliser);
        vector<vector<int> > job_arrivee_LORCH;
        vector<vector<vector<int> > > ListeJobArrivalSimulation_LORCH;
        
        /*** @TODO Faire la simulation différente dans chacun des deux cas OA et MDP, la distriubtion sera la même, mais pas les jobs arrivées ***/
        /*** Une simu pour OA et une simu pour MDP et comparaison en moyenne sur un horizon de temps long ***/
        while (t<T) {
                /**************************************************** PARTIE MDP ****************************************************/
                if (index_etat_suivant_MDP + delai_k_MDP*tailleListemin>Matrice_vitesse.size())
                        cout << "OUT OF BOUND" << endl;
                vitesse_MDP = Matrice_vitesse[index_etat_suivant_MDP + delai_k_MDP*tailleListemin];
                StockageVitesseMDP.push_back(vitesse_MDP);
                // @TODO création de de l'ensemble des jobs arrivée a(t)
                // @TODO Calcul du nombre de job executé
                int res_MDP=vitesse_MDP;
                int nbre_job_executer_MDP=0;
                for (int i=0; i<liste_job_present_MDP.size(); i++) {
                        if (res_MDP-liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0]>=0) {
                                nbre_job_executer_MDP++;
                                res_MDP-=liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0];
                        } else {
                                break;
                        }
                }
                delai_k_MDP = current_inter_arrival_time_MDP;
                Nbre_job_MDP = (int)(etat_suivant_MDP.size())- nbre_job_executer_MDP;
                // Création du job A(t+1)
                while (current_inter_arrival_time_MDP==0 && Nbre_job_MDP<TailleMaxBuffer) {
                        // @TODO A supprimer si on veut garder les valeurs des probas
                        //                        if (t<T-max_deadline) {
                        current_size = Simu_VectorProba(size_proba)+1;
                        current_deadline = Simu_VectorProba(deadline_proba)+1;
                        vector<int> Ajout; Ajout.push_back(current_size); Ajout.push_back(current_deadline);
                        job_arrivee_MDP.push_back(Ajout);
                        //                        }
                        Nbre_job_MDP++;
                        current_inter_arrival_time_MDP = Simu_VectorProba(inter_arrival_time_proba);
                        //                        if (Affichage_sortie)
                        //                                cout << "INTER_ARRIVAL_TIME Pdt larrivée: " << current_inter_arrival_time << endl;
                }
                ListeJobArrivalSimulation_MDP.push_back(job_arrivee_MDP);
                // Avec vitesse en t et A(t+1)- : détermine letat suivant en t+1
                index_etat_suivant_MDP = calcul_liste_etat_suivant(0,liste_job_present_MDP, job_arrivee_MDP, index_etat_suivant_MDP, state_space,  vitesse_MDP,Vector_State_inter_arrival_time);
                if (index_etat_suivant_MDP>state_space.size())
                        cout << "OUT OF BOUNDED" << endl;
                etat_suivant_MDP=state_space[index_etat_suivant_MDP];
                ListeEtatMDP.push_back(etat_suivant_MDP);
                //                /******* LORCH Case *******/
                //                LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
                //                Calcul_Vitesse_Lorch_WCET(etat_suivant_LORCH, LorchSpeedParJob, max_size);
                //                if (etat_suivant_LORCH.empty())
                //                        LorchSpeedParJob.push_back(0); //Cas spécifique zéro job présent
                //                StockageVitessesLorchParJobs.push_back(LorchSpeedParJob); // @TODO prendre en compte le max_speeds
                //                calcul_liste_etat_suivant_Lorch(liste_job_present_LORCH, job_arrivee_MDP, etat_suivant_LORCH, LorchSpeedParJob);
                //                ListeEtatLORCH.push_back(etat_suivant_LORCH);
                
                if (current_inter_arrival_time_MDP==0) {
                        current_inter_arrival_time_MDP = Simu_VectorProba(inter_arrival_time_proba_Normaliser)+1;
                }
                job_arrivee_MDP.erase(job_arrivee_MDP.begin(), job_arrivee_MDP.end());
                
                current_inter_arrival_time_MDP--;
                
                /**************************************************** PARTIE OA ****************************************************/
                Calcul_Vitesse_OA_WCET(etat_suivant_OA, vitesse_OA, max_size);
                //  vitesse_OA=min(max_speeds,vitesse_OA); // @TODO prend en compte la max_speeds
                StockageVitesseOA.push_back(vitesse_OA);
                // @TODO Calcul du nombre de job executé
                int res_OA=vitesse_OA;
                int nbre_job_executer_OA=0;
                for (int i=0; i<liste_job_present_OA.size(); i++) {
                        if (res_OA-liste_job_present_OA[i][0]+etat_suivant_OA[i][0]>=0) {
                                nbre_job_executer_OA++;
                                res_OA-=liste_job_present_OA[i][0]+etat_suivant_OA[i][0];
                        } else {
                                break;
                        }
                }
                delai_k_OA=current_inter_arrival_time_OA;
                Nbre_job_OA=(int)(etat_suivant_OA.size())- nbre_job_executer_OA;
                // Création du job A(t+1)
                while (current_inter_arrival_time_OA==0 && Nbre_job_OA<TailleMaxBuffer) {
                        // @TODO A supprimer si on veut garder les valeurs des probas
                        //                        if (t<T-max_deadline) {
                        current_size = Simu_VectorProba(size_proba)+1;
                        current_deadline = Simu_VectorProba(deadline_proba)+1;
                        vector<int> Ajout; Ajout.push_back(current_size); Ajout.push_back(current_deadline);
                        job_arrivee_OA.push_back(Ajout);
                        //                        }
                        Nbre_job_OA++;
                        current_inter_arrival_time_OA = Simu_VectorProba(inter_arrival_time_proba);
                        //                        if (Affichage_sortie)
                        //                                cout << "INTER_ARRIVAL_TIME Pdt larrivée: " << current_inter_arrival_time << endl;
                }
                ListeJobArrivalSimulation_OA.push_back(job_arrivee_OA);
                // @DONE implementer Changement detat OA avec connaissance vitesse et job_arrive
                calcul_liste_etat_suivant_OA(liste_job_present_OA, job_arrivee_OA, etat_suivant_OA, vitesse_OA);
                ListeEtatOA.push_back(etat_suivant_OA);
                if (current_inter_arrival_time_OA==0) {
                        current_inter_arrival_time_OA = Simu_VectorProba(inter_arrival_time_proba_Normaliser)+1;
                }
                job_arrivee_OA.erase(job_arrivee_OA.begin(), job_arrivee_OA.end());
                
                current_inter_arrival_time_OA--;
                
                /**************************************************** PARTIE LORCH ****************************************************/
                LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
                Calcul_Vitesse_Lorch_WCET(etat_suivant_LORCH, LorchSpeedParJob, max_size,size_proba);
                if (etat_suivant_LORCH.empty())
                        LorchSpeedParJob.push_back(0); //Cas spécifique zéro job présent
                StockageVitessesLorchParJobs.push_back(LorchSpeedParJob); // @TODO prendre en compte le max_speeds
                // @TODO faire une fonciton qui dénombre les jobs executés avec les vitesses données
                int nbre_job_executer_LORCH=0;
                if (!(etat_suivant_LORCH.empty())) {
                        for (int i=0; i<LorchSpeedParJob.size(); i++) {
                                if (LorchSpeedParJob[i]-liste_job_present_LORCH[i][0]+etat_suivant_LORCH[i][0]>=0) {
                                        nbre_job_executer_LORCH++;
                                }
                        }
                } else {
                        nbre_job_executer_LORCH=0;
                }
                delai_k_LORCH=current_inter_arrival_time_LORCH;
                Nbre_job_LORCH=(int)(etat_suivant_LORCH.size())- nbre_job_executer_LORCH;
                // Création du job A(t+1)
                while (current_inter_arrival_time_LORCH==0 && Nbre_job_LORCH<TailleMaxBuffer) {
                        // @TODO A supprimer si on veut garder les valeurs des probas
                        //                        if (t<T-max_deadline) {
                        current_size = Simu_VectorProba(size_proba)+1;
                        current_deadline = Simu_VectorProba(deadline_proba)+1;
                        vector<int> Ajout; Ajout.push_back(current_size); Ajout.push_back(current_deadline);
                        job_arrivee_LORCH.push_back(Ajout);
                        //                        }
                        Nbre_job_LORCH++;
                        current_inter_arrival_time_LORCH = Simu_VectorProba(inter_arrival_time_proba);
                        //                        if (Affichage_sortie)
                        //                                cout << "INTER_ARRIVAL_TIME Pdt larrivée: " << current_inter_arrival_time << endl;
                }
                ListeJobArrivalSimulation_LORCH.push_back(job_arrivee_LORCH);
                
                calcul_liste_etat_suivant_Lorch(liste_job_present_LORCH, job_arrivee_LORCH, etat_suivant_LORCH, LorchSpeedParJob);
                ListeEtatLORCH.push_back(etat_suivant_LORCH);
                
                if (current_inter_arrival_time_LORCH==0) {
                        current_inter_arrival_time_LORCH = Simu_VectorProba(inter_arrival_time_proba_Normaliser)+1;
                }
                job_arrivee_LORCH.erase(job_arrivee_LORCH.begin(), job_arrivee_LORCH.end());
                
                current_inter_arrival_time_LORCH--;
                
                if (AffichageSimu) {
                        cout << "============ MDP en t+1 ============" << endl;
                        cout << liste_job_present_MDP << endl;
                        cout << etat_suivant_MDP << endl;
                        cout << "Vitesse en t: " << vitesse_MDP << endl;
                        cout << "============ Lorch en t+1 ============" << endl;
                        cout << liste_job_present_LORCH << endl;
                        cout << etat_suivant_LORCH << endl;
                        cout << "Vitesse en t: " << LorchSpeedParJob << endl;
                        cout << "============ OA en t+1 ============" << endl;
                        cout << liste_job_present_OA << endl;
                        cout << etat_suivant_OA << endl;
                        cout << "Vitesse en t: " << vitesse_OA << endl;
                }
                pasdeTemps.push_back(t);
                t++;
                
        }
        
        
        //        if (Affichage_sortie) {
        //                cout << "========================================================== > TIME:   " << t << endl;
        //                cout << "==> InterArrivalTime courant Dois valoir 0: " << current_inter_arrival_time << endl;
        //        }
        //        // @TODO création de de l'ensemble des jobs arrivée a(t)
        //        // @TODO Calcul du nombre de job executé
        //        int res=vitesse_MDP;
        //        nbre_job_executer=0;
        //        for (int i=0; i<liste_job_present_MDP.size(); i++) {
        //                if (res-liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0]>=0) {
        //                        nbre_job_executer++;
        //                        res-=liste_job_present_MDP[i][0]+etat_suivant_MDP[i][0];
        //                } else {
        //                        break;
        //                }
        //        }
        //        if (Affichage_sortie)
        //                cout << "NBRE JOB EXEC ===> " << nbre_job_executer << endl;
        //        
        //        int nbre_jobs = (int)(etat_suivant_MDP.size()) - nbre_job_executer;
        //        if (Affichage_sortie)
        //                cout << "NBRE de jobs " << nbre_jobs << endl;
        //        vector<vector<int> > Nouvel_Arrivee;
        //        bool JobEstArrive=false;
        //        if (nbre_jobs==TailleMaxBuffer) {
        //                if (Affichage_sortie)
        //                        cout << "ERREUR" << endl;
        //        }
        //        while (current_inter_arrival_time==0 && nbre_jobs<TailleMaxBuffer) {
        //                // @TODO Tirer c_i selon U(1,S)
        //                //                        current_size = unif_size(max_size);
        //                current_size = Simu_VectorProba(size_proba)+1; // vecteur proba
        //                // @TODO Tirer d_i selon U(1,\Delta)
        //                //                        current_deadline = unif_size(max_deadline);
        //                current_deadline = Simu_VectorProba(deadline_proba)+1; // vecteur proba
        //                // @TODO Rajouter le vecteur Nouvel Arrive
        //                vector<int> Ajout; Ajout.push_back(current_size); Ajout.push_back(current_deadline);
        //                job_arrivee.push_back(Ajout);
        //                nbre_jobs++; // Rajout d'un job dans lespace detat
        //                // @TODO Tirer l_i selon U(0,L)
        //                //                        current_inter_arrival_time = unif_size(max_inter_arrival_time+1)-1;
        //                current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba); // vecteur proba
        //                if (Affichage_sortie)
        //                        cout << "==> InterArrivalTime courant pour apres un arrivée: " << current_inter_arrival_time << endl;
        //                if (!JobEstArrive) {
        //                        pasdeTemps.push_back(t);
        //                        JobEstArrive=true;
        //                }
        //                delai_k=0;
        //        }
        //        if (Affichage_sortie)
        //                cout << "job_ arrivee: " << job_arrivee << endl;
        //        ListeJobArrivalSimulation.push_back(job_arrivee);
        //        if (Affichage_sortie)
        //                cout << "VALEUR ECART: " << current_inter_arrival_time << endl;
        //        
        //        // @DONE choix de vitesse + Avancement de l'etat
        //        
        //        /******* MDP Case *******/
        //        // calcul_liste_etat_suivant() DETERMINE etat en fonction de la liste job_arrive => index etat suivant comprend it,at et vt-1
        //        index_etat_suivant_MDP = calcul_liste_etat_suivant(0,liste_job_present_MDP, job_arrivee, index_etat_suivant_MDP, state_space,  vitesse_MDP,Vector_State_inter_arrival_time);
        //        etat_suivant_MDP=state_space[index_etat_suivant_MDP];
        //        ListeEtatMDP.push_back(etat_suivant_MDP);
        //        // Pris en compte l'inter-arrival time mais seulement dans le calcul de vitesse, pas pour determiner letat suivant car non necessaire
        //        if (Affichage_sortie)
        //                cout << index_etat_suivant_MDP << " ET " << delai_k << " ET " << index_etat_suivant_MDP + delai_k*tailleListemin << " <  " << Matrice_vitesse.size() << endl;
        //        // @TODO Rajout du cas delai_k==L sans arriver de tache (\ie cas ==3)
        //        if (delai_k==max_inter_arrival_time) {
        //                delai_k=0;
        //        }
        //        if (!(index_etat_suivant_MDP + delai_k*tailleListemin < Matrice_vitesse.size())) {
        //                cout << "OUT OF BOUND" << endl;
        //        }
        //        vitesse_MDP = Matrice_vitesse[index_etat_suivant_MDP + delai_k*tailleListemin];
        //        if (Affichage_sortie)
        //                cout << "===> VITESSE MDP: " << vitesse_MDP << endl;
        //        StockageVitesseMDP.push_back(vitesse_MDP);
        //        
        //        /******* LORCH Case *******/
        //        calcul_liste_etat_suivant_Lorch(liste_job_present_Lorch, job_arrivee, etat_suivant_Lorch, LorchSpeedParJob);
        //        ListeEtatLorch.push_back(etat_suivant_Lorch);
        //        LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
        //        Calcul_Vitesse_Lorch_WCET(etat_suivant_Lorch, LorchSpeedParJob, max_size);
        //        // @TODO prend en compte le max_speeds
        //        StockageVitessesLorchParJobs.push_back(LorchSpeedParJob);
        //        
        //        /******* (OA) Case *******/
        //        // @TODO implementer Chnagement detat OA avec connaissance vitesse et job_arrive
        //        calcul_liste_etat_suivant_OA(liste_job_present_OA, job_arrivee, etat_suivant_OA, vitesse_OA);
        //        ListeEtatOA.push_back(etat_suivant_OA);
        //        // @TODO A parti de letat calcul la vitesse
        //        Calcul_Vitesse_OA_WCET(etat_suivant_OA, vitesse_OA, max_size);
        //        //                // @TODO prend en compte la max_speeds
        //        //                vitesse_OA=min(max_speeds,vitesse_OA);
        //        StockageVitesseOA.push_back(vitesse_OA);
        //        
        //        if (Affichage_sortie) {
        //                cout << "============ MDP ============" << endl;
        //                cout << liste_job_present_MDP << endl;
        //                cout << etat_suivant_MDP << endl;
        //                cout << "speed: " << vitesse_MDP << endl;
        //                cout << "============ Lorch ============" << endl;
        //                cout << liste_job_present_Lorch << endl;
        //                cout << etat_suivant_Lorch << endl;
        //                cout << "speed: " << LorchSpeedParJob << endl;
        //                cout << "============ OA ============" << endl;
        //                cout << liste_job_present_OA << endl;
        //                cout << etat_suivant_OA << endl;
        //                cout << "speed: " << vitesse_OA << endl;
        //        }
        //        job_arrivee.erase(job_arrivee.begin(), job_arrivee.end());
        //        
        //        // @TODO Passe à l'état suivant si on atteint le nombre de jobs max
        //        // On tire l_i selon la loi restante adapté à U(1,L)
        //        // @TODO => Trouver une meilleure solution
        //        while (current_inter_arrival_time == 0) {
        //                // @TODO Tirer l_i selon U(1,L) current_inter_arrival_time = unif_size(max_inter_arrival_time); // @TODO Faire la fonction simulation pour un vecteur de proba
        //                current_inter_arrival_time = Simu_VectorProba(inter_arrival_time_proba_Normaliser);
        //                if (Affichage_sortie)
        //                        cout << "==> InterArrivalTime courant pour apres un arrivée bloqué: " << current_inter_arrival_time << endl;
        //        }
        //        
        //        // @TODO Descente du temps en fonction de l'inter-arrival time
        //        // PAS de temps ou on ne fait rien arriver
        //        // MAIS @DONE choix de vitesse + Avancement de l'etat
        //        delai_k=0;
        //        while (current_inter_arrival_time>1 && t<T) {
        //                t++;
        //                current_inter_arrival_time--;
        //                delai_k++;
        //                if (Affichage_sortie) {
        //                        cout << "SAUT d'un ETAT " << endl;
        //                        cout << "========================================================== > TIME:   " << t << endl;
        //                        cout << "==> InterArrivalTime courant deduite du fait que l'InterArrivalTime est fort: " << current_inter_arrival_time << endl;
        //                }
        //                /******* MDP Case *******/
        //                // calcul_liste_etat_suivant() DETERMINE etat en fonction de la liste job_arrive => index etat suivant comprend it,at et vt-1
        //                index_etat_suivant_MDP = calcul_liste_etat_suivant(0,liste_job_present_MDP, job_arrivee, index_etat_suivant_MDP, state_space,  vitesse_MDP,Vector_State_inter_arrival_time);
        //                etat_suivant_MDP=state_space[index_etat_suivant_MDP];
        //                ListeEtatMDP.push_back(etat_suivant_MDP);
        //                cout << index_etat_suivant_MDP << " ET " << delai_k << " ET " << index_etat_suivant_MDP + delai_k*tailleListemin << " <  " << Matrice_vitesse.size() << endl;
        //                // @TODO Rajout du cas delai_k==L sans arriver de tache (\ie cas ==3)
        //                if (delai_k==max_inter_arrival_time) {
        //                        delai_k=0;
        //                }
        //                if (!(index_etat_suivant_MDP + delai_k*tailleListemin < Matrice_vitesse.size()))
        //                        cout << "OUT OF BOUND" << endl;
        //                // Pris en compte l'inter-arrival time mais seulement dans le calcul de vitesse, pas pour determiner letat suivant car non necessaire !
        //                vitesse_MDP = Matrice_vitesse[index_etat_suivant_MDP + delai_k*tailleListemin];
        //                if (Affichage_sortie)
        //                        cout << "VITESSE MDP: " << vitesse_MDP << endl;
        //                StockageVitesseMDP.push_back(vitesse_MDP);
        //                
        //                /******* LORCH Case *******/
        //                calcul_liste_etat_suivant_Lorch(liste_job_present_Lorch, job_arrivee, etat_suivant_Lorch, LorchSpeedParJob);
        //                ListeEtatLorch.push_back(etat_suivant_Lorch);
        //                LorchSpeedParJob.erase(LorchSpeedParJob.begin(), LorchSpeedParJob.end());
        //                Calcul_Vitesse_Lorch_WCET(etat_suivant_Lorch, LorchSpeedParJob, max_size);
        //                StockageVitessesLorchParJobs.push_back(LorchSpeedParJob);
        //                
        //                /******* OA Case *******/
        //                calcul_liste_etat_suivant_OA(liste_job_present_OA, job_arrivee, etat_suivant_OA, vitesse_OA);
        //                ListeEtatOA.push_back(etat_suivant_OA);
        //                Calcul_Vitesse_OA_WCET(etat_suivant_OA, vitesse_OA, max_size);
        //                StockageVitesseOA.push_back(vitesse_OA);
        //                if (Affichage_sortie) {
        //                        cout << "============ MDP ============" << endl;
        //                        cout << liste_job_present_MDP << endl;
        //                        cout << etat_suivant_MDP << endl;
        //                        cout << "speed: " << vitesse_MDP << endl;
        //                        cout << "============ Lorch ============" << endl;
        //                        cout << liste_job_present_Lorch << endl;
        //                        cout << etat_suivant_Lorch << endl;
        //                        cout << "speed: " << LorchSpeedParJob << endl;
        //                        cout << "============ OA ============" << endl;
        //                        cout << liste_job_present_OA << endl;
        //                        cout << etat_suivant_OA << endl;
        //                        cout << "speed: " << vitesse_OA << endl;
        //                }
        //        }
        //        t++;
        //        current_inter_arrival_time--;
        //        delai_k++;
        //}
        
        /************ SIMULATION TERMINEE ===> RESULTATS ************/
        vector<int> Somme_StockageVitessesLorchParJobs;
        for (int i=0; i<StockageVitessesLorchParJobs.size(); i++) {
                if (!(StockageVitessesLorchParJobs[i].empty())) {
                        int somme_int = 0;
                        for (int j=0; j<StockageVitessesLorchParJobs[i].size(); j++) {
                                somme_int+=StockageVitessesLorchParJobs[i][j];
                        }
                        Somme_StockageVitessesLorchParJobs.push_back(somme_int);
                } else {
                        Somme_StockageVitessesLorchParJobs.push_back(0);
                }
        }
        if (Affichage_sortie)
                cout << "Somme_StockageVitessesLorchParJobs " << Somme_StockageVitessesLorchParJobs << endl;
        if (Affichage_sortie)
                cout << "pasDeTemps: " << pasdeTemps << endl;
        /*** MDP ***/
        int somme=0;
        int value_energie_MDP=0;
        for (int i=0; i<StockageVitesseMDP.size(); i++) {
                somme+=StockageVitesseMDP[i];
                value_energie_MDP+=pow(StockageVitesseMDP[i],3);
        }
        if (Affichage_sortie)
                cout << "tot: " << somme << "  Et energie: " << value_energie_MDP << "   Stockage vitesse MDP  : " << StockageVitesseMDP << endl;
        /*** LORCH ***/
        somme=0;int value_energie_Lorch=0;
        for (int i=0; i<Somme_StockageVitessesLorchParJobs.size(); i++) {
                somme+=Somme_StockageVitessesLorchParJobs[i];
                value_energie_Lorch+=pow(Somme_StockageVitessesLorchParJobs[i],3);
        }
        if (Affichage_sortie) {
                cout << "tot: " << somme << "  Et energie: " << value_energie_Lorch << "   Stockage vitesse LORCH: " << Somme_StockageVitessesLorchParJobs << endl;
                cout << "stockage vitesse Lorch par pas de tmeps" << endl;
                Affiche(StockageVitessesLorchParJobs); cout << endl;
        }
        /*** OA ***/
        int somme_OA=0;
        int value_energie_OA=0;
        for (int i=0; i<StockageVitesseOA.size(); i++) {
                somme_OA+=StockageVitesseOA[i];
                value_energie_OA+=pow(StockageVitesseOA[i],3);
        }
        if (Affichage_sortie)
                cout << "tot: " << somme_OA << "  Et energie: " << value_energie_OA << "   Stockage vitesse OA  : " << StockageVitesseOA << endl;
        /*** Qté de Job_Arrivée ***/
        somme=0;
        for (int i=0; i<ListeJobArrivalSimulation.size(); i++) {
                for (int j=0; j<ListeJobArrivalSimulation[i].size(); j++) {
                        somme+=ListeJobArrivalSimulation[i][j][0];
                }
        }
        if (Affichage_sortie)
                cout << "Quantité de travail:  " << somme << "   Et les jobs vus sont: " << endl;
        /*** Affichage des jobs, etats LORCH, MDP, OA ***/
        if (AffichageSimu) {
                int iter=0;
                for (int i=0; i<StockageVitesseMDP.size(); i++) {
                        /*** Liste Jobs ***/
                        cout << "++++++++++++++++++++++++++++++++++++++++++ " << i << endl;
                        if (pasdeTemps[iter]==i) {
                                cout << "ARRIVEE: ";
                                if (iter<ListeJobArrivalSimulation.size()) {
                                        Affiche(ListeJobArrivalSimulation[iter]);
                                        iter++;
                                } else {
                                        cout << "OUT" << endl;
                                }
                                cout << endl;
                        } else {
                                cout << "NONE = Rien d'arrivée à ce pas de temps" << endl;
                        }
                        /*** LORCH Etat + vitesse ***/
                        cout << "======== LORCH ========== " << StockageVitessesLorchParJobs[i] << endl;
                        if (i<ListeEtatLORCH.size()) {
                                Affiche(ListeEtatLORCH[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                        /*** MDP Etat + vitesse ***/
                        cout << "======== MDP ========== " << StockageVitesseMDP[i] << endl;
                        if (i<ListeEtatMDP.size()) {
                                cout << " Delai:  " << Vector_State_inter_arrival_time[i] << endl;
                                Affiche(ListeEtatMDP[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                        /*** OA Etat + vitesse ***/
                        cout << "======== OA ========== " << StockageVitesseOA[i] << endl;
                        if (i<ListeEtatOA.size()) {
                                Affiche(ListeEtatOA[i]);
                        } else {
                                cout << "OUT" << endl;
                        }
                }
        }
        bool affichageParSimu=false;
        if (affichageParSimu) {
                cout << "tot: " << somme << "   Et energie: " << value_energie_Lorch << "  Stockage vitesse LORCH: " << Somme_StockageVitessesLorchParJobs << endl;
                cout << "tot: " << somme << "   Et energie: " << value_energie_MDP << "   Stockage vitesse MDP  : " << StockageVitesseMDP << endl;
                cout << "tot: " << somme_OA << "Et energie: " << value_energie_OA << "      Stockage vitesse OA   : " << StockageVitesseOA << endl;
                
                // @TODO Faire affichage du cout
                // @TODO apple à la fonction de dessin en R
                cout << "Energie Diff Lorch - MDP  :  " << value_energie_Lorch-value_energie_MDP << endl;
                cout << "Energie Diff OA - MDP     :  " << value_energie_OA-value_energie_MDP << endl;
                if (Monitoring) {
                        // @TODO
                        //                        cout << "Proba en nbre de job arrivé par taille: " << Nbre_job_arriv/ << endl;
                        Monitoring_vector=Monitoring_vector + Nbre_job_arriv;
                }
        }
        vector<double> MDP_Lorch,MDP_OA;
        MDP_Lorch.push_back(value_energie_MDP); MDP_Lorch.push_back(value_energie_Lorch);
        MDP_OA.push_back(value_energie_MDP); MDP_OA.push_back(value_energie_OA);
        Stock_MDP_Lorch.push_back(MDP_Lorch);
        Stock_MDP_OA.push_back(MDP_OA);
        vector <int> valueDiffSortie; valueDiffSortie.push_back(value_energie_Lorch-value_energie_MDP);valueDiffSortie.push_back(value_energie_OA-value_energie_MDP);
        return valueDiffSortie;
}