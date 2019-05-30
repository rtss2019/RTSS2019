//
//  parser.hpp
//  finite_case
//
//  Created by Stéphan on 12/01/2018.
//  Copyright © 2018 Stéphan. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <stdio.h>

using namespace std;

// liste des types
typedef enum {
        T_INT,
        T_VECTOR,
        T_DOUBLE
} T_type;
// structure de comparaison pour la map
struct ltstr
{
        bool operator()(const char* s1, const char* s2) const
        {
                return strcmp(s1, s2) < 0;
        }
};

// structure pour les valeurs de la map
struct TypeVal
{
        bool is_set; // true if the value has been set
        T_type type; // true type of the value
        union {
                int V_int;
                vector<int> *V_vector;
                vector<double> *V_double;
        }; // value
        TypeVal (){
                is_set=false;
        };
        ~TypeVal () {};
};

typedef std::map<const char *, TypeVal,ltstr> HashParam;
typedef pair<const char*, TypeVal> type_def;

HashParam init_hash_map (const char *s);

unsigned int skip_space(unsigned int space, char* line);
unsigned int next_space(unsigned int space, char* line);
unsigned int skip_space_reverse(unsigned int space, char* line);
unsigned int next_fig(unsigned int first_indice, char *line);
bool is_number (char *c);

void set_from_map (const HashParam &M, const char *name , int &retour );
void set_from_map (const HashParam &M, const char *name , vector<int> ** retour);
void set_from_map (const HashParam &M, const char *name , vector<double> ** retour);


void destruction_hashparam(HashParam M);


#endif /* parser_hpp */
