#include "ReadAnchors.h"
#include <vector>
#include "Anchor.h"
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

vector <Anchor> readAnchors(){


    istream& fin=cin;
    if (!fin)
    {
        throw runtime_error("errore apertura file");
    }

    vector <Anchor> anchors;
    int max_x=0;
    int max_y=0;

    int xb,xe,yb,ye,w;
    int i=1;

    //ancora begin
      
    anchors.insert(anchors.begin(), Anchor(0,0,0,0,0,0));

    while(fin >> xb >> xe >> yb >> ye >> w){

        if(xb >= xe || yb >= ye || w <0){
            cerr << "Ancora non valida. xb: " << xb << ", yb: " << yb << ", xe: " << xe << ", ye: " << ye << ", w: " << w << endl;
            continue; // Salta questa iterazione e continua con la successiva
        }
        anchors.emplace_back(xb,xe,yb,ye,w,i++);
        if (xe > max_x) max_x = xe;
        if (ye > max_y) max_y = ye;
        
    }
    

    //ancora end
        
    anchors.insert(anchors.end(),Anchor(max_x+ 1, max_x + 1,
                            max_y + 1, max_y + 1,
                            0, anchors.size()) );
    
    //fin.close();

    std::sort(anchors.begin(),anchors.end(),[](const Anchor& a, const Anchor& b){
        return a.getXend()<b.getXend();
    });

    return anchors;
}
