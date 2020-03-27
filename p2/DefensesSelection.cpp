// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;


float DefenseValue(Defense* d){
    return (d->damage/d->attacksPerSecond)*4/5 + d->range*d->dispersion*d->health*1/5;
}


void cellvalue(std::vector<std::vector<float>> &m, std::list<Defense*> defenses, unsigned int ases){
    std::list<Defense*>::iterator it = defenses.begin();
    for(int i = 0; i < ases + 1; i++){
        if(i < (*it)->cost ) m[0][i] = 0;
        else m[0][i] = DefenseValue(*it);
    }

    for(int j =1; j < defenses.size(); j++, it++){
        for(int k = 0 ; k < ases + 1; k++){
            if(k < (*it)->cost) m[j][k] = m[j-1][k];
            else m[j][k] = std::max(m[j-1][k], m[j-1][k - (*it)->cost] + DefenseValue(*it));
        }
    }

}


void topDefenses(std::vector<std::vector<float>> &m, std::list<int> &selectedIDs, std::list<Defense *> defenses, unsigned int ases){
    
    std::list<Defense *>::iterator itDefe = defenses.end();
    --itDefe; 
    int j = ases;
    for (int i = defenses.size()-1; i > 0 && ases > 0; --i, --itDefe){
        if (m[i][j] != m[i-1][j]){
            selectedIDs.push_back((*itDefe)->id);
            ases -= (*itDefe)->cost;
            j-=(*itDefe)->cost;
        }
    }
    if(ases >= (*defenses.begin())->cost){
            selectedIDs.push_back((*defenses.begin())->id);
            ases -= (*defenses.begin())->cost;
    }
    
}


void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles){
    
    selectedIDs.push_front((*defenses.begin())->id);
    ases -= (*defenses.begin())->cost;
    defenses.pop_front();
    std::vector<std::vector<float>> m(defenses.size(), std::vector<float>(ases + 1));
    
    cellvalue(m, defenses, ases);
    topDefenses(m, selectedIDs, defenses, ases);

}
