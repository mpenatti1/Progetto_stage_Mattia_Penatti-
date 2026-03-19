# MAXIMAL COLINEAR CHAINING 

## In generale

Il Maximal Colinear Chaining è un algoritmo applicato nella bioinformatica con lo scopo di trovare, dati 2 genomi, la **sequenza di corrispondenze (ancore) con massimo score**. Esso viene impiegato nel contesto del macroproblema dell’**allineamento di sequenze** (*sequence alignment*) tra k genomi.   
Applicazioni comuni:
- allineamento di sequenze genomiche
- genome mapping
- comparazione tra genomi
- analisi di similarità tra stringhe  

Il sequence alignment è suddiviso in 3 fasi: 
1) Ricerca dei frammenti simili tra i genomi
2) **Chaining**
3) Allineamento delle regioni tra le ancore

In questa libreria ci occupiamo del secondo punto -- la fase di Chaining delle ancore.

## Overview
Date in input n ancore, restituisce la catena di ancore con massimo score. 
Devono essere rispettati **2 vincoli**: 
- **colinearità** --> le ancore devono essere colineari
- **non sovrapposizione** --> le ancore non devono essere sovrapposte

Ogni ancora/frammento è una corrispondenza tra i 2 genomi. Essa è caratterizzata da un punto di inizio, un punto di fine, e un peso.  
Il formato di ogni ancora è quindi del tipo: ( x_begin, y_begin, x_end, y_end, weight )  
Dati 2 frammenti f e f', per rispettare i 2 vincoli durante la concatenazione, viene verificato :  
`f.x_end < f'.x_begin && f.y_end < f'.y_begin con f << f' `   
Durante l' esecuzione a ogni ancora viene associato uno score. Tramite una funzione di ricerca dedita a  trovare il miglio predecessore, la RMQ (Range Maximum Query), ad ogni frammento f' viene concatenato la catena di frammenti che presenta il massimo score.   
Lo score quindi è definito come: *f'.score = f.score + f'.weight*  
Nel chaining problem teniamo in considerazione anche la distanza tra i frammenti, i gap cost, i quali rappresentano una penalità : maggiore è la distanza tra le ancore, maggiore è il costo sottratto allo score.
Dati 2 frammenti f e f', definiamo il gap cost come: 
- **gc(f',f) = d( beg(f') , end(f) )**  **∀ f << f'**
##### (d = distanza tra i 2 punti)    

Riscriviamo lo score di ogni frammento come:
- **f'.score = f'.weight + f.score - gc(f',f)**  

Associamo ad ogni ancora una nuova variabile, la **priority**, essa indica lo score corretto che tiene conto anche del costo geometrico verso il nodo terminale.
Definita come:    
- **f'.priority= f'.score - gc(f', t)**  

Dove t rappresenta un' ancora fittizia posta alla fine della catena.  

<u>**La catena restituita dalla RMQ e infine dal programma sarà la catena con priority massima.** </u>

Date queste definizioni procediamo all' utilizzo effettivo del programma.


## Utilizzo

### Main
Nel Main devono essere incluse:   
```cpp
 #include "Anchor.h"   
 #include "ChainSolver.h" 
 #include <vector>   
 #include "ReadAnchors.h"
 ```      

È consigliabile inserire il file main.cpp nella cartella src insieme agli altri sorgenti. In questo modo, i percorsi degli #include rimangono coerenti e non è necessario specificare cammini relativi complessi per raggiungere gli header della libreria.  

Per l' esecuzione è necessario inserire nel codice:   
```cpp
vector <Anchor> anchors = readAnchors();  
solve(anchors);
```


### Makefile 
La libreria dispone di un makefile, per la compilazione quindi basta utilizzare il comando <u>**make**</u>.  
Il programma di base ha output di debug, per evitare la loro stampa è necessario utilizzare il comando <u>**make release**</u>.  
Il comando restituirà un eseguibile nominato maximalColinearChaining.  
Per l' esecuzione : **./maximalColinearChaining < input.txt > output.txt 2> debug.txt**  


### Formato Input

Lista di ancore ( x_begin, y_begin, x_end, y_end, weight)  
Esempio:  

1 1 3 5 6  
2 4 4 6 4  
5 6 7 9 10  
8 9 10 11 7   

### Formato Output

La catena colineare non sovrapposta di ancore.  
Esempio:  

x_begin y_begin x_end y_end weight  
0 0 0 0 0  
1 1 3 5 6  
5 6 7 9 10  
8 9 10 11 7  
11 12 11 12 0  
Score totale: 15    

Alla catena vengono aggiunte un' ancora di inizio (0,0,0,0,0) e una di fine (11,12,11,12,0), necessarie per l' algoritmo.  
<u>L' ancora (fittizzia) di fine ha come coordinate il punto di end dell' ultma ancora **reale** incrementato di 1.</u> 

## Note
- Le ancore sono assunte già calcolate nella fase di seeding.
- Il Chaining non modifica le ancore ma resttuisce la sottosequenza ottimale.
- Le ancore di Start e di End sono aggiunte artificialmente.

## Complessità

- Time complexity: O(n log(n) ) 
- Space complexity: O(n)

## Referenze 
- Dispense del corso di Genome Comparison, Freie Universität Berlin  
https://www.mi.fu-berlin.de/wiki/pub/ABI/GenomeComparisonP4/chaining.pdf
- KD-Tree - https://en.wikipedia.org/wiki/K-d_tree
## Autore
- Mattia Penatti


























