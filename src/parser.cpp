//
//  parser.cpp
//  finite_case
//
//  Created by Stéphan on 12/01/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//
#include "backward_resolution.hpp" // Affichage du vector<int>
#include "parser.hpp"
#include <cstring>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>


//typedef std::map<const char *, int> HashParam;

// Initialisation de la map< ... >
/*!
 * \brief Création et initialisation de la table de hashage
 */
HashParam create_hash_map ()
{
        HashParam M;
        TypeVal T;
        
        T.type = T_INT;
        M["maturity"] = T;
        M["maximal speeds processor"] = T;
        M["sample"] = T;
        M["maximal deadline reduction"] = T;
        
        T.type = T_VECTOR;
        M["offset"] = T;
        M["period"] = T;
        M["size"] = T;
        M["deadline"] = T;
        
        T.type = T_DOUBLE;
        M["arrival rate"] = T;
        
        return M;
}


unsigned int next_space(unsigned int space, char* line){
        // avance jusqu'au prochain mot ou chiffre
        // Prerequis: etre sur un espace
        unsigned int i = space;
        while (strncmp (line + i, " ", 1) != 0 && strcmp (line + i, "") != 0)
                i++;
        i++;
        return i;
}

unsigned int skip_space(unsigned int space, char* line){
        unsigned int i = space;
        while (strncmp (line + i, " ", 1) == 0)
                i++;
        return i;
}


unsigned int skip_space_reverse(unsigned int space, char* line){
        unsigned int i = space;
        while (strncmp (line + i, " ", 1) == 0)
                i--;
        i++;
        return i;
}


bool is_number (char *c){
        if ((int(*c-'0') >= 0 && int(*c-'0') <= 9) || (int(*c-'-') == 0))
                return true;
        return false;
}

unsigned int next_fig(unsigned int first_indice, char *line){
        unsigned int i = first_indice;
        while (!is_number(line+i))
                i++;
        i++;
        return i;
}

/*!
 *  \brief tableau qui contient notre fichier
 */
static char** data;
/*!
 * \brief le nombre de ligne du fichier
 */
static unsigned int n;

HashParam init_hash_map (const char *s){
        // donné pour stocker les données au fur et à mesure de lecture
        int Prov_maturity = 0, Prov_maximal_speeds = 0, Prov_sample = 0, Prov_reduction = 0;
        vector<int> Prov_offset,Prov_period,Prov_size,Prov_deadline;
        vector<double> Prov_arrival_rate;
        vector<char*> type;
        unsigned int i;
        int j;
        
        if (s == NULL) {
                std::cout<<"Erreur : il n'y a pas de nom de fichier passé en paramètre"<<std::endl;
        } else {
                //                cout << "bien lu en paramètre" << endl;
                ifstream fichier(s);
                if (fichier){
                        string ligne;
                        i = 0;
                        // compte les lignes du fichier
                        while(getline(fichier,ligne))
                                i++;
                        n = i;
                        fichier.clear();
                        fichier.seekg(0, ios::beg);
                        
                        data = new char*[i];
                        size_t size;
                        j = 0;
                        while (getline(fichier,ligne)) {
                                size = ligne.size() + 2;
                                data[j] = new char[ size ];
                                strncpy( data[j], ligne.c_str(), size );
                                j++;
                        }
                } else {
                        std::cerr<<"Erreur : le fichier "<< s <<" n'existe pas."<<std::endl;
                }
                fichier.close();
                // Partie
                i = 0;
                while (i<n){
                        if (strncmp (data[i], "##", 1) != 0 && strncmp (data[i], "", 1) != 0) {
                                //                                cout << data[i] << endl;
                                int first_fig = next_fig(0,data[i]);
                                unsigned int skip = skip_space_reverse(first_fig-2, data[i]);
                                int test = atoi (data[i] + first_fig -1);
                                if (strncmp (data[i],"maturity", skip) == 0) {
                                        Prov_maturity = test;
                                } else if (strncmp (data[i],"maximal speeds processor", skip) == 0) {
                                        Prov_maximal_speeds = test;
                                } else if (strncmp (data[i],"sample", skip) == 0) {
                                        Prov_sample = test;
                                } else if (strncmp (data[i],"maximal deadline reduction", skip) == 0) {
                                        Prov_reduction = test;
                                } else if (strncmp (data[i],"offset", skip) == 0) {
                                        Prov_offset.push_back(test);
                                } else if (strncmp (data[i],"period", skip) == 0) {
                                        Prov_period.push_back(test);
                                } else if (strncmp (data[i],"size", skip) == 0) {
                                        Prov_size.push_back(test);
                                } else if (strncmp (data[i],"deadline", skip) == 0) {
                                        Prov_deadline.push_back(test);
                                } else if (strncmp (data[i],"arrival rate", skip) == 0) {
                                        double test1 = atof (data[i] + first_fig-1);
                                        Prov_arrival_rate.push_back(test1);
                                }
                                if (type.size()<9) {
                                        char* key2 = new char [skip+1];
                                        strncpy(key2, data[i], skip+1);
                                        key2[skip]='\0';
                                        type.push_back(key2);
                                }
                        }
                        i++;
                }
        }
        // création et insertion dans la map
        HashParam M = create_hash_map ();
        TypeVal Ta = TypeVal();
        Ta.is_set = true;
        for (int i=0; i<4; i++) {
                Ta.type = T_INT;
                if (i==0) {
                        Ta.V_int = Prov_maturity;
                } else if (i==1) {
                        Ta.V_int = Prov_maximal_speeds;
                } else if (i==2) {
                        Ta.V_int = Prov_sample;
                } else if (i==3){
                        Ta.V_int = Prov_reduction;
                }
                M[type[i]] = Ta;
        }
        for (int i=4; i<type.size(); i++) {
                Ta.type = T_VECTOR;
                if (i==4) {
                        Ta.V_vector = new vector<int> (Prov_offset);
                } else if (i==5) {
                        Ta.V_vector = new vector<int> (Prov_period);
                } else if (i==6) {
                        Ta.V_vector = new vector<int> (Prov_size);
                } else if (i==7){
                        Ta.V_vector = new vector<int> (Prov_deadline);
                } else if (i==8){
                        Ta.type = T_DOUBLE;
                        Ta.V_double = new vector<double> (Prov_arrival_rate);
                }
                M[type[i]] = Ta;
                // test *Ta.V_vector pour afficher le vecteur
                // *(M.at("size").V_vector) : Accès au vecteur pour l'affichage
        }
        return M;
}

void set_from_map (const HashParam &M, const char *name , int &retour){
        if (M.at(name).is_set)
                retour =  M.at(name).V_int;
}

void set_from_map (const HashParam &M, const char *name , vector<int> ** retour){
        if (M.at(name).is_set)
                *retour =  M.at(name).V_vector;
}

void set_from_map (const HashParam &M, const char *name , vector<double> ** retour){
        if (M.at(name).is_set)
                *retour =  M.at(name).V_double;
}

void destruction(type_def T){
        if(T.second.is_set) {
                if (T.second.type == T_VECTOR)
                        delete T.second.V_vector;
                if (T.second.type == T_DOUBLE)
                        delete T.second.V_double;
        }
}

void destruction_hashparam(HashParam M) {
        for_each(M.begin(), M.end(), destruction);
        for (unsigned int i = 0; i<n; i++ )
                delete [] data[i];
        delete [] data;
}