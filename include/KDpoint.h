#pragma once
#define KDPOINT_H
class KDpoint{

    private:
    int x,y;
    int id;
    int gc;
    int priority;

    public : 

        
        KDpoint(int xe, int ye, int i);

        int getX() const;
        int getY() const;
        int getPriority() const;
        int getId() const;
        int getGc() const { return gc; }


        void setGc(int xb, int yb);
        void setPriority(int score);
        

}; 
