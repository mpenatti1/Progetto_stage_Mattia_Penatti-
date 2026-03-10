#ifndef ANCHORS_H
#define ANCHORS_H

class Anchor{

    private : 
        int x_begin,y_begin;
        int x_end,y_end;
        int weight;
        int score;
        int prec;

    public : 

        Anchor ();
        Anchor(int xb, int yb, int xe, int ye, int w);

        int getXbegin() const;
        int getYbegin() const;
        int getXend() const;
        int getYend() const;
        int getWeight() const;
        int getScore()const;
        int getPrec()const;
        void setScore(int s);
        void setPrec(int p);

};

#endif