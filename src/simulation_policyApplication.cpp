//
//  simulation_policyApplication.cpp
//  finite_case
//
//  Created by *** on 02/10/2018.
//  Copyright © 2018 ***. All rights reserved.
//

#include "simulation_policyApplication.hpp"
#include "output_R.hpp"

#include <cmath>
#include <algorithm>

// Calcul de la valeur de OA à partir de ETaches_courant
int Calcul_vitesse_OA_ETaches(map<int, vector<int> >  & ETaches_courant,vector<int> & vitesse_OA) {
        int somme = 0;
        int saut = 0;
        vector<double> pente;
        for (map<int, vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end(); it++) {
                saut=0;
                for (int i=0; i<it->second.size(); i++) {
                        saut+=it->second[i];
                }
                somme+=saut;
                if (it->first!=0) {
                        pente.push_back(ceil((double)somme/(double)(it->first)));
                } else {
                        pente.push_back(0);
                }
        }
        
        // @TODO ajout dans le cas on a aucune ETaches_courant, on distingue les cas
        if (ETaches_courant.size()==0) {
                vitesse_OA.push_back(0);
        } else {
                vitesse_OA.push_back(max_element(pente.begin(),pente.end())[0]);
        }
        return 1;
}

/***********************************************************************************************/
// testAlgo_simulation_et_calcul_vitesse : Simule les taches sur T : nouveau jobs ou pas selon la proba donner en parametre
//                                         Determine letat courant et calcule letat suivant
//
// Utilise la fonction
//
// sortie:
//          sortie_testAlgo_simulation_et_calcul_vitesse : vector<vector<double> >
//                              a             : vector<double>, temps d'arrivées des jobs sur T.
//                              d             : vector<double>, temps d'arrêt des jobs (représentation des deadlines) sur T.
//                              deadline_alea : vector<double>, deadlines des tâches arrivées sur T.
//                              vitesse       : vector<double>, vitesse choisie par backward iteration sur T.
//                              vitesse_OA    : vector<double>, vitesse choisie par OA sur T.
//                              w_value       : vector<double>, quantité de travail exécutée sur T.
//                              ens_taille    : vector<double>, tailles des tâches arrivées sur T.
//
// entrée :
//          espace_detat_init     : int, taille maximale des jobs
//          p_size       : vector<double>, probabilité de la taille d'un job
//          T            : int, échéance sur laquelle on fait notre étude
//          max_deadline : int, deadline maximale des jobs
//          choix_action : vector<vector<double> >, amtrices des actions choisies en fonction du temps et des états.
//          iteration    : int, ?????????????
//          dp           : bool, dans quel mode est on, cas dynamic programming (DP) [matrice de vitesse, dépend du temps] ou value iteration (VI) [vecteur de vitesse, dépend du temps]
//          jobs_T       : vector<vector<double> >, ensemble des jobs avec leurs caractéristiques (taille & deadline) calculé avec la fonctuion simulation_job.
/***********************************************************************************************/

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse(vector<vector<double> > & espace_detat_init, vector<double> p_size,int T,int max_deadline, vector<vector<double> > choix_action, int iteration, bool dp, vector<vector<double> > & jobs_T) {
        
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba;
        vector<double> deadline_alea;
        vector<double> ens_taille;
        
        //Effectue la somme des p
        vector<double> sum_p;
        double acc = 0;
        for (int i = 0; i < p_size.size(); i++) {
                acc += p_size[i];
                sum_p.push_back(acc);
        }
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value;
        vector<double> vitesse_OA;
        vector<double> vitesse;
        double travail_total = 0;
        double travail_total_OA = 0;
        double proba_select = 0;
        int deadline_value;
        vector<double> nvel_etat;
        vector<double> nvel_etat_res;
        vector<double> nvel_etat_OA;
        int taille;
        
        // AVANT i<fin
        for (int i=0; i<T; i++) {
                //                cout << i << endl;
                //                //set.seed(i);
                //                // La graine
                //                iteration++;
                //                uint param_seed = RAND_MAX/iteration;
                //                srand(param_seed);
                //                //srand((long)time(i));
                
                //                // Quel type de tache arrive ?
                //                nbre_alea = rand()/(double)RAND_MAX;
                //                //-> cout << "nbre_alea " << nbre_alea <<endl;// determine la taille
                //                for (int k =0; k<sum_p.size(); k++) {
                //                        if (sum_p[k] >= nbre_alea) {
                //                                proba_select = k;
                //                                //cout << "k " << k << endl;
                //                                break;
                //                        }
                //                }
                //                taille = proba_select;
                //                //taille = 4;
                //                deadline_value = (int)(rand() % (max_deadline - 1 + 1)+ 1);
                //
                //                deadline_value = max_deadline;
                taille = jobs_T[0][i];
                deadline_value = jobs_T[1][i];
                //                int deadlinet[] = {1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2};
                //                int probat[] = {2,1,1,4,4,0,4,4,2,1,0,0,0,2,4,4,2,1,1,4};
                if (i>T-max_deadline) {
                        taille = 0;
                        deadline_value = 1;
                } else {
                        // CAS ou l'on rentre en dur les taches cf journal 2017-12-05, Tag: CODE_CPP
                        //                        taille = probat[i];
                        //                        deadline_value = deadlinet[i];
                }
                // d_value : vecteur qui corespond aux futurs états
                // TODO : ajouter au fur et a mesure les valeurs a d_value (addition de tous les coefficients qui suivent le coefficients corrspondant a deadline 
                if (deadline_value != 0) {
                        // enregistre et détermine la deadline)
                        // borne sup => i+max_deadline (inf egale)
                        for (int k=i+deadline_value; k<d_value.size(); k++){
                                d_value[k] += taille;
                        }
                        aa.push_back(aa[i]+taille);
                } else {
                        aa.push_back(0);
                }
                deadline_alea.push_back(deadline_value);
                // A AJOUTER
                //                cout << taille << ";" << deadline_value << endl;
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                
                // détermine la position dans le vecteur des deadlines jusqu'ou on peut connaitre
                //
                //deadline_max = max(i+deadline_value,deadline_max);
                // sélection du nouvel etat dans d_value
                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                for (int k = i+1; k<=i+max_deadline; k++) {
                        nvel_etat.push_back(d_value[k]);
                }
                // prendre en compte le travail
                nvel_etat_res = nvel_etat;
                nvel_etat = nvel_etat_res-travail_total;
                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                // prendre en compte le travail OA
                nvel_etat_OA = nvel_etat_res-travail_total_OA;
                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                //                cout << "nvel etat " << nvel_etat << endl;
                
                
                //                cout << "nvel etat " << nvel_etat << "nvel etat OA " << nvel_etat_OA << endl;
                // Recherche de la position de letat en fonction du nouvel etat considéré
                std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat);
                std::vector<vector<double> >::iterator it_OA = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_OA);
                
                // vitesse choisie au temps t dans le cas MDP
                //bool cas_DP = true;
                //bool cas_VI = false;
                
                if (i<T) {
                        if (dp) {
                                vitesse.push_back(choix_action[T-i-1][distance(espace_detat_init.begin(), it)]);
                                //                                cout << "vitesse " << vitesse << endl;
                        } else {
                                /***** CAS UNE TACHES ALTERNEES ******/
                                vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                
                                //                                /***** CAS DEUX TACHES ALTERNEES ******/
                                //                                if (i%2==0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                                //                                /***** CAS QUATRE TACHES ALTERNEES ******/
                                //                                if (i%4 == 0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 1) {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 2) {
                                //                                        vitesse.push_back(choix_action[2][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 3) {
                                //                                        vitesse.push_back(choix_action[3][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                                //                                /***** CAS HUIT TACHES ALTERNEES ******/
                                //                                if (i%8 == 0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 1) {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 2) {
                                //                                        vitesse.push_back(choix_action[2][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 3) {
                                //                                        vitesse.push_back(choix_action[3][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 4) {
                                //                                        vitesse.push_back(choix_action[4][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 5) {
                                //                                        vitesse.push_back(choix_action[5][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 6) {
                                //                                        vitesse.push_back(choix_action[6][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 7) {
                                //                                        vitesse.push_back(choix_action[7][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                                
                        }
                }
                // vitesse choisie au temps t avec Optimal Available
                int pos_OA = (int)distance(espace_detat_init.begin(),it_OA);
                vitesse_OA.push_back(calcul_vitesse_OA(pos_OA, espace_detat_init));
                // quantité totale de travail éxécuté.
                travail_total += vitesse[vitesse.size()-1];
                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                w_value.push_back(travail_total);
                ens_taille.push_back(taille);
                proba.push_back(proba_select);
        }
        //        cout << "deadlinealea  " << deadline_alea;
        //        cout << "taille  " << ens_taille;
        //        cout << " ========================== FIN ALGO ==========================" << endl;
        //        cout << "vitesse MDP: " << vitesse;
        //        cout << "vitesse OA: " << vitesse_OA;
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a;
        a= aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}

/***********************************************************************************************/
// simulation
//
// Utilise la fonction testAlgo_simulation_et_calcul_vitesse
//
// sortie:
//          sortie_simulation: vector<vector<double> >
//                      cout_online      : vector<double>, cout de backward iteration
//                      cout_OA          : vector<double>, cout de la politique OA
//
// entrée :
//          T                  : int, période sur laquelle on réalise l'expérience
//          p_size             : vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          max_deadline       : int, échéance maximale d'un job
//          max_size           : int, taille maximale d'un job
//          max_vitesse        : int, vitesse maximale du processeur
//          espace_detat_init  : vector<vector<double> >, ensemble des états possible
//          p_deadline         : vector<double>, probabilité de l'échéance d'un job, p_deadline.size() = max_deadline
//          sortie_choix       : vector<vector<vector<<double> >>, correspond à la sortie _backward =>  nrj_int & choix_action
//          iteration          : int, ?????????
//          dp                 : bool, VI ou DP
//          jobs_T             : vector<vector<double> >, une simulation aléatoire avec création des jobs sur un horizon T
/***********************************************************************************************/
vector<double> simulation(int T,vector<double> p_size,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<vector<double> > > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T, bool tracer_courbe) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse(espace_detat_init, p_size, T, max_deadline,sortie_choix[1], iteration, dp, jobs_T);
        cout << "====" << endl;
        cout << sortie_bis << endl;
        if (tracer_courbe)
                tracer_courbe_function(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0;
        double cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        cout << "===================================================================" << endl;
        cout << "============================= COUT DP =============================" << endl;
        cout << "     - Optimal Available : " << cout_OA << endl;
        cout << "     - Online DP         : " << cout_online << endl;
        
        vector<double> sortie_simulation;
        sortie_simulation.push_back(cout_online);
        sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}

/***********************************************************************************************/
// simulationVI
//
// Utilise la fonction testAlgo_simulation_et_calcul_vitesse
//
// sortie:
//          sortie_simulation: vector<vector<double> >
//                      cout_online      : vector<double>, cout de backward iteration
//                      cout_OA          : vector<double>, cout de la politique OA
//
// entrée :
//          T                  : int, période sur laquelle on réalise l'expérience
//          p_size             : vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          max_deadline       : int, échéance maximale d'un job
//          max_size           : int, taille maximale d'un job
//          max_vitesse        : int, vitesse maximale du processeur
//          espace_detat_init  : vector<vector<double> >, ensemble des états possible
//          p_deadline         : vector<double>, probabilité de l'échéance d'un job, p_deadline.size() = max_deadline
//          sortie_choix       : vector<vector<vector<<double> > >,
//          iteration          : int, ?????????
//          dp                 : bool, VI ou DP
//          jobs_T             : vector<vector<double> >, une simulation aléatoire avec création des jobs sur un horizon T
/***********************************************************************************************/
vector<double> simulationVI(int T,vector<double> p_size,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<double> > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T, bool tracer_courbe) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse(espace_detat_init, p_size, T, max_deadline,sortie_choix, iteration, dp, jobs_T);
        //        cout << "====" << endl;
        //        cout << sortie_bis << endl;
        
        if (tracer_courbe)
                Tracer_simulation_VI(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0;
        double cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        cout << "===================================================================" << endl;
        cout << "============================= COUT VI =============================" << endl;
        cout << "     - Optimal Available : " << cout_OA << endl;
        cout << "     - Online VI         : " << cout_online << endl;
        
        vector<double> sortie;
        sortie.push_back(cout_online);
        sortie.push_back(cout_OA);
        return(sortie);
}



vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_esp_restreint(vector<vector<double> > & espace_detat_init, vector<double> p_size,int T,int max_reduc,int max_deadline, vector<vector<double> > choix_action, int iteration, bool dp, vector<vector<double> > & jobs_T) {
        
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba;
        vector<double> deadline_alea;
        vector<double> ens_taille;
        
        //Effectue la somme des p
        vector<double> sum_p;
        double acc = 0;
        for (int i = 0; i < p_size.size(); i++) {
                acc += p_size[i];
                sum_p.push_back(acc);
        }
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value;
        vector<double> vitesse_OA;
        vector<double> vitesse;
        double travail_total = 0;
        double travail_total_OA = 0;
        double proba_select = 0;
        int deadline_value;
        vector<double> nvel_etat;
        vector<double> nvel_etat_res;
        vector<double> nvel_etat_OA;
        int taille;
        
        int arret = max_deadline;
        max_deadline = max_reduc;
        
        // AVANT i<fin
        for (int i=0; i<T; i++) {
                //                //set.seed(i);
                //                // La graine
                //                iteration++;
                //                uint param_seed = RAND_MAX/iteration;
                //                srand(param_seed);
                //                //srand((long)time(i));
                
                //@TODO faire appel à simulation jobs sur de x à x+1 pour simuler (1 taille et 1 deadline)*nombre_de_taches
                // A la place de la récupération dans le tableau
                // jobs_T vector< vector< vector<double> > >, cf cahier
                //@FINTODO
                taille = jobs_T[0][i];
                deadline_value = jobs_T[1][i];
                
                if (i>T-arret) {
                        taille = 0;
                        deadline_value = 1;
                } else {
                        // CAS ou l'on rentre en dur les taches cf journal 2017-12-05, Tag: CODE_CPP
                        //                        taille = probat[i];
                        //                        deadline_value = deadlinet[i];
                }
                // d_value : vecteur qui corespond aux futurs états
                //@TODO : ajouter au fur et a mesure les valeurs a d_value (addition de tous les coefficients qui suivent le coefficients corrspondant a deadline 
                if (deadline_value != 0) {
                        // enregistre et détermine la deadline)
                        // borne sup => i+max_deadline (inf egale)
                        for (int k=i+deadline_value; k<d_value.size(); k++){
                                d_value[k] += taille;
                        }
                        aa.push_back(aa[i]+taille);
                } else {
                        aa.push_back(0);
                }
                deadline_alea.push_back(deadline_value);
                
                //@TODO creation de deadline_value et taille que l'on remplit au fur et à mesure
                //@FINTODO
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                
                // détermine la position dans le vecteur des deadlines jusqu'ou on peut connaitre
                // sélection du nouvel etat dans d_value
                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                for (int k = i+1; k<=i+max_deadline; k++) {
                        nvel_etat.push_back(d_value[k]);
                }
                // prendre en compte le travail
                nvel_etat_res = nvel_etat;
                nvel_etat = nvel_etat_res-travail_total;
                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                // prendre en compte le travail OA
                nvel_etat_OA = nvel_etat_res-travail_total_OA;
                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                //                cout << "nvel etat " << nvel_etat << "nvel etat OA " << nvel_etat_OA << endl;
                
                // Recherche de la position de letat en fonction du nouvel etat considéré
                std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat);
                std::vector<vector<double> >::iterator it_OA = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_OA);
                
                // vitesse choisie au temps t dans le cas MDP
                if (i<T) {
                        if (dp) {
                                vitesse.push_back(choix_action[T-i-1][distance(espace_detat_init.begin(), it)]);
                        } else {
                                /***** CAS UNE TACHES ALTERNEES ******/
                                vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                
                                //                                /***** CAS DEUX TACHES ALTERNEES ******/
                                //                                if (i%2==0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                                //                                /***** CAS QUATRE TACHES ALTERNEES ******/
                                //                                if (i%4 == 0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 1) {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 2) {
                                //                                        vitesse.push_back(choix_action[2][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%4 == 3) {
                                //                                        vitesse.push_back(choix_action[3][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                                //                                /***** CAS HUIT TACHES ALTERNEES ******/
                                //                                if (i%8 == 0) {
                                //                                        vitesse.push_back(choix_action[0][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 1) {
                                //                                        vitesse.push_back(choix_action[1][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 2) {
                                //                                        vitesse.push_back(choix_action[2][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 3) {
                                //                                        vitesse.push_back(choix_action[3][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 4) {
                                //                                        vitesse.push_back(choix_action[4][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 5) {
                                //                                        vitesse.push_back(choix_action[5][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 6) {
                                //                                        vitesse.push_back(choix_action[6][distance(espace_detat_init.begin(), it)]);
                                //                                } else if (i%8 == 7) {
                                //                                        vitesse.push_back(choix_action[7][distance(espace_detat_init.begin(), it)]);
                                //                                }
                                
                        }
                }
                // vitesse choisie au temps t avec Optimal Available
                int pos_OA = (int)distance(espace_detat_init.begin(),it_OA);
                vitesse_OA.push_back(calcul_vitesse_OA(pos_OA, espace_detat_init));
                // quantité totale de travail éxécuté.
                travail_total += vitesse[vitesse.size()-1];
                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                w_value.push_back(travail_total);
                ens_taille.push_back(taille);
                proba.push_back(proba_select);
        }
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a;
        a= aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}


vector<double> simulation_esp_restreint(int T,vector<double> p_size,int max_reduc,int max_deadline, int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<vector<double> > > sortie_choix, int iteration, bool dp, vector<vector<double>  > & jobs_T) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_esp_restreint(espace_detat_init, p_size, T, max_reduc, max_deadline,sortie_choix[1], iteration, dp, jobs_T);
        cout << "sortie " << sortie_bis <<endl;
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0;
        double cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        //        cout << "===================================================================" << endl;
        //        cout << "============================= COUT DP =============================" << endl;
        //        cout << "     - Optimal Available" << endl;
        //        cout << cout_OA << endl;
        //        cout << "     - Online DP " << endl;
        //        cout << cout_online << endl;
        
        vector<double> sortie_simulation;
        sortie_simulation.push_back(cout_online);
        sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}



vector<double> simulationVI_esp_restreint(int T,vector<double> p_size,int max_reduc, int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline, vector<vector<double> > sortie_choix, int iteration, bool dp, vector<vector<double> > & jobs_T) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_esp_restreint(espace_detat_init, p_size, T, max_reduc, max_deadline,sortie_choix, iteration, dp, jobs_T);
        cout << "sortie " << sortie_bis <<endl;
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0;
        double cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        //        cout << "===================================================================" << endl;
        //        cout << "============================= COUT VI =============================" << endl;
        //        cout << "     - Optimal Available" << endl;
        //        cout << cout_OA << endl;
        //        cout << "     - Online VI " << endl;
        //        cout << cout_online << endl;
        
        vector<double> sortie;
        sortie.push_back(cout_online);
        sortie.push_back(cout_OA);
        return(sortie);
}

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_general(vector<vector<double> > & espace_detat_init,int T,int max_deadline,int max_period, vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res) {
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba,deadline_alea,ens_taille;
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value, vitesse_OA,vitesse;
        double travail_total = 0,travail_total_OA = 0, proba_select = 0;
        int deadline_value, taille;
        vector<double> nvel_etat, nvel_etat_res, nvel_etat_OA;
        
        for (int i=0; i<T; i++) {
                taille = jobs_T[0][i];
                deadline_value = jobs_T[1][i];
                
                //                int deadlinet[] = {1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2};
                //                int probat[] = {2,1,1,4,4,0,4,4,2,1,0,0,0,2,4,4,2,1,1,4};
                
                
                //@TODO  remplacer max_deadline par 3
                if (i>T-max_deadline_res) {
                        taille = 0;
                        deadline_value = 1;
                } else {
                        // CAS ou l'on rentre en dur les taches cf journal 2017-12-05, Tag: CODE_CPP
                        //                        taille = probat[i];
                        //                        deadline_value = deadlinet[i];
                }
                // d_value : vecteur qui corespond aux futurs états
                // TODO : ajouter au fur et a mesure les valeurs a d_value (addition de tous les coefficients qui suivent le coefficients corrspondant a deadline 
                for (int k=i+deadline_value; k<d_value.size(); k++){
                        d_value[k] += taille;
                }
                aa.push_back(aa[i]+taille);
                
                deadline_alea.push_back(deadline_value);
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                // détermine la position dans le vecteur des deadlines jusqu'ou on peut connaitre
                
                // CAS Non OA, sélection du nouvel etat dans d_value
                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                for (int k = i+1; k<=i+max_deadline; k++) {
                        nvel_etat.push_back(d_value[k]);
                }
                // prendre en compte le travail
                nvel_etat_res = nvel_etat;
                nvel_etat = nvel_etat_res-travail_total;
                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                // CAS OA, prendre en compte le travail OA
                nvel_etat_OA.erase(nvel_etat_OA.begin(), nvel_etat_OA.end());
                for (int k = i+1; k<=i+max_deadline_res; k++) {
                        nvel_etat_OA.push_back(d_value[k]);
                }
                //                nvel_etat_OA = nvel_etat_res-travail_total_OA;
                nvel_etat_OA = nvel_etat_OA-travail_total_OA;
                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                //                cout << "nvel etat " << nvel_etat << "nvel etat OA " << nvel_etat_OA << endl;
                // Recherche de la position de letat en fonction du nouvel etat considéré
                std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat);
                //std::vector<vector<double> >::iterator it_OA = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_OA);
                
                // vitesse choisie au temps t dans le cas MDP
                if (i<T) {
                        if (dp) {
                                vitesse.push_back(choix_action[T-i-1][distance(espace_detat_init.begin(), it)]);
                        } else {
                                for (int k=0;k<max_period;k++) {
                                        if (i%max_period == k) {
                                                vitesse.push_back(choix_action[k][distance(espace_detat_init.begin(), it)]);
                                        }
                                }
                        }
                }
                
                // vitesse choisie au temps t avec Optimal Available
                //int pos_OA = (int)distance(espace_detat_init.begin(),it_OA);
                //vitesse_OA.push_back(calcul_vitesse_OA(pos_OA, espace_detat_init));
                vitesse_OA.push_back(calcul_vitesse_OA(nvel_etat_OA));
                
                // quantité totale de travail éxécuté.
                travail_total += vitesse[vitesse.size()-1];
                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                w_value.push_back(travail_total);
                ens_taille.push_back(taille);
                proba.push_back(proba_select);
        }
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a = aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}


vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_restreint(vector<vector<double> > & espace_detat_init,int T,int max_deadline,int max_period, vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res) {
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba,deadline_alea,ens_taille;
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value, vitesse_OA,vitesse;
        double travail_total = 0,travail_total_OA = 0, proba_select = 0;
        int deadline_value, taille;
        vector<double> nvel_etat, nvel_etat_res, nvel_etat_OA;
        
        for (int i=0; i<T; i++) {
                taille = jobs_T[0][i];
                deadline_value = jobs_T[1][i];
                cout << "pas: " << i << " taille " << taille << "  deadline  " << deadline_value << endl;
                //                int deadlinet[] = {1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2}; //                int probat[] = {2,1,1,4,4,0,4,4,2,1,0,0,0,2,4,4,2,1,1,4};
                //@TODO  remplacer max_deadline par 3
                if (i>T-max_deadline) {
                        taille = 0;
                        deadline_value = 1;
                } else {
                        // CAS ou l'on rentre en dur les taches cf journal 2017-12-05, Tag: CODE_CPP
                        //                        taille = probat[i];
                        //                        deadline_value = deadlinet[i];
                }
                // d_value : vecteur qui corespond aux futurs états
                // TODO : ajouter au fur et a mesure les valeurs a d_value (addition de tous les coefficients qui suivent le coefficients corrspondant a deadline 
                for (int k=i+deadline_value; k<d_value.size(); k++){
                        d_value[k] += taille;
                }
                aa.push_back(aa[i]+taille);
                
                deadline_alea.push_back(deadline_value);
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                // détermine la position dans le vecteur des deadlines jusqu'ou on peut connaitre
                
                /**** @TODO ****/
                //@TODO Determiner letat resteint qui correspond au k premier pas + le dernier que ce soit pour OA ou pour DP
                
                
                //@TODO Recuperer les deux premiers pas, i et i+1
                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                for (int k = i+1; k<=i+max_deadline_res; k++) {
                        nvel_etat.push_back(d_value[k]);
                }
                nvel_etat.push_back(d_value[i+max_deadline]);
                
                nvel_etat_res = nvel_etat;
                nvel_etat = nvel_etat_res-travail_total;
                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                //@TODO Recuperer le pas \Delta
                nvel_etat_OA.erase(nvel_etat_OA.begin(), nvel_etat_OA.end());
                for (int k = i+1; k<=i+max_deadline; k++) {
                        nvel_etat_OA.push_back(d_value[k]);
                }
                //nvel_etat_OA.push_back(d_value[i+max_deadline]);
                
                nvel_etat_OA = nvel_etat_OA-travail_total_OA;
                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                /**** @TODO ****/
                
                //                // CAS Non OA, sélection du nouvel etat dans d_value
                //                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                //                for (int k = i+1; k<=i+max_deadline; k++) {
                //                        nvel_etat.push_back(d_value[k]);
                //                }
                //                // prendre en compte le travail
                //                nvel_etat_res = nvel_etat;
                //                nvel_etat = nvel_etat_res-travail_total;
                //                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                //                        if (*it < 0) {
                //                                *it = 0;
                //                        }
                //                }
                //
                //                // CAS OA, prendre en compte le travail OA
                //                nvel_etat_OA.erase(nvel_etat_OA.begin(), nvel_etat_OA.end());
                //                for (int k = i+1; k<=i+max_deadline_res; k++) {
                //                        nvel_etat_OA.push_back(d_value[k]);
                //                }
                //                //                nvel_etat_OA = nvel_etat_res-travail_total_OA;
                //                nvel_etat_OA = nvel_etat_OA-travail_total_OA;
                //                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                //                        if (*it < 0) {
                //                                *it = 0;
                //                        }
                //                }
                
                cout << d_value;
                cout << "nvel etat " << nvel_etat << "nvel etat OA " << nvel_etat_OA;
                // Recherche de la position de letat en fonction du nouvel etat considéré
                std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat);
                //                std::vector<vector<double> >::iterator it_OA = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_OA);
                
                cout << "iterateur " << distance(espace_detat_init.begin(), it) << endl;
                
                // vitesse choisie au temps t dans le cas MDP
                if (i<T) {
                        if (dp) {
                                vitesse.push_back(choix_action[T-i-1][distance(espace_detat_init.begin(), it)]);
                        } else {
                                for (int k=0;k<max_period;k++) {
                                        if (i%max_period == k) {
                                                vitesse.push_back(choix_action[k][distance(espace_detat_init.begin(), it)]);
                                        }
                                }
                        }
                }
                
                // vitesse choisie au temps t avec Optimal Available
                //int pos_OA = (int)distance(espace_detat_init.begin(),it_OA);
                //vitesse_OA.push_back(calcul_vitesse_OA(pos_OA, espace_detat_init));
                
                //@TODO Rajouter dans le cas ou l'on veut faire démarrer OA plus tard pour eviter de prendre en compte le fait que l'on démarre à l'état (0,...,0)
                //@@@DONE TRICHE 1, améliore les trois premiers pas (décalage dans le temps)
                //                if (i<3) {
                //                        vitesse_OA.push_back(vitesse[i]);
                //                } else {
                cout << "new etat OA " << nvel_etat_OA << endl;
                vitesse_OA.push_back(calcul_vitesse_OA(nvel_etat_OA));
                //                }
                
                //@@@DONE TRICHE 2 Pour résoudre le problème de vitesse max fixé à 0 (attention dans certains cas on a un etat inconnu ??
                if (vitesse[i]==0 || nvel_etat[max_deadline_res-1]==0) {
                        vitesse[i]=vitesse_OA[i];
                }
                cout << "choix de vitesse : " << vitesse[i] << "  OA: " << vitesse_OA[i] << endl;cout << endl;
                
                
                // quantité totale de travail éxécuté.
                travail_total += vitesse[vitesse.size()-1];
                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                w_value.push_back(travail_total);
                ens_taille.push_back(taille);
                proba.push_back(proba_select);
        }
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a = aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}


vector<double> simulation_restreint(int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_restreint(espace_detat_init, T, max_deadline,max_period,sortie_choix[1], dp, jobs_T,max_deadline_res);
        
        if (tracer_courbe)
                tracer_courbe_function(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0, cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        if (cout_online > 100000) {
                cout << "STOP probleme vitesse incohérente" << endl;
                cout << cout_online << "   " << cout_OA << endl;
                cout << sortie_bis[3] << " vOA " << sortie_bis[4] << endl;
        }
        vector<double> sortie_simulation;
        sortie_simulation.push_back(cout_online);sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}

vector<double> simulation_general(bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_general(espace_detat_init, T, max_deadline,max_period,sortie_choix[1], dp, jobs_T,max_deadline_res);
        
        if (tracer_courbe)
                tracer_courbe_function(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0, cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        vector<double> sortie_simulation;
        
        //        bool affichage_resultat=false;
        if (affichage_resultat) {
                cout << "==============> VALEUR VI <================" << endl;
                cout << "OA:   " << action_gen_OA;
                cout << "VI<-c(" << action_gen;
        }
        
        sortie_simulation.push_back(cout_online);sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}


vector<double> simulation_VI(double gamma, bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int res_max_deadline) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_general(espace_detat_init, T, max_deadline,max_period,sortie_choix[1], dp, jobs_T, res_max_deadline);
        
        if (tracer_courbe)
                Tracer_simulation_VI(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0, cout_online = 0;
        
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += pow(gamma,i)*cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += pow(gamma,i)*cout_nrj(action_gen[i],max_vitesse);
        }
        //        bool affichage_resultat=true;
        if (affichage_resultat) {
                cout << "==============> VALEUR ValueIteration <================" << endl;
                cout << "  vitesse<-c(" << action_gen;
        }
        //        cout << "vitesseOA<-c(" << action_gen_OA;
        //        cout << "taille<-c(" << jobs_T[0];
        //        cout << "deadline<-c(" << jobs_T[1];
        
        vector<double> sortie; sortie.push_back(cout_online);sortie.push_back(cout_OA);
        return(sortie);
}



bool affiche_result =true;

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_restreint_ktaches(vector<vector<int> > & espace_detat_init,int T,int max_deadline,int max_period, vector<vector<double> > choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res) {
        //int fin = T-max_deadline;
        int val = 2*T,deadline_value, taille;
        vector<double> aa, dd, proba,deadline_alea,ens_taille, d_value(val,0), w_value, vitesse_OA,vitesse, nvel_etat, nvel_etat_res, nvel_etat_OA;
        aa.push_back(0); dd.push_back(0);
        
        // Initialisation de l'etat
        vector<int> nvel_etat_entree;
        for (int i=0; i<espace_detat_init[0].size(); i++) {
                nvel_etat_entree.push_back(0);
        }
        
        vector<vector<int> >::iterator it=find(espace_detat_init.begin(), espace_detat_init.end(), nvel_etat_entree);
        int position_etat_courant=(int)(it-espace_detat_init.begin()), position_etat_suivant;
        vector<int> NouvellesTaches,Etat_courant_reduit, tache_simuler,vitesseOA;
        NouvellesTaches.push_back(0);NouvellesTaches.push_back(0);
        
        map<int, vector<int> > ETaches_courant, ETaches_courant_OA;
        int reduction_deadline = 2;
        
        //        vitesse.push_back(1);
        //        vitesse.push_back(2);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(2);vitesse.push_back(3);vitesse.push_back(4);vitesse.push_back(2);vitesse.push_back(3);vitesse.push_back(7);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(3);vitesse.push_back(1);vitesse.push_back(0);
        //        vitesseOA.push_back(1);
        //        vitesseOA.push_back(2);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(2);vitesseOA.push_back(3);vitesseOA.push_back(4);vitesseOA.push_back(2);vitesseOA.push_back(3);vitesseOA.push_back(7);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(3);vitesseOA.push_back(1);vitesseOA.push_back(0);
        //
        //        vitesseOA.clear();
        
        //        //@TODO enlever
        //        int deadlinet[] = {4,4,0,2,4,2,4,4,4,4,4,4,2,4,4,4,4,1,1,1};
        //        int probat[] = {6,6,0,3,6,3,6,6,6,6,6,6,3,6,6,6,6,0,0,0};
        //        int deadlinet[] = {0,4,2,4,0,4,4,2,0,4,0,4,4,4,4,4,4,1,1,1};
        //        int probat[] = {0,6,3,6,0,6,6,3,0,6,0,6,6,6,6,6,6,6,6,0,0,0};
        
        
        for (int i=0; i<T; i++) {
                
                //@TODO  remplacer max_deadline par 3
                if (i>T-max_deadline) {
                        taille = 0;
                        deadline_value = 0;
                } else {
                        //@TODO rajouter
                        taille = jobs_T[0][i];
                        deadline_value = jobs_T[1][i];
                        //                        //@TODO A rajouter
                        //                        deadline_value = deadlinet[i];
                        //                        taille = probat[i];
                }
                if (affiche_result) {
                        cout << endl;
                        cout << "==<=== pas: " << i << " taille " << taille << "  deadline  " << deadline_value << endl;
                }
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                //@TODO calculer l'état courant, puis déterminer sa position
                if (i==0) {
                        //                        position_etat_suivant=recherche_etat_suiv_restreint_ktaches(position_etat_courant,  deadline_value , taille, NouvellesTaches, 0, espace_detat_init);
                        
                        // ADD
                        Actualise_etat_suivant(ETaches_courant,taille,deadline_value,0);
                        
                        // Calcul de la valeur de OA à partir de ETaches_courant
                        Actualise_etat_suivant(ETaches_courant_OA,taille,deadline_value,0);
                        Calcul_vitesse_OA_ETaches(ETaches_courant_OA,vitesseOA);
                        
                        
                        
                        int sortie_value = Construction_Etat_restreint(ETaches_courant,reduction_deadline,Etat_courant_reduit, tache_simuler);
                        Etat_courant_reduit[Etat_courant_reduit.size()-1] = sortie_value;
                        
                        vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),Etat_courant_reduit);
                        position_etat_suivant = (int)(it-espace_detat_init.begin());
                        // ****
                        
                        
                } else {
                        if (affiche_result) {
                                cout << "vitesse :  " << vitesse;
                                if (i==4) {
                                        cout << "test" << endl;
                                }
                        }
                        //                        position_etat_suivant=recherche_etat_suiv_restreint_ktaches(position_etat_courant, deadline_value, taille, NouvellesTaches, vitesse[i-1], espace_detat_init);
                        // ADD
                        
                        Actualise_etat_suivant(ETaches_courant,taille,deadline_value,vitesse[i-1]);
                        
                        Actualise_etat_suivant(ETaches_courant_OA,taille,deadline_value,vitesseOA[i-1]);
                        
                        Calcul_vitesse_OA_ETaches(ETaches_courant_OA,vitesseOA);
                        if (affiche_result) {
                                cout << "vitesseOA " << vitesseOA << endl;
                                cout << "Etat courant " << endl;
                                
                                for (map<int, vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end(); it++) {
                                        cout << it->first << "  " << it->second;
                                }
                                cout << "Etat courant OA " << endl;
                                for (map<int, vector<int> >::iterator it = ETaches_courant_OA.begin(); it != ETaches_courant_OA.end(); it++) {
                                        cout << it->first << "  " << it->second;
                                }
                                cout << "vitesse0A choisie " << vitesseOA[i] << endl;
                        }
                        int sortie_value= Construction_Etat_restreint(ETaches_courant,reduction_deadline,Etat_courant_reduit, tache_simuler);
                        Etat_courant_reduit[Etat_courant_reduit.size()-1] = sortie_value;
                        
                        vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),Etat_courant_reduit);
                        position_etat_suivant = (int)(it-espace_detat_init.begin());
                        // ****
                }
                
                
                //@TODO utiliser la fonction recherche_etat_suiv_restreint_ktachespour determiner l'état dapres, il faut:
                // -> taille, deadline_value : caractéristiques de la nouvelle tache donnée par le tableau jobs_T
                // -> Pas de nouvellestaches donc soit vecteur nul, soit vecteur (0,0)
                // -> vitesse_pas_prec, est la vitesse enregistré dans la table
                
                
                //                //@TODO Recuperer les deux premiers pas, i et i+1
                //                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                //                for (int k = i+1; k<=i+max_deadline_res; k++) {
                //                        nvel_etat.push_back(d_value[k]);
                //                }
                //                nvel_etat.push_back(d_value[i+max_deadline]);
                //
                //                nvel_etat_res = nvel_etat;
                //                nvel_etat = nvel_etat_res-travail_total;
                //                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                //                        if (*it < 0) {
                //                                *it = 0;
                //                        }
                //                }
                //                //@TODO Recuperer le pas \Delta
                //                nvel_etat_OA.erase(nvel_etat_OA.begin(), nvel_etat_OA.end());
                //                for (int k = i+1; k<=i+max_deadline; k++) {
                //                        nvel_etat_OA.push_back(d_value[k]);
                //                }
                //                //nvel_etat_OA.push_back(d_value[i+max_deadline]);
                //
                //                nvel_etat_OA = nvel_etat_OA-travail_total_OA;
                //                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                //                        if (*it < 0) {
                //                                *it = 0;
                //                        }
                //                }
                
                //                cout << d_value;
                
                //@TODO decommenter
                if (affiche_result) {
                        cout << "nvel etat " << espace_detat_init[position_etat_suivant];
                }
                // Recherche de la position de letat en fonction du nouvel etat considéré
                //                std::vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat);
                //                std::vector<vector<double> >::iterator it_OA = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_OA);
                
                
                //                cout << "iterateur " << distance(espace_detat_init.begin(), it) << endl;
                
                // vitesse choisie au temps t dans le cas MDP
                if (i<T) {
                        if (dp) {
                                vitesse.push_back(choix_action[T-i-1][position_etat_suivant]);
                                if (affiche_result) {
                                        cout << "vitesse choisie: " << choix_action[T-i-1][position_etat_suivant] << endl;
                                }
                                //                                cout <<choix_action[T-i-1]<< endl;
                                
                        } else {
                                for (int k=0;k<max_period;k++) {
                                        if (i%max_period == k) {
                                                vitesse.push_back(choix_action[k][position_etat_suivant]);
                                        }
                                }
                        }
                }
                
                //                // vitesse choisie au temps t avec Optimal Available
                //                //int pos_OA = (int)distance(espace_detat_init.begin(),it_OA);
                //                //vitesse_OA.push_back(calcul_vitesse_OA(pos_OA, espace_detat_init));
                //                cout << "new etat OA " << nvel_etat_OA << endl;
                //                vitesse_OA.push_back(calcul_vitesse_OA(nvel_etat_OA));
                //
                //                // quantité totale de travail éxécuté.
                //                travail_total += vitesse[vitesse.size()-1];
                //                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                //                w_value.push_back(travail_total);
                //                ens_taille.push_back(taille);
                //                proba.push_back(proba_select);
                position_etat_courant=position_etat_suivant;
                
                Etat_courant_reduit.clear(); tache_simuler.clear();
        }
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a = aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        
        for (int i=0; i<vitesseOA.size();i++) {
                vitesse_OA.push_back((int)vitesseOA[i]);
        }
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}


vector<double> simulation_restreint_ktaches(int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<vector<vector<double> > > sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_restreint_ktaches(espace_detat_init, T, max_deadline,max_period,sortie_choix[1], dp, jobs_T,max_deadline_res);
        
        if (tracer_courbe)
                tracer_courbe_function(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        cout << "==============> VALEUR <================" << endl;
        cout << "vitesse<-c(" << action_gen;
        cout << "vitesseOA<-c(" << action_gen_OA;
        cout << "taille<-c(" << jobs_T[0];
        cout << "deadline<-c(" << jobs_T[1];
        
        double cout_OA = 0, cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        cout << "RES ====================>" << endl;
        cout << "cout OA       : " << cout_OA << endl;
        cout << "cout online   : " << cout_online << endl;
        cout << "value: " << (cout_OA-cout_online)/cout_online << endl;
        if (cout_OA>cout_online) {
                cout << "<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> GAGN" << (cout_OA-cout_online)/cout_online << endl;
        }
        cout << "RES ====================>" << endl;
        
        if (cout_online > 100000) {
                cout << "STOP probleme vitesse incohérente" << endl;
                cout << cout_online << "   " << cout_OA << endl;
                cout << sortie_bis[3] << " vOA " << sortie_bis[4] << endl;
        }
        vector<double> sortie_simulation;
        sortie_simulation.push_back(cout_online);sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}

vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_restreint_ktachesQlearning(vector<vector<int> > & espace_detat_init,int T,int max_deadline,int max_period, vector<int> & choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res) {
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba,deadline_alea,ens_taille;
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value, vitesse_OA,vitesse;
        int deadline_value, taille;
        vector<double> nvel_etat, nvel_etat_res, nvel_etat_OA;
        
        
        // Initialisation de l'etat
        vector<int> nvel_etat_entree;
        for (int i=0; i<espace_detat_init[0].size(); i++) {
                nvel_etat_entree.push_back(0);
        }
        
        vector<vector<int> >::iterator it=find(espace_detat_init.begin(), espace_detat_init.end(), nvel_etat_entree);
        int position_etat_courant=(int)(it-espace_detat_init.begin());
        int position_etat_suivant;
        vector<int> NouvellesTaches; NouvellesTaches.push_back(0);NouvellesTaches.push_back(0);
        
        // ADD
        vector<int> Etat_courant_reduit, tache_simuler;
        map<int, vector<int> > ETaches_courant, ETaches_courant_OA;
        int reduction_deadline = 2;
        vector<int> vitesseOA;
        
        //        //@TODO enlever
        //        int deadlinet[] = {4,4,0,2,4,2,4,4,4,4,4,4,2,4,4,4,4,1,1,1};
        //        int probat[] = {6,6,0,3,6,3,6,6,6,6,6,6,3,6,6,6,6,0,0,0};
        //        int deadlinet[] = {0,4,2,4,0,4,4,2,0,4,0,4,4,4,4,4,4,1,1,1};
        //        int probat[] = {0,6,3,6,0,6,6,3,0,6,0,6,6,6,6,6,6,6,6,0,0,0};
        
        affiche_result=false;
        for (int i=0; i<T; i++) {
                //                cout << "PAS:    " << i << endl;
                
                //@TODO  remplacer max_deadline par 3
                if (i>T-max_deadline) {
                        taille = 0;
                        deadline_value = 0;
                } else {
                        //@TODO rajouter
                        taille = jobs_T[0][i];
                        deadline_value = jobs_T[1][i];
                        //                        //@TODO A rajouter
                        //                        deadline_value = deadlinet[i];
                        //                        taille = probat[i];
                }
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                //@TODO calculer l'état courant, puis déterminer sa position
                if (i==0) {
                        Actualise_etat_suivant(ETaches_courant,taille,deadline_value,0);
                        
                        // Calcul de la valeur de OA à partir de ETaches_courant
                        Actualise_etat_suivant(ETaches_courant_OA,taille,deadline_value,0);
                        Calcul_vitesse_OA_ETaches(ETaches_courant_OA,vitesseOA);
                        
                        int sortie_value = Construction_Etat_restreint(ETaches_courant,reduction_deadline,Etat_courant_reduit, tache_simuler);
                        Etat_courant_reduit[Etat_courant_reduit.size()-1] = sortie_value;
                        //                        cout << "Etat courant " << Etat_courant_reduit << endl;
                        
                        vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),Etat_courant_reduit);
                        position_etat_suivant = (int)(it-espace_detat_init.begin());
                        //                        cout << "pos suivant : " << espace_detat_init[position_etat_suivant];
                } else {
                        //                        cout << "========= >>>>>> taille " << i << "  " << taille << "   dealdine  " << deadline_value << endl;
                        Actualise_etat_suivant(ETaches_courant,taille,deadline_value,vitesse[i-1]);
                        
                        Actualise_etat_suivant(ETaches_courant_OA,taille,deadline_value,vitesseOA[i-1]);
                        
                        Calcul_vitesse_OA_ETaches(ETaches_courant_OA,vitesseOA);
                        
                        if (affiche_result) {
                                cout << "vitesseOA " << vitesseOA << endl;
                                cout << "Etat courant " << endl;
                                
                                for (map<int, vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end(); it++) {
                                        cout << it->first << "  " << it->second;
                                }
                                cout << "Etat courant OA " << endl;
                                for (map<int, vector<int> >::iterator it = ETaches_courant_OA.begin(); it != ETaches_courant_OA.end(); it++) {
                                        cout << it->first << "  " << it->second;
                                }
                                cout << "vitesse0A choisie " << vitesseOA[i] << endl;
                        }
                        int sortie_value= Construction_Etat_restreint(ETaches_courant,reduction_deadline,Etat_courant_reduit, tache_simuler);
                        Etat_courant_reduit[Etat_courant_reduit.size()-1] = sortie_value;
                        //                        cout << "Etat courant " << Etat_courant_reduit << endl;
                        
                        vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),Etat_courant_reduit);
                        position_etat_suivant = (int)(it-espace_detat_init.begin());
                        //                        cout << "pos suivant : " << espace_detat_init[position_etat_suivant];
                }
                
                
                //@TODO decommenter
                if (affiche_result) {
                        cout << "nvel etat " << espace_detat_init[position_etat_suivant];
                }
                
                // vitesse choisie au temps t dans le cas MDP
                if (i<T) {
                        vitesse.push_back(choix_action[position_etat_suivant]);
                        //                        cout << "VITESSE :  " << choix_action[position_etat_suivant] << endl;
                }
                
                position_etat_courant=position_etat_suivant;
                
                Etat_courant_reduit.clear(); tache_simuler.clear();
        }
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a = aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        
        for (int i=0; i<vitesseOA.size();i++) {
                vitesse_OA.push_back((int)vitesseOA[i]);
        }
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}

vector<double> simulation_restreint_ktachesQlearning(int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<int> & sortie_choix, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int max_deadline_res) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_restreint_ktachesQlearning(espace_detat_init, T, max_deadline,max_period,sortie_choix, dp, jobs_T,max_deadline_res);
        
        if (tracer_courbe)
                tracer_courbe_function(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        cout << "==============> VALEUR <================" << endl;
        cout << "vitesse<-c(" << action_gen;
        cout << "vitesseOA<-c(" << action_gen_OA;
        cout << "taille<-c(" << jobs_T[0];
        cout << "deadline<-c(" << jobs_T[1];
        
        double cout_OA = 0, cout_online = 0;
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += cout_nrj(action_gen[i],max_vitesse);
        }
        cout << "RES ====================>" << endl;
        cout << "cout OA       : " << cout_OA << endl;
        cout << "cout online   : " << cout_online << endl;
        if (cout_OA>cout_online) {
                cout << "<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> GAGN" << (cout_OA-cout_online)/cout_online << endl;
        }
        cout << "RES ====================>" << endl;
        
        if (cout_online > 100000) {
                cout << "STOP probleme vitesse incohérente" << endl;
                cout << cout_online << "   " << cout_OA << endl;
                cout << sortie_bis[3] << " vOA " << sortie_bis[4] << endl;
        }
        vector<double> sortie_simulation;
        sortie_simulation.push_back(cout_online);sortie_simulation.push_back(cout_OA);
        return(sortie_simulation);
}







vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_general_Qlearning(vector<vector<int> > & espace_detat_init,int T,int max_deadline,int max_period, vector<int> choix_action, bool dp, vector<vector<int> > & jobs_T,int max_deadline_res,vector<int> & EtatVisiter) {
        vector<double> aa;
        aa.push_back(0);
        vector<double> dd;
        dd.push_back(0);
        //int fin = T-max_deadline;
        vector<double> proba,deadline_alea,ens_taille;
        int val = 2*T;
        vector<double> d_value(val,0);
        vector<double> w_value, vitesse_OA,vitesse;
        double travail_total = 0,travail_total_OA = 0, proba_select = 0;
        int deadline_value, taille;
        vector<double> nvel_etat, nvel_etat_res, nvel_etat_OA;
        vector<int> etatVisiteQlearning;
        
        max_deadline_res=max_deadline; // Pour le cas non espace d'état réduit
        
        for (int i=0; i<T; i++) {
                taille = jobs_T[0][i];
                deadline_value = jobs_T[1][i];
                if (i>T-max_deadline_res) {
                        taille = 0;
                        deadline_value = 1;
                }
                // d_value : vecteur qui corespond aux futurs états
                // TODO : ajouter au fur et a mesure les valeurs a d_value (addition de tous les coefficients qui suivent le coefficients corrspondant a deadline 
                for (int k=i+deadline_value; k<d_value.size(); k++){
                        d_value[k] += taille;
                }
                aa.push_back(aa[i]+taille);
                
                deadline_alea.push_back(deadline_value);
                
                /*************************************************************/
                /* Détermine l'etat courant et calcul l'etat suivant         */
                /*************************************************************/
                // détermine la position dans le vecteur des deadlines jusqu'ou on peut connaitre
                
                // CAS Non OA, sélection du nouvel etat dans d_value
                nvel_etat.erase(nvel_etat.begin(), nvel_etat.end());
                for (int k = i+1; k<=i+max_deadline; k++) {
                        nvel_etat.push_back(d_value[k]);
                }
                // prendre en compte le travail
                nvel_etat_res = nvel_etat;
                nvel_etat = nvel_etat_res-travail_total;
                for (vector<double>::iterator it = nvel_etat.begin(); it != nvel_etat.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                // CAS OA, prendre en compte le travail OA
                nvel_etat_OA.erase(nvel_etat_OA.begin(), nvel_etat_OA.end());
                for (int k = i+1; k<=i+max_deadline_res; k++) {
                        nvel_etat_OA.push_back(d_value[k]);
                }
                //                nvel_etat_OA = nvel_etat_res-travail_total_OA;
                nvel_etat_OA = nvel_etat_OA-travail_total_OA;
                for (vector<double>::iterator it = nvel_etat_OA.begin(); it != nvel_etat_OA.end(); it++) {
                        if (*it < 0) {
                                *it = 0;
                        }
                }
                
                // Recherche de la position de letat en fonction du nouvel etat considéré
                vector<int> nvel_etat_double;
                for (int lll=0;lll<nvel_etat.size();lll++) {
                        nvel_etat_double.push_back((int)nvel_etat[lll]);
                }
                //                cout << "etat " << nvel_etat_double << " vs " << nvel_etat_OA << endl;
                std::vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(),nvel_etat_double);
                
                vitesse.push_back((double)choix_action[distance(espace_detat_init.begin(), it)]);
                
                //                std::vector<int>::iterator it2 = find(EtatVisiter.begin(),EtatVisiter.end(),(int)distance(espace_detat_init.begin(), it));
                //                if (it2==EtatVisiter.end())
                //                        etatVisiteQlearning.push_back((int)distance(espace_detat_init.begin(), it));
                // vitesse choisie au temps t avec Optimal Available
                
                vitesse_OA.push_back(calcul_vitesse_OA(nvel_etat_OA));
                //                cout << "vitesses: " << vitesse[vitesse.size()-1] << " vs " << vitesse_OA[vitesse_OA.size()-1] << endl;
                
                // quantité totale de travail éxécuté.
                travail_total += vitesse[vitesse.size()-1];
                travail_total_OA += vitesse_OA[vitesse_OA.size()-1];
                w_value.push_back(travail_total);
                ens_taille.push_back(taille);
                proba.push_back(proba_select);
        }
        
        //        if (etatVisiteQlearning.size()!=0) {
        //                cout << "etat visité: " << etatVisiteQlearning << endl;
        //        }
        
        int test = 0;
        for (int i = 0; i< vitesse.size(); i++) {
                test += vitesse[i];        }
        vector<double> a = aa;
        int deb = (int)(a.size());
        // Place les derniers éléments de a qui permettent de "finir la courbe"
        for (int i=deb; i<deb+max_deadline; i++) {
                a.push_back(a[deb-1]);
        }
        // suppression du 0 du debut sur aa
        a.erase(a.begin());
        // adaptation des deadlines à la taille des taches déjà arrivé
        vector<double> d;
        for (int i = 0; i<a.size(); i++) {
                d.push_back(d_value[i]);
        }
        vector<vector<double> > sortie_testAlgo_simulation_et_calcul_vitesse;
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(a);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(d);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(deadline_alea);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(vitesse_OA);sortie_testAlgo_simulation_et_calcul_vitesse.push_back(w_value);
        sortie_testAlgo_simulation_et_calcul_vitesse.push_back(ens_taille);
        return(sortie_testAlgo_simulation_et_calcul_vitesse);
}


vector<double> simulation_Qlearning(double gamma, bool affichage_resultat,int T,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & espace_detat_init, vector<int> vitesse_gen, bool dp, vector<vector<int> > & jobs_T, bool tracer_courbe,int res_max_deadline, vector<int> & EtatVisiter) {
        
        vector<vector<double> > sortie_bis = testAlgo_simulation_et_calcul_vitesse_general_Qlearning(espace_detat_init, T, max_deadline,max_period,vitesse_gen, dp, jobs_T, res_max_deadline, EtatVisiter);
        
        if (tracer_courbe)
                Tracer_simulation_VI(sortie_bis);
        
        vector<double> action_gen = sortie_bis[3];
        vector<double> action_gen_OA = sortie_bis[4];
        
        double cout_OA = 0, cout_online = 0;
        
        for (int i=0; i<action_gen.size();i++) {
                cout_OA += pow(gamma,i)*cout_nrj(action_gen_OA[i],max_vitesse);
                cout_online += pow(gamma,i)*cout_nrj(action_gen[i],max_vitesse);
        }
        
        //        bool affichage_resultat=true;
        if (affichage_resultat) {
                cout << "==============> VALEUR Qlearning <================" << endl;
                cout << "  vitesse<-c(" << action_gen;
                cout << "vitesseOA<-c(" << action_gen_OA;
                cout << "   taille<-c(" << jobs_T[0];
                cout << " deadline<-c(" << jobs_T[1];
        }
        
        vector<double> sortie; sortie.push_back(cout_online);sortie.push_back(cout_OA);
        return(sortie);
}



