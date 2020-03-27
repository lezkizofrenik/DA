// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include <stack>
#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"
//

#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;
struct compare {
    bool operator()(Defense * d1, Defense * d2) {return ((d1->health*1/5 + d1->attacksPerSecond*1/5 + d1->range*1/5 + d1->damage*1/5
                                                        + d1-> dispersion*1/5) > (d2->health*1/5 + d2->attacksPerSecond*1/5 + d2->range*1/5 + d2->damage*1/5
                                                        + d2-> dispersion*1/5));}
};

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); }
void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight){ i_out = (int)(pos.y * 1.0f/cellHeight); j_out = (int)(pos.x * 1.0f/cellWidth); }

// Dado por la distancia al centro, la distancia la primera defensa y la distancia 
// al obstaculo más cercano
float cellValue(int row, int col, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, Defense* defense) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    int nNearObjects = 0;
   
    //Idea alternativa: float distanciaMax = sqrt( pow(cellHeight,2) + pow(cellWidth,2));
    float distanciaMax =sqrt(pow(cellHeight, 2) + pow(cellWidth, 2));
    Vector3 cell = cellCenterToPosition(row, col, cellWidth, cellHeight);
    Vector3 center(mapWidth/2, mapHeight/2, 0);

    for(List<Object*>::iterator it = obstacles.begin();it != obstacles.end();it++){
        if(_distance(cell, (*it)->position) - (*it)->radio <= distanciaMax) nNearObjects++;
    }

    // Si lo pongo en negativo puedo usar la funcion de seleccion de maximizar
    // Ya que el resultado óptimo es el mas pequeño posible
	return nNearObjects + (-(_distance(cell, defense->position) - defense->radio)/100 - _distance(cell, center)); // implemente aqui la funci�n que asigna valores a las celdas
}


// Dado por la distancia al centro y por el número de obstáculos cercanos 
float FirstDefensecellValue(int row, int col, int nCellsWidth, int nCellsHeight
	, float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
        
        float cellWidth = mapWidth / nCellsWidth;
        float cellHeight = mapHeight / nCellsHeight; 
        int nObstacles = 0;
  
        Vector3 cell = cellCenterToPosition(row, col, cellWidth, cellHeight);
        Vector3 center(mapWidth/2, mapHeight/2, 0);

        for(List<Object*>::iterator it = obstacles.begin();it != obstacles.end();it++){
            if( _distance(cell, (*it)->position) - (*it)->radio - (*defenses.begin())->radio <= (*defenses.begin())->range) 
            
                    //distanciaMax = _distance(cell, (*it)->position) - (*it)->radio;
                    nObstacles++;
        }
               
        return nObstacles - _distance(cell, center); //Si fuera óptimo tiene que salir un coeficiente grande

}


/*Una función de factibilidad que comprueba si un conjunto de candidatos se puede ampliar para obtener 
 * una solución (no necesariamente óptima)
 * Función: ¿Se puede colocar una defensa sin que colisione con otra o con una montaña?
 * -> Nota: Debe comprobarse que la defensa existe y que aun no se ha colocado*/


bool feasibility(int id, int row, int col, int nCellsWidth, int nCellsHeight, 
                float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses){
    
    bool b=true; //Hasta que no se demuestre lo contrario, puede colocarse
    Vector3 defense=cellCenterToPosition(row, col, mapWidth / nCellsWidth, mapHeight / nCellsHeight);
   
    List<Defense*>::iterator dfP;
    for(dfP = defenses.begin(); dfP != defenses.end() && (*dfP)->id != id; dfP++ ){}
    //Lo localiza en la lista para poder acceder a sus características

    //Comprobación de si se sale del mapa:
    if( defense.x - (*dfP)->radio < 0 ||
        defense.x + (*dfP)->radio > mapWidth ||
        defense.y - (*dfP)->radio < 0 ||
        defense.y + (*dfP)->radio > mapHeight ||
        row < 0 || row > nCellsHeight || col < 0 || col > nCellsWidth) b = false; 


    //Comprueba si colisiona con el resto de defensas
    for(List<Defense*>::iterator dfIt = defenses.begin(); dfIt != defenses.end() && b!= false; dfIt++){
            //Si comprueba que colisiona con alguno, deja de mirar
        if( _distance(defense, (*dfIt)->position) - (*dfP)->radio  - (*dfIt)->radio < 0) b = false;
    }
    
    for(List<Object*>::iterator m = obstacles.begin(); m != obstacles.end() && b!= false; m++ ){
    //Compruebo si colisiona con alguna montaña
        if((_distance(defense, (*m)->position) - (*dfP)->radio  - (*m)->radio < 0)) b = false;
    }

    return b;

}


/* Función de selección: indica el candidato más prometedor.
 * En nuestro caso sería el de mayor valor (puntuación).*/

Vector3 selection(std::vector<std::vector<float>>& MapValue, float cellHeight,
						 float cellWidth, int nCellsWidth, int nCellsHeight, std::list<Defense*> defenses) {
    float r= -INF_F;
    int ii,jj;
    
    for(int i = 0; i < nCellsHeight; i++){
        for(int j = 0; j < nCellsWidth; j++){ 
            if(MapValue[i][j] > r){ 
                    r = MapValue[i][j];
                    ii= i;
                    jj = j;
            }  
        }
    }

    MapValue[ii][jj] = -INF_F; //Simbólico para marcarlo como seleccionado
    //Devuelve el valor máximo
    return cellCenterToPosition(ii, jj, cellWidth, cellHeight);
}


void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , std::list<Object*> obstacles, std::list<Defense*> defenses) {

   
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    int maxAttemps = 1000;
   
    //inicializo el mapa de valores
    std::vector<std::vector<float>> MapValue(nCellsHeight, std::vector<float>(nCellsWidth));
    std::vector<std::vector<float>> MapValueFirstD(nCellsHeight, std::vector<float>(nCellsWidth));

    std::list<Defense*> defensescopy = defenses;
    defensescopy.erase(defensescopy.begin());
    defensescopy.sort(compare());
    
    for(int i = 0; i < nCellsHeight; i++){
        for(int j = 0; j < nCellsWidth; j++){
            
            MapValue[i][j] = cellValue(i, j, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, *defenses.begin());
            MapValueFirstD[i][j] = FirstDefensecellValue(i, j, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
           
        }
    }
    
   int row, col; //variables para obtener la fila y la columna a partir de positiontoCell
   Vector3 v;
   bool firstTime = false;
   for( List<Defense*>::iterator currentDefense = defensescopy.begin(); currentDefense != defensescopy.end() && maxAttemps > 0; maxAttemps--){
        if(!firstTime){ //Como estoy recorriendo la copia sin la primera defensa, tengo que hacer este bloque aparte para utilizar
                        //la lista original
            v = selection(MapValueFirstD, cellHeight, cellWidth, nCellsWidth, nCellsHeight, defenses);
             positionToCell(v, row, col, cellWidth, cellHeight);
            if(feasibility((*defenses.begin())->id, row, col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)){
                maxAttemps = 1000;
                (*defenses.begin())->position = v;
                firstTime=true;
             } 
           
        }else{
            v = selection(MapValue, cellHeight, cellWidth, nCellsWidth, nCellsHeight, defenses);
            positionToCell(v, row, col, cellWidth, cellHeight);
            if(feasibility((*currentDefense)->id, row, col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)){
                maxAttemps = 1000;
                (*currentDefense)->position = v;
                currentDefense++;
            } 
        }
   }
   
}



#ifdef PRINT_DEFENSE_STRATEGY

    float** cellValues = new float* [nCellsHeight]; 
    for(int i = 0; i < nCellsHeight; ++i) {
       cellValues[i] = new float[nCellsWidth];
       for(int j = 0; j < nCellsWidth; ++j) {
           cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
       }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
                         , cellValues, std::list<Defense*>(), true);

    for(int i = 0; i < nCellsHeight ; ++i)
        delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif

