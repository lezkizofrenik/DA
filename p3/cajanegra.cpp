#include<stdio.h>
#include <stdlib.h> 
#include <vector>
#include<iostream>
#include<algorithm>

void fusion(std::vector<int> &v, int i, int j, int k){
    int left= k-i+1, right= j-k, iaux = 0, jaux = 0, kaux = i;

    std::vector<int> L(left), R(right);

    for (int r = 0; r < right; ++r) R[r] = v[k+1+r];
    for (int x = 0; x < left; ++x) L[x] = v[x+i];

    while (iaux < left && jaux < right){
        if(L[iaux] <= R[jaux]){
            v[kaux] = L[iaux];
            iaux++;
        }
        else{
            v[kaux] = R[jaux];
            jaux++;
        }

        kaux++;
    }

    while (jaux < right){
        v[kaux] = R[jaux];
        jaux++;
        kaux++;
    }
    while (iaux < left){
        v[kaux] = L[iaux];
        iaux++;
        kaux++;
    }

}


void sortFusion(std::vector<int> &v, int i, int j){ 
    if (i < j) { 
        int k = i+(j-i)/2; 
        sortFusion(v, i, k); 
        sortFusion(v, k+1, j); 
        fusion(v, i, j, k); 
    } 
} 


int pivote(std::vector<int> &v, int start, int end)  {  
    int pivot = v[end]; 
    int min = (start - 1);  
  
    for (int i = start; i < end; i++){  
        if (v[i] <= pivot){  
            min++; 
            std::swap(v[min], v[i]);  
        }  
    }  
    std::swap(v[min + 1], v[end]);  
    return (min + 1);  
}  


void quickSort(std::vector<int> &v, int i, int j){

    if (i < j) {
        int pivot = pivote(v, i, j);
        quickSort(v, i, pivot-1);
        quickSort(v, pivot+1, j);
    }
}

void mostrar(std::vector<int> &v){
    for(int i = 0; i < v.size(); i++)
        std::cout << i << ": " << v[i] << std::endl;
}

void generar(std::vector<int> &v){
    for(int i = 0; i < 15; i++) v.push_back(rand());
}

void comprobar(std::vector<int> &v){
     std::vector<int> ordenado = v;
     std::sort(ordenado.begin(), ordenado.end());
     if(std::equal(ordenado.begin(), ordenado.end(), v.begin())) 
        std::cout << "Ha ordenado correctamente" << std::endl;
     else std::cout << "Ha ordenado mal" << std::endl;
}

int main(){

    std::vector<int> v;
    std::cout << "QUICKSORT" << std::endl;
    generar(v);
    quickSort(v, 0, v.size()-1);
    comprobar(v);
    
    std::cout << "FUSION" << std::endl;
    generar(v);
    sortFusion(v, 0, v.size()-1);
    comprobar(v);

}
