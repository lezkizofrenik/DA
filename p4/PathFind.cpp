// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0);
}


void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight){
    i_out = (int)(pos.y * 1.0f / cellHeight);
    j_out = (int)(pos.x * 1.0f / cellWidth);
}


float sDistance(AStarNode *originNode, AStarNode *foundNode, float cellWidth, float cellHeight){
    return _distance(originNode->position, foundNode->position);
}


bool operator==(const AStarNode &o, const AStarNode &t){
    return (_distance(o.position, t.position) == 0.0);
}


bool compareNodes(const AStarNode *o, const AStarNode *t){
    return (o->F > t->F);
}

float costDefense(Defense* d1){
    return (d1->health*1/5 + d1->attacksPerSecond*1/5 + d1->range*1/5 + d1->damage*1/5
                                                        + d1-> dispersion*1/5);
}

void DEF_LIB_EXPORTED calculateAdditionalCost(float **cellCost, int cellsWidth, int cellsHeight, 
            float mapWidth, float mapHeight, List<Object *> obstacles, List<Defense *> defenses){

    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;
    List<Defense *>::iterator itDef;
    List<Object *>::iterator itObs;
    Vector3 cellPosition;
    float cost = 0;

    for (int i = 0; i < cellsHeight; i++){
            for (int j = 0; j < cellsWidth; j++){

                cellPosition = cellCenterToPosition(i, j, cellWidth, cellHeight);
                cost=0.0;
                for (itDef = defenses.begin(); itDef != defenses.end(); itDef++){
                        float rr= 4* _distance((*itDef)->position, cellPosition) - (*itDef)->radio;
                        for (itObs = obstacles.begin(); itObs != obstacles.end(); itObs++){
                            if(_distance((*itObs)->position, cellPosition) - (*itObs)->radio < rr )
                                rr += pow(_distance(cellPosition, (*itObs)->position),3);
                        }
                
                    cost +=(rr + costDefense(*itDef));
                }
                cellCost[i][j] = cost ;
            }

        }
}


void DEF_LIB_EXPORTED calculatePath(AStarNode *originNode, AStarNode *foundNode, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight, float **cellCost, std::list<Vector3> &path){
    std::vector<AStarNode *> opened, closed;
    List<AStarNode *>::iterator it;
    int i, j;
    float distance;
    bool found = false;

    AStarNode *current = originNode;
    positionToCell(originNode->position, i, j, cellsWidth, cellsHeight);

    current->G = 0;
    current->H = sDistance(originNode, foundNode, cellsWidth, cellsHeight) + cellCost[i][j];
    current->F = current->G + current->H;
    opened.push_back(current);
    std::make_heap(opened.begin(), opened.end());

    while (!found && !opened.empty()){ 
        current = opened.front();
        std::pop_heap(opened.begin(), opened.end(), compareNodes);
        opened.pop_back();
        closed.push_back(current);
        if (*current == *foundNode) found = true;
        else{
            for (it = current->adjacents.begin(); it != current->adjacents.end(); it++){
                if (std::find(closed.begin(), closed.end(), (*it)) == closed.end()){
                    if (std::find(opened.begin(), opened.end(), (*it)) == opened.end()){
                        (*it)->parent = current;
                        positionToCell((*it)->position, i, j, cellsWidth, cellsHeight);
                        (*it)->H = sDistance((*it), foundNode, cellsWidth, cellsHeight) + cellCost[i][j];
                        (*it)->G = current->G + _distance(current->position, (*it)->position);
                        (*it)->F = (*it)->H + (*it)->G;
                        opened.push_back((*it));
                        std::push_heap(opened.begin(), opened.end(), compareNodes);
                    }
                    else{
                        distance = _distance(current->position, (*it)->position);
                        if ((*it)->G > current->G + distance)  {
                            (*it)->G = current->G + distance;
                            (*it)->parent = current;
                            (*it)->F = (*it)->H + (*it)->G;
                            std::sort_heap(opened.begin(), opened.end(), compareNodes);
                        }
                    }
                }
            }
        }
    }
    while (current->parent != originNode){
        current = current->parent;
        path.push_front(current->position);
    }
}