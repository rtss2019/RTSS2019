
//
//  backward_resolution.cpp
//  finite_case
//
//  Created by Stéphan on 02/05/2017.
//  Copyright © 2017 Stéphan. All rights reserved.
//

#include "backward_resolution.hpp"
#include "parser.hpp"
#include <algorithm>
#include <iterator>
#include <list>
#include <math.h>
#include <cmath>
#include <assert.h>
#include <fstream>
#include <iomanip>
using namespace std;
bool visible_general=false;

/***** AFFICHE VECTEUR ********/
void Affiche(const vector<double> & vecteur) {
        // utilisation d'un itérateur
        for (vector<double>::const_iterator i = vecteur.begin(); i != vecteur.end(); ++i)
                cout << *i << ' ';
        return void();
}
void Affiche(const vector<int> & vecteur) {
        // utilisation d'un itérateur
        for (vector<int>::const_iterator i = vecteur.begin(); i != vecteur.end(); ++i)
                cout << *i << ' ';
        return void();
}
/***** AFFICHE ESPACE DETAT ********/
void Affiche(const vector<vector<double> > & vecteur) {
        // utilisation d'un itérateur
        for (int i = 0; i < (int)vecteur.size(); i++) {
                cout << i << ":  "; Affiche(vecteur[i]); cout<<endl;
        }
        return void();
}
void Affiche(const vector<vector<vector<double> > > & vecteur){
        for (int i = 0; i < (int)vecteur.size(); i++) {
                cout << i << ":  "; Affiche(vecteur[i]); cout<<endl;
        }
        return void();
}

/***** AFFICHE ESPACE DETAT ********/
void Affiche(const vector<vector<int> > & vecteur) {
        // utilisation d'un itérateur
        for (int i = 0; i < (int)vecteur.size(); i++) {
                cout << "       "; Affiche(vecteur[i]); cout<<endl;
        }
        return void();
}
void Affiche_int(const vector<vector<vector<int> > > & vecteur){
        for (int i = 0; i < (int)vecteur.size(); i++) {
                cout << i << ":  "; Affiche(vecteur[i]); cout<<endl;
        }
        return void();
}

/***** AJOUT SCALAIRE ********/
vector<int> Affiche(vector<int> & vecteur, int scalaire) {
        for (int i = 0; i < (int)vecteur.size(); i++) {
                vecteur.at(i) = vecteur.at(i) + scalaire;
        }
        return(vecteur);
}
/***** AJOUT VECTOR ********/
vector<int> add_vector(vector<int> & vecteur1, vector<int> & vecteur2) {
        vector<int> vecteur;
        for (int i = 0; i < (int)vecteur1.size(); i++) {
                vecteur.at(i) = vecteur1.at(i) + vecteur2.at(i);
        }
        return(vecteur);
}

/***************************************************************************************************************************************/
// cout_nrj: coût énergétique lié à la vitesse => fonction de coût de backward itération (Ici fonction coût = fonction cubique)
//           donne le cout en fonction de la vitesse, choix d'une fonction quadratique pour commencer
//
// sortie:
//          double : energie, énergie consommé par le processeur sous une vitesse fixée
//_re
// entrée :
//          vitesse : vitesse à laquelle fonctionne le processeur
/***************************************************************************************************************************************/
double cout_nrj(const double & vitesse, double vitesseMax) {
        double energie;
        if (vitesse <=vitesseMax) {
                energie = vitesse*vitesse*vitesse;
        } else {
                energie = +INFINITY;
        }
        return energie;
}

double cout_nrj_learning(const double & vitesse, double vitesseMax, int vitesseMin) {
        double energie;
        if (vitesse <=vitesseMax && vitesse >=vitesseMin) {
                energie = vitesse*vitesse*vitesse;
        } else {
                energie = +INFINITY;
        }
        return energie;
}


/********************************** ESPACE DETAT ***********************************************/
/***********************************************************************************************/
//  calculEspaceDetat: crée l'espace detat adequat
//  Entrée: max_deadline, max_size, nbre_vitesse
//  Sortie: l'espace d'etat
/***********************************************************************************************/
vector<vector<double> > calculEspaceDetat(vector<vector<double> > & nombre,int k,int size,int max_deadline,int borne) {
        vector<vector<double> > new_nombre;
        if (k>0) {
                for (int i=0; i < nombre.size(); i++) {
                        int fin = (max_deadline-k+1)*size - nombre.at(i).at(nombre.at(i).size()-1);
                        for (int s=0; s < fin+1; s++) {
                                vector<double> nombre_inter;
                                nombre_inter.push_back(s);
                                for (int k =0; k < nombre.at(i).size(); k++) {
                                        nombre_inter.push_back(nombre.at(i).at(k)+s);
                                }
                                new_nombre.push_back(nombre_inter);
                        }
                }
                return(calculEspaceDetat(new_nombre,k-1,size,max_deadline,borne));
        } else {
                return(nombre);
        }
}

/***********************************************************************************************/
// creationEspaceDetat: crée l'espace detat adequat
//
// sortie:
//          vector<vector<double> > espace_detat, espace d'état créé
//
// entrée :
//          max_size            : int, taille maximale d'une tache
//          max_deadline        : int, deadline maximale d'une tache
//          espace_detat        : vector<vector<double> >, espace d'état à remplir
/***********************************************************************************************/
vector<vector<double> > creation_espace_detat(int max_size, int max_deadline, vector<vector<double> > & espace_detat) {
        int borne = max_deadline * max_size;
        vector<vector<double> > nombre;
        for (int i=0; i<max_size+1; i++) {
                vector<double> val;
                val.push_back(i);
                nombre.push_back(val);
                val.pop_back();
                
        }
        espace_detat=nombre;
        espace_detat = calculEspaceDetat(espace_detat,max_deadline-1,max_size,max_deadline,borne);
        return(espace_detat);
}



vector<vector<int> > calculEspaceDetat(vector<vector<int> > & nombre,int k,int size,int max_deadline,int borne) {
        vector<vector<int> > new_nombre;
        if (k>0) {
                for (int i=0; i < nombre.size(); i++) {
                        int fin = (max_deadline-k+1)*size - nombre.at(i).at(nombre.at(i).size()-1);
                        for (int s=0; s < fin+1; s++) {
                                vector<int> nombre_inter;
                                nombre_inter.push_back(s);
                                for (int k =0; k < nombre.at(i).size(); k++) {
                                        nombre_inter.push_back(nombre.at(i).at(k)+s);
                                }
                                new_nombre.push_back(nombre_inter);
                        }
                }
                return(calculEspaceDetat(new_nombre,k-1,size,max_deadline,borne));
        } else {
                return(nombre);
        }
}

/***********************************************************************************************/
// creationEspaceDetat: crée l'espace detat adequat
//
// sortie:
//          vector<vector<double> > espace_detat, espace d'état créé
//
// entrée :
//          max_size            : int, taille maximale d'une tache
//          max_deadline        : int, deadline maximale d'une tache
//          espace_detat        : vector<vector<double> >, espace d'état à remplir
/***********************************************************************************************/
vector<vector<int> > creation_espace_detat(int max_size, int max_deadline, vector<vector<int> > & espace_detat) {
        int borne = max_deadline * max_size;
        vector<vector<int> > nombre;
        for (int i=0; i<max_size+1; i++) {
                vector<int> val;
                val.push_back(i);
                nombre.push_back(val);
                val.pop_back();
        }
        espace_detat=nombre;
        espace_detat = calculEspaceDetat(espace_detat,max_deadline-1,max_size,max_deadline,borne);
        return(espace_detat);
}

/***********************************************************************************************/
//  suppression_etat_inutile:
//  Entrée: max_deadline, max_size
//  Sortie: l'espace d'etat
/***********************************************************************************************/
vector<vector<int> > suppression_etat_inutile(int max_size,int max_deadline,vector<vector<int> > & espace_detat_init) {
        vector<int> etat_int;
        int etat_supprime = 0;
        for (int i = 0;i < espace_detat_init.size(); i++) {
                int borne = max_deadline;
                etat_int = espace_detat_init[i];
                //                cout << "etat intermediaire " << etat_int << endl;
                //                cout << "i " << i << endl;
                //                if (espace_detat_init[i][0] > max_size) {
                //                        espace_detat_init.erase(espace_detat_init.begin()+i);
                //                        etat_supprime = 1;
                //                        cout << "un tetat supprime" << endl;
                //                }
                //                else if (espace_detat_init[i][1] > 2*max_size) {
                //                        espace_detat_init.erase(espace_detat_init.begin()+i);
                //                        etat_supprime = 1;
                //                        cout << "un tetat supprime" << endl;
                //                }
                //                else {
                for (int d=0; d < borne; d++) {
                        if (espace_detat_init[i][d] > (i+2)*max_size) {
                                espace_detat_init.erase(espace_detat_init.begin()+i);
                                etat_supprime = 1;
                                cout << "un tetat supprime" << endl;
                                break;
                        }
                }
                if (etat_supprime == 1) {
                        i=i-1;
                        etat_supprime=0;
                }
        }
        return(espace_detat_init);
}



/********************************** ETAT SUIVANT **************************************************************************************************************************/
/**************************************************************************************************************************************************************************/
// recherche_etat_suiv : donne la position de l'état suivant en fonction de l'etat courant et de la deadline de tache considéré et de la taille de tache considéré
//
// sortie:
//          int, état trouvé lors de la recherche
//
// entrée :
//          indice_etat_prec      : int, ensemble des états possible
//          deadline_tache        : int, taille maximale de la tache considéré
//          taille_tache          : int, deadline maximale de la tache considéré
//          vitesse_pas_prec      : int, vitesse considéré pour le pas suivant au pas prec
//          espace_detat_init     : vector<vector<double> >, espace d'état à parcourir
//
/**************************************************************************************************************************************************************************/


int recherche_etat_suiv(int indice_etat_prec, int deadline_tache, int taille_tache, int vitesse_pas_prec, vector<vector<double> > & espace_detat_init) {
        vector<double> etat_prec = espace_detat_init.at(indice_etat_prec); vector<double> etat_suiv;
        for (int i=1; i < etat_prec.size(); i++) {
                if (etat_prec[i] > 0) {
                        etat_suiv.push_back(etat_prec[i]);
                } else {
                        etat_suiv.push_back(0);
                }
        }
        etat_suiv.push_back(etat_prec[etat_prec.size()-1]);
        etat_suiv = etat_suiv - vitesse_pas_prec;
        for (int i=0; i< etat_suiv.size(); i++) {
                if (etat_suiv[i] < 0) {
                        etat_suiv[i] = 0;
                }
        }
        for (int i = deadline_tache-1; i<etat_suiv.size(); ++i) {
                etat_suiv[i] += taille_tache;
        }
        // etat_suiv est un etat et il faut le rechercher dans la liste
        std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(), etat_suiv);
        return((int)(distance(espace_detat_init.begin(),it)));
}

int recherche_etat_suiv_restreint(int indice_etat_prec, int deadline_tache, int taille_tache, int reduction_deadline, int reduction_taille, int vitesse_pas_prec, vector<vector<double> > & espace_detat_init) {
        vector<double> etat_prec = espace_detat_init.at(indice_etat_prec); vector<double> etat_suiv;
        for (int i=1; i < etat_prec.size()-1; i++) {                                                                            // Décalage du vecteur dans le temps sur les k premiers pas
                if (etat_prec[i] > 0) {
                        etat_suiv.push_back(etat_prec[i]);
                } else {
                        etat_suiv.push_back(0);
                }
        }
        etat_suiv.push_back(etat_prec[reduction_deadline-1]+reduction_taille);                                                  // Ajout de la taille du à reduction_deadline calculé de manière statistique
        etat_suiv.push_back(etat_prec[etat_prec.size()-1]);                                                                     // Ajout du dernier élément Delta
        etat_suiv = etat_suiv - vitesse_pas_prec;                                                                               // Enleve la vitesse proposée
        for (int i=0; i< etat_suiv.size(); i++) {
                if (etat_suiv[i] < 0) {
                        etat_suiv[i] = 0;
                }
        }
        for (int i = deadline_tache-1; i<etat_suiv.size(); ++i) {
                etat_suiv[i] += taille_tache;
        }
        //@TODO rajouter le decalage de reduction_deadline+1 à la fin de l'ensemble des taches considérées
        std::vector<vector<double> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(), etat_suiv);         // etat_suiv est un etat et il faut le rechercher dans la liste
        return((int)(distance(espace_detat_init.begin(),it)));
}

int recherche_etat_suiv_restreint_ktaches(int position_etat_courant, int deadline_tache, int taille_tache, vector<int> & NouvellesTaches, int vitesse_pas_prec, vector<vector<int> > & espace_detat_init) {
        vector<int> etat_prec = espace_detat_init.at(position_etat_courant); vector<int> etat_suiv = etat_prec;
        
        //        cout << " tache arrivée " << taille_tache << "  " << deadline_tache << " vitesse " << vitesse_pas_prec << "   taches:  " << NouvellesTaches;
        //        cout << "etat prec " << etat_prec << endl;
        // 1) Decalage en temps
        for (int i=0; i < etat_prec.size()-1; i++) {
                if (i%2!=0)
                        etat_suiv[i] = max(etat_prec[i]-1,0);
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 2) Rajoute les NouvellesTaches
        vector<int>::iterator it = etat_suiv.begin()+etat_prec.size()-1;
        for (int i=0; i<NouvellesTaches.size(); i++) {
                if (NouvellesTaches[i]!=0) {
                        etat_suiv.insert(it, NouvellesTaches[i]);
                        it=etat_suiv.begin()+etat_suiv.size()-1;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 3) enleve les 0
        int test_value=0;
        int i=0;
        while (etat_suiv[i]==0) {
                if (etat_suiv[i]==0 && i<etat_suiv.size()-1) {
                        etat_suiv.erase(etat_suiv.begin());
                        etat_suiv.erase(etat_suiv.begin());
                } else {
                        break;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 4) soustrait vitesse
        int vitesse_value = vitesse_pas_prec;
        int res;
        for (int i=0; i<etat_suiv.size()-1; i++) {
                if (i%2==0 && vitesse_value>0) {
                        res = etat_suiv[i];
                        etat_suiv[i]=max(res-vitesse_value,0);
                        vitesse_value-=res;
                }
        }
        etat_suiv[etat_suiv.size()-1]-=vitesse_pas_prec;
        //        cout << "etat suiv " << etat_suiv << endl;
        // 4bis) enleve les 0
        test_value=0;
        i=0;
        while (etat_suiv[i]==0) {
                if (etat_suiv[i]==0 && i<etat_suiv.size()-1) {
                        etat_suiv.erase(etat_suiv.begin());
                        etat_suiv.erase(etat_suiv.begin());
                } else {
                        break;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 5) Ajout de la Tache deadline_tache/taille_tache au bon endroit
        bool fin = false;
        if (taille_tache!=0) {
                // il faut ajouter taille_tache,deadline_tache
                for (int j=0;j<etat_suiv.size()-1;j++) {
                        if (j%2!=0) { // cas etude deadline
                                if (etat_suiv[j]>deadline_tache) {
                                        etat_suiv.insert(etat_suiv.begin()+j-1, taille_tache);
                                        etat_suiv.insert(etat_suiv.begin()+j, deadline_tache);
                                        fin =true;
                                        break;
                                }
                        }
                }
                if (!fin) {
                        etat_suiv.insert(etat_suiv.begin()+etat_suiv.size()-1, taille_tache);
                        etat_suiv.insert(etat_suiv.begin()+etat_suiv.size()-1, deadline_tache);
                }
                etat_suiv[etat_suiv.size()-1]+=taille_tache;
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 6) coupe pour avoir seulement k taches prises en compte
        vector<int> etat_suiv_new;
        for (int i=0;i<etat_suiv.size(); i++) {
                if (i<etat_prec.size()-1) {
                        etat_suiv_new.push_back(etat_suiv[i]);
                } else if (i==etat_suiv.size()-1) {
                        etat_suiv_new.push_back(etat_suiv[i]);
                }
        }
        etat_suiv=etat_suiv_new;
        //        cout << "etat suiv " << etat_suiv << endl;
        // 7) Rajoute des 0 s'il ne fait pas la bonne longueur
        while (etat_suiv.size()!=etat_prec.size()) {
                etat_suiv.insert(etat_suiv.begin(), 0);
        }
        
        // rajouter le decalage de reduction_deadline+1 à la fin de l'ensemble des taches considérées
        std::vector<vector<int> >::iterator it2 = find(espace_detat_init.begin(),espace_detat_init.end(), etat_suiv);         // etat_suiv est un etat et il faut le rechercher dans la liste
        //        cout << "position etat suivant  " << (int)(distance(espace_detat_init.begin(),it)) << "   " << espace_detat_init[(int)(distance(espace_detat_init.begin(),it))];
        int sortie =(int)(distance(espace_detat_init.begin(),it2));
        if (sortie==(int)(espace_detat_init.size())) {;
                sortie=RAND_MAX;
                //                //@DONE Rajout de la suppressiond e setats qui ne peuvent pas être visité
                //                std::vector<vector<int> >::iterator it = find(espace_detat_init.begin(),espace_detat_init.end(), etat_prec);         // etat_suiv est un etat et il faut le rechercher dans la liste
                //                position = (int)(it - espace_detat_init.begin());
                //                
                //                // supprimer les positions de stockahge du vecteurs nrj + vitesse
                //                espace_detat_init.erase(it);
                if (visible_general==true) {
                        cout << "Etat non existant";
                        cout << endl;
                }
        }
        if (visible_general==true) {
                cout << "etat_suiv " << etat_suiv;
        }
        return sortie;
}


int recherche_etat_suiv_restreint_ktaches_bis(int position_etat_courant, int deadline_tache, int taille_tache, vector<int> & NouvellesTaches, int vitesse_pas_prec, vector<vector<int> > & espace_detat_init, vector<int> & etat_sortie) {
        vector<int> etat_prec = espace_detat_init.at(position_etat_courant); vector<int> etat_suiv = etat_prec;
        
        //        cout << " tache arrivée " << taille_tache << "  " << deadline_tache << " vitesse " << vitesse_pas_prec << "   taches:  " << NouvellesTaches;
        //        cout << "etat prec " << etat_prec << endl;
        cout << "debut du calcul pour la recherche " << endl;
        
        // 1) Decalage en temps
        for (int i=0; i < etat_prec.size()-1; i++) {
                if (i%2!=0)
                        etat_suiv[i] = max(etat_prec[i]-1,0);
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 2) Rajoute les NouvellesTaches
        vector<int>::iterator it = etat_suiv.begin()+etat_prec.size()-1;
        for (int i=0; i<NouvellesTaches.size(); i++) {
                if (NouvellesTaches[i]!=0) {
                        etat_suiv.insert(it, NouvellesTaches[i]);
                        it=etat_suiv.begin()+etat_suiv.size()-1;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 3) enleve les 0
        int test_value=0;
        int i=0;
        while (etat_suiv[i]==0) {
                if (etat_suiv[i]==0 && i<etat_suiv.size()-1) {
                        etat_suiv.erase(etat_suiv.begin());
                        etat_suiv.erase(etat_suiv.begin());
                } else {
                        break;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 4) soustrait vitesse
        int vitesse_value = vitesse_pas_prec;
        int res;
        for (int i=0; i<etat_suiv.size()-1; i++) {
                if (i%2==0 && vitesse_value>0) {
                        res = etat_suiv[i];
                        etat_suiv[i]=max(res-vitesse_value,0);
                        vitesse_value-=res;
                }
        }
        etat_suiv[etat_suiv.size()-1]-=vitesse_pas_prec;
        //        cout << "etat suiv " << etat_suiv << endl;
        // 4bis) enleve les 0
        test_value=0;
        i=0;
        while (etat_suiv[i]==0) {
                if (etat_suiv[i]==0 && i<etat_suiv.size()-1) {
                        etat_suiv.erase(etat_suiv.begin());
                        etat_suiv.erase(etat_suiv.begin());
                } else {
                        break;
                }
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 5) Ajout de la Tache deadline_tache/taille_tache au bon endroit
        bool fin = false;
        if (taille_tache!=0) {
                // il faut ajouter taille_tache,deadline_tache
                for (int j=0;j<etat_suiv.size()-1;j++) {
                        if (j%2!=0) { // cas etude deadline
                                if (etat_suiv[j]>deadline_tache) {
                                        etat_suiv.insert(etat_suiv.begin()+j-1, taille_tache);
                                        etat_suiv.insert(etat_suiv.begin()+j, deadline_tache);
                                        fin =true;
                                        break;
                                }
                        }
                }
                if (!fin) {
                        etat_suiv.insert(etat_suiv.begin()+etat_suiv.size()-1, taille_tache);
                        etat_suiv.insert(etat_suiv.begin()+etat_suiv.size()-1, deadline_tache);
                }
                etat_suiv[etat_suiv.size()-1]+=taille_tache;
        }
        //        cout << "etat suiv " << etat_suiv << endl;
        // 6) coupe pour avoir seulement k taches prises en compte
        vector<int> etat_suiv_new;
        for (int i=0;i<etat_suiv.size(); i++) {
                if (i<etat_prec.size()-1) {
                        etat_suiv_new.push_back(etat_suiv[i]);
                } else if (i==etat_suiv.size()-1) {
                        etat_suiv_new.push_back(etat_suiv[i]);
                }
        }
        etat_suiv=etat_suiv_new;
        //        cout << "etat suiv " << etat_suiv << endl;
        // 7) Rajoute des 0 s'il ne fait pas la bonne longueur
        while (etat_suiv.size()!=etat_prec.size()) {
                etat_suiv.insert(etat_suiv.begin(), 0);
        }
        cout << "fin du calcul pour la recherche " << endl;
        // rajouter le decalage de reduction_deadline+1 à la fin de l'ensemble des taches considérées
        std::vector<vector<int> >::iterator it2 = find(espace_detat_init.begin(),espace_detat_init.end(), etat_suiv);         // etat_suiv est un etat et il faut le rechercher dans la liste
        //        cout << "position etat suivant  " << (int)(distance(espace_detat_init.begin(),it)) << "   " << espace_detat_init[(int)(distance(espace_detat_init.begin(),it))];
        int sortie =(int)(distance(espace_detat_init.begin(),it2));
        if (sortie==(int)(espace_detat_init.size())) {
                sortie=RAND_MAX;
        }
        etat_sortie = etat_suiv;
        return sortie;
}

/***********************************************************************************************************************************************/
// Fonction calculant la VITESSE OPTIMALE et la QUANTITE D'ENERGIE associé pour l'état position_etat_courant
//
// utilise la fonction recherche_etat_suiv,
//
// sortie:
//          vitesse_et_energie : vector<vector<double> >
//                      nrj              : vector<double>, énergie utilisé pour le calcul
//                      vitesse          : vector<double>, vitesse optimale calculée (meilleur vitesse
//
// entrée :
//          position_etat_courant : vector<vector<double> >, ensemble des états possible
//          espace_detat_init                : vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          nrj_prec              : vector<double>, taille maximale d'un job
//          p_size                : vector<double>, probabilité de la taille d'un job
//          p_deadline            : vector<double>, probabilité de la deadline d'un job
//          max_vitesse           : int, vitesse maximale utilisée
//          deadline_tache        : int, taille maximale de la tache considéré
//          taille_tache          : int, deadline maximale de la tache considéré
//
// @TODO correspond à ce qu'il faut modifier pour introduire la probabilité de la deadline
/***********************************************************************************************************************************************/

vector<double> calcul_meilleur_vitesse(int position_etat_courant,vector<vector<double> > & espace_detat_init, vector<double> nrj_prec,vector<double> p_size, vector<double> p_deadline, int max_vitesse,int deadline_tache,int taille_tache) {
        bool etat_analyse=espace_detat_init[position_etat_courant][0]==0 && espace_detat_init[position_etat_courant][1]==6 && espace_detat_init[position_etat_courant][2]==12;
        vector<double> etat_courant = espace_detat_init[position_etat_courant];
        vector<double> valeur_nrj_suiv_res;                                             // l'ensemble des valeurs d'nrj possibles en testant toutes les vitesses autorisées
        // parcours l'ensemble des vitesses disponibles et regarde laquelle est la meilleur:
        // On part du minimum possible, i.e. on execute la vitesse qui fait tout le travail au pas de temps suivant, et on vérifie que l'on est audessus des pas de temps qui sont déjà présent en mémoire
        vector<double> borne_inf_vitesse;
        // etat_courant[i] - (i-1) * max_vitesse
        for (int i = 0;i < etat_courant.size(); i++) {                                 // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
                borne_inf_vitesse.push_back(ceil((double)etat_courant[i]/(double)(i+1)));
        }
        // VITESSE MAXIMALE des vitesses a respecter
        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());
        // BORNE SUR LES VITESSES par : soit la vitesse max permise, soit par la vitesse qui effectue tout le travail déjà arrivé.
        int arret = min(etat_courant[etat_courant.size()-1],(double)max_vitesse);
        // On boucle sur toutes les vitesses, on regarde les valeurs obtenues et on prend la vitesse et l'energie minimal, ie celle qui consomme le moins d'énergie au cours du temps (on teste avec toutes les vitesses et toutes les tailles de tache possible).
        vector<double> vitesse_possible,indice_etat_suivant_tache;
        
        for (int i = borneInfVitesse; i <= arret; i++) {                                                // BOUCLE sur les VITESSES = i
                // CAS PAS D'ARRIVEE DE TACHE, on enregistre sa position
                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
                indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,0,0,i,espace_detat_init));
                // @TODO double BOUCLE deadline et size pour inclure le vecteur p_deadline
                //  for (int deadline = 1; deadline <= deadline_tache; deadline++) {
                for (int size = 1; size <= taille_tache; size++) {
                        // CAS TACHE TAILLE s ARRIVE : on enregistre sa position
                        indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,deadline_tache,size,i,espace_detat_init));
                        //                        indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,deadline,size,i,espace_detat_init));
                }
                // TODO determiner la position dans le vecteur
                
                double cout_valeur_nrj_suiv_res = cout_nrj(i,max_vitesse);
                
                // int position_vecteur_indice_etat_suivant=0;
                // for (int val_p_deadline = 0; val_p_deadline < p_deadline.size(); val_p_deadline++) {
                for (int val_p_size = 0; val_p_size < p_size.size(); val_p_size++) {
                        cout_valeur_nrj_suiv_res += p_size[val_p_size]*nrj_prec[indice_etat_suivant_tache[val_p_size]];
                        if (etat_analyse) {
                                //                                cout << p_size[val_p_size] << "  " << nrj_prec[indice_etat_suivant_tache[val_p_size]] << " " << espace_detat_init[indice_etat_suivant_tache[val_p_size]];
                        }
                        // @TODO cout_valeur_nrj_suiv_res += p_deadline[val_p_deadline] * p_size[val_p_size] * nrj_prec[indice_etat_suivant_tache[position_vecteur_indice_etat_suivant]];
                        // iteration++;
                        //}
                }
                valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res);
                vitesse_possible.push_back(i);
        }
        std::vector<double>::iterator it =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end());
        // @TODO remettre en cause l'utilisation de la fonction distance => l'indice ne correspond pas forcément à la distance ????
        double pos = distance(valeur_nrj_suiv_res.begin(), it);
        double nrj;
        int vitesse;
        if (it!= valeur_nrj_suiv_res.end()) {
                //                cout << " VISITE " << position_etat_courant << "  : " << espace_detat_init[position_etat_courant] << endl;
                nrj = *it; vitesse = vitesse_possible[(int)pos];
        } else {
                //                cout << "JAMAIS VISITE " << position_etat_courant << "  : " << espace_detat_init[position_etat_courant]<< endl;
                nrj = 0; vitesse = 0;
        }
        vector<double> vitesse_et_energie; vitesse_et_energie.push_back(nrj); vitesse_et_energie.push_back((double)vitesse);
        return(vitesse_et_energie);
}


/*************************************************************************************************************************/
// Simulation_general
// 2 étapes:
//            - repérage des nouveaux états
//            - calcul d'actions et nrj
//
// utilise la fonction calcul_meilleur_vitesse
//
// sortie:
//          sortie_simul: vector<vector<double> >
//                      vitesseCast      : vector<double>, vitesse optimale calculée
//                      nrj              : vector<double>, énergie utilisé pour le calcul
//
// entrée :
//          espace_detat_init : vector<vector<double> >, ensemble des états possible
//          p_size            : vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          max_vitesse       : int, vitesse maximale utilisée
//          vitesse           : vector<double>, probabilité de l'échéance d'un job, p_deadline.size() = max_deadline
//          nrj_prec          : vector<double>, taille maximale d'un job
//          nrj               : vector<double>, échéance maximale d'un job
//          deadline_tache    : int, taille maximale de la tache considéré
//          taille_tache      : int, deadline maximale de la tache considéré
//
// TODO ENLEVER les variables présentes mais non utilisées : choix_action_int,nrj,t,T
/*************************************************************************************************************************/


vector<vector<double> > simulation_general(vector<vector<double> > & espace_detat_init,vector<double> p_size,vector<double> p_deadline,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj,int deadline_tache,int taille_tache) {
        vector<double> vitesse_et_energie;
        for (int s  = 0; s < espace_detat_init.size(); s++) {
                vitesse_et_energie = calcul_meilleur_vitesse(s, espace_detat_init, nrj_prec,p_size,p_deadline, max_vitesse, deadline_tache, taille_tache);
                nrj[s] = vitesse_et_energie[0];
                vitesse[s] = vitesse_et_energie[1];
        }
        vector<vector<double> > sortie_simul;
        std::vector<double> vitesseCast(vitesse.begin(), vitesse.end());
        sortie_simul.push_back(vitesseCast);
        sortie_simul.push_back(nrj);
        return(sortie_simul);
}


double calcul_vitesse_OA(int etat,vector<vector<double> > espace_detat_init) {
        vector<double> pente;
        for (int i=0; i<espace_detat_init[etat].size(); i++) {
                pente.push_back((double)espace_detat_init[etat][i]/(double)(i+1));
        }
        return(ceil(*max_element(pente.begin(), pente.end())));
}
double calcul_vitesse_OA(vector<double> & nvel_etat_OA) {
        vector<double> pente;
        for (int i=0; i<nvel_etat_OA.size(); i++) {
                pente.push_back((double)nvel_etat_OA[i]/(double)(i+1));
        }
        return(ceil(*max_element(pente.begin(), pente.end())));
}


/*****************************************************************************************************/
// Test Algo : Création de la courbe qui sera utilisée en offline : en sortie, on a: a et d :
// On peut donc déterminer la courbe offline
// On peut aussi déterminer à partir de l'état de départ la courbe online simulé
// entrée: la proba d'arrivee de tache, le temps detude
// sortie: une liste comprenant une trajectoire des taches arrivées, les deadlines et, les dates darrivée de taches
/*****************************************************************************************************/

/*vector<vector<double> > testAlgo(vector<double> p,int T,int max_deadline) {
 vector<double> aa(0);
 int fin = T-max_deadline;
 vector<double> proba;
 // Effectue la somme des p
 vector<double> sum_p;
 double acc = 0;
 for (int i = 0; i < p.size(); i++) {
 acc += p[i];
 sum_p.push_back(acc);
 }
 cout << "sump  " << sum_p;
 
 int deadlinet[] = {3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3};
 int probat[] = {3,0,6,0,3,3,3,3,3,3,6,3,6,6,3,3,3};
 double nbre_alea;
 for (int i=0; i<fin; i++) {
 //nbre_alea= runif(1,0,1);
 //proba_select<-which(sum_p[]>=nbre_alea)[1];
 aa.push_back(aa[aa.size()-1]+probat[i]);
 //proba<-c(proba,proba_select);
 }
 
 
 fin = max_deadline-1;
 //aa<-aa[1:length(aa)];
 for (int i=0; i<fin; i++) {
 aa.push_back(aa[aa.size()-1]);
 }
 int fini = T-max_deadline;
 
 vector<double> dd;
 
 for (int i=0; i<fin; i++) {
 dd<-c(dd,0);
 }
 dd<-c(dd,aa[1:fini],aa[fini+1]);
 a<-aa;
 d<-dd;
 vector<vector<double> > sortie;
 sortie.push_back(aa);
 sortie.push_back(dd);
 sortie.push_back(proba);
 return(sortie);
 }*/


/*****************************************************************************************************/
// Simulation à partir de choix_action déterminé par l'algo de programmation dynamique
// On part de l'etat (O,0,0,0)
// entrée: espace_detat_d0: espace detat, spointOffline: points de la courbe offline, choix_action: actions de lalgo, dernchgmt: 4 ou 5
// sortie: action_gen: action genere, val_w: somme des vaelurs ajouté de w, Futur: valeurs des deadlines futur
/*****************************************************************************************************/

/*vector<vector<double> > simulation(vector<vector<double> > espace_detat_init, vector<vector<double> > pointOffline,vector<vector<double> > choix_action,int max_deadline,int T) {
 cout << "simulation" << endl;
 fin<-length(pointOffline[[1]])-max_deadline;
 w_value<-NULL;
 vitesse_OA<-NULL;
 vitesse<-NULL;
 travail_total<-0;
 travail_total_OA<-0;
 for (ts in 1:fin) {
 cout << "------- pas ------" << endl;
 cout << ts << endl;
 //cout << nvel_etat);
 top<-ts+max_deadline;
 deb<-ts+1;
 nvel_etat<-NULL;
 // Détermine l'état suivant
 for (j in deb:top) {
 nvel_etat<-c(nvel_etat,pointOffline[[2]][j]);
 }
 cout << nvel_etat << endl;
 nvel_etat_OA<-nvel_etat-travail_total_OA;
 nvel_etat<-nvel_etat-travail_total;
 nvel_etat[which(nvel_etat < 0)] = 0;
 nvel_etat_OA[which(nvel_etat_OA < 0)] = 0;
 nvel_etat<-list(nvel_etat);
 nvel_etat_OA<-list(nvel_etat_OA);
 // vitesse choisie au temps t avec backward resolution
 etat<-which(espace_detat_init %in% nvel_etat);
 etat_OA<-which(espace_detat_init %in% nvel_etat_OA);
 if (ts<T) {
 vitesse<-c(vitesse,choix_action[[T-ts]][etat]);
 cout << "vitesse" << endl;
 cout << choix_action[[T-ts]][etat] << endl;
 }
 // vitesse choisie au temps t avec Optimal Available
 vitesse_OA<-c(vitesse_OA,calcul_vitesse_OA(etat_OA,espace_detat_init));
 cout << "vitesse Optimal Available" << endl;
 cout << calcul_vitesse_OA(etat_OA,espace_detat_init) << endl;
 // quantité totale de travail éxécuté.
 travail_total<-travail_total+vitesse[length(vitesse)];
 travail_total_OA<-travail_total_OA+vitesse_OA[length(vitesse_OA)];
 w_value<-c(w_value,travail_total);
 }
 vector<vector<double> > sortie;
 sortie.push_back(vitesse);
 sortie.push_back(vitesse_OA);
 sortie.push_back(w_value);
 return(sortie);
 }*/


vector<double> nvel_etat_ac_nvel_tache(int i, vector<double> nvel_etat, int deadline_value, int taille, int deadline_max, int max_deadline, vector<double> d_value) {
        int top = deadline_max; int fini,arret; int deb = i+1;
        // Détermine l'état suivant
        for (int j=deb; j < top; j++) {                                 //le mon sert a garder que les taches qui ont été éxécutées
                nvel_etat.push_back(d_value[j]);
        }
        if (nvel_etat.size() != max_deadline) {
                fini = (int)nvel_etat.size();
                arret = max_deadline-1;
                for (int alpha = fini; alpha < arret; alpha++) {
                        if (nvel_etat.size() != 0) {
                                nvel_etat.push_back(nvel_etat[fini]);
                        } else {
                                nvel_etat.push_back(0);
                        }
                }
        }
        return(nvel_etat);
}



// Utilisé dans simulation_job
double choisir_proba(vector<double> & sum_pa1, double & nbre_alea) {
        double proba_select;
        for (int k =0; k<sum_pa1.size(); k++) {
                if (sum_pa1[k] >= nbre_alea) {
                        proba_select = k;
                        break;
                }
        }
        return proba_select;
}


/***********************************************************************************************/
// simulation_job : Simule des jobs : nouveau jobs ou pas selon la proba donner en parametre
//
// Utilise la fonction choisir_proba
//
// sortie:
//          sortie_simulation_job : vector<vector<double> >
//                              sortie_taille   : vector<double>, ensemble des tailles choisies aléatoirement sur T pas de temps.
//                              sortie_deadline : vector<double>, ensemble des deadlines choisies aléatoirement sur T pas de temps.
//
// entrée :
//          max_size     : int, taille maximale des jobs
//          max_deadline : int, deadline maximale des jobs
//          p_size       : vector<double>, probabilité de la taille d'un job
//          iteration    : int, ?????????????
//          T            : int, échéance sur laquelle on fait notre étude
/***********************************************************************************************/
//@TODO Généraliser pour le cas où l'on a des périodes différentes
vector<vector<double> > simulation_job(int max_size,int max_deadline, vector<double> p_size, int iteration, int T) {
        vector<vector<double> > sortie_simulation_job;
        vector<double> sortie_taille;
        vector<double> sortie_deadline;
        
        //        /***** ADD POUR 2 TACHES *****/
        //        vector<double> pa1; pa1.push_back(0.25);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.75);
        //        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
        
        /***** ADD POUR k JOBS k>3 *****/
        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
        
        vector<double> sum_pa1,sum_pa2,sum_pa3;
        double acca1 = 0;
        for (int i = 0; i < pa1.size(); i++) {
                acca1 += pa1[i];
                sum_pa1.push_back(acca1);
        }
        double acca2 = 0;
        for (int i = 0; i < pa2.size(); i++) {
                acca2 += pa2[i];
                sum_pa2.push_back(acca2);
        }
        double acca3 = 0;
        for (int i = 0; i < pa3.size(); i++) {
                acca3 += pa3[i];
                sum_pa3.push_back(acca3);
        }
        /***** END RAJOUT PLUSIEURS TACHES *****/
        /***** END ADD POUR LE MULTI - JOB *****/
        
        //Effectue la somme des p
        vector<double> sum_p;
        double acc = 0;
        for (int i = 0; i < p_size.size(); i++) {
                acc += p_size[i];
                sum_p.push_back(acc);
        }
        double nbre_alea;
        int deadline_value = 0;
        int taille = 0;
        
        for (int i=0; i<T; i++) {
                //                iteration++;
                //                uint param_seed = RAND_MAX/iteration;
                //                srand(param_seed);
                
                //@TODO                 cas multitache au même pas de temps => jobs_T plus grand
                nbre_alea = rand()/(double)RAND_MAX;
                //                cout << nbre_alea << endl;
                //                double nbre_alea2 = rand()/(double)RAND_MAX;
                //                cout << nbre_alea2 << endl;
                //                cout << "===" << endl;
                //                int taille1 = choisir_proba(sum_pa1, nbre_alea);
                //                int taille2 = choisir_proba(sum_pa2, nbre_alea2);
                //                int deadline_value1 =3;
                //                int deadline_value2 =2;
                //                cout << "Prem: " << taille1 << " " << deadline_value1 << " Et " << taille2 << " " << deadline_value2 << endl;
                //                //@FINTODO                 Si deux taches sont exécutées en parallèle on double la taille de jobs_T (les vecteurs sortie_ taille et sortie_ deadline vont être deux fois plus grands)
                
                
                /***** DEB 1 TACHE MONO JOB *****/
                taille = choisir_proba(sum_p, nbre_alea);
                //deadline_value = (int)(rand() % (max_deadline - 1 + 1)+ 1);
                deadline_value = 3;
                //                cout << "(" << taille << "," << deadline_value << ")" << endl;
                /***** END TACHE MONO JOB *****/
                
                //                /***** DEUX TACHES ADD POUR LE MULTI - JOB *****/
                //                if (i%2 == 0) {
                //                        //cas tache de taille 2 ou 0 et de deadline 1
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 2;
                //                } else {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 1;
                //                }
                //                /***** END RAJOUT PLUSIEURS TACHES *****/
                
                //                /***** ADD QUATRE TACHES POUR LE MULTI - JOB 2 *****/
                //                if (i%4 == 0) {
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%4 == 1) {
                //                        taille = choisir_proba(sum_pa3, nbre_alea);
                //                        deadline_value = min(3,max_deadline);
                //                } else if (i%4 == 2) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%4 == 3) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 1;
                //                }
                //                /***** FIN ADD POUR LE MULTI - JOB 2 *****/
                
                //                /**** ADD 8 TACHES POUR LE MULTI - JOB 3 ****/
                //                if (i%8 == 0) {
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 1;
                //                } else if (i%8 == 1) {
                //                        taille = choisir_proba(sum_pa3, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 2) {
                //                        taille = choisir_proba(sum_pa3, nbre_alea);
                //                        deadline_value = 3;
                //                } else if (i%8 == 3) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 4) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 1;
                //                } else if (i%8 == 5) {
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 6) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 3;
                //                } else if (i%8 == 7) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 2;
                //                }
                //                /***** FIN ADD POUR LE MULTI - JOB 3 *****/
                
                //                /**** ADD TEST SIMU PAPIER ****/
                //                if (i%8 == 0) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 3;
                //                } else if (i%8 == 1) {
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 1;
                //                } else if (i%8 == 2) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 3) {
                //                        taille = choisir_proba(sum_pa3, nbre_alea);
                //                        deadline_value = 3;
                //                } else if (i%8 == 4) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 5) {
                //                        taille = choisir_proba(sum_pa1, nbre_alea);
                //                        deadline_value = 1;
                //                } else if (i%8 == 6) {
                //                        taille = choisir_proba(sum_pa2, nbre_alea);
                //                        deadline_value = 2;
                //                } else if (i%8 == 7) {
                //                        taille = choisir_proba(sum_pa3, nbre_alea);
                //                        deadline_value = 2;
                //                }
                //                /***** FIN TEST SIMU PAPIER *****/
                
                /***** END ADD POUR LE MULTI - JOB *****/
                
                sortie_taille.push_back(taille);
                sortie_deadline.push_back(deadline_value);
        }
        sortie_simulation_job.push_back(sortie_taille);
        sortie_simulation_job.push_back(sortie_deadline);
        return sortie_simulation_job;
}

/*************************************************************************************/
// Simule une tache : nouveau jobs ou pas selon la proba donner en parametre
// Determine letat courant et calcule letat suivant
/*************************************************************************************/
// On doit enregistrer les taches intermédiaires lors de la simulation présnet dans job_T
// Détermine l'état avec job_T et l'état courant
vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_casréduit(vector<vector<double> > & espace_detat_init, vector<double> p_reduit,int T,int max_deadline, vector<vector<double> > choix_action, int iteration, bool dp, vector<vector<double> > & jobs_T) {
        vector<vector<double> > sortie;
        vector<double> aa,dd, proba,deadline_alea, ens_taille;
        aa.push_back(0);
        dd.push_back(0);
        //Effectue la somme des p
        vector<double> sum_p_reduit;
        double acc = 0;
        for (int i = 0; i < p_reduit.size(); i++) {
                acc += p_reduit[i];
                sum_p_reduit.push_back(acc);
        }
        
        for (int i=0; i<T; i++) {
                
        }
        return(sortie);
}

/****************************************************************************************************************************/
// Algo de resolution backward en temps fini
// Utilise la fonction simulaton_generale
//
// sortie:
//          sortie_backward_resolution: vector<vector<double> >
//                      nrj_int      : vector<double>, énergie intermédiaire utilisé pour le calcul
//                      choix_action : vector<double>, action (vitesse) choisie par le programme d'iteration backward
//
// entrée :
//          T: int, échéance de l'expérience
//          p_size: vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          p_deadline: vector<double>, probabilité de l'échéance d'un job, p_deadline.size() = max_deadline
//          max_size: int, taille maximale d'un job
//          max_deadline: int, échéance maximale d'un job
//          espace_detat_init: vector<vector<double> >, ensemble des états possible
/****************************************************************************************************************************/

/***********************************************************************************************/
// TODO Rajouter le cas périodes différentes
/***********************************************************************************************/


// Algo simulation online dans le cas mise D=0 pour tous les états.
vector<vector<vector<double> > > backward_resolution(int T,vector<double> p_size,vector<double> p_deadline, int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init) {
        int longueur = (int)espace_detat_init.size();
        vector<double> nrj(longueur,0);
        vector<double> nrj_prec(nrj);
        vector<vector<double> > nrj_int;
        int deadline_tache = 0;
        vector<double> pa1,pa2;
        
        vector<vector<double> > choix_action;
        
        vector<double> vitesse(longueur,0);
        int ind_bcl = T-1;
        vector<double> pSize(p_size);
        vector<double> pDeadline(p_deadline);
        
        vector<vector<double> > sortie_simul;
        int tt;
        for (int t=ind_bcl; t>=0; t--) {
                // nrj étape précédente.
                nrj_prec = nrj;
                cout << "==================================== T ====================================" << t << endl;
                // Distinguer les max_deadline derniers cas, car dans ce cas p=0
                int limite = T-max_deadline;
                if (t>limite) {
                        pSize[0] = 1;
                        for (int ii = 1; ii< pSize.size(); ii++) {
                                pSize[ii]=0;
                        }
                        pDeadline[0] = 1;
                        for (int ii = 1; ii< pDeadline.size(); ii++) {
                                pDeadline[ii]=0;
                        }
                } else {
                        /***** ADD POUR LE MONO - JOB *****/
                        pSize=p_size;
                        pDeadline=p_deadline;
                        deadline_tache = max_deadline;
                        /**** END 1 TACHE MONO JOB ****/
                        
                        //                        /***** ADD POUR LE MULTI - JOB DEUX *****/
                        //                        vector<double> pa1; pa1.push_back(0.25);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.75);
                        //                        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
                        //                        //                        cout << "pa1 " << pa1 << endl;
                        //                        //                        cout << "pa2 " << pa2 << endl;
                        //                        if (t%2 == 0) {
                        //                                pSize=pa2;
                        //                                deadline_tache = 2;
                        //                        } else {
                        //                                pSize=pa1;
                        //                                deadline_tache = 1;
                        //                        }
                        //                        cout << deadline_tache << "  " << pSize << endl;
                        //                        /***** FIN ADD POUR LE MULTI - JOB 2 *****/
                        
                        //                        /***** ADD POUR LE MULTI - JOB QUATRE *****/
                        //                        
                        //                        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
                        //                        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
                        //                        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
                        //                        
                        //                        if (t%4 == 0) {
                        //                                pSize=pa2;
                        //                                deadline_tache = 2;
                        //                        } else if (t%4 == 1) {
                        //                                pSize=pa3;
                        //                                deadline_tache = min(3, max_deadline);
                        //                        } else if (t%4 == 2) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 2;
                        //                        } else if (t%4 == 3) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 1;
                        //                        }
                        //                        /***** FIN ADD POUR LE MULTI - JOB 4 *****/
                        
                        
                        
                        //                        /***** ADD POUR LE MULTI - JOB HUIT *****/
                        //                        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
                        //                        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
                        //                        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
                        //                        
                        //                        if (t%8 == 0) {
                        //                                pSize=pa2;
                        //                                deadline_tache = 1;
                        //                        } else if (t%8 == 1) {
                        //                                pSize=pa3;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 2) {
                        //                                pSize=pa3;
                        //                                deadline_tache = 3;
                        //                        } else if (t%8 == 3) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 4) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 1;
                        //                        } else if (t%8 == 5) {
                        //                                pSize=pa2;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 6) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 3;
                        //                        } else if (t%8 == 7) {
                        //                                pSize=pa1;
                        //                                deadline_tache = 2;
                        //                        }
                        //                        /***** FIN ADD POUR LE MULTI - JOB HUIT *****/
                        
                        //                        /***** TEST SIMU PAPIER HUIT TACHES *****/
                        //                        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
                        //                        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
                        //                        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
                        //                        
                        //                        if (t%8 == 0) {
                        //                                p1=pa1;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 1) {
                        //                                p1=pa3;
                        //                                deadline_tache = 3;
                        //                        } else if (t%8 == 2) {
                        //                                p1=pa1;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 3) {
                        //                                p1=pa1;
                        //                                deadline_tache = 1;
                        //                        } else if (t%8 == 4) {
                        //                                p1=pa2;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 5) {
                        //                                p1=pa3;
                        //                                deadline_tache = 2;
                        //                        } else if (t%8 == 6) {
                        //                                p1=pa1;
                        //                                deadline_tache = 3;
                        //                        } else if (t%8 == 7) {
                        //                                p1=pa2;
                        //                                deadline_tache = 1;
                        //                        }
                        //                        /***** FIN TEST SIMU PAPIER 8 TACHES *****/
                        
                }
                
                if (t==0) {tt=0;} else {tt = t-1;}
                int taille_tache = max_size;
                sortie_simul = simulation_general(espace_detat_init, pSize, pDeadline, max_vitesse, vitesse, nrj_prec, nrj, deadline_tache, taille_tache);
                vitesse = sortie_simul[0];
                nrj = sortie_simul[1];
                choix_action.push_back(vitesse);
                nrj_int.push_back(nrj);
        }
        vector<vector<vector<double> > > sortie_backward_resolution;
        sortie_backward_resolution.push_back(nrj_int);
        sortie_backward_resolution.push_back(choix_action);
        return(sortie_backward_resolution);
}
//
//vector<double> calcul_meilleur_vitesse2(int position_etat_courant,vector<vector<double> > & espace_detat_init, vector<double> nrj_prec,vector<double> p,vector<double> q,int max_vitesse,int deadline_tache,int taille_tache) {
//        vector<double> etat_courant = espace_detat_init[position_etat_courant];
//        // l'ensemble des valeurs d'nrj possibles en testant toutes les vitesses autorisées
//        vector<double> valeur_nrj_suiv_res;
//        // parcours l'ensemble des vitesses disponibles et regarde laquelle est la meilleur:
//        // On part du minimum possible, i.e. on execute la vitesse qui fait tout le travail au pas de temps suivant, et on vérifie que l'on est audessus des pas de temps qui sont déjà présent en mémoire
//        vector<double> borne_inf_vitesse;
//        // etat_courant[i] - (i-1) * max_vitess
//        // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
//        for (int i = 0;i < etat_courant.size(); i++) {
//                borne_inf_vitesse.push_back(ceil(etat_courant[i]/(i+1)));
//        }
//        // VITESSE MAXIMALE des vitesses a respecter
//        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());
//        // BORNE SUR LES VITESSES par : soit la vitesse max permise, soit par la vitesse qui effectue tout le travail déjà arrivé.
//        int arret = min(etat_courant[etat_courant.size()-1],(double)max_vitesse);
//        // On boucle sur toutes les vitesses, on regarde les valeurs obtenues et on prend la vitesse et l'energie minimal, ie celle qui consomme le moins d'énergie au cours du temps (on teste avec toutes les vitesses et toutes les tailles de tache possible).
//        vector<double> vitesse_possible;
//        vector<double> indice_etat_suivant_tache;
//        
//        // BOUCLE sur les VITESSES = i
//        for (int i = borneInfVitesse; i <= arret; i++) {
//                // CAS PAS D'ARRIVEE DE TACHE, on enregistre sa position
//                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
//                indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,0,0,i,espace_detat_init));
//                for (int size = 1; size <= taille_tache; size++) {
//                        // CAS TACHE TAILLE s ARRIVE : on enregistre sa position
//                        indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,deadline_tache,size,i,espace_detat_init));
//                }
//                double cout_valeur_nrj_suiv_res = cout_nrj(i);
//                // Le NOMBRE DE PROBA dépend de la taille
//                for (int val_p = 0; val_p < p.size(); val_p++) {
//                        cout_valeur_nrj_suiv_res += p[val_p]*nrj_prec[indice_etat_suivant_tache[val_p]];
//                }
//                valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res);
//                vitesse_possible.push_back(i);
//        }
//        std::vector<double>::iterator it =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end());
//        double pos = distance(valeur_nrj_suiv_res.begin(), it);
//        double nrj;
//        int vitesse;
//        if (it!= valeur_nrj_suiv_res.end()) {
//                nrj = *it;
//                vitesse = vitesse_possible[(int)pos];
//        } else {
//                nrj = 0;
//                vitesse = 0;
//        }
//        vector<double> vitesse_et_energie;
//        vitesse_et_energie.push_back(nrj);
//        vitesse_et_energie.push_back((double)vitesse);
//        return(vitesse_et_energie);
//}


///***********************************************************************************************/
//// Simulation_general:
//// 2 étapes: - repérage des nouveaux états
////           - calcul d'actions et nrj
//// sortie: vitesse, nrj
//// variable présente mais non utilisé : choix_action_int,nrj,t,T
///***********************************************************************************************/
//vector<vector<double> > simulation_general2(vector<vector<double> > & espace_detat_init,vector<double> p,vector<double> q,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj,int deadline_tache,int taille_tache) {
//        for (int s  = 0; s < espace_detat_init.size(); s++) {
//                vector<double> vitesse_et_energie = calcul_meilleur_vitesse2(s, espace_detat_init, nrj_prec,p,q,max_vitesse, deadline_tache, taille_tache);
//                nrj[s] = vitesse_et_energie[0];
//                vitesse[s] = vitesse_et_energie[1];
//        }
//        vector<vector<double> > sortie_simul;
//        std::vector<double> vitesseCast(vitesse.begin(), vitesse.end());
//        sortie_simul.push_back(vitesseCast);
//        sortie_simul.push_back(nrj);
//        return(sortie_simul);
//}


///***********************************************************************************************/
//// Algo de resolution backward en temps fini et comparaison avec le cas offline
//// sortie: graphique
///***********************************************************************************************/
//// Algo simulation online dans le cas mise D=0 pour tous les états.
//vector<vector<vector<double> > > backward_resolution2(int T,vector<double> p,vector<double> q,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_offline) {
//        int longueur = (int)espace_detat_init.size();
//        vector<double> nrj(longueur,0);
//        vector<double> nrj_prec(nrj);
//        vector<vector<double> > nrj_int;
//        vector<vector<double> > ensemble_tache;
//        
//        vector<vector<double> > choix_action;
//        
//        vector<double> vitesse(longueur,0);
//        int ind_bcl = T-1;
//        vector<double> p1(p);
//        
//        vector<vector<double> > sortie_simul;
//        int tt;
//        cout << "==================================================<>AQ" << endl;
//        for (int t=ind_bcl; t>=0; t--) {
//                // nrj étape précédente.
//                nrj_prec = nrj;
//                cout << "==================================== T ====================================" << t << endl;
//                // Distinguer les max_deadline derniers cas, car dans ce cas p=0
//                int limite = T-max_deadline;
//                if (t>limite) {
//                        p1[0] = 1;
//                        for (int ii = 1; ii< p1.size(); ii++) {
//                                p1[ii]=0;
//                        }
//                } else {
//                        p1=p;
//                }
//                
//                if (t==0) {tt=0;} else {tt = t-1;}
//                int deadline_tache = max_deadline;
//                int taille_tache = max_size;
//                sortie_simul = simulation_general2(espace_detat_init, p1,q, max_vitesse, vitesse, nrj_prec, nrj, deadline_tache, taille_tache);
//                vitesse = sortie_simul[0];
//                nrj = sortie_simul[1];
//                choix_action.push_back(vitesse);
//                nrj_int.push_back(nrj);
//                cout << "vitesse " << vitesse << endl;
//        }
//        cout << ensemble_tache << endl;
//        vector<vector<vector<double> > > sortie;
//        sortie.push_back(nrj_int);
//        //vector<vector<double> > ChoixActionCast(choix_action.begin(),choix_action.end());
//        sortie.push_back(choix_action);
//        return(sortie);
//}

/****************************************************************************************************/
/****************************************************************************************************/
// semi-norme : retourne la semi-norme span : max-min
// sortie: la valeur de la norm
/****************************************************************************************************/
double span(vector<double> a) {
        //        cout << "MAX: " << *max_element(a.begin(), a.end()) << endl;
        //        cout << "MIN: " << *min_element(a.begin(), a.end()) << endl;
        return (*max_element(a.begin(), a.end()) - *min_element(a.begin(), a.end()));
}


/***********************************************************************************************/
// value_iteration
//
// Utilise la fonction simulaton_generale
//
// sortie:
//          sortie_value_iteration: vector<vector<double> >
//                      nrj_suiv      : vector<double>, énergie intermédiaire calculée pour déterminer la vitesse optimale, censée converger
//                      vitesse       : vector<double>, vitesse à chosiir à chaque état, actualisé à chaque tour de boucle
//                      n_sortie      : vector<double>, nombre d'itération avant la sortie de l'algorithme VI
//                      max_sortie    : vector<double>, span, dernier écart entre les énergie lors du rpocessus d'itération.
//
// entrée :
//          p_size: vector<double>, probabilité de la taille d'un job, p_size.size() = max_size
//          max_deadline       : int, échéance maximale d'un job
//          max_size           : int, taille maximale d'un job
//          max_vitesse        : int, vitesse maximale du processeur
//          espace_detat_init  : vector<vector<double> >, ensemble des états possible
//          p_deadline: vector<double>, probabilité de l'échéance d'un job, p_deadline.size() = max_deadline
/***********************************************************************************************/

vector<vector<double> > value_iteration( vector<double> p_size,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_deadline) {
        /******** AVANT MONO TACHE ********/
        int longueur = (int)espace_detat_init.size();
        vector<double> nrj_suiv(longueur,0);
        vector<double> nrj_cour(nrj_suiv);
        
        vector<vector<double> > choix_action, sortie;
        vector<double> vitesse(longueur,0);
        
        double epsilon = 0.01;
        int n = 0;
        // appelle de la fonction simulation
        sortie = simulation_general(espace_detat_init,p_size,p_deadline,max_vitesse,vitesse,nrj_cour,nrj_suiv,max_deadline,max_size);
        
        nrj_suiv=sortie[1];
        //        cout << "========" << endl;
        //        cout << sortie << endl;
        while (!(span(nrj_suiv-nrj_cour)<epsilon)) {
                //                cout << span(nrj_suiv-nrj_cour) << endl;
                //                cout << "=======================" << endl;
                //                cout << nrj_cour;
                //                cout << nrj_suiv;
                //                if (n%100==0) {
                //                vector<double> aaa=nrj_suiv-nrj_cour;
                //                cout << "span " << aaa;
                //                cout << "=======================" << endl;
                //                cout << nrj_suiv;
                //                }
                
                
                nrj_cour=nrj_suiv;
                sortie = simulation_general(espace_detat_init,p_size,p_deadline, max_vitesse,vitesse,nrj_cour,nrj_suiv,max_deadline,max_size);
                vitesse = sortie[0];
                nrj_suiv = sortie[1];
                n=n+1;
                //                cout << "========" << endl;
                //                //                cout << sortie << endl;
                //                vector<double> aa=nrj_suiv-nrj_cour;
                //                //                cout << "span " << aa << endl;
                cout << "n " << n << endl;
                cout << "span " << span(nrj_suiv-nrj_cour) << endl;
        }
        vector<vector<double> > sortie_value_iteration;
        sortie_value_iteration.push_back(nrj_suiv);
        sortie_value_iteration.push_back(vitesse);
        vector<double> n_sortie(1);
        n_sortie[0] = n;
        sortie_value_iteration.push_back(n_sortie);
        vector<double> max_sortie(1);
        vector<double> nrj_sortie = nrj_suiv - nrj_cour;
        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        sortie_value_iteration.push_back(max_sortie);
        
        
        
        //        /**** CAS DEUX TACHES ****/
        //        int longueur = (int)espace_detat_init.size();
        //        vector<double> nrj_suiv1(longueur,0);
        //        vector<double> nrj_cour1(nrj_suiv1);
        //        vector<double> nrj_suiv2(longueur,0);
        //        vector<double> nrj_cour2(nrj_suiv2);
        //        //vector<vector<double> > nrj_int, nrj_cour, nrj_suiv;
        //        
        //        vector<vector<double> > choix_action, sortie,sortie1,sortie2;
        //        vector<double> vitesse(longueur,0),vitesse1(longueur,0),vitesse2(longueur,0);
        //        
        //        /*choix_action<-NULL;
        //         choix_action_int<-list(c(1:long)*0);*/
        //        
        //        double epsilon = 0.01;
        //        int n = 0;
        //        //première valeur fixé
        //        //nrj_cour<-list(c(1:fin)*0);
        //        //int t = 19; // normalement inutilisé
        //        // appelle de la fonction simulation
        //        
        //        //cas tache de taille 2 ou 0 et de deadline 1
        //        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0.8);pa1.push_back(0);pa1.push_back(0);
        //        //        pa1.push_back(0.2);
        //        //        pa1.push_back(0);
        //        //        pa1.push_back(0);
        //        //        pa1.push_back(0.6);
        //        //        pa1.push_back(0);
        //        //        pa1.push_back(0);
        //        //        pa1.push_back(0.2);
        //        int deadline_tache1 = 2;
        //        // cas tache de taille 3 ou 0 et de deadline 3
        //        vector<double> pa2; pa2.push_back(0.25);pa2.push_back(0);pa2.push_back(0);pa2.push_back(0);pa2.push_back(0.75);
        //        //        pa2.push_back(0.2);
        //        //        pa2.push_back(0);
        //        //        pa2.push_back(0);
        //        //        pa2.push_back(0.6);
        //        //        pa2.push_back(0);
        //        //        pa2.push_back(0);
        //        //        pa2.push_back(0.2);
        //        int deadline_tache2 = 1;
        //        
        //        sortie1 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //        sortie2 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv1,deadline_tache2,max_size);
        //        
        //        cout << "========" << endl;
        //        cout << "sortie : " << sortie1 << sortie2 << endl;
        //        cout << "========" << endl;
        //        nrj_suiv1=sortie1[1];
        //        nrj_suiv2=sortie2[1];
        //        
        //        while (!(span(nrj_suiv2-nrj_cour1)<epsilon) && !(span(nrj_suiv1-nrj_cour2)<epsilon)) {
        //                nrj_cour2=nrj_suiv1;
        //                nrj_cour1=nrj_suiv2;
        //                sortie1 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //                sortie2 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv1,deadline_tache2,max_size);
        //                cout << "sortie 1 et 2:  " << sortie1 << sortie2 << endl;
        //                vitesse1 = sortie1[0];
        //                nrj_suiv1 = sortie1[1];
        //                vitesse2 = sortie2[0];
        //                nrj_suiv2 = sortie2[1];
        //                n=n+1;
        //                /*cout << "vitesse : " << vitesse << endl;
        //                 cout << "nrj_suiv : " << nrj_suiv << endl;
        //                 cout << "nrj_ocur : " << nrj_cour << endl;*/
        //                
        //                //                cout << "n " << n << endl;
        //                //                cout << "span1 " << span(nrj_suiv2-nrj_cour1) << endl;
        //                //                cout << "span2 " << span(nrj_suiv1-nrj_cour2) << endl;
        //                
        //                //		# cas ou ça boucle
        //                //		if (sp==span(nrj_suiv[[1]]-nrj_cour[[1]])) {
        //                //			break;
        //                //		}
        //        }
        //        vector<vector<double> > sortie_value_iteration;
        //        sortie_value_iteration.push_back(nrj_suiv1);
        //        sortie_value_iteration.push_back(nrj_suiv2);
        //        sortie_value_iteration.push_back(vitesse1);
        //        sortie_value_iteration.push_back(vitesse2);
        //        vector<double> n_sortie(1);
        //        n_sortie[0] = n;
        //        sortie_value_iteration.push_back(n_sortie);
        //        vector<double> max_sortie(1);
        //        vector<double> nrj_sortie = nrj_suiv1 - nrj_cour1;
        //        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        //        sortie_value_iteration.push_back(max_sortie);
        //        
        //        
        
        
        //        /**** CAS QUATRE TACHES ****/
        //        int longueur = (int)espace_detat_init.size();
        //        vector<double> nrj_suiv1(longueur,0);
        //        vector<double> nrj_cour1(nrj_suiv1);
        //        vector<double> nrj_suiv2(longueur,0);
        //        vector<double> nrj_cour2(nrj_suiv2);
        //        vector<double> nrj_suiv3(longueur,0);
        //        vector<double> nrj_cour3(nrj_suiv3);
        //        vector<double> nrj_suiv4(longueur,0);
        //        vector<double> nrj_cour4(nrj_suiv4);
        //        
        //        vector<vector<double> > choix_action, sortie,sortie1,sortie2,sortie3,sortie4;
        //        vector<double> vitesse(longueur,0),vitesse1(longueur,0),vitesse2(longueur,0),vitesse3(longueur,0),vitesse4(longueur,0);
        //        
        //        double epsilon = 0.01;
        //        int n = 0;
        //        
        //        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
        //        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
        //        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
        //        int deadline_tache1=2;
        //        int deadline_tache2=3;//min(3,max_deadline);
        //        int deadline_tache3=2;
        //        int deadline_tache4=1;
        //        
        //        cout << "==AAA===" << endl;
        //        
        //        nrj_suiv1 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv1,deadline_tache1,max_size)[1];
        //        cout << nrj_suiv1 << endl;
        //        nrj_suiv2 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv2,deadline_tache2,max_size)[1];
        //        cout << "=====" << endl;
        //        
        //        cout << nrj_suiv2 << endl;
        //        nrj_suiv3 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour3,nrj_suiv3,deadline_tache3,max_size)[1];
        //        cout << "=====" << endl;
        //        
        //        cout << nrj_suiv3 << endl;
        //        nrj_suiv4 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour4,nrj_suiv4,deadline_tache4,max_size)[1];
        //        cout << "=====" << endl;
        //        
        //        cout << nrj_suiv4 << endl;
        //        
        //        
        //        while (!(span(nrj_cour4-nrj_suiv1)<epsilon) && !(span(nrj_cour3-nrj_suiv4)<epsilon) && !(span(nrj_cour2-nrj_suiv3)<epsilon) && !(span(nrj_cour1-nrj_suiv2)<epsilon)) {
        //                nrj_cour1=nrj_suiv2;
        //                nrj_cour2=nrj_suiv3;
        //                nrj_cour3=nrj_suiv4;
        //                nrj_cour4=nrj_suiv1;
        //                
        //                sortie1 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv1,deadline_tache1,max_size);
        //                sortie2 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv2,deadline_tache2,max_size);
        //                sortie3 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour3,nrj_suiv3,deadline_tache3,max_size);
        //                sortie4 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour4,nrj_suiv4,deadline_tache4,max_size);
        //                
        //                vitesse1 = sortie1[0];
        //                nrj_suiv1 = sortie1[1];
        //                vitesse2 = sortie2[0];
        //                nrj_suiv2 = sortie2[1];
        //                vitesse3 = sortie3[0];
        //                nrj_suiv3 = sortie3[1];
        //                vitesse4 = sortie4[0];
        //                nrj_suiv4 = sortie4[1];
        //                n=n+1;
        //                
        //                cout << "n " << n << endl;
        //                //                cout << "span1 " << span(nrj_suiv2-nrj_cour1) << endl;
        //                //                cout << "span2 " << span(nrj_suiv1-nrj_cour2) << endl;
        //                cout << "========" << endl;
        //                cout << sortie1 << endl;
        //                cout << sortie2 << endl;
        //                cout << sortie3 << endl;
        //                cout << sortie4 << endl;
        //                
        //        }
        //        vector<vector<double> > sortie_value_iteration;
        //        sortie_value_iteration.push_back(nrj_suiv1);
        //        sortie_value_iteration.push_back(nrj_suiv2);
        //        sortie_value_iteration.push_back(nrj_suiv3);
        //        sortie_value_iteration.push_back(nrj_suiv4);
        //        sortie_value_iteration.push_back(vitesse1);
        //        sortie_value_iteration.push_back(vitesse2);
        //        sortie_value_iteration.push_back(vitesse3);
        //        sortie_value_iteration.push_back(vitesse4);
        //        vector<double> n_sortie(1);
        //        n_sortie[0] = n;
        //        sortie_value_iteration.push_back(n_sortie);
        //        vector<double> max_sortie(1);
        //        vector<double> nrj_sortie = nrj_suiv1 - nrj_cour1;
        //        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        //        sortie_value_iteration.push_back(max_sortie);
        
        
        // /****** CAS 2 taches alternées dans l'algo 4 taches FONCTIONNE ******/
        //        int longueur = (int)espace_detat_init.size();
        //        vector<double> nrj_suiv1(longueur,0);
        //        vector<double> nrj_cour1(nrj_suiv1);
        //        vector<double> nrj_suiv2(longueur,0);
        //        vector<double> nrj_cour2(nrj_suiv2);
        //        vector<double> nrj_suiv3(longueur,0);
        //        vector<double> nrj_cour3(nrj_suiv3);
        //        vector<double> nrj_suiv4(longueur,0);
        //        vector<double> nrj_cour4(nrj_suiv4);
        //        //vector<vector<double> > nrj_int, nrj_cour, nrj_suiv;
        //        
        //        vector<vector<double> > choix_action, sortie,sortie1,sortie2,sortie3,sortie4;
        //        vector<double> vitesse(longueur,0),vitesse1(longueur,0),vitesse2(longueur,0),vitesse3(longueur,0),vitesse4(longueur,0);
        //        
        //        double epsilon = 0.01;
        //        int n = 0;
        //        
        //        
        //        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0.8);pa1.push_back(0);pa1.push_back(0);
        //        int deadline_tache1 = 2;
        //        vector<double> pa2; pa2.push_back(0.25);pa2.push_back(0);pa2.push_back(0);pa2.push_back(0);pa2.push_back(0.75);
        //        int deadline_tache2 = 1;
        //        
        //        sortie1 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //        sortie2 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour2,nrj_suiv3,deadline_tache2,max_size);
        //        sortie3 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour3,nrj_suiv4,deadline_tache1,max_size);
        //        sortie4 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour4,nrj_suiv1,deadline_tache2,max_size);
        //        
        //        //        sortie1 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //        //        sortie2 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour2,nrj_suiv1,deadline_tache2,max_size);
        //        
        //        //        nrj_suiv1=sortie1[1];
        //        //        nrj_suiv2=sortie2[1];
        //        
        //        nrj_suiv1=sortie1[1];
        //        nrj_suiv2=sortie2[1];
        //        nrj_suiv3=sortie3[1];
        //        nrj_suiv4=sortie4[1];
        //        
        //        while (!(span(nrj_suiv1-nrj_cour2)<epsilon) && !(span(nrj_suiv2-nrj_cour3)<epsilon) && !(span(nrj_suiv3-nrj_cour2)<epsilon)) {
        //                //                nrj_cour2=nrj_suiv1;
        //                //                nrj_cour1=nrj_suiv2;
        //                //                sortie1 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //                //                sortie2 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour2,nrj_suiv1,deadline_tache2,max_size);
        //                
        //                nrj_cour4=nrj_suiv3;
        //                nrj_cour3=nrj_suiv2;
        //                nrj_cour2=nrj_suiv1;
        //                nrj_cour1=nrj_suiv4;
        //                
        //                sortie1 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour1,nrj_suiv2,deadline_tache1,max_size);
        //                sortie2 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour2,nrj_suiv3,deadline_tache2,max_size);
        //                sortie3 = simulation_general(espace_detat_init,pa1,max_vitesse,vitesse,nrj_cour3,nrj_suiv4,deadline_tache1,max_size);
        //                sortie4 = simulation_general(espace_detat_init,pa2,max_vitesse,vitesse,nrj_cour4,nrj_suiv1,deadline_tache2,max_size);
        //                
        //                
        //                vitesse1 = sortie1[0];
        //                nrj_suiv1 = sortie1[1];
        //                vitesse2 = sortie2[0];
        //                nrj_suiv2 = sortie2[1];
        //                vitesse3 = sortie3[0];
        //                nrj_suiv3 = sortie3[1];
        //                vitesse4 = sortie4[0];
        //                nrj_suiv4 = sortie4[1];
        //                n=n+1;
        //                //                vitesse1 = sortie1[0];
        //                //                nrj_suiv1 = sortie1[1];
        //                //                vitesse2 = sortie2[0];
        //                //                nrj_suiv2 = sortie2[1];
        //                
        //                cout << "n " << n << endl;
        //                //                cout << "span1 " << span(nrj_suiv2-nrj_cour1) << endl;
        //                //                cout << "span2 " << span(nrj_suiv1-nrj_cour2) << endl;
        //                
        //                //		# cas ou ça boucle
        //                //		if (sp==span(nrj_suiv[[1]]-nrj_cour[[1]])) {
        //                //			break;
        //                //		}
        //        }
        //        vector<vector<double> > sortie_sortie;
        //        sortie_sortie.push_back(nrj_suiv1);
        //        sortie_sortie.push_back(nrj_suiv2);
        //        sortie_sortie.push_back(nrj_suiv3);
        //        sortie_sortie.push_back(nrj_suiv4);
        //        sortie_sortie.push_back(vitesse1);
        //        sortie_sortie.push_back(vitesse2);
        //        sortie_sortie.push_back(vitesse3);
        //        sortie_sortie.push_back(vitesse4);
        //        vector<double> n_sortie(1);
        //        n_sortie[0] = n;
        //        sortie_sortie.push_back(n_sortie);
        //        vector<double> max_sortie(1);
        //        vector<double> nrj_sortie = nrj_suiv1 - nrj_cour1;
        //        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        //        sortie_sortie.push_back(max_sortie);
        
        
        
        //        /**** CAS HUIT TACHES ****/
        //        int longueur = (int)espace_detat_init.size();
        //        vector<double> nrj_suiv1(longueur,0);
        //        vector<double> nrj_cour1(nrj_suiv1);
        //        vector<double> nrj_suiv2(longueur,0);
        //        vector<double> nrj_cour2(nrj_suiv2);
        //        vector<double> nrj_suiv3(longueur,0);
        //        vector<double> nrj_cour3(nrj_suiv3);
        //        vector<double> nrj_suiv4(longueur,0);
        //        vector<double> nrj_cour4(nrj_suiv4);
        //        vector<double> nrj_suiv5(longueur,0);
        //        vector<double> nrj_cour5(nrj_suiv5);
        //        vector<double> nrj_suiv6(longueur,0);
        //        vector<double> nrj_cour6(nrj_suiv6);
        //        vector<double> nrj_suiv7(longueur,0);
        //        vector<double> nrj_cour7(nrj_suiv7);
        //        vector<double> nrj_suiv8(longueur,0);
        //        vector<double> nrj_cour8(nrj_suiv8);
        //        
        //        vector<vector<double> > choix_action, sortie,sortie1,sortie2,sortie3,sortie4,sortie5,sortie6,sortie7,sortie8;
        //        vector<double> vitesse(longueur,0),vitesse1(longueur,0),vitesse2(longueur,0),vitesse3(longueur,0),vitesse4(longueur,0),vitesse5(longueur,0),vitesse6(longueur,0),vitesse7(longueur,0),vitesse8(longueur,0);
        //        
        //        double epsilon = 0.01;
        //        int n = 0;
        //        
        //        vector<double> pa1; pa1.push_back(0.2);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0);pa1.push_back(0.8);
        //        vector<double> pa2; pa2.push_back(0.2);pa2.push_back(0);pa2.push_back(0.8);pa2.push_back(0);pa2.push_back(0);
        //        vector<double> pa3; pa3.push_back(0.2);pa3.push_back(0.8);pa3.push_back(0);pa3.push_back(0);pa3.push_back(0);
        //        int deadline_tache1=1;
        //        int deadline_tache2=2;
        //        int deadline_tache3=3;
        //        int deadline_tache4=2;
        //        int deadline_tache5=1;
        //        int deadline_tache6=2;
        //        int deadline_tache7=3;
        //        int deadline_tache8=2;
        //        
        //        nrj_suiv1 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv1,deadline_tache1,max_size)[1];
        //        nrj_suiv2 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv2,deadline_tache2,max_size)[1];
        //        nrj_suiv3 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour3,nrj_suiv3,deadline_tache3,max_size)[1];
        //        nrj_suiv4 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour4,nrj_suiv4,deadline_tache4,max_size)[1];
        //        nrj_suiv5 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour5,nrj_suiv1,deadline_tache5,max_size)[1];
        //        nrj_suiv6 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour6,nrj_suiv2,deadline_tache6,max_size)[1];
        //        nrj_suiv7 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour7,nrj_suiv3,deadline_tache7,max_size)[1];
        //        nrj_suiv8 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour8,nrj_suiv4,deadline_tache8,max_size)[1];
        //        
        //        while (!(span(nrj_cour1-nrj_suiv2)<epsilon) && !(span(nrj_cour2-nrj_suiv3)<epsilon) && !(span(nrj_cour3-nrj_suiv4)<epsilon) && !(span(nrj_cour4-nrj_suiv5)<epsilon) && !(span(nrj_cour5-nrj_suiv6)<epsilon) && !(span(nrj_cour6-nrj_suiv7)<epsilon) && !(span(nrj_cour7-nrj_suiv8)<epsilon) && !(span(nrj_cour8-nrj_suiv1)<epsilon)) {
        //                nrj_cour1=nrj_suiv2;
        //                nrj_cour2=nrj_suiv3;
        //                nrj_cour3=nrj_suiv4;
        //                nrj_cour4=nrj_suiv5;
        //                nrj_cour5=nrj_suiv6;
        //                nrj_cour6=nrj_suiv7;
        //                nrj_cour7=nrj_suiv8;
        //                nrj_cour8=nrj_suiv1;
        //                
        //                sortie1 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour1,nrj_suiv1,deadline_tache1,max_size);
        //                sortie2 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour2,nrj_suiv2,deadline_tache2,max_size);
        //                sortie3 = simulation_general(espace_detat_init,pa3,p_deadline,max_vitesse,vitesse,nrj_cour3,nrj_suiv3,deadline_tache3,max_size);
        //                sortie4 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour4,nrj_suiv4,deadline_tache4,max_size);
        //                sortie5 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour5,nrj_suiv1,deadline_tache5,max_size);
        //                sortie6 = simulation_general(espace_detat_init,pa2,p_deadline,max_vitesse,vitesse,nrj_cour6,nrj_suiv2,deadline_tache6,max_size);
        //                sortie7 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour7,nrj_suiv3,deadline_tache7,max_size);
        //                sortie8 = simulation_general(espace_detat_init,pa1,p_deadline,max_vitesse,vitesse,nrj_cour8,nrj_suiv4,deadline_tache8,max_size);
        //                
        //                vitesse1 = sortie1[0];
        //                nrj_suiv1 = sortie1[1];
        //                vitesse2 = sortie2[0];
        //                nrj_suiv2 = sortie2[1];
        //                vitesse3 = sortie3[0];
        //                nrj_suiv3 = sortie3[1];
        //                vitesse4 = sortie4[0];
        //                nrj_suiv4 = sortie4[1];
        //                vitesse5 = sortie5[0];
        //                nrj_suiv5 = sortie5[1];
        //                vitesse6 = sortie6[0];
        //                nrj_suiv6 = sortie6[1];
        //                vitesse7 = sortie7[0];
        //                nrj_suiv7 = sortie7[1];
        //                vitesse8 = sortie8[0];
        //                nrj_suiv8 = sortie8[1];
        //                n=n+1;
        //                
        //                cout << "n " << n << endl;
        //                //                cout << "span1 " << span(nrj_suiv2-nrj_cour1) << endl;
        //                //                cout << "span2 " << span(nrj_suiv1-nrj_cour2) << endl;
        //        }
        //        vector<vector<double> > sortie_sortie;
        //        sortie_sortie.push_back(nrj_suiv1);
        //        sortie_sortie.push_back(nrj_suiv2);
        //        sortie_sortie.push_back(nrj_suiv3);
        //        sortie_sortie.push_back(nrj_suiv4);
        //        sortie_sortie.push_back(nrj_suiv5);
        //        sortie_sortie.push_back(nrj_suiv6);
        //        sortie_sortie.push_back(nrj_suiv7);
        //        sortie_sortie.push_back(nrj_suiv8);
        //        sortie_sortie.push_back(vitesse1);
        //        sortie_sortie.push_back(vitesse2);
        //        sortie_sortie.push_back(vitesse3);
        //        sortie_sortie.push_back(vitesse4);
        //        sortie_sortie.push_back(vitesse5);
        //        sortie_sortie.push_back(vitesse6);
        //        sortie_sortie.push_back(vitesse7);
        //        sortie_sortie.push_back(vitesse8);
        //        vector<double> n_sortie(1);
        //        n_sortie[0] = n;
        //        sortie_sortie.push_back(n_sortie);
        //        vector<double> max_sortie(1);
        //        vector<double> nrj_sortie = nrj_suiv1 - nrj_cour1;
        //        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        //        sortie_sortie.push_back(max_sortie);
        //        return(sortie_sortie);
        
        return(sortie_value_iteration);
}


/***********************************************************************************************/
// reduc_esp_etat
//
// Utilise la fonction testAlgo_simulation_et_calcul_vitesse
//
// sortie:
//          sortie            : int, programmation dynamique pour trouver le pas de temps optimal (deadline_max optimale) à partir duquel on peut réduire l'espace d'état
//
// entrée :
//          taille            : int, taille maximale d'un job
//          Temps             : int, ???????
/***********************************************************************************************/
int reduc_esp_etat(int taille, int Temps) {
        int sortie = 0;
        int k = 6;
        int T = 128;
        int S = taille;
        double value, value_pt;
        
        vector<vector<double> > tab;
        
        vector<vector<double> > ind_min;
        vector<double> int_min;
        vector<double> prov;
        // initialisation de la prmemière ligne
        for (int i=1; i<=T; i++) {
                prov.push_back(4/2*((double)T)/(double)i);
        }
        tab.push_back(prov);
        ind_min.push_back(prov*0);
        
        // initialisationde toutes les autres acse a +infinity
        for (int i=1; i<=k; i++) {
                prov.erase(prov.begin(),prov.end());
                for (int j = 1; j<=T; j++) {
                        prov.push_back(+INFINITY);
                }
                tab.push_back(prov);
                ind_min.push_back(prov);
        }
        
        // prog dyn //
        int deb;
        S=4;
        for (int j=1; j<=k; j++){
                for (int i=1;i<T; i++) {
                        int_min.erase(int_min.begin(),int_min.end());
                        deb = i+1;
                        for (int l=deb; l<=T; l++) {
                                value = ((double)(l-1))*(double)S/((double)(i)*2);
                                value_pt = (double)tab[j-1][l-1];
                                if (value > value_pt) {
                                        int_min.push_back(value);
                                } else {
                                        int_min.push_back(value_pt);
                                }
                        }
                        //recherche de la valeur minimale
                        double val = *min_element(int_min.begin(), int_min.end());
                        tab[j][i-1] = val;
                        ind_min[j][i-1] = min_element(int_min.begin(), int_min.end())-int_min.begin()+i+1;
                }
        }
        
        // Recherche des tau intermédiaires
        vector<int> tau;
        int cour=ind_min[ind_min.size()-1][0];
        tau.push_back(cour);
        int suiv;
        int deb2 = (int)(ind_min.size()-2);
        for (int i=deb2; i>0; i--) {
                suiv = ind_min[i][cour-1];
                tau.push_back(suiv);
                cour = suiv;
        }
        cout << "les pas de temps : " << tau<< endl;
        return sortie;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////            NOUVELLE VERSION             /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<double> calcul_meilleur_vitesse(double gamma, int position_etat_courant,vector<vector<double> > & espace_detat_init,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse) {
        vector<double> etat_courant = espace_detat_init[position_etat_courant];
        // l'ensemble des valeurs d'nrj possibles en testant toutes les vitesses autorisées
        vector<double> valeur_nrj_suiv_res;
        // parcours l'ensemble des vitesses disponibles et regarde laquelle est la meilleur:
        // On part du minimum possible, i.e. on execute la vitesse qui fait tout le travail au pas de temps suivant, et on vérifie que l'on est audessus des pas de temps qui sont déjà présent en mémoire
        vector<double> borne_inf_vitesse;
        // etat_courant[i] - (i-1) * max_vitesse
        // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
        for (int i = 0;i < etat_courant.size(); i++) {
                borne_inf_vitesse.push_back(ceil((double)etat_courant[i]/(double)(i+1)));
        }
        
        // VITESSE MAXIMALE des vitesses a respecter
        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());
        int value_OA = borneInfVitesse;
        // BORNE SUR LES VITESSES par : soit la vitesse max permise, soit par la vitesse qui effectue tout le travail déjà arrivé.
        int arret = min(etat_courant[etat_courant.size()-1],(double)max_vitesse);
        // On boucle sur toutes les vitesses, on regarde les valeurs obtenues et on prend la vitesse et l'energie minimal, ie celle qui consomme le moins d'énergie au cours du temps (on teste avec toutes les vitesses et toutes les tailles de tache possible).
        vector<double> vitesse_possible, indice_etat_suivant_tache, vitesse_et_energie;
        // @TODO A enlever fait pour comparer avec l'apprentissage !
        double cout_valeur_nrj_suiv_res;
        
        // VITESSE MINIMALE supérieur à w(1)
        borneInfVitesse=(int)(etat_courant[0]);
        int borneInfVitesse_res=borneInfVitesse,arret_res=arret;
        if (arret<borneInfVitesse) {
                borneInfVitesse=+INFINITY;
                arret=+INFINITY;
        }
        
        // CAS On choisit la politique (OA) pour chaque état // CAS OA en dur 
        bool Only_OA=false;
        if (Only_OA) {
                borneInfVitesse=value_OA; arret=value_OA;
        }
        
        if (arret_res>=borneInfVitesse_res) {
                for (int ivitesse = borneInfVitesse; ivitesse <= arret; ivitesse++) {
                        //                        cout << ivitesse << " => "  << espace_detat_init[position_etat_courant];
                        // CAS PAS D'ARRIVEE DE TACHE, on enregistre sa position
                        indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
                        //indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,0,0,i,espace_detat_init));
                        // @TODO double BOUCLE deadline et size pour inclure le vecteur p_deadline
                        //  for (int deadline = 1; deadline <= deadline_tache; deadline++) {
                        for (int jobdiff=0; jobdiff<JobsToExecute.size(); jobdiff++) {
                                indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,JobsToExecute[jobdiff].deadline,JobsToExecute[jobdiff].taille,ivitesse,espace_detat_init));
                        }
                        cout_valeur_nrj_suiv_res = cout_nrj(ivitesse,max_vitesse);
                        for (int indice_nouvJob=0; indice_nouvJob<indice_etat_suivant_tache.size(); indice_nouvJob++) {
                                //@TODO Rajouter le cas (0,0)
                                cout_valeur_nrj_suiv_res += gamma * (JobsToExecute[indice_nouvJob].arrival_rate)*nrj_prec[indice_etat_suivant_tache[indice_nouvJob]];
                        }
                        valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res);
                        vitesse_possible.push_back(ivitesse);
                }
                std::vector<double>::iterator it =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end());
                // @TODO remettre en cause l'utilisation de la fonction distance => l'indice ne correspond pas forcément à la distance ????
                double nrj, pos = distance(valeur_nrj_suiv_res.begin(), it); int vitesse;
                if (it!= valeur_nrj_suiv_res.end()) {
                        nrj = *it; vitesse = vitesse_possible[(int)pos];
                } else {
                        nrj = 0; vitesse = 0;
                }
                vitesse_et_energie.push_back(nrj); vitesse_et_energie.push_back((double)vitesse);
        } else {
                vitesse_et_energie.push_back(+INFINITY);
                vitesse_et_energie.push_back(+INFINITY);
        }
        return(vitesse_et_energie);
}


vector<vector<double> > simulation_general(double gamma, vector<vector<double> > & espace_detat_init,vector<Job> & JobsToExecute,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj) {
        vector<double> vitesse_et_energie;
        vector<vector<double> > sortie_simul;
        for (int s  = 0; s < espace_detat_init.size(); s++) {
                vitesse_et_energie = calcul_meilleur_vitesse(gamma, s, espace_detat_init, nrj_prec,JobsToExecute, max_vitesse);
                nrj[s] = vitesse_et_energie[0]; vitesse[s] = vitesse_et_energie[1];
        }
        std::vector<double> vitesseCast(vitesse.begin(), vitesse.end());
        sortie_simul.push_back(vitesseCast); sortie_simul.push_back(nrj);
        return(sortie_simul);
}

// Algo simulation online dans le cas mise D=0 pour tous les états.
vector<vector<vector<double> > > backward_resolution(double gamma, int T, map<int,vector<Job> > & matrix_job, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init) {
        int longueur = (int)espace_detat_init.size();
        vector<double> nrj(longueur,0),nrj_prec(nrj),vitesse(longueur,0);
        vector<vector<double> > nrj_int, choix_action, sortie_simul;
        
        double arrival_rate_zero = 0;
        int tt,ind_bcl = T-1;
        
        vector<Job> JobsToExecute;
        for (int t=ind_bcl; t>=0; t--) {
                // nrj étape précédente.
                nrj_prec = nrj;
                cout << "==================================== T ====================================" << t << endl;
                // Distinguer les max_deadline derniers cas, car dans ce cas p=0
                int limite = T-max_deadline;
                if (t>limite) {
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                } else {
                        // JobsToExecute : ensemble des jobs a executer sur 1 pas de temps 
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                        for (int i=0; i< matrix_job.size(); i++) {
                                for (int j=0; j<matrix_job[i].size(); j++) {
                                        map<int,vector<Job> >::iterator it;
                                        it = matrix_job.find(i);
                                        if (it!=matrix_job.end()) {
                                                int period = (matrix_job[i][j]).period;
                                                if ((t-i)%period==0) {
                                                        // Sélection des jobs Jobs avec leur arrival time a executer au temps t
                                                        JobsToExecute.push_back(matrix_job[i][j]);
                                                        arrival_rate_zero+=matrix_job[i][j].arrival_rate;
                                                }
                                        }
                                }
                        }
                }
                JobsToExecute.at(0).arrival_rate= 1-arrival_rate_zero;
                if (t==0) {tt=0;} else {tt = t-1;}
                //                cout << "value: " << JobsToExecute[0].taille << "  " << JobsToExecute[0].deadline << endl;
                sortie_simul = simulation_general(gamma, espace_detat_init,JobsToExecute, max_vitesse, vitesse, nrj_prec, nrj);
                vitesse = sortie_simul[0];
                nrj = sortie_simul[1];
                choix_action.push_back(vitesse);
                nrj_int.push_back(nrj);
                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                arrival_rate_zero=0;
        }
        vector<vector<vector<double> > > sortie_backward_resolution;
        sortie_backward_resolution.push_back(nrj_int);
        sortie_backward_resolution.push_back(choix_action);
        return(sortie_backward_resolution);
}


bool comparaison_boucle(bool value, vector<vector<double> > & nrj_suiv, vector<vector<double> > & nrj_cour, double epsilon) {
        //        // cas non discounté
        //        value = true;
        //        for (int i= 0; i< nrj_suiv.size(); i++) {
        //                if (i==nrj_suiv.size()-1) {
        //                        cout << span(nrj_cour[i]-nrj_suiv[0]) << endl;
        //                        value = value && !(span(nrj_cour[i]-nrj_suiv[0])<epsilon);
        //                } else {
        //                        cout << span(nrj_cour[i]-nrj_suiv[i+1]) << endl;
        //                        value = value && !(span(nrj_cour[i]-nrj_suiv[i+1])<epsilon);
        //                }
        //        }
        // cas discounté
        value=false;
        for (int i=0; i<nrj_suiv[0].size(); i++) {
                if (abs(nrj_suiv[0][i]-nrj_cour[0][i])>epsilon) {
                        value=true;
                        break;
                }
        }
        return value;
}

bool comparaison_boucle_span(bool value, vector<vector<double> > & nrj_suiv, vector<vector<double> > & nrj_cour, double epsilon) {
        // cas non discounté
        value = true;
        for (int i= 0; i< nrj_suiv.size(); i++) {
                if (i==nrj_suiv.size()-1) {
                        cout << span(nrj_cour[i]-nrj_suiv[0]) << endl;
                        value = value && !(span(nrj_cour[i]-nrj_suiv[0])<epsilon);
                } else {
                        cout << span(nrj_cour[i]-nrj_suiv[i+1]) << endl;
                        value = value && !(span(nrj_cour[i]-nrj_suiv[i+1])<epsilon);
                }
        }
        return value;
}

vector<vector<vector<double> > > value_iteration(double gamma, map<int, vector<Job> > & matrix_job,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init) {
        int longueur = (int)espace_detat_init.size();
        
        vector<vector<double> > nrj_suiv,nrj_cour;
        for (int i=0; i<max_period; i++) {
                vector<double> nrj_suiv_int(longueur,0), nrj_cour_int(nrj_suiv_int);
                nrj_suiv.push_back(nrj_suiv_int);
                nrj_cour.push_back(nrj_cour_int);
        }
        
        vector<vector<vector<double> > > sortie;
        vector<vector<double> > vitesse;
        for (int i=0; i<max_period; i++) {
                vector<double> vitesse_int(longueur,0);
                vitesse.push_back(vitesse_int);
        }
        
        double epsilon=0.001, compteur=0;
        int n = 0;
        
        vector<Job> JobsToExecute;
        Job jobsnul = Job();
        for (int i=0; i<max_period; i++) {
                if (i==max_period-1) {
                        // Préparation JobsToExecute
                        JobsToExecute.push_back(jobsnul);
                        for (int offset=0; offset<matrix_job.size(); offset++) {
                                for (int j=0; j<matrix_job[offset].size(); j++) {
                                        if (offset%matrix_job[offset][j].period==i%matrix_job[offset][j].period) {
                                                JobsToExecute.push_back(matrix_job[offset][j]);
                                                compteur+=matrix_job[offset][j].arrival_rate;
                                        }
                                }
                                JobsToExecute[0].arrival_rate = 1 - compteur;
                        }
                        // Calcul de Value_Iteration
                        compteur=0;
                        sortie.push_back(simulation_general(gamma, espace_detat_init,JobsToExecute,max_vitesse,vitesse[i],nrj_cour[i],nrj_suiv[0]));
                        JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                } else {
                        // Préparation JobsToExecute
                        JobsToExecute.push_back(jobsnul);
                        for (int offset=0; offset<matrix_job.size(); offset++) {
                                for (int j=0; j<matrix_job[offset].size(); j++) {
                                        if (offset%matrix_job[offset][j].period==i%matrix_job[offset][j].period) {
                                                JobsToExecute.push_back(matrix_job[offset][j]);
                                                compteur+=matrix_job[offset][j].arrival_rate;
                                        }
                                }
                                JobsToExecute[0].arrival_rate = 1 - compteur;
                        }
                        // Calcul de Value_Iteration
                        compteur=0;
                        sortie.push_back(simulation_general(gamma, espace_detat_init,JobsToExecute,max_vitesse,vitesse[i],nrj_cour[i],nrj_suiv[i+1]));
                        JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                }
                nrj_suiv[i]=sortie[i][1];
        }
        
        bool value = true;
        //comparaison_boucle_span à la place de comparaison_boucle
        while (comparaison_boucle(value,nrj_suiv,nrj_cour,epsilon)) {
                value = true;
                
                // nrj_cour <- nrj_suiv avec alternance
                for (int i=0; i<max_period; i++) {
                        if (i==max_period-1) {
                                nrj_cour[i]=nrj_suiv[0];
                        } else {
                                nrj_cour[i]=nrj_suiv[i+1];
                        }
                }
                // Prise en compte de chaque periode et simulation avec les jobs existants
                for (int i=0; i<max_period; i++) {
                        if (i==max_period-1) {
                                // Préparation JobsToExecute
                                JobsToExecute.push_back(jobsnul);
                                for (int offset=0; offset<matrix_job.size(); offset++) {
                                        for (int j=0; j<matrix_job[offset].size(); j++) {
                                                if (offset%matrix_job[offset][j].period==i%matrix_job[offset][j].period) {
                                                        JobsToExecute.push_back(matrix_job[offset][j]);
                                                        compteur+=matrix_job[offset][j].arrival_rate;
                                                }
                                        }
                                        JobsToExecute[0].arrival_rate = 1 - compteur;
                                }
                                // Calcul de Value_Iteration
                                compteur=0;
                                sortie[i] = simulation_general(gamma, espace_detat_init,JobsToExecute,max_vitesse,vitesse[i],nrj_cour[i],nrj_suiv[0]);
                                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                        } else {
                                // Préparation JobsToExecute
                                JobsToExecute.push_back(jobsnul);
                                for (int offset=0; offset<matrix_job.size(); offset++) {
                                        for (int j=0; j<matrix_job[offset].size(); j++) {
                                                if (offset%matrix_job[offset][j].period==i%matrix_job[offset][j].period) {
                                                        JobsToExecute.push_back(matrix_job[offset][j]);
                                                        compteur+=matrix_job[offset][j].arrival_rate;
                                                }
                                        }
                                        JobsToExecute[0].arrival_rate = 1 - compteur;
                                }
                                // Calcul de Value_Iteration
                                compteur=0;
                                sortie[i] = simulation_general(gamma, espace_detat_init,JobsToExecute,max_vitesse,vitesse[i],nrj_cour[i],nrj_suiv[i+1]);
                                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                        }
                        
                }
                for (int i=0; i<max_period; i++) {
                        vitesse[i] = sortie[i][0];
                        nrj_suiv[i] = sortie[i][1];
                }
                n=n+1;
                if (n%100==0)
                        cout << n << endl;
        }
        vector<vector<vector<double> > > sortie_value_iteration;
        sortie_value_iteration.push_back(nrj_suiv); sortie_value_iteration.push_back(vitesse);
        vector<vector<double> > ensemble;
        vector<double> n_sortie(1), max_sortie(1), nrj_sortie = nrj_suiv[0] - nrj_cour[0];
        n_sortie[0] = n;
        ensemble.push_back(n_sortie);
        max_sortie[0] = *max_element(nrj_sortie.begin(),nrj_sortie.end());
        ensemble.push_back(max_sortie);
        sortie_value_iteration.push_back(ensemble);        
        return(sortie_value_iteration);
}



//////////////////////////////////////////////////////
////////////////// SIMULATION ////////////////////////
//////////////////////////////////////////////////////




/***********************************************************************************************/
vector<vector<int> > simulation_job_generale(map<int,vector<Job> > & matrix_job, int max_size,int max_deadline,int max_period, int T) {
        vector<vector<int> > sortie_simulation_job;
        vector<int> sortie_taille, sortie_deadline;
        double somme=0, nbre_alea=0;
        int taille=0, deadline=0,period;
        bool sortie_bcl= false;
        
        // Dans matrix_job tjs rangé dans le même ordre
        for (int t=0; t<T; t++) {
                // @TODO cahnger le rand car RAND_MAX est petit par rapport au nombre de simualtion
                nbre_alea = rand()/(double)RAND_MAX;
                for (int offset=0; offset<matrix_job.size(); offset++) {
                        somme=0;
                        for (int j=0; j < matrix_job[offset].size(); j++) {
                                period = matrix_job[offset][j].period;
                                somme += matrix_job[offset][j].arrival_rate;
                                if ((t-offset)%period == 0) {
                                        if (somme>=nbre_alea) {
                                                taille= matrix_job[offset][j].taille;
                                                deadline= matrix_job[offset][j].deadline;
                                                sortie_bcl=true;
                                                break;
                                        }
                                }
                        }
                        if (sortie_bcl) {
                                sortie_bcl=false;
                                break;
                        }
                }
                sortie_taille.push_back(taille); sortie_deadline.push_back(deadline);
                //                cout << " job choisi: " << taille << " " << deadline<< endl;
                somme=0;taille=0;deadline=0;
        }
        sortie_simulation_job.push_back(sortie_taille); sortie_simulation_job.push_back(sortie_deadline);
        return sortie_simulation_job;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**                                             PARTIE ESPACE DETAT RESTREINT                                                                **/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<vector<double> > Ajout_dernier_pas_espace_restreint(vector<vector<double> > & ens_espace_restreint_simu, int max_deadline, int max_size, int reduction_deadline) {
        vector<vector<double> > sortie_new_map;
        vector<double> ajout;
        int fin = max_size*max_deadline;
        int size = (int)ens_espace_restreint_simu.size();
        for (int i=0; i<size; i++) {
                int deb = ens_espace_restreint_simu[i][reduction_deadline-1];
                ajout = ens_espace_restreint_simu[i];
                for (int T=deb; T<fin; T++) {
                        ajout.push_back(T);
                        sortie_new_map.push_back(ajout);
                        ajout.pop_back();
                }
        }
        return sortie_new_map;
}

// cas Reduction espace detat iid
int Simulation_pas_suivant_iid(int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, map<int, vector<double> > & proba_k_restreint) {
        vector<int> nbredepassage(max_size*max_deadline);
        vector<vector<int> > job_arrive;
        int sample = 10000; int somme_value=0,somme_value_k=0, somme_der=0, diff_k_T=0,ct,dt,qte0=0,qte1=0;
        
        for (int k=0; k<sample; k++) {
                for (int i=0; i<=max_deadline;i++) {                                                                                                                            // Arrivée de tache en i
                        job_arrive = simulation_job_generale(matrix_job, max_size, max_deadline,max_period,1);
                        ct=job_arrive[0][0];dt=job_arrive[1][0];
                        //                        cout << "tache: " << ct << " " << dt << endl;
                        if (i+dt <= max_deadline) {                                                                                                                             // calcul de la quantité de travail en 0
                                qte0 += ct;
                        }
                        if (i+dt <= max_deadline+1) {
                                qte1 += ct;
                        }
                        if (i+dt <= max_deadline+reduction_deadline) {                                                                                                          // calcul de la quantité de travail en k
                                somme_value_k += ct;
                        }
                        if (i+dt <= max_deadline+reduction_deadline+1) {                                                                                                        // calcul de la quantité de travail en k+1
                                somme_value += ct;
                        }
                        somme_der += ct;                                                                                                                                        // calcul de la quantité de travail en T
                }
                //                cout << "etat: " << qte0-qte0 << "  " << qte1-qte0 << "  " << somme_value_k-qte0 << "  " << somme_value-qte0 << " -> " << somme_der-qte0 << endl;
                diff_k_T = somme_der-somme_value_k;
                proba_k_restreint[diff_k_T].push_back(somme_value-somme_value_k);
                nbredepassage[diff_k_T]++;
                somme_der=0;somme_value=0;somme_value_k=0;diff_k_T=0;qte0=0;qte1=0;
        }
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                vector<double>::iterator it2 = max_element(it->second.begin(), it->second.end());
                vector<double> value(*it2+1,0);
                for (vector<double>::iterator itj=it->second.begin(); itj!=it->second.end(); ++itj) {
                        value[*itj]++;
                }
                value=value/(double)nbredepassage[it->first];
                proba_k_restreint[it->first] = value;
        }
        cout << "========================= nouvelle valeure en k =========================" << endl;
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                cout << "it->first: " << it->first << " : " << it->second << endl;
        }
        cout << "========================= FIN SIMU =========================" << endl;
        return 1;
}

int Simulation_pas_suivant(vector<vector<double> > & ens_espace_restreint_simu, int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, map<int, vector<double> > & proba_k_restreint){
        //@TODO simulation de -\Delta (0) à  0 (Delta)
        //@TODO calculer letat présent entre 0 et k + T, stocker et additionner Wk+1-Wk
        //@TODO calcul du nombre de passage dans un etat pour diviser après
        vector<int> nbredepassage(ens_espace_restreint_simu.size());
        vector<vector<int> > job_arrive;
        int sample = 10000;
        vector<double> etat_provisoire(reduction_deadline+1);
        vector<double> vide(reduction_deadline+1,0);
        int ct,dt,pos,res_avt_dern;
        int somme_value=0;
        int qte_travail0=0;
        
        for (int k=0; k<sample; k++) {
                for (int i=0; i<=max_deadline;i++) {
                        // Arrivée de tache en i
                        job_arrive = simulation_job_generale(matrix_job, max_size, max_deadline,max_period,1);
                        ct=job_arrive[0][0];dt=job_arrive[1][0];
                        cout << "tache: " << ct << " " << dt << endl;
                        // calcul de la quantité de travail en 0
                        if (i+dt <= max_deadline) {
                                qte_travail0 += ct;
                        }
                        // calcul de letat
                        for (int j=1; j<reduction_deadline+1; j++) {
                                
                                if (i+dt <= j+max_deadline) {
                                        etat_provisoire[j-1] += ct;
                                }
                        }
                        // calcul de la quantité de travail en k+1
                        if (i+dt <= max_deadline+reduction_deadline+1) {
                                somme_value += ct;
                        }
                        // calcul de la quantité de travail en T
                        etat_provisoire[etat_provisoire.size()-1] += ct;
                }
                res_avt_dern = etat_provisoire[etat_provisoire.size()-2];
                cout << etat_provisoire;
                etat_provisoire= etat_provisoire-qte_travail0;
                cout << "etat provisoire: " << etat_provisoire;
                cout << " qté travail " << somme_value-res_avt_dern << " < " << etat_provisoire[etat_provisoire.size()-1] << endl;
                cout << "----------------" << endl;
                // Recherche de letat simulé
                std::vector<vector<double> >::iterator it = find(ens_espace_restreint_simu.begin(),ens_espace_restreint_simu.end(), etat_provisoire);
                pos = (int)(distance(ens_espace_restreint_simu.begin(),it));
                //                cout << ens_espace_restreint_simu[pos] << "  " << etat_provisoire << endl;
                // rajout de la proba de k+1 dans letat simulé
                proba_k_restreint[pos].push_back(somme_value-res_avt_dern);
                nbredepassage[pos]++;
                
                if (somme_value-res_avt_dern==3) {
                        cout << "eh oui "<< endl;
                }
                // initialisation des variables
                somme_value=0;qte_travail0=0;
                etat_provisoire=vide;
        }
        // print les états visités
        for (int i=0; i< nbredepassage.size(); i++) {
                if (nbredepassage[i]!=0) {
                        cout << ens_espace_restreint_simu[i];
                }
        }
        cout << "===================" <<  endl;
        cout << endl;
        
        // parcours de la map proba_k_restreint pour calculer les statistiques d'arrivée de taches
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                vector<double>::iterator it2 = max_element(it->second.begin(), it->second.end());
                vector<double> value(*it2+1,0);
                for (vector<double>::iterator itj=it->second.begin(); itj!=it->second.end(); ++itj) {
                        value[*itj]++;
                }
                value=value/(double)nbredepassage[it->first];
                proba_k_restreint[it->first] = value;
        }
        // print état et proba associé en k+1
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                cout << "it->first: " << it->first << "  " << ens_espace_restreint_simu[it->first] << it->second << endl;
        }
        cout << "FIN " << endl;
        return 1;
}



/**********************************************************************************************************************************************************************************/
/**       Creation du nou**/
/**********************************************************************************************************************************************************************************/


int New_job_probability_arrival(map<int, vector<Job> > & matrix_job,map<int, vector<Job> > & matrix_job_restreint,map<int, vector<double> > & proba_k_restreint, int reduction_deadline) {
        vector<Job> ens_job_restreint;
        // garde les probabilités d'arrivé pour les jobs inférieur à max_deadline_reduction
        for (int i=0; i<matrix_job.size(); i++) {
                for (int j=0; j<matrix_job[i].size(); j++) {
                        if (matrix_job[i][j].deadline<reduction_deadline) {
                                if (matrix_job_restreint.size()<i) {
                                        ens_job_restreint.push_back(matrix_job[i][j]);
                                        matrix_job_restreint[i] = ens_job_restreint;
                                } else {
                                        matrix_job_restreint[i].push_back(matrix_job[i][j]);
                                }
                        }
                }
        }
        
        // Rajout la nouvelle proba pour chaque etat
        // tableau qui a chaque etat associe une suite de Job de deadline = k, vec des tailles et de sprobas diff
        // Attention ici la proba dépend de létat
        Job nouvelle_tache;
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                for (int i=0;i<it->second.size(); i++) {
                        if (it->second[i]!= 0) {
                                if (i != 0) {
                                        nouvelle_tache.deadline = reduction_deadline;
                                        nouvelle_tache.taille = i;
                                        nouvelle_tache.period = 1;
                                        nouvelle_tache.arrival_rate = it->second[i];
                                        matrix_job_restreint[0].push_back(nouvelle_tache);
                                }
                        }
                }
        }
        // Rajout les taches avec deadline l > k et remplace leur deadline l par k+1.
        for (int i=0; i<matrix_job.size(); i++) {
                for (int j=0; j<matrix_job[i].size(); j++) {
                        if (matrix_job[i][j].deadline>reduction_deadline) {
                                ens_job_restreint.push_back(matrix_job[i][j]);
                                matrix_job_restreint[i].push_back(matrix_job[i][j]);
                                matrix_job_restreint[i][matrix_job_restreint[i].size()-1].deadline = reduction_deadline;
                        } else {
                                //                                matrix_job_restreint[i].push_back(matrix_job[i][j]);
                        }
                }
        }
        cout << "FIN" << endl;
        return 0;
}


vector<double> calcul_meilleur_vitesse(int position_etat_courant,vector<vector<double> > & espace_detat_init,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse,map<int,vector<double> > & proba_k_restreint, int reduction_deadline) {
        vector<double> etat_courant = espace_detat_init[position_etat_courant];
        // l'ensemble des valeurs d'nrj possibles en testant toutes les vitesses autorisées
        vector<double> valeur_nrj_suiv_res;
        // parcours l'ensemble des vitesses disponibles et regarde laquelle est la meilleur: On part du minimum possible, i.e. on execute la vitesse qui fait tout le travail au pas de temps suivant, et on vérifie que l'on est audessus des pas de temps qui sont déjà présent en mémoire
        vector<double> borne_inf_vitesse;
        // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
        for (int i = 0;i < etat_courant.size(); i++) {
                borne_inf_vitesse.push_back(ceil((double)etat_courant[i]/(double)(i+1)));
        }
        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());
        // BORNE SUR LES VITESSES par : soit la vitesse max permise, soit par la vitesse qui effectue tout le travail déjà arrivé.
        int arret = min(etat_courant[etat_courant.size()-1],(double)max_vitesse);
        //        cout << "avt: " << arret << endl;
        // Vitesse ne peut pas dépasser w(k) dans le cas reduit
        arret = min((double)arret,etat_courant[etat_courant.size()-2]);
        borneInfVitesse = min(borneInfVitesse,arret);
        //        cout << "apres: " << arret << endl;
        // On boucle sur toutes les vitesses, on regarde les valeurs obtenues et on prend la vitesse et l'energie minimal, ie celle qui consomme le moins d'énergie au cours du temps (on teste avec toutes les vitesses et toutes les tailles de tache possible).
        vector<double> vitesse_possible;
        vector<double> indice_etat_suivant_tache, indice_etat_suivant_tache2;
        
        // Cas premier pas
        vector<Job> Job_temp;
        Job nouvelle_tache;
        //        double valeur_init_p0 = JobsToExecute[0].arrival_rate;
        
        cout << "===============  EN ENTREE : ";
        for (int j=0; j<JobsToExecute.size(); j++) {
                cout << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
        }
        
        vector<double> proba_k1 = proba_k_restreint[etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2]];
        
        // @ENLEVER
        // Détermination des taches a rajouter depend de wT-wk
        //        if (JobsToExecute[0].arrival_rate!=1 || JobsToExecute[0].deadline!=0) {
        //                // prendre le bon vecteur de proba
        //                vector<double> proba_k1;
        //                proba_k1 = proba_k_restreint[etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2]];
        //                //                cout << "proba " << proba_k1 << endl;
        //                //                cout << "taille "<< etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2] << endl;
        //                
        //                for (int l=0; l<proba_k1.size(); ++l) {
        //                        //                        cout << "prob " << prob << endl;
        //                        if (l==0) {
        //                                prob=proba_k1[l];
        //                        }
        //                        if (proba_k1[l]!= 0 && l!=0) {
        //                                nouvelle_tache.deadline = reduction_deadline;
        //                                nouvelle_tache.taille = etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2];
        //                                nouvelle_tache.period = 1;
        //                                nouvelle_tache.arrival_rate = proba_k1[l];
        //                                Job_temp.push_back(nouvelle_tache);
        //                        }
        //                }
        //        }
        
        
        
        //                for (int j=0; j<JobsToExecute.size(); j++) {
        //                        cut << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
        //                }
        //                cout << "===============  prob : " << prob << endl;
        //         Ajout de proba_k1 au 0 de JobsToExecute
        
        //        // @ENLEVER
        //        if(JobsToExecute[0].taille==0 && JobsToExecute[0].arrival_rate!=1) {
        //                JobsToExecute[0].arrival_rate = prob;
        //                JobsToExecute[0].period = 1;
        //        }
        
        //        // @ENLEVER
        //        // @Garder en mémoire les états à supprimer
        //        int Suppress=-1;// etat a supprimer à la fin de la fonction
        //        for (int i=0;i<Job_temp.size();i++) {
        //                JobsToExecute.push_back(Job_temp[i]);
        //                Suppress++;
        //        }
        cout << "===============  UTILISER  : ";
        for (int j=0; j<JobsToExecute.size(); j++) {
                cout << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
        }
        
        // BOUCLE sur les VITESSES = i
        for (int i = borneInfVitesse; i <= arret; i++) {
                // CAS PAS D'ARRIVEE DE TACHE, on enregistre sa position
                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
                //indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,0,0,i,espace_detat_init));
                
                for (int j=0; j<JobsToExecute.size(); j++) {
                        indice_etat_suivant_tache.push_back(recherche_etat_suiv(position_etat_courant,JobsToExecute[j].deadline,JobsToExecute[j].taille,i,espace_detat_init));
                }
                
                double cout_valeur_nrj_suiv_res = cout_nrj(i,max_vitesse);
                
                for (int j=0; j<indice_etat_suivant_tache.size(); j++) {
                        //@TODO Rajouter le cas (0,0)
                        //                        cout << JobsToExecute[j].arrival_rate << endl;
                        cout_valeur_nrj_suiv_res += (JobsToExecute[j].arrival_rate)*nrj_prec[indice_etat_suivant_tache[j]];
                }
                valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res);
                vitesse_possible.push_back(i);
        }
        std::vector<double>::iterator it =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end());
        // @TODO remettre en cause l'utilisation de la fonction distance => l'indice ne correspond pas forcément à la distance ????
        double pos = distance(valeur_nrj_suiv_res.begin(), it);
        double nrj;
        int vitesse;
        if (it!= valeur_nrj_suiv_res.end()) {
                nrj = *it;
                vitesse = vitesse_possible[(int)pos];
        } else {
                nrj = 0;
                vitesse = 0;
        }
        vector<double> vitesse_et_energie;
        vitesse_et_energie.push_back(nrj);
        vitesse_et_energie.push_back((double)vitesse);
        //        cout << "nrj " << nrj << " vitesse " << vitesse << endl;
        //        cout << "===============" << endl;
        
        // Remettre JobsTo execute a sa valeur initiale
        // @TODO supprimer les etats et mettre la proba 0 arrivée à valeur_init_p0
        //        vector<Job>::iterator it_sup = JobsToExecute.begin();
        //        cout << "valeur à supprimer :  " << Suppress << endl;
        
        //        // @ENLEVER
        //        if (Suppress!=-1) {
        //                Suppress++;
        //                for (int i=0; i<Suppress; i++) {
        //                        it_sup++;
        //                }
        //                it_sup++;
        //                for (int i=Suppress; i<JobsToExecute.size(); i++) {
        //                        JobsToExecute.erase(it_sup);
        //                }
        //                JobsToExecute[0].arrival_rate = valeur_init_p0;
        //        }
        
        
        //        for (vector<Job>::iterator it=JobsToExecute.end(); it >JobsToExecute.end()-Suppress; --it) {
        //                JobsToExecute.erase(it);
        //        }
        
        cout << "===============  EN SORTIE : " ;
        for (int j=0; j<JobsToExecute.size(); j++) {
                cout << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
        }
        cout << endl;
        return(vitesse_et_energie);
}

/************************************************************************************************************/
/************************************************************************************************************/
/***                                 CAS RESTREINT, (k,\Delta)                                            ***/
/************************************************************************************************************/
/************************************************************************************************************/

vector<double> calcul_meilleur_vitesse_restreint(int position_etat_courant,vector<vector<double> > & espace_detat_init,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse,map<int,vector<double> > & proba_k_restreint, int reduction_deadline) {
        vector<double> etat_courant = espace_detat_init[position_etat_courant];
        vector<double> valeur_nrj_suiv_res,borne_inf_vitesse,vitesse_possible, indice_etat_suivant_tache, indice_etat_suivant_tache2,proba_k1;
        
        /************ Borne des vitesses possibles ************/
        for (int i = 0;i < etat_courant.size(); i++) {
                borne_inf_vitesse.push_back(ceil((double)etat_courant[i]/(double)(i+1)));                                                                                                                               // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
        }
        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());                                                                                                         // Vitesse maximale que l'on doit réaliser pour que ce soit réalisable
        int arret = min(etat_courant[etat_courant.size()-1],(double)max_vitesse);                                                                                                                       // vitesse < w(Delta) et < Vmax
        arret = min((double)arret,etat_courant[etat_courant.size()-2]);                                                                                                                                 // vitesse < w(k) dans le cas reduit
        borneInfVitesse = min(borneInfVitesse,arret);                                                                                                                                                   // borneInfVitesse < arret, par def
        
        proba_k1 = proba_k_restreint[etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2]];                                                                                          // probabilité de jump entre k et k+1, vecteur dont la position correspond à la taille du saut et la valeur la probabilité d'avoir cette valeur
        
        /************ Calcul de la vitesse à utiliser ************/
        for (int i = borneInfVitesse; i <= arret; i++) {                                                                                                                                                // BOUCLE sur les VITESSES = i
                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
                for (int j=0; j<JobsToExecute.size(); j++) {                                                                                                                                            // ITERATION sur le taches qui arrivent
                        for (int k=0; k<proba_k1.size(); k++) {                                                                                                                                         // ITERATION sur les jumps de k-> k+1
                                if (proba_k1[k]!=0) {                                                                                                                                                   // ne considère que les pas k-> k+1 non nul
                                        indice_etat_suivant_tache.push_back(recherche_etat_suiv_restreint(position_etat_courant,min(JobsToExecute[j].deadline,reduction_deadline+1),JobsToExecute[j].taille,reduction_deadline,k,i,espace_detat_init));             // (k,reduction_deadline) et (taille,deadline) avec vitesse i
                                }
                        }
                }
                double cout_valeur_nrj_suiv_res = cout_nrj(i,max_vitesse);                                                                                                                              // cout du choix de cette vitesse
                for (int j=0; j<JobsToExecute.size(); j++) {
                        for (int k=0; k<proba_k1.size(); k++) {
                                if (proba_k1[k]!=0) {
                                        cout_valeur_nrj_suiv_res += (JobsToExecute[j].arrival_rate)*proba_k1[k]*nrj_prec[indice_etat_suivant_tache[j]];                                                 // ajout en fonction des probas considérer, on multiplie la proba d'arrivée d'une tache par la proba de passage de k->k+1
                                }
                        }
                }
                valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res);                                                                                                                                // ensemble des couts possibles en fonction de la vitesse
                vitesse_possible.push_back(i);                                                                                                                                                          // enregistrement des vitesses
        }
        std::vector<double>::iterator it =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end());                                                                                        // recherche du cout minimal
        double pos, nrj; int vitesse;
        // nrj = distance(valeur_nrj_suiv_res.begin(), it);                                                                                                                                                // recherche de la positon du cout minimal
        if (it!= valeur_nrj_suiv_res.end()) {
                nrj = *it; vitesse = vitesse_possible[(int)pos];                                                                                                                                        // cas une vitesse correspond
        } else {
                nrj = 0; vitesse = 0;                                                                                                                                                                   // cas aucune vitesse ne correspond
        }
        vector<double> vitesse_et_energie; vitesse_et_energie.push_back(nrj); vitesse_et_energie.push_back((double)vitesse);                                                                            // vecteur de sortie (ernergie, vitesse)
        return(vitesse_et_energie);
}


vector<vector<double> > simulation_general_restreint(vector<vector<double> > & espace_detat_init,vector<Job> & JobsToExecute,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj, map<int,vector<double> > & proba_k_restreint,int reduction_deadline) {
        for (int s  = 0; s < espace_detat_init.size(); s++) {
                //                cout << "valeur de l'etat : " << s <<  " qui vaut " << espace_detat_init[s];
                vector<double> vitesse_et_energie = calcul_meilleur_vitesse_restreint(s, espace_detat_init, nrj_prec,JobsToExecute, max_vitesse,proba_k_restreint,reduction_deadline);                  // ATTENTION CHANGER calucl_meilleur_vitesse_restreint en calcul_meilleur_vitesse
                nrj[s] = vitesse_et_energie[0]; vitesse[s] = vitesse_et_energie[1];
        }
        vector<vector<double> > sortie_simul; vector<double> vitesseCast(vitesse.begin(), vitesse.end()); sortie_simul.push_back(vitesseCast); sortie_simul.push_back(nrj);
        return(sortie_simul);
}

vector<vector<vector<double> > > backward_resolution_reduit(int T, map<int,vector<Job> > & matrix_job, map<int,vector<double> > & proba_k_restreint, int reduction_deadline, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init) {
        int longueur = (int)espace_detat_init.size();
        vector<double> nrj(longueur,0);
        vector<double> nrj_prec(nrj);
        vector<vector<double> > nrj_int, choix_action, sortie_simul;
        double arrival_rate_zero = 0;
        vector<double> vitesse(longueur,0);
        int ind_bcl = T-1;
        vector<Job> JobsToExecute;
        int tt, limite;
        
        for (int t=ind_bcl; t>=0; t--) {
                nrj_prec = nrj;                                                                                                                                 // nrj étape précédente.
                cout << "==================================== T ====================================" << t << endl;
                limite = T-max_deadline;                                                                                                                        // Distinguer les max_deadline derniers cas, car dans ce cas p=0
                if (t>limite) {
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                } else {                                                                                                                                        // JobsToExecute : ensemble des jobs a executer sur 1 pas de temps 
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                        arrival_rate_zero=0;
                        for (int i=0; i< matrix_job.size(); i++) {                                                                                              // Calcul de la probabilité d'avoir une tache nulle
                                for (int j=0; j<matrix_job[i].size(); j++) {
                                        map<int,vector<Job> >::iterator it;
                                        it = matrix_job.find(i);
                                        if (it!=matrix_job.end()) {
                                                int period = (matrix_job[i][j]).period;
                                                if ((t-i)%period==0) {                                                                                          // Sélection des jobs Jobs avec leur arrival time a executer au temps t
                                                        JobsToExecute.push_back(matrix_job[i][j]);
                                                        arrival_rate_zero+=matrix_job[i][j].arrival_rate;
                                                        cout << arrival_rate_zero << endl;
                                                }
                                        }
                                }
                        }
                }
                JobsToExecute.at(0).arrival_rate= 1-arrival_rate_zero;                                                                                           // Donne la probabilité de non arrivée de tache (calculé par comparaison avec les taches existantes
                if (t==0) {tt=0;} else {tt = t-1;}
                cout << "nbre de jobs " << JobsToExecute.size() << endl;
                for (int j=0; j<JobsToExecute.size(); j++) {
                        cout << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
                }
                sortie_simul = simulation_general_restreint(espace_detat_init,JobsToExecute, max_vitesse, vitesse, nrj_prec, nrj,proba_k_restreint,reduction_deadline);
                vitesse = sortie_simul[0]; nrj = sortie_simul[1];
                choix_action.push_back(vitesse);
                nrj_int.push_back(nrj);
                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                arrival_rate_zero=0;
        }
        vector<vector<vector<double> > > sortie_backward_resolution; sortie_backward_resolution.push_back(nrj_int); sortie_backward_resolution.push_back(choix_action);
        return(sortie_backward_resolution);
}


int Etat_suppression(vector<vector<double> > & ens_espace_restreint_simu_final, map<int, vector<double> > & proba_k_restreint) {
        int diff;
        vector<int>::iterator it_res;
        vector<int> ens_diff;
        
        for (map<int, vector<double> >::iterator it=proba_k_restreint.begin(); it!=proba_k_restreint.end(); ++it) {
                ens_diff.push_back(it->first);
        }
        for (vector<vector<double> >::iterator it_ens = ens_espace_restreint_simu_final.begin(); it_ens!=ens_espace_restreint_simu_final.end();++it_ens) {
                diff = (*it_ens)[(*it_ens).size()-1]-(*it_ens)[(*it_ens).size()-2];
                it_res = find(ens_diff.begin(),ens_diff.end(),diff);
                if (it_res == ens_diff.end()) {
                        ens_espace_restreint_simu_final.erase(it_ens);
                        it_ens--;
                }
        }
        return 1;
}

// CAS k taches : Construction à partir de l'ensemble des taches et du dernier pas somme_der de l'état considéré en Delta sans le dernier pas
int Construction_etat_espace_restreint(map<int,vector<int> > & ensemble_taches, int nbreDeTaches, vector<int> & etat_considere, int wDelta) {
        int taille=2*nbreDeTaches;
        int iteration=0;
        for (map<int,vector<int> >::iterator it=ensemble_taches.begin(); it!=ensemble_taches.end();++it) {
                //  cout << it->first << " => " << it->second;
                for (int il=0; il<it->second.size();il++) {
                        if (iteration<taille) {
                                etat_considere.push_back(it->second[il]);
                                etat_considere.push_back(it->first);
                                iteration+=2;
                        } else {
                                //                                cout << "Autre " <<  it->first << "  " << it->second[il] << endl;;
                                //                                break;
                        }
                }
        }
        etat_considere.push_back(wDelta);
        // rajout des zeros en debut si besoin
        while (etat_considere.size()!=2*nbreDeTaches+1) {
                etat_considere.insert(etat_considere.begin(), 0); etat_considere.insert(etat_considere.begin(), 0);
        }
        return 1;
}

// CAS k sauts : Construction à partir de l'ensemble des taches de l'état considéré en Delta sans le dernier pas
int Construction_etat_espace_restreint_ksaut(int m, map<int, vector<int> > & ensemble_taches,vector<int> & etat_considere_ksaut, int wDelta) {
        int compteur = m; int val=0;
        for (map<int, vector<int> >::iterator it = ensemble_taches.begin();it!=ensemble_taches.end();++it) {
                if (compteur>0) {
                        //                        cout << "tache:  " << it->first << "  " << it->second;
                        for (int kl=0; kl<it->second.size();kl++) {
                                val+=it->second[kl];
                        }
                        etat_considere_ksaut.push_back(val);etat_considere_ksaut.push_back(it->first);
                }
                compteur--;
                val=0;
        }
        etat_considere_ksaut.push_back(wDelta);
        
        // rajout des zeros en debut si besoin
        while (etat_considere_ksaut.size()!=2*m+1) {
                etat_considere_ksaut.insert(etat_considere_ksaut.begin(), 0); etat_considere_ksaut.insert(etat_considere_ksaut.begin(), 0);
        }
        //        int nb_tache = m;
        //        for (int l=0; l<etat_provisoire.size(); l++) {
        //                if (etat_provisoire[l+1] > etat_provisoire[l] && nb_tache>0) {
        //                        //                        cout << l+1 << "    " << etat_provisoire[l+1]-etat_provisoire[l] << endl;
        //                        etat_considere_ksaut.push_back(etat_provisoire[l+1]-etat_provisoire[l]);
        //                        etat_considere_ksaut.push_back(l+1);
        //                        nb_tache--;
        //                }
        //        }
        //        if (nb_tache!=0) {
        //                int taille_nb_tache = m*2-nb_tache*2;
        //                for (int ii=0; ii<taille_nb_tache; ii++) {
        //                        etat_considere_ksaut.insert(etat_considere_ksaut.begin(),0);
        //                }
        //        }
        return 1;
}

/**** Recherche de la tache qui est rajouté artificiellement au pas k+1 ****/
// Recherche de letat rajouté, on enlève -1 pour mettre en place le shift
int Recherche_tache_rajouter(vector<int> & etat_considere, vector<int> & etat_considere_suivant, vector<int> & NouvelleTache, int dt) {
        vector<int> etat_considere_test = etat_considere; int val=0;
        int compteur=0;
        for (vector<int>::iterator it_kl = etat_considere_test.begin(); it_kl != etat_considere_test.end()-1; ++it_kl) {
                if (compteur%2==1) {
                        if (*it_kl==1 || *it_kl==0) {
                                etat_considere_test.erase(it_kl);
                                etat_considere_test.erase(it_kl--);
                                it_kl--;
                        } else {
                                val=(*it_kl)--;
                        }
                }
                compteur++;
        }
        
        // Recherche de la tache ajoutée en comparant etat_considere_prec et etat_considere_suivant
        compteur=0;
        for (int kl=0; kl<etat_considere_suivant.size()-1; kl++) {
                if (compteur%2==1 && etat_considere_test.size()>kl) {
                        if (etat_considere_test[kl] != etat_considere_suivant[kl] && etat_considere_test[kl]<dt) {
                                NouvelleTache.push_back(etat_considere_test[kl-1]);NouvelleTache.push_back(etat_considere_test[kl]);
                        }
                }
                if (compteur%2==1 && etat_considere_test.size()<=kl && etat_considere_suivant[kl]<dt) {
                        NouvelleTache.push_back(etat_considere_suivant[kl-1]);NouvelleTache.push_back(etat_considere_suivant[kl]);
                }
                compteur++;
        }
        if (NouvelleTache.empty()) {
                NouvelleTache.push_back(0);NouvelleTache.push_back(0);
        }
        //        cout << "Nouvelles taches:  " << NouvelleTache;
        return 1;
}

int Recherche_tache_rajouter_ameliorer(map<int, vector<int> > & ensemble_taches_suivant, map<int, vector<int> > & ensemble_taches, vector<int> & NouvelleTache) {
        int nb_tache = 2;
        for (map<int, vector<int> >::iterator it = ensemble_taches_suivant.begin(); it != ensemble_taches_suivant.end(); it++) {
                //for (int i=0; i<ensemble_taches_suivant.size(); i++) {
                //int deadline = ensemble_taches_suivant[i]->first;
                //for (int j=0; j<ensemble_taches_suivant[i].size(); j++) {
                for (int j=0; j<(it->second).size(); j++) {
                        //for (i=0; i<(*it).siz
                        cout << "jhkj " << (it->second)[j] << " et " <<  (it->first) <<  endl;
                        if (nb_tache>0) {
                                nb_tache--;
                        } else if (nb_tache == 0) {
                                NouvelleTache[0] = (it->second)[j];
                                NouvelleTache[1] = it->first;
                        }
                }
        }
        cout << "Quelle nouvelle tache ? " << NouvelleTache;
        cout << endl;
        return 0;
}


int Creation_ensemble_etat_suivant(map<int, vector<int> > & ensemble_taches,map<int, vector<int> > & ensemble_taches_suivant,int dt,int ct) {
        // Shift lespace considere vers la droite, avancé du temps + ajout d ela nouvelle tache (c,d)
        bool dejaAjouter = false;
        for (map<int,vector<int> >::iterator it=ensemble_taches.begin(); it!=ensemble_taches.end();++it) {
                if (it->first-1>dt && !dejaAjouter) {
                        //                                cout << "position de la nouvelle tache si AJOUTé: " << dt << " et " << ensemble_taches_suivant[dt].size() << endl;
                        //                                posdt=dt;posdtvect = (int)ensemble_taches_suivant[dt].size() ;
                        ensemble_taches_suivant[dt].push_back(ct);
                        dejaAjouter=true;
                }
                if (it->first-1>0) {
                        ensemble_taches_suivant[it->first-1]=it->second;
                }
        }
        if (!dejaAjouter) {
                ensemble_taches_suivant[dt].push_back(ct); //posdt=dt;posdtvect = (int)ensemble_taches_suivant[dt].size() ;
                //                        cout << "position de la nouvelle tache si AJOUTé: " << dt << " et " << ensemble_taches_suivant[dt].size() << endl;
        }
        return 1;
}




/****
 Nouvelle fonction créé pour la partie simulation
 ****/

int Construction_Etat_restreint(map<int,vector<int> > & ETaches_courant,int reduction_deadline, vector<int> & Etat_courant_reduit, vector<int> & tache_simuler) {
        int res = reduction_deadline;
        int taille=0;
        bool sortie = true;
        map<int,vector<int> >::iterator it_res;int i_res;
        
        // Ajout 1 à 1 les jobs
        for (map<int,vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end(); it++) {
                for (int i=0; i<it->second.size(); i++) {
                        if (res>0) {
                                Etat_courant_reduit.push_back((it->second)[i]);
                                Etat_courant_reduit.push_back((it->first));
                                res--;
                        } else {
                                // tache à simuler
                                // tache_simuler = it->second;
                                if (sortie) {
                                        tache_simuler.push_back((it->second)[i]); tache_simuler.push_back((int)(it->first)-1);
                                        sortie = false;
                                        it_res = it;
                                        i_res=i;
                                }
                        }
                }
        }
        for (map<int,vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end(); it++) {
                for (int i=0; i<it->second.size(); i++) {
                        taille+=(it->second)[i];
                }
        }
        // Ajout de la somme totale des jobs
        Etat_courant_reduit.push_back(taille);
        // Rajoute les zeros si besoins 
        while (Etat_courant_reduit.size()<reduction_deadline*2) {
                Etat_courant_reduit.insert(Etat_courant_reduit.begin(), 0);
                Etat_courant_reduit.insert(Etat_courant_reduit.begin(), 0);
        }
        int taille_reduit = 0;
        for (int i=0; i < Etat_courant_reduit.size()-1; i++) {
                if (i%2 == 0) {
                        taille_reduit+=Etat_courant_reduit[i];
                }
        }
        //        cout << "taille " << taille << " taille reduit " << taille_reduit << endl;
        Etat_courant_reduit[Etat_courant_reduit.size()-1] -= taille_reduit;
        
        //@TODO Généraliser à plusieurs cas => exception si on doit rajouter plusieurs jobs d'un coup
        // boucle sur les deadlines, verif valeur = 1
        bool il_test=false;
        for (int il=0; il<Etat_courant_reduit.size()-1; il++) {
                if (il%2!=0) {
                        if (Etat_courant_reduit[il]==1) {
                                il_test=true;
                        } else {
                                il_test=false;
                                break;
                        }
                }
        }
        
        if (il_test && Etat_courant_reduit[Etat_courant_reduit.size()-1]!=0) {
                //                cout << "ok" << endl;
                //@TODO ajouter le job en plus
                if (i_res+1 < (int)((it_res->second).size())) {
                        tache_simuler.push_back((it_res->second)[i_res+1]);tache_simuler.push_back((int)(it_res->first)-1);
                } else {
                        it_res++;
                        if (it_res != ETaches_courant.end()) {
                                tache_simuler.push_back(it_res->second[0]);tache_simuler.push_back((int)(it_res->first)-1);
                        }
                }
        }
        
        if (tache_simuler.size()==0) {
                tache_simuler.push_back(0); tache_simuler.push_back(0);
        }
        return taille;
}


int Enregistre_table(vector<int> & Etat_courant_reduit, vector<int> & tache_simuler, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter) {
        //@TODO vérifier si l'état existe dans la liste Etat_visiter
        // si oui, ajout + ajout de tache_simuler dans la liste tache_rajouter et incrémentation du nombre correspondant dansnb_tache_rajouter
        vector<vector<int> > Ajout;
        int pos_etat, pos_etat2;
        vector<vector<int> >::iterator it_find2, it_find;
        it_find = find(Etat_visiter.begin(), Etat_visiter.end(), Etat_courant_reduit);
        
        // Etat jamais visité
        if (it_find==Etat_visiter.end()) {
                // ajout de l'etat
                Etat_visiter.push_back(Etat_courant_reduit);
                // Ajout de la tache simuler
                Ajout.push_back(tache_simuler);
                tache_rajouter.push_back(Ajout);
                vector<double> Ajout_job;
                Ajout_job.push_back(1);
                nb_tache_rajouter[(int)tache_rajouter.size()-1] = Ajout_job;
        } else {
                // Etat deja visité
                // Recupere la position de letat dans l'ensemble des etats visité
                pos_etat = (int)(it_find-Etat_visiter.begin());
                vector<vector<int> > Ajout_test; Ajout_test.push_back(tache_simuler);
                // itérateur sur l'ensemble des taches rajoutées pour un etat visité
                vector<vector<int> > VectInt = tache_rajouter[pos_etat];
                it_find2 = find(VectInt.begin(), VectInt.end(), tache_simuler);
                //                cout << "iterateur: " << *it_find2 << endl;
                //                cout << "===" << endl;
                if (it_find2 == VectInt.end()) {
                        // Cas tache simulé non présent, on la rajoute et initialise son compteur
                        tache_rajouter[pos_etat].push_back(tache_simuler);
                        nb_tache_rajouter[pos_etat].push_back(1);
                } else {
                        // Cas tache simulé présent, on recherche la position de cette tache simulé da,s tache_rajouter et on incrémente le compteur
                        pos_etat2 = (int)(it_find2 - VectInt.begin());
                        nb_tache_rajouter[pos_etat][pos_etat2]++;
                }
                
        }
        return 0;
}

int Actualise_etat_suivant(map<int,vector<int> > & ETaches_courant, int ct, int dt, int vitesse) {
        //@TODO shift -1 en deadline (suppression des jobs finis) + ajout de la nouvelle tache au bon endroit
        map<int,vector<int> > ETaches_suivant;
        int pos, vitesse_int = vitesse;
        int value_int=0;
        
        // total ensemble des valeurs à enlever
        bool perte = true;
        //        cout << "deb etat actualisé " << endl;
        // Intégrer la nouvelle tache au fur et à mesure
        for (map<int,vector<int> >::iterator it = ETaches_courant.begin(); it != ETaches_courant.end();it++) {
                if (it->first>1) {
                        pos = it->first - 1;
                        if (perte) {
                                for (int i = 0; i<(it->second).size(); i++) {
                                        if (perte) {
                                                value_int = max(0,it->second[i]-vitesse_int);
                                                vitesse_int-=it->second[i];
                                                // Si value_int > 0, alors il n'y a plus de nouveaux jobs à executer avec la vitesse
                                                // donc on peut l'ajouter dans le nouvel espace de taches suivants
                                                if (value_int>0) {
                                                        vector<int> Ajout_jobs;
                                                        Ajout_jobs.push_back(value_int);
                                                        ETaches_suivant[pos] = Ajout_jobs;
                                                        perte = false;
                                                }
                                        } else {
                                                ETaches_suivant[pos].push_back(it->second[i]);
                                        }
                                }
                        } else {
                                ETaches_suivant[pos] = it->second;
                        }
                        //                                }
                        //                }
                } else if (it->first==1) {
                        if (vitesse>0) {
                                for (int i = 0; i<(it->second).size(); i++) {
                                        vitesse_int-=it->second[i];
                                }
                        }
                }
        }
        //        cout << "decalge + vitesse " << endl;
        bool present = false;
        for (map<int,vector<int> >::iterator it = ETaches_suivant.begin(); it != ETaches_suivant.end();it++) {
                if (it->first == dt) {
                        it->second.push_back(ct);
                        present = true;
                }
        }
        //        cout << "ajout tache" << endl;
        // Rajouter et dt !=0; => le cas ou on a seulement la tache (0,0), ie en fin de backward resolution ou si on a aucun job qui arrive
        if (!present && dt != 0) {
                vector<int> Ajout;
                Ajout.push_back(ct);
                ETaches_suivant[dt] = Ajout;
        }
        ETaches_courant = ETaches_suivant;
        //        cout << "fin etat actulaisé " << endl;
        
        return 0;
}


int Simulation_pas_suivant_iid_premier_pas(int T,int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, vector<vector<int> > & proba_k_restreint, vector<vector<int> > & Espace_detat_reduit_kpremieresTaches, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter) {
        vector<vector<int> > job_arrive;
        int ct,dt;
        int nb_simulation = 10000;
        
        int testpassage = 0;
        for (int k=0; k<nb_simulation; k++) {
                
                vector<int> Etat_courant_reduit, tache_simuler;
                map<int, vector<int> > ETaches_courant, ETaches_suivant;
                
                for (int i=0; i<T;i++) {
                        if (visible_general) {
                                cout << "========================> iteration: " << i << endl;
                        }
                        // Determine l'etat courant reduit à partir de ETaches_courant
                        // Détermine la reduction+1ième tache
                        Construction_Etat_restreint(ETaches_courant,reduction_deadline,Etat_courant_reduit, tache_simuler);
                        if (visible_general) {
                                cout << "Etat courant reduit " << Etat_courant_reduit;
                        }
                        if (Etat_courant_reduit[0]==1 && Etat_courant_reduit[1]==1 && Etat_courant_reduit[2]==1 && Etat_courant_reduit[3]==1) {
                                cout << "test " << endl;
                        }
                        // Enregistre dans une table etat courant reduit et la tache simulée
                        Enregistre_table(Etat_courant_reduit, tache_simuler, Etat_visiter, tache_rajouter, nb_tache_rajouter);
                        if (visible_general) {
                                cout << "tache suivante  " << tache_simuler << endl;
                                if (Etat_courant_reduit[1]==1 && Etat_courant_reduit[3]==1) {
                                        cout << "un autre job " << endl;
                                }
                        }
                        
                        cout << "PAS DE TEMPS SUIVANT" << endl;
                        
                        // Arrivé de la nouvelle tache
                        job_arrive = simulation_job_generale(matrix_job, max_size, max_deadline,max_period,1);
                        ct=job_arrive[0][0];dt=job_arrive[1][0];
                        
                        // CAS PREEMPTION du premier job ====================
                        // @DONE generalisation dans le cas ou c'est n'importe quelle tache qui preempte la première
                        // test correspond à la première valeur de la deadline utilisé
                        // ig_res correspond à l'indice de la deadline correspondante, pour pouvoir recuperer la taille
                        int test=-1; int ig_res = -1;
                        for (int ig=0; ig <Etat_courant_reduit.size()-1;ig++) {
                                if (ig%2!=0) {
                                        if (Etat_courant_reduit[ig] != 0) {
                                                test = Etat_courant_reduit[ig]-1;
                                                ig_res = ig;
                                                break;
                                        }
                                }
                        }
                        cout << "==========================================> value:  " << Etat_courant_reduit[3]-1 << "  " << test << endl;
                        if (dt<test && dt!=0) {
                                double nbre_alea = 0;
                                //                        if (dt==2 && Etat_courant_reduit[1]>2) {
                                // @TODO ATTENTION influence de l'alea
                                nbre_alea = rand()/(double)RAND_MAX;
                                //                                }
                                double somme=0;
                                int vitesse=0;
                                while (!(somme>=nbre_alea)) {
                                        somme+=1/(double)Etat_courant_reduit[ig_res-1];
                                        vitesse++;
                                }
                                ETaches_courant[Etat_courant_reduit[ig_res]][0] -= vitesse;
                                cout << ig_res<< "  vs  " << "3"<< endl;
                                cout << "pb potentiel:  " << vitesse <<  endl;
                                testpassage++;
                        }
                        
                        if (visible_general) {
                                cout << "===============================> job arrive " << ct << "  " << dt << endl;
                        }
                        /*********************************************************************/
                        
                        // Calcul etat suivant avec ETaches_courant et job_arrive (remplace la valeur de ETaches_courant)
                        Actualise_etat_suivant(ETaches_courant,ct,dt,0);
                        if (visible_general) {
                                for (map<int, vector<int> >::iterator it=ETaches_courant.begin(); it!=ETaches_courant.end(); it++) {
                                        cout << "deadline   " << it->first << "  taille " << it->second;
                                }
                                cout << "etat reduit " << Etat_courant_reduit;
                                if (Etat_courant_reduit[1]==2 && Etat_courant_reduit[3]==2 && Etat_courant_reduit[4]!=0) {
                                        cout << "stop" << endl;
                                }
                        }
                        Etat_courant_reduit.clear(); tache_simuler.clear();
                }
                if (visible_general) {
                        cout << " sortie apres T iterations " << endl;
                }
        }
        if (visible_general) {
                for (map<int,vector<double> >::iterator it= nb_tache_rajouter.begin();it!=nb_tache_rajouter.end();it++) {
                        cout << "nombre de passage etat: " << it->first <<  "  " << it->second;
                }
        }
        
        // Transforme le nombre de passage par etat en proba
        int compt=0;
        for (int kl=0; kl<nb_tache_rajouter.size();kl++) {
                for (int il=0; il<nb_tache_rajouter[kl].size();il++) {
                        compt+=nb_tache_rajouter[kl][il];
                }
                for (int il=0; il<nb_tache_rajouter[kl].size();il++) {
                        nb_tache_rajouter[kl][il]= nb_tache_rajouter[kl][il]/compt;
                }
                compt=0;
        }
        if (visible_general) {
                // CAS K PREMIERS PAS: affichage de la forme reduite du type detat de la tache qui arrive avce sa probabilité associée
                cout << "AFFICHAGE =====" << endl;
                for (int il=0; il < Etat_visiter.size();il++) {
                        cout << "etat : "  << Etat_visiter[il];
                        cout << "--------" << endl;
                        for (int kl=0; kl < tache_rajouter[il].size();kl++) {
                                cout << "tache: " << tache_rajouter[il][kl];
                                cout << "proba: " << nb_tache_rajouter[il][kl] << endl;
                                cout << "--------" << endl;
                        }
                        cout << "============" << endl;
                }
                cout << "SORTIE " << endl;
        }
        int init = testpassage;
        for (int i= init; i<60; i++) {
                testpassage++;
        }
        cout << "test passage : " << testpassage << endl;
        return 1;
}


vector<double> calcul_meilleur_vitesse_restreint_ktaches(int position_etat_courant,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter, int reduction_deadline, int max_deadline ) {
        // ATTENTION etat_courant = (c1,d1,...,ck,dk,wdelta)
        vector<int> etat_courant = Espace_detat_reduit_kpremieresTaches[position_etat_courant];
        vector<double> valeur_nrj_suiv_res,borne_inf_vitesse,vitesse_possible, indice_etat_suivant_tache, indice_etat_suivant_tache2,proba_k1;
        
        /************ Borne des vitesses possibles ************/
        //@DONE changer la borne de vitesse possible => c_i/d_i => Addition(etat_courant[i])/etat_courant[i+1]
        int addition=0;
        for (int i = 0;i < etat_courant.size()-1; i++) {
                if (etat_courant[i]==0 || etat_courant[i+1]==0) {
                        //                        cout << "c vaut 0" << endl;
                        borne_inf_vitesse.push_back(0);
                } else {
                        addition+=etat_courant[i];
                        borne_inf_vitesse.push_back(ceil((double)addition/(double)etat_courant[i+1])); // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
                }
                i++;
        }
        addition=0;
        borne_inf_vitesse.push_back(ceil((double)etat_courant[etat_courant.size()-1]/(double)max_deadline)); // VITESSE MINIMALE : première vitesse supérieure à w(i)/i
        
        int borneInfVitesse = *max_element(borne_inf_vitesse.begin(), borne_inf_vitesse.end());  // Vitesse maximale que l'on doit réaliser pour que ce soit réalisable
        int arret = min(etat_courant[etat_courant.size()-1],max_vitesse);  // vitesse < w(Delta) et < Vmax
        //arret = min(arret,etat_courant[etat_courant.size()-1]);  // vitesse < w(k) dans le cas reduit
        borneInfVitesse = min(borneInfVitesse,arret); // borneInfVitesse < arret, par def
        
        //        cout << "vitesse : " << borneInfVitesse << "  et arret  " << arret << endl;
        
        /************* PARTIE RECUPERATION DE LA TACHE OBTENU PAR SIMULATION *************/
        //        proba_k1 = matrix_proba[]
        /*
         proba_k1 = proba_k_restreint[etat_courant[etat_courant.size()-1]-etat_courant[etat_courant.size()-2]];  // probabilité de jump entre k et k+1, vecteur dont la position correspond à la taille du saut et la valeur la probabilité d'avoir cette valeur
         */
        
        // etat_ simplifier correspond à (d1,c2,d2,wDelta-c1-c2)
        vector<int> etat_simplifier;int somme=0;
        for (int i=0; i<etat_courant.size()-1;i++) {
                if (i%2==1) {
                        etat_simplifier.push_back(etat_courant[i]);
                } else {
                        somme+=etat_courant[i];
                }
        }
        etat_simplifier.push_back(etat_courant[etat_courant.size()-1]-somme);
        
        //Recherche de letat courant simplifié dans l'espace detat simplifié
        vector<vector<int> > NouvellesTaches; vector<double> proba;int taille_proba; vector<int> TacheNulle;
        etat_simplifier = etat_courant;
        etat_simplifier.erase(etat_simplifier.begin());
        etat_simplifier[etat_simplifier.size()-1] = etat_courant[etat_courant.size()-1]-somme;
        vector<vector<int> >::iterator it = find(Etat_visiter.begin(),Etat_visiter.end(),etat_simplifier);
        
        if (it != Etat_visiter.end()) {
                int position_etat_simplifier = int(it-Etat_visiter.begin());
                //                cout << "it " << it-Etat_visiter.begin() << endl;
                //                cout << tache_rajouter;
                NouvellesTaches = tache_rajouter[position_etat_simplifier];
                //                cout << "proba ";
                
                proba = nb_tache_rajouter[position_etat_simplifier]; // proba est equivalent à proba_k1
                
                if (it==Etat_visiter.end()) {
                        //                        cout << "ETAT NON  VISITE !!!!!!! " << endl;
                } else {
                        //                        cout << "OK c'est rare " << endl;
                }
                //                cout << "ATTENTION ======= etat " << etat_courant << "et donc : " << etat_simplifier << endl;
                taille_proba = (int)proba.size();
        } else {
                taille_proba=0; TacheNulle.push_back(0); TacheNulle.push_back(0);
                NouvellesTaches.push_back(TacheNulle);
                proba.push_back(1);
        }
        if (NouvellesTaches.empty()) {
                TacheNulle.push_back(0); TacheNulle.push_back(0);
                NouvellesTaches.push_back(TacheNulle);
                proba.push_back(1);
        }
        /******************************************************************************/
        
        //////////////////// A MODIFIER
        /************ Calcul de la vitesse à utiliser ************/
        if (visible_general==true) {
                cout << "Nouvelles taches " << endl;
                for (int i=0; i<JobsToExecute.size(); i++) {
                        cout << JobsToExecute[i].taille << " et  " << JobsToExecute[i].deadline << endl;
                }
                cout << "Tache à ajouté par simu " << NouvellesTaches;
        }
        vector<double> proba_res; int pos_etat;
        //        cout << "==== CALCUL DE LETAT SUIVANT ===== " << endl;
        for (int i = borneInfVitesse; i <= arret; i++) {
                //                cout << "vitesse " << i << endl;
                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin(),indice_etat_suivant_tache.end());
                proba_res.erase(proba_res.begin(),proba_res.end());
                if (position_etat_courant==3282) {
                        cout << "nnnn" << endl;
                }
                // calcul des etats suivants
                // Iteration sur les tahces qui arrivent
                for (int j=0; j<JobsToExecute.size(); j++) {
                        // Iteration sur les taches simulées
                        for (int k=0; k<NouvellesTaches.size(); k++) {
                                // @TODO CAS PREEMPTION pour le premier job
                                // Considerez les cas ou il ya preemption, si tache
                                // Comparer c1 et c2 et en fonction calculer les probas
                                //                                if (JobsToExecute[j].deadline < Espace_detat_reduit_kpremieresTaches[position_etat_courant][1] && JobsToExecute[j].deadline !=0) {
                                //                                        if (etat_simplifier[etat_simplifier.size()-1] >0) {
                                //                                                vector<int> AddJob;
                                //                                                int max_size = 6; // a généraliser
                                //                                                AddJob.push_back(min(max_size,etat_simplifier[etat_simplifier.size()-1]));
                                //                                                AddJob.push_back(etat_simplifier[etat_simplifier.size()-2]-1);
                                //                                                NouvellesTaches.push_back(AddJob);
                                //                                        } else if (etat_simplifier[etat_simplifier.size()-1]==0) {
                                //                                                cout << "diff de 0, deja pris en compte" << endl;
                                //                                        }
                                //                                }
                                /**********/
                                //                                if (k==1) {
                                //                                        cout << "stop" << endl;
                                //                                }
                                //                                cout << "indice_ etat _ suivant " << indice_etat_suivant_tache;
                                indice_etat_suivant_tache.push_back(recherche_etat_suiv_restreint_ktaches(position_etat_courant, JobsToExecute[j].deadline,JobsToExecute[j].taille, NouvellesTaches[k],i,Espace_detat_reduit_kpremieresTaches));
                                // @TODO CAS PREEMPTION pour le premier job
                                //                                if (JobsToExecute[j].deadline < Espace_detat_reduit_kpremieresTaches[position_etat_courant][1] && JobsToExecute[j].deadline !=0) {
                                //                                        if (etat_simplifier[etat_simplifier.size()-1] >0) {
                                //                                                proba_res.push_back(JobsToExecute[j].arrival_rate);
                                //                                                indice_etat_suivant_tache.erase(indice_etat_suivant_tache.begin());
                                //                                                proba_res.erase(proba_res.begin());
                                //                                                /**********/
                                //                                        } else {
                                //                                                proba_res.push_back(JobsToExecute[j].arrival_rate * proba[k]);
                                //                                        }
                                //                                } else {
                                proba_res.push_back(JobsToExecute[j].arrival_rate * proba[k]);
                                //                                }
                        }
                }
                //                cout << "=> indice_etat_suivant_tache: " << indice_etat_suivant_tache;
                double cout_valeur_nrj_suiv_res = cout_nrj(i,max_vitesse);
                // Iteration sur les tahces qui arrivent
                int somme_pos=0;
                
                
                for (int j=0; j<JobsToExecute.size(); j++) {
                        // Iteration sur les taches simulées
                        for (int k=0; k<NouvellesTaches.size(); k++) {
                                //                                cout << "indice:  " << indice_etat_suivant_tache[pos_etat];
                                pos_etat=somme_pos;
                                if (indice_etat_suivant_tache[pos_etat]!=RAND_MAX) {
                                        //                                        cout << " ->  " << Espace_detat_reduit_kpremieresTaches[indice_etat_suivant_tache[pos_etat]];
                                        //                                        cout << "indice: " << pos_etat << " etat suivant " <<  indice_etat_suivant_tache[pos_etat] << "  " << proba_res[pos_etat] << "  " << JobsToExecute[j].taille << " " << JobsToExecute[j].deadline << "  et  " << NouvellesTaches[k][0] << "  " << NouvellesTaches[k][1] << endl;
                                        cout_valeur_nrj_suiv_res += proba_res[pos_etat]*nrj_prec[indice_etat_suivant_tache[pos_etat]]; // ajout en fonction des probas considérer, on multiplie la proba d'arrivée d'une tache par la proba de passage de k->k+1
                                        if (position_etat_courant==3282) {
                                                cout << " valeue: " << proba_res[pos_etat] << "   et    " << nrj_prec[indice_etat_suivant_tache[pos_etat]] << endl;
                                        }
                                } else {
                                        cout_valeur_nrj_suiv_res = RAND_MAX;
                                }
                                somme_pos++;
                        }
                }
                valeur_nrj_suiv_res.push_back(cout_valeur_nrj_suiv_res); // ensemble des couts possibles en fonction de la vitesse
                vitesse_possible.push_back(i);
        }
        if (position_etat_courant==3282) {
                cout << "valeur nrj suiv res " << endl;
                cout << valeur_nrj_suiv_res;
                //                cout << "vitesse possible " << endl;
                //                cout << vitesse_possible;
        }
        //        cout << "valeur de l'nrj :  " <<  valeur_nrj_suiv_res;
        std::vector<double>::iterator it2 =  min_element(valeur_nrj_suiv_res.begin(), valeur_nrj_suiv_res.end()); // recherche du cout minimal
        double nrj; int vitesse;
        if (it2!= valeur_nrj_suiv_res.end()) {
                nrj = *it2; vitesse = vitesse_possible[(int)distance(valeur_nrj_suiv_res.begin(), it2)];   // cas une vitesse correspond
        } else {
                nrj = RAND_MAX; vitesse = RAND_MAX;   // cas aucune vitesse ne correspond
        }
        vector<double> vitesse_et_energie;
        if (visible_general==true) {
                cout << "RESULTATS: lenergie: " << nrj << "  et la vitesse " << vitesse << endl;
        }
        vitesse_et_energie.push_back(nrj); vitesse_et_energie.push_back((double)vitesse); // vecteur de sortie (ernergie, vitesse)
        return(vitesse_et_energie);
}

vector<vector<double> > simulation_general_restreint_ktaches(vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,vector<Job> & JobsToExecute,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter,int reduction_deadline,int max_deadline) {
        for (int s  = 0; s < Espace_detat_reduit_kpremieresTaches.size(); s++) {
                //                if (Espace_detat_reduit_kpremieresTaches[s][4]==15 &&  Espace_detat_reduit_kpremieresTaches[s][3]==2 && Espace_detat_reduit_kpremieresTaches[s][2]==6 && Espace_detat_reduit_kpremieresTaches[s][1]==1 && Espace_detat_reduit_kpremieresTaches[s][0]==3) {//s==1973
                //                        
                //                        cout << "eryetu" << endl;
                //                }
                // 149
                if (s== 20) { //24
                        cout << "eryetu" << endl;
                }
                if (visible_general==true) {
                        cout << "=======================================>>>>>> valeur de l'etat : " << s <<  " qui vaut " << Espace_detat_reduit_kpremieresTaches[s];
                }
                vector<double> vitesse_et_energie = calcul_meilleur_vitesse_restreint_ktaches(s, Espace_detat_reduit_kpremieresTaches, nrj_prec,JobsToExecute, max_vitesse, Etat_visiter,tache_rajouter,nb_tache_rajouter,reduction_deadline,max_deadline);  // ATTENTION CHANGER calcul_meilleur_vitesse_restreint en calcul_meilleur_vitesse
                //                if (vitesse_et_energie[0]==RAND_MAX) {
                //                        nrj.erase(nrj.begin()+s);
                //                        vitesse.erase(vitesse.begin()+s);
                //                } else {
                nrj[s] = vitesse_et_energie[0]; vitesse[s] = vitesse_et_energie[1];
                //                }
                //                if (s==3443) {
                //                        cout << "eryetu" << endl;
                //                }
        }
        vector<vector<double> > sortie_simul; vector<double> vitesseCast(vitesse.begin(), vitesse.end()); sortie_simul.push_back(vitesseCast); sortie_simul.push_back(nrj);
        return(sortie_simul);
}


vector<vector<vector<double> > > backward_resolution_reduit_ktaches(int T, map<int,vector<Job> > & matrix_job, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter, int reduction_deadline, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches) {
        int longueur = (int)Espace_detat_reduit_kpremieresTaches.size();
        vector<double> nrj(longueur,0);
        vector<double> nrj_prec(nrj);
        vector<vector<double> > nrj_int, choix_action, sortie_simul;
        double arrival_rate_zero = 0;
        vector<double> vitesse(longueur,0);
        int ind_bcl = T-1;
        vector<Job> JobsToExecute;
        int tt, limite;
        
        for (int t=ind_bcl; t>=0; t--) {
                nrj_prec = nrj; // nrj étape précédente.
                cout << "==================================== T ====================================" << t << endl;
                limite = T-max_deadline;// Distinguer les max_deadline derniers cas, car dans ce cas p=0
                if (t>limite) {
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                } else { // JobsToExecute : ensemble des jobs a executer sur 1 pas de temps 
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                        arrival_rate_zero=0;
                        for (int i=0; i< matrix_job.size(); i++) { // Calcul de la probabilité d'avoir une tache nulle
                                for (int j=0; j<matrix_job[i].size(); j++) {
                                        map<int,vector<Job> >::iterator it;
                                        it = matrix_job.find(i);
                                        if (it!=matrix_job.end()) {
                                                int period = (matrix_job[i][j]).period;
                                                if ((t-i)%period==0) { // Sélection des jobs Jobs avec leur arrival time a executer au temps t
                                                        JobsToExecute.push_back(matrix_job[i][j]);
                                                        arrival_rate_zero+=matrix_job[i][j].arrival_rate;
                                                        cout << arrival_rate_zero << endl;
                                                }
                                        }
                                }
                        }
                }
                JobsToExecute.at(0).arrival_rate= 1-arrival_rate_zero; // Donne la probabilité de non arrivée de tache (calculé par comparaison avec les taches existantes
                if (t==0) {tt=0;} else {tt = t-1;}
                cout << "nbre de jobs " << JobsToExecute.size() << endl;
                for (int j=0; j<JobsToExecute.size(); j++) {
                        cout << "probabilité d'arrivée des taches : " << JobsToExecute[j].taille << "  " << JobsToExecute[j].deadline << "   " << JobsToExecute[j].arrival_rate  << endl;
                }
                sortie_simul = simulation_general_restreint_ktaches(Espace_detat_reduit_kpremieresTaches,JobsToExecute, max_vitesse, vitesse, nrj_prec, nrj, Etat_visiter,tache_rajouter,nb_tache_rajouter,reduction_deadline,max_deadline);
                cout << "=====" << endl;
                cout << sortie_simul[0];
                cout << sortie_simul[1];
                cout << "=====" << endl;
                for (int i=0; i< sortie_simul[1].size();i++) {
                        if (sortie_simul[1][i]>1000000) {
                                cout << i << " : " << Espace_detat_reduit_kpremieresTaches[i];
                        }
                }
                vitesse = sortie_simul[0]; nrj = sortie_simul[1];
                // AFFICHAGE DES ETATS CONSIDERES
                int compteur = 0;
                if (visible_general==true) {
                        for (int ti=0; ti<vitesse.size();ti++) {
                                if (nrj[ti]<2147483646) {
                                        cout << "VALEUR: "  << ti << " etat: " << Espace_detat_reduit_kpremieresTaches[ti] << " nrj " << nrj[ti] << " vitesse  " << vitesse[ti] << endl;
                                        compteur++;
                                }
                        }
                        cout << "nbre detat visité: " << compteur << "  sur   " << Espace_detat_reduit_kpremieresTaches.size() << endl;
                }
                choix_action.push_back(vitesse);
                nrj_int.push_back(nrj);
                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                arrival_rate_zero=0;
        }
        vector<vector<vector<double> > > sortie_backward_resolution; sortie_backward_resolution.push_back(nrj_int); sortie_backward_resolution.push_back(choix_action);
        return(sortie_backward_resolution);
}

//
//
//vector<vector<double> > test(vector<vector<int> > & nombre,int k,int size,int max_deadline,int borne) {
//        vector<vector<double> > new_nombre;
//        if (k>0) {
//                for (int i=0; i < nombre.size(); i++) {
//                        int fin = (max_deadline-k+1)*size - nombre.at(i).at(nombre.at(i).size()-1);
//                        for (int s=0; s < fin+1; s++) {
//                                vector<double> nombre_inter;
//                                nombre_inter.push_back(s);
//                                for (int k =0; k < nombre.at(i).size(); k++) {
//                                        nombre_inter.push_back(nombre.at(i).at(k)+s);
//                                }
//                                new_nombre.push_back(nombre_inter);
//                        }
//                }
//                return(calculEspaceDetat(new_nombre,k-1,size,max_deadline,borne));
//        } else {
//                return(nombre);
//        }
//}
//
//int Creation_espace_detat_restreint_ktaches(int max_deadline,int max_size,int nbre_taches,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches) {
//        vector<int> ajout;
//        int compteur=0;
//        
//        int c1=0;int d1=0; int c2=0;int d2=0;
//        for (int wdelta=c1+c2; wdelta <= max_deadline*max_size;wdelta++) {
//                ajout.push_back(c1);ajout.push_back(d1);ajout.push_back(c2);ajout.push_back(d2);ajout.push_back(wdelta);
//                Espace_detat_reduit_kpremieresTaches.push_back(ajout);
//                ajout.clear();
//        }
//        
//        //@TODO TO CHANGE, ici on a le cas (0,0,c2,d2)
//        for (int c2=1;c2<=max_size; c2++) {
//                for (int d2=1; d2<=max_deadline; d2++) {
//                        for (int wdelta=c1+c2; wdelta <= max_deadline*max_size;wdelta++) {
//                                ajout.push_back(c1);ajout.push_back(d1);ajout.push_back(c2);ajout.push_back(d2);ajout.push_back(wdelta);
//                                //                                                        compteur++;
//                                //                                                        Espace_detat_reduit_kpremieresTaches[compteur]=ajout;
//                                Espace_detat_reduit_kpremieresTaches.push_back(ajout);
//                                ajout.clear();
//                        }
//                }
//        }
//        
//        //@TODO TO CHANGE, ici on a le cas c1,c2>0
//        // Pour le cas m=2 uniquement ! boucle imbriqués, récursivité pour le cas m taches et non 2 taches comme avant
//        // On veut (c1,d1,...,ck,dk,wD)
//        for (int c1=1;c1<=max_size; c1++) {
//                for (int d1=1; d1<=max_deadline; d1++) {
//                        for (int c2=1;c2<=max_size; c2++) {
//                                for (int d2=d1; d2<=max_deadline; d2++) {
//                                        for (int wdelta=c1+c2; wdelta <= max_deadline*max_size;wdelta++) {
//                                                ajout.push_back(c1);ajout.push_back(d1);ajout.push_back(c2);ajout.push_back(d2);ajout.push_back(wdelta);
//                                                //                                                        compteur++;
//                                                //                                                        Espace_detat_reduit_kpremieresTaches[compteur]=ajout;
//                                                Espace_detat_reduit_kpremieresTaches.push_back(ajout);
//                                                ajout.clear();
//                                        }
//                                }
//                        }
//                }
//        }
//        
//        
//        int borne = max_deadline * max_size;
//        vector<vector<double> > nombre;
//        for (int i=0; i<max_size+1; i++) {
//                vector<double> val;
//                val.push_back(i);
//                nombre.push_back(val);
//                val.pop_back();
//        }
//        
//        
//        
//        
//        
//        return 1;
//}
//
//
//
//
//
//
//
////vector<vector<double> > calculEspaceDetat_restreint_ktaches(vector<vector<double> > & nombre,int k,int size,int max_deadline,int borne) {
////
////}
//
//vector<vector<double> > calculEspaceDetat(vector<vector<double> > & nombre,int k,int size,int max_deadline,int borne) {
//        vector<vector<double> > new_nombre;
//        if (k>0) {
//                forEspa (int i=0; i < nombre.size(); i++) {
//                        int fin = (max_deadline-k+1)*size - nombre.at(i).at(nombre.at(i).size()-1);
//                        for (int s=0; s < fin+1; s++) {
//                                vector<double> nombre_inter;
//                                nombre_inter.push_back(s);
//                                for (int k =0; k < nombre.at(i).size(); k++) {
//                                        nombre_inter.push_back(nombre.at(i).at(k)+s);
//                                }
//                                new_nombre.push_back(nombre_inter);
//                        }
//                }
//                return(calculEspaceDetat(new_nombre,k-1,size,max_deadline,borne));
//        } else {
//                return(nombre);
//        }
//}
//
///***********************************************************************************************/
//// creationEspaceDetat: crée l'espace detat adequat
////
//// sortie:
////          vector<vector<double> > espace_detat, espace d'état créé
////
//// entrée :
////          max_size            : int, taille maximale d'une tache
////          max_deadline        : int, deadline maximale d'une tache
////          espace_detat        : vector<vector<double> >, espace d'état à remplir
///***********************************************************************************************/
//vector<vector<double> > creation_espace_detat(int max_size, int max_deadline, vector<vector<double> > & espace_detat) {
//        int borne = max_deadline * max_size;
//        vector<vector<double> > nombre;
//        for (int i=0; i<max_size+1; i++) {
//                vector<double> val;
//                val.push_back(i);
//                nombre.push_back(val);
//                val.pop_back();
//        }
//        espace_detat=nombre;
//        espace_detat = calculEspaceDetat(espace_detat,max_deadline-1,max_size,max_deadline,borne);
//        return(espace_detat);
//}


vector<vector<int> > test(vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,int deadline_prec,int size_prec,int nbre_taches,int max_size,int max_deadline,int borne) {
        if (nbre_taches>0) {
                vector<int> value_int;
                vector<vector<int> >::iterator it;
                int compteur=0;
                // on prend les deux derniers pas et on les répète
                // taille -1
                for (int j=0; j<Espace_detat_reduit_kpremieresTaches.size();j++) {
                        if (compteur>=Espace_detat_reduit_kpremieresTaches.size()) {
                                break;
                        }
                        //                        cout << "dec:  " << compteur << endl;
                        value_int=Espace_detat_reduit_kpremieresTaches[compteur];
                        Espace_detat_reduit_kpremieresTaches[compteur].push_back(1);
                        for (int i=2;i<=max_size;i++) {
                                //                                cout << "dec2:  " << compteur <<  "  " << i  <<endl;
                                value_int.push_back(i);
                                //                                cout << "value int " << value_int;
                                it = Espace_detat_reduit_kpremieresTaches.begin()+compteur+1;
                                //                                cout << "taille :  " << Espace_detat_reduit_kpremieresTaches.size() << endl;
                                Espace_detat_reduit_kpremieresTaches.emplace(it, value_int);
                                value_int.pop_back();
                                compteur++;
                        }
                        compteur++;
                }
                //                cout << Espace_detat_reduit_kpremieresTaches ;
                
                compteur=0;
                for (int j=0; j<Espace_detat_reduit_kpremieresTaches.size();j++) {
                        if (compteur>=Espace_detat_reduit_kpremieresTaches.size()) {
                                break;
                        }
                        
                        //                        cout << "dec:  " << compteur << endl;
                        value_int=Espace_detat_reduit_kpremieresTaches[compteur];
                        Espace_detat_reduit_kpremieresTaches[compteur].push_back(min(max_deadline,(int)value_int[value_int.size()-2]));
                        //                        cout << "value " << (int)value_int[value_int.size()-2]+1 << endl;
                        int deb = min((int)value_int[value_int.size()-2]+1,max_deadline);
                        // cas max_deadline à la tache d'avant
                        if ((int)value_int[value_int.size()-2]==max_deadline) {
                                deb= max_deadline+1;
                        }
                        for (int i=deb;i<=max_deadline;i++) {
                                //                                cout << "dec2:  " << compteur <<  "  " << i  <<endl;
                                value_int.push_back(i);
                                //                                cout << "value int " << value_int;
                                it = Espace_detat_reduit_kpremieresTaches.begin()+compteur+1;
                                //                                cout << "taille :  " << Espace_detat_reduit_kpremieresTaches.size() << endl;
                                Espace_detat_reduit_kpremieresTaches.emplace(it, value_int);
                                value_int.pop_back();
                                compteur++;
                        }
                        
                        compteur++;
                        //                        cout << "taille " << Espace_detat_reduit_kpremieresTaches.size() << " compteur :   " << compteur << endl;
                        //                        cout << "  " << endl;
                }
                
                //                cout << Espace_detat_reduit_kpremieresTaches << endl;
                
                
                return test(Espace_detat_reduit_kpremieresTaches,deadline_prec,size_prec,nbre_taches-1,max_size, max_deadline,borne);
        } else {
                return Espace_detat_reduit_kpremieresTaches;
        }
}





int creation_etat_restreint_ktaches(int max_deadline, int max_size, int nbre_taches, vector<vector<int > > & Espace_detat_reduit_kpremieresTaches, int nbre_tache_enlever) {
        
        int borne = max_deadline * max_size;
        vector<vector<int> > nombre;
        vector<int> val;
        // Cas première taches
        for (int i=1; i<max_size+1; i++) {
                //@DONE Remettre la boucle
                for (int j=0;j<nbre_tache_enlever*2;j++) {
                        val.push_back(0);
                }
                //                        val.push_back(0);
                val.push_back(i);
                nombre.push_back(val);
                val.pop_back();
                //@DONE Remettre la boucle
                for (int j=0;j<nbre_tache_enlever*2;j++) {
                        val.pop_back();
                }
                //                        val.pop_back();
                //                        val.pop_back();
        }
        //        nbre_taches--;
        nbre_taches = nbre_taches-nbre_tache_enlever;
        Espace_detat_reduit_kpremieresTaches = nombre;
        vector<int> value_int; int compteur;
        vector<vector<int> >::iterator it_int,it;// = Espace_detat_reduit_kpremieresTaches.begin();
        //it_int++;
        //cout << &Espace_detat_reduit_kpremieresTaches[0][0] << endl;
        //                Espace_detat_reduit_kpremieresTaches.emplace(it_int, value_int);
        //
        //
        //cout << &Espace_detat_reduit_kpremieresTaches[0][0] << endl;
        //        cout << Espace_detat_reduit_kpremieresTaches;
        //
        
        compteur=0;
        // Creation de l'ensemble des couples (taille,deadline) créé
        for (int j=0; j<Espace_detat_reduit_kpremieresTaches.size();j++) {
                if (compteur>=Espace_detat_reduit_kpremieresTaches.size()) {
                        break;
                }
                //                cout << "dec:  " << compteur << endl;
                value_int=Espace_detat_reduit_kpremieresTaches[compteur];
                Espace_detat_reduit_kpremieresTaches[compteur].push_back(1);
                for (int i=2;i<=max_deadline;i++) {
                        //                        cout << "dec2:  " << compteur <<  "  " << i  <<endl;
                        value_int.push_back(i);
                        //                        cout << "value int " << value_int;
                        it = Espace_detat_reduit_kpremieresTaches.begin()+compteur+1;
                        //                        cout << "taille :  " << Espace_detat_reduit_kpremieresTaches.size() << endl;
                        Espace_detat_reduit_kpremieresTaches.emplace(it, value_int);
                        value_int.pop_back();
                        compteur++;
                }
                compteur++;
                //                cout << "taille " << Espace_detat_reduit_kpremieresTaches.size() << " compteur :   " << compteur << endl;
                //                cout << "  " << endl;
        }
        int deadline_prec=max_deadline;int size_prec=max_size;
        Espace_detat_reduit_kpremieresTaches = test(Espace_detat_reduit_kpremieresTaches,deadline_prec,size_prec,nbre_taches-1,max_size, max_deadline,borne);
        
        compteur=0;
        //        cout << Espace_detat_reduit_kpremieresTaches <<endl;
        
        
        
        int nbre_0_add = nbre_taches*2 - (int)(Espace_detat_reduit_kpremieresTaches.front().size());
        if (nbre_0_add!=0) {
                for (vector<vector<int > >::iterator it = Espace_detat_reduit_kpremieresTaches.begin(); it!=Espace_detat_reduit_kpremieresTaches.end(); it++) {
                        for (int i=0; i<nbre_0_add; i++) {
                                (*it).push_back(0);
                        }
                }
        }
        //        cout << Espace_detat_reduit_kpremieresTaches <<endl;
        
        //        cout << "=====" << endl;
        
        
        // Rajoute le dernier pas ie la somme totale possible
        for (int j=0; j<Espace_detat_reduit_kpremieresTaches.size();j++) {
                if (compteur>=Espace_detat_reduit_kpremieresTaches.size()) {
                        break;
                }
                //                cout << "dec:  " << compteur << endl;
                value_int=Espace_detat_reduit_kpremieresTaches[compteur];
                int somme=0;
                for (int k=0; k<Espace_detat_reduit_kpremieresTaches[compteur].size(); k++) {
                        if (k%2==0)
                                somme+=Espace_detat_reduit_kpremieresTaches[compteur][k];
                }
                Espace_detat_reduit_kpremieresTaches[compteur].push_back(somme);
                int deb=somme+1;
                //int maxDelta = min(max_deadline*max_size, somme+(max_deadline-value_int[value_int.size()-1])*max_size);
                //                || (value_int[value_int.size()-1]!= max_deadline-1 || value_int[value_int.size()-3]!= max_deadline-1)
                int maxDelta;
                if (nbre_tache_enlever==0) {
                        if (value_int[value_int.size()-1]!= max_deadline && !(value_int[value_int.size()-1]== max_deadline-1 && value_int[value_int.size()-3]== max_deadline-1)) {
                                maxDelta = max_size*max_deadline;
                        } else {
                                maxDelta = deb-1;     
                        }
                } else {
                        maxDelta = deb-1;
                        //                        cout << "test" << endl;
                }
                //                cout << value_int;
                //                cout << deb << "  " << maxDelta << endl;
                for (int i=deb;i<=maxDelta;i++) {
                        //                        cout << "dec2:  " << compteur <<  "  " << i  <<endl;
                        value_int.push_back(i);
                        //                        cout << "value int " << value_int;
                        it = Espace_detat_reduit_kpremieresTaches.begin()+compteur+1;
                        //                        cout << "taille :  " << Espace_detat_reduit_kpremieresTaches.size() << endl;
                        Espace_detat_reduit_kpremieresTaches.emplace(it, value_int);
                        value_int.pop_back();
                        compteur++;
                }
                compteur++;
                //                cout << "taille " << Espace_detat_reduit_kpremieresTaches.size() << " compteur :   " << compteur << endl;
                //                        cout << Espace_detat_reduit_kpremieresTaches << endl;
                //                cout << "  " << endl;
        }
        
        return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////            CAS      LORCH               /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/** VALEUR ENTREE **/
// J_i(c_i,d_i) given, avec 2 lois de probabilité
// Loi de probabilité 1: f() sur e_i = f(c_i)
// Loi de probabilité 2: g() sur r_i = g(r_i)

vector<vector<vector<double> > > LORSCH_backward_resolution(double gamma, int T, map<int,vector<Job> > & matrix_job, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init) {
        int longueur = (int)espace_detat_init.size();
        vector<double> nrj(longueur,0);
        vector<double> nrj_prec(nrj);
        vector<vector<double> > nrj_int;
        
        vector<vector<double> > choix_action;
        double arrival_rate_zero = 0;
        
        vector<double> vitesse(longueur,0);
        int ind_bcl = T-1;
        
        vector<Job> JobsToExecute;
        
        vector<vector<double> > sortie_simul;
        int tt;
        for (int t=ind_bcl; t>=0; t--) {
                // nrj étape précédente.
                nrj_prec = nrj;
                cout << "==================================== T ====================================" << t << endl;
                // Distinguer les max_deadline derniers cas, car dans ce cas p=0
                int limite = T-max_deadline;
                if (t>limite) {
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                } else {
                        // JobsToExecute : ensemble des jobs a executer sur 1 pas de temps 
                        Job jobsnul = Job();
                        JobsToExecute.push_back(jobsnul);
                        for (int i=0; i< matrix_job.size(); i++) {
                                for (int j=0; j<matrix_job[i].size(); j++) {
                                        map<int,vector<Job> >::iterator it;
                                        it = matrix_job.find(i);
                                        if (it!=matrix_job.end()) {
                                                int period = (matrix_job[i][j]).period;
                                                if ((t-i)%period==0) {
                                                        // Sélection des jobs Jobs avec leur arrival time a executer au temps t
                                                        JobsToExecute.push_back(matrix_job[i][j]);
                                                        arrival_rate_zero+=matrix_job[i][j].arrival_rate;
                                                }
                                        }
                                }
                        }
                }
                JobsToExecute.at(0).arrival_rate= 1-arrival_rate_zero;
                //                JobsToExecute.at(0).execution_time_rate = 
                if (t==0) {tt=0;} else {tt = t-1;}
                //                cout << "value: " << JobsToExecute[0].taille << "  " << JobsToExecute[0].deadline << endl;
                /**/ //@TODO Rajout de la prise en compte de taux du temps d'execution
                
                
                
                sortie_simul = simulation_general(gamma, espace_detat_init,JobsToExecute, max_vitesse, vitesse, nrj_prec, nrj);
                vitesse = sortie_simul[0];
                nrj = sortie_simul[1];
                choix_action.push_back(vitesse);
                nrj_int.push_back(nrj);
                JobsToExecute.erase(JobsToExecute.begin(), JobsToExecute.end());
                arrival_rate_zero=0;
        }
        vector<vector<vector<double> > > sortie_backward_resolution;
        sortie_backward_resolution.push_back(nrj_int);
        sortie_backward_resolution.push_back(choix_action);
        return(sortie_backward_resolution);
}

