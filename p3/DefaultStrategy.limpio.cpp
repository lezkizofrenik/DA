// ###### Config options ################



// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"

using namespace Asedio;   

/*
struct compare {
    bool operator()(Defense * d1, Defense * d2) {return ((d1->health*1/5 + d1->attacksPerSecond*1/5 + d1->range*1/5 + d1->damage*1/5
                                                        + d1-> dispersion*1/5) > (d2->health*1/5 + d2->attacksPerSecond*1/5 + d2->range*1/5 + d2->damage*1/5
                                                        + d2-> dispersion*1/5));}
};
*/
struct compare {
    bool operator()(const cellValueStruct &d1, cellValueStruct &d2) {return d1 > d2;}
};

struct cellValueStruct{

    cellValueStruct(int xx = 0, int yy = 0, float vv = 0.0) : x(xx), y(yy), value(vv) {}

    bool operator<(const cellValueStruct &d){
        return value < d.value;
    }

    bool operator>(const cellValueStruct &d){
        return value > d.value;
    }

    bool operator<=(const cellValueStruct &d){
        return (value <= d.value) || (value == d.value);
    }

    int x, y;
    float value;
};
struct compare {
    bool operator()(const cellValueStruct &d1, cellValueStruct &d2) {return d1 > d2;}
};

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight){ return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); }
void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight){ i_out = (int)(pos.y * 1.0f/cellHeight); j_out = (int)(pos.x * 1.0f/cellWidth); }


bool feasibility(int id, int row, int col, int nCellsWidth, int nCellsHeight, 
                float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses){
    
    bool b=true; //Hasta que no se demuestre lo contrario, puede colocarse
    Vector3 defense=cellCenterToPosition(row, col, mapWidth / nCellsWidth, mapHeight / nCellsHeight);
   
    List<Defense*>::iterator dfP;
    for(dfP = defenses.begin(); dfP != defenses.j() && (*dfP)->id != id; dfP++ ){}
    //Lo localiza en la lista para poder acceder a sus características

    //Comprobación de si se sale del mapa:
    if( defense.x - (*dfP)->radio < 0 ||
        defense.x + (*dfP)->radio > mapWidth ||
        defense.y - (*dfP)->radio < 0 ||
        defense.y + (*dfP)->radio > mapHeight ||
        row < 0 || row > nCellsHeight || col < 0 || col > nCellsWidth) b = false; 


    //Comprueba si colisiona con el resto de defensas
    for(List<Defense*>::iterator dfIt = defenses.begin(); dfIt != defenses.j() && b!= false; dfIt++){
            //Si comprueba que colisiona con alguno, deja de mirar
        if( _distance(defense, (*dfIt)->position) - (*dfP)->radio  - (*dfIt)->radio < 0) b = false;
    }
    
    for(List<Object*>::iterator m = obstacles.begin(); m != obstacles.j() && b!= false; m++ ){
    //Compruebo si colisiona con alguna montaña
        if((_distance(defense, (*m)->position) - (*dfP)->radio  - (*m)->radio < 0)) b = false;
    }

    return b;

}


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

// ##### FUSIÓN ####
void orderFusion(std::vector<cellValueStruct> &v, int i, int j){
    if(j-i > 1){ //Mientras se pueda dividir:
        int k = (i+j)/2;
        orderFusion(v, i, k);
        orderFusion(v, k+1, j);
        fusion(v, i, j, k);
    }
   
}


void fusion(std::vector<cellValueStruct> &v, int i, int j, int k){
    int aux[j-i+1];
	int iLeft = i, iRight = k+1, cont = 0;

	while(iLeft <= k && iRight <= j) {
		if(v[iLeft] <= v[iRight]) {
			aux[cont] = v[iLeft];
			iLeft += 1;
		}
		else {
			aux[cont] = v[iRight];
			iRight += 1;
		}
        cont += 1;
	}

	for(cont = i; cont <= j; cont += 1) {
		v[cont] = aux[cont - i]
	}
}

// #### QUICKSORT ####
void quickSort(std::vector<cellValueStruct> &v, int i, int j){

    if (j-i>1) {
        int pivot = pivote(v, i, j);
        quicksort(v, i, pivot - 1);
        quicksort(v, pivot + 1, j);
    }
}


int pivote(std::vector<cellValueStruct> &v, int start, int end)  {  
    int pivot = v[end]; 
    int min = (start - 1);  
  
    for (int i = start; i <= end - 1; i++){  
        if (v[i] < pivot){  
            min++; 
            std::swap(v[min], v[i]);  
        }  
    }  
    std::swap(v[min + 1], v[end]);  
    return (min + 1);  
}  

// #### MONTICULO ####

void heap(std::vector<cellValueStruct> &v){
    std::make_heap(v.begin(), v.end());
    std::sort_heap(v.begin(), v.end());
}

float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 

    Vector3 cell = cellCenterToPosition(row, col, cellWidth, cellHeight);
    Vector3 center(mapWidth/2, mapHeight/2, 0);

	return  -_distance(cell, center); 
}

void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
	cronometro cNoOrder, cQuickSort, cFusion, cHeap;
 
    for(int it = 0; it<3; it++){
    
        //inicializo el mapa de valores
        std::vector<std::vector<float>> MapValue(nCellsHeight, std::vector<float>(nCellsWidth));
        int maxAttemps = 1000;
        //defensescopy.sort(compare());
        std::vector<cellValueStruct> cells;

        switch(it){
            case 0:
                cNoOrder.activar();
            case 1: 
                cFusion.activar();
            break;
            case 2:   
                cQuickSort.activar();
            break;
            case 3: 
                cHeap.activar();
            break;
        }

        for(int i = 0; i < nCellsHeight; i++){
            for(int j = 0; j < nCellsWidth; j++){
                MapValue[i][j] = defaultCellValue(i, j, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, *defenses.begin());
            }
        }

        if(it!=0){
            for(int i = 0; i < nCellsHeight; i++){
                for(int j = 0; j < nCellsWidth; j++){
                    cells.push_back(cellValueStruct(i, j, MapValue[i][j]));
                }
            }
        }
        
        //cells.sort(compare()); //Mejor caso: todo ordenado

        switch(i){
            case 1: 
                orderFusion(cells, 0, cells.size()-1); //SIZE ESTA BIEN O HAY QUE RESTARLE?
            break;
            case 2:   
                quickSort(cells, 0, cells.size()-1); //SIZE ESTA BIEN O HAY QUE RESTARLE?
            break;
            case 3: 
                heap(cells); 
            break;
        }

        int row, col, i = 0;
  
        for( List<Defense*>::iterator currentDefense = defenses.begin(); currentDefense != defenses.end() && maxAttemps > 0; maxAttemps--){
            if(it!=0) Vector3 v(cells[i].x, cells[i].y);
            else Vector 3 v = selection(MapValue, cellHeight, cellWidth, nCellsWidth, nCellsHeight, defenses);
            positionToCell(v, row, col, cellWidth, cellHeight);
            if(feasibility((*currentDefense)->id, row, col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)){
                maxAttemps = 1000;
                (*currentDefense)->position = v;
                currentDefense++;
                i++; //No se si deberia intentar una posicion hasta colocar una defensa o si al reves
            } 
        }
    
        switch(it){
            case 0:
                cNoOrder.parar();
            case 1: 
                cFusion.parar();
            break;
            case 2:   
                cQuickSort.parar();
            break;
            case 3: 
                cHeap.parar();
            break;
        }

        if(it!=0) cells.clear();
    }

    std::cout 
    << "No order: " << cNoOrder.tiempo() << "\n" 
    << "Fusion: " << cFusion.tiempo() << "\n"
    << "QuickSort: " << cQuickSort.tiempo() << "\n"
    << "Montículo: " << cHeap.tiempo() << "\n" <<
    std::endl;
}
