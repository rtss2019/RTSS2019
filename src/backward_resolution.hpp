
//
//  backward_resolution.hpp
//  finite_case
//
//  Created by Stéphan on 02/05/2017.
//  Copyright © 2017 Stéphan. All rights reserved.
//

#ifndef backward_resolution_hpp
#define backward_resolution_hpp

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <numeric>
#include <map>


using namespace std;

struct Job {
        int period;
        int taille;
        int deadline;
        double arrival_rate;
        double execution_time_rate;
        Job (){
                period=0;
                taille=0;
                deadline=0;
                arrival_rate=0;
        };
        ~Job () {};
};

/*********** SURCHARGE d'opérateur pour la classe vector ***********/
template <class Ty, class Alloc, typename T>
std::vector<Ty,Alloc>& operator*=(std::vector<Ty,Alloc> & this_, T v_)
{
        for( std::size_t i = 0; i < this_.size(); ++i) this_[i]*= (Ty)v_; // C++11
        return this_;
}

template <class Ty, class Alloc, class T>
std::vector<Ty,Alloc> operator*(std::vector<Ty,Alloc> const & A, T const & a)
{
        std::vector<Ty,Alloc> tmp(A);
        tmp *= a;
        return tmp;
}

// soustrait une constante a tous les elements de mon vecteur
template <class Ty, class Alloc, typename T>
std::vector<Ty,Alloc>& operator-=(std::vector<Ty,Alloc> & this_, T v_)
{
        for( std::size_t i = 0; i < this_.size(); ++i) this_[i]-= (Ty)v_; // C++11
        return this_;
}

template <class Ty, class Alloc, class T>
std::vector<Ty,Alloc> operator-(std::vector<Ty,Alloc> const & A, T const & a)
{
        std::vector<Ty,Alloc> tmp(A);
        tmp -= a;
        return tmp;
}
// additionne une constante a tous les elements de mon vecteur
template <class Ty, class Alloc, typename T>
std::vector<Ty,Alloc>& operator+=(std::vector<Ty,Alloc> & this_, T v_)
{
        for( std::size_t i = 0; i < this_.size(); ++i) this_[i]-= (Ty)v_; // C++11
        return this_;
}

template <class Ty, class Alloc, class T>
std::vector<Ty,Alloc> operator+(std::vector<Ty,Alloc> const & A, T const & a)
{
        std::vector<Ty,Alloc> tmp(A);
        tmp += a;
        return tmp;
}
template <class Ty, class Alloc, class T>
std::vector<Ty,Alloc> operator*(T const & a, std::vector<Ty,Alloc> const & A)
{
        std::vector<Ty,Alloc> tmp(A);
        tmp *= a;
        return tmp;
}

template <class Ty, class Alloc, class T>
std::vector<Ty,Alloc> operator/(std::vector<Ty,Alloc> const & A, T const & a)
{
        std::vector<Ty,Alloc> tmp(A);
        tmp *= 1/a;
        return tmp;
}

template <class Ty, class Alloc>
std::vector<Ty,Alloc> operator+(std::vector<Ty,Alloc> const & A, std::vector<Ty,Alloc> const & B)
{
        std::vector<Ty,Alloc> tmp(A);
        for( std::size_t i = 0; i < A.size(); ++i) tmp[i] = A[i] + B[i]; // C++11
        return tmp;
}

template <class Ty, class Alloc>
std::vector<Ty,Alloc> operator-(std::vector<Ty,Alloc> const & A, std::vector<Ty,Alloc> const & B)
{
        std::vector<Ty,Alloc> tmp(A);
        for( std::size_t i = 0; i < A.size(); ++i) tmp[i] = A[i] - B[i]; // C++11
        return tmp;
}

template <class Ty, class Alloc>
std::ostream &operator<<(std::ostream &out, std::vector<Ty,Alloc> &vecteur) {
        for( std::size_t i = 0; i < vecteur.size(); ++i) cout << std::setprecision(12) << vecteur[i] << ','; // C++11
        cout << endl;
        return out;
}

/*template <class Ty, class Alloc>
 std::ostream &operator<<(std::ostream &out, std::vector<Ty,Alloc> &vecteur) {
 for( std::size_t i = 0; i < vecteur.size(); ++i) cout << vecteur[i] << ' '; // C++11
 cout << endl;
 return out;
 }*/
/***********************************************************************/


/***** AFFICHE VECTEUR ********/
void Affiche(const vector<double> & vecteur);
void Affiche(const vector<int> & vecteur);
/***** AFFICHE ESPACE DETAT ********/
void Affiche(const vector<vector<vector<double> > > & vecteur);
void Affiche(const vector<vector<double> > & vecteur);
void Affiche(const vector<vector<int> > & vecteur);
void Affiche_int(const vector<vector<vector<int> > > & vecteur);
vector<int> Affiche(vector<int> & vecteur, int scalaire);
vector<int> add_vector(vector<int> & vecteur1, vector<int> & vecteur2);

/***** FUNCTION COUT *****/
double cout_nrj(const double & vitesse, double vitesseMax);
double cout_nrj_learning(const double & vitesse, double vitesseMax, int vitesseMin);

vector<vector<double> > calculEspaceDetat(vector<vector<double> > & nombre,int k,int size,int max_deadline,int borne);
vector<vector<double> > creation_espace_detat(int max_size, int max_deadline, vector<vector<double> > & espace_detat);
vector<vector<int> > calculEspaceDetat(vector<vector<int> > & nombre,int k,int size,int max_deadline,int borne);
vector<vector<int> > creation_espace_detat(int max_size, int max_deadline, vector<vector<int> > & espace_detat);
vector<vector<int> > suppression_etat_inutile(int max_size,int max_deadline,vector<vector<int> > & espace_detat_init);

int recherche_etat_suiv(int indice_etat_prec, int deadline_tache, int taille_tache, int vitesse_pas_prec, vector<vector<double> > & espace_detat_init);
vector<double> calcul_meilleur_vitesse(double gamma,int position_etat_courant,vector<vector<double> > & espace_detat_init, vector<double> nrj_prec,vector<double> p_size,vector<double> p_deadline, int max_vitesse,int deadline_tache,int taille_tache);

vector<vector<double> > simulation_general(vector<vector<double> > & espace_detat_init,vector<double> p_size, vector<double> p_deadline, int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj,int deadline_tache,int taille_tache);
//vector<vector<double> > testAlgo(vector<double> p,int T,int max_deadline);
//vector<double> comble(vector<double> nvel_etat);
double calcul_vitesse_OA(int etat,vector<vector<double> > espace_detat_init);
double calcul_vitesse_OA(vector<double> & nvel_etat_OA);

vector<double> nvel_etat_ac_nvel_tache(int i, vector<double> nvel_etat, int deadline_value, int taille, int deadline_max, int max_deadline, vector<double> d_value);
vector<vector<double> > testAlgo_simulation_et_calcul_vitesse(vector<vector<double> > & espace_detat_init, vector<double> p,int T,int max_deadline, vector<vector<double> > choix_action, int iteration, bool dp);
vector<vector<double> > testAlgo_simulation_et_calcul_vitesse_casréduit(vector<vector<double> > & espace_detat_init, vector<double> p,int T,int max_deadline, vector<vector<double> > choix_action, int iteration, bool dp, vector<vector<double>  > & jobs_T);


double choisir_proba(vector<double> & sum_pa1, double & nbre_alea);
vector<vector<double> > simulation_job(int max_size,int max_deadline, vector<double> p, int iteration, int T);

//vector<double> calcul_meilleur_vitesse2(int position_etat_courant,vector<vector<double> > & espace_detat_init, vector<double> nrj_prec,vector<double> p,vector<double> q,int max_vitesse,int deadline_tache,int taille_tache)
//vector<vector<double> > simulation_general2(vector<vector<double> > & espace_detat_init,vector<double> p,vector<double> q,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj,int deadline_tache,int taille_tache);


vector<vector<vector<double> > > backward_resolution(int T,vector<double> p_size,vector<double> p_deadline, int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init);
//vector<vector<vector<double> > > backward_resolution2(int T,vector<double> p,vector<double> q,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_offline);

double span(vector<double> a);
vector<vector<double> > value_iteration(vector<double> p,int max_deadline,int max_size,int max_vitesse,vector<vector<double> > & espace_detat_init,vector<double> p_offline);

//vector<double> pente_progdyn(int k, int x, int T, vector<vector<double> > & tab_pentes, int taille, vector<double> & pas_de_temps_initiaux);
//vector<double> reduction_espace_detat_point_discretisation(vector<vector<double> > & espace_detat_init, int taille, int deadline, int T);

int reduc_esp_etat(int taille, int Temps);
//int reduc_esp_etat_loi_uniforme(int taille, int Temps);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<double> calcul_meilleur_vitesse(int position_etat_courant,vector<vector<double> > & espace_detat_init,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse, int reduction_deadline);
vector<vector<double> > simulation_general(vector<vector<double> > & espace_detat_init,vector<Job> & JobsToExecute,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj,int reduction_deadline);
vector<vector<vector<double> > > backward_resolution(double gamma,int T, map<int,vector<Job> > & matrix_job, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init);
vector<vector<vector<double> > > value_iteration(double gamma,map<int, vector<Job> > & matrix_job,int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init);

vector<vector<int> > simulation_job_generale(map<int,vector<Job> > & matrix_job, int max_size,int max_deadline,int max_period, int T);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**                                             PARTIE ESPACE DETAT RESTREINT                                                                **/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
vector<vector<double> >  Ajout_dernier_pas_espace_restreint(vector<vector<double> > & ens_espace_restreint_simu, int max_deadline, int max_size, int reduction_deadline);
int Simulation_pas_suivant_iid(int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, map<int, vector<double> > & proba_k_restreint);
int Simulation_pas_suivant(vector<vector<double> > & ens_espace_restreint_simu, int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, map<int, vector<double> > & proba_k_restreint);
int New_job_probability_arrival(map<int, vector<Job> > & matrix_job,map<int, vector<Job> > & matrix_job_restreint,map<int, vector<double> > & proba_k_restreint, int reduction_deadline);
vector<vector<vector<double> > > backward_resolution_reduit(int T, map<int,vector<Job> > & matrix_job, map<int,vector<double> > & proba_k_restreint, int reduction_deadline, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<double> > & espace_detat_init);
int Etat_suppression(vector<vector<double> > & ens_espace_restreint_simu_final, map<int, vector<double> > & proba_k_restreint);

int recherche_etat_suiv_restreint(int indice_etat_prec, int deadline_tache, int taille_tache, int reduction_deadline, int reduction_taille, int vitesse_pas_prec, vector<vector<int> > & espace_detat_init);
vector<double> calcul_meilleur_vitesse_restreint(int position_etat_courant,vector<vector<double> > & espace_detat_init,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse,map<int,vector<double> > & proba_k_restreint, int reduction_deadline);

// Partie k prmeières taches espace restreint
int Simulation_pas_suivant_iid_premier_pas(int T, int max_deadline, int max_size, int max_period, int reduction_deadline, map<int,vector<Job> > & matrix_job, vector<vector<int> > & proba_k_restreint,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter);

vector<double> calcul_meilleur_vitesse_restreint_ktaches(int position_etat_courant,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,vector<double> & nrj_prec, vector<Job> & JobsToExecute, int max_vitesse, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter, int reduction_deadline,int max_deadline);
vector<vector<double> > simulation_general_restreint_ktaches(vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,vector<Job> & JobsToExecute,int max_vitesse,vector<double> vitesse,vector<double> nrj_prec,vector<double> nrj, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter,int reduction_deadline,int max_deadline);
vector<vector<vector<double> > > backward_resolution_reduit_ktaches(int T, map<int,vector<Job> > & matrix_job, vector<vector<int> > & Etat_visiter, vector<vector<vector<int> > > & tache_rajouter, map<int,vector<double> > & nb_tache_rajouter, int reduction_deadline, int max_deadline,int max_size,int max_vitesse,int max_period,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches);

int recherche_etat_suiv_restreint_ktaches(int position_etat_courant, int deadline_tache, int taille_tache, vector<int> & NouvellesTaches, int vitesse_pas_prec, vector<vector<int> > & espace_detat_init);

int Creation_espace_detat_restreint_ktaches(int max_deadline,int max_size,int nbre_taches,vector<vector<int> > & Espace_detat_reduit_kpremieresTaches);

int creation_etat_restreint_ktaches(int max_deadline, int max_size, int nbre_taches, vector<vector<int > > & Espace_detat_reduit_kpremieresTaches, int nbre_tache_enlever);
vector<vector<int> > test(vector<vector<int> > & Espace_detat_reduit_kpremieresTaches,int deadline_prec,int size_prec,int nbre_taches,int max_size,int max_deadline,int borne);

int Construction_Etat_restreint(map<int,vector<int> > & ETaches_courant,int reduction_deadline, vector<int> & Etat_courant_reduit, vector<int> & tache_simuler);
int Actualise_etat_suivant(map<int,vector<int> > & ETaches_courant, int ct, int dt, int vitesse);


#endif /* backward_resolution_hpp */
