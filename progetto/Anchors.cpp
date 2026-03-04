#include "Anchors.h"

    Anchor::Anchor() : x_begin(0), y_begin(0), x_end(0), y_end(0), weight(0) {}
    Anchor::Anchor(int xb,int yb,int xe,int ye,int w){
            x_begin=xb;
            x_end=xe;
            y_begin=yb;
            y_end=ye;
            weight=w; 
            score=0;
            prec=-1;
        }
    int Anchor::getXbegin() const { return  x_begin; }
    int Anchor::getXend() const {return x_end;}
    int Anchor::getYbegin() const {return y_begin;}
    int Anchor::getYend() const {return y_end;} 
    int Anchor::getWeight() const {return weight;}
    int Anchor::getScore() const {return score;}
    int Anchor::getPrec() const {return prec;}
    void Anchor::setScore(int s) {score=weight+s;}
    void Anchor::setPrec(int p) {prec=p;}

    
