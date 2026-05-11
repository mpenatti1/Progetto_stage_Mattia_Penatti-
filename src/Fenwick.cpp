#include "Fenwick.h"

Fenwick::Fenwick(int size){

    n=size;
    bit.assign(n+1, {std::numeric_limits<int>::min(), -1}); // Inizializza con punteggi minimi e id -1
}

void Fenwick::update(int i, int score, int id){

    for(; i <=n; i+=i&(-i)){
        if(score > bit[i].maxScore){
            bit[i].maxScore = score;
            bit[i].bestId = id;
        }
    }

}

FenwickNode Fenwick::query(int i){

    FenwickNode res = {std::numeric_limits<int>::min(), -1}; // Inizializza con punteggio minimo e id -1
    for(; i > 0; i-=i&(-i)){
        if(bit[i].maxScore > res.maxScore){
            res = bit[i];
        }
    }
    return res;
}