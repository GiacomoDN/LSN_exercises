#include <iostream>
#include <math.h>
#include "position.h"

using namespace std;

void Position :: initialize(int dim){
    _ndim = dim;
    _x.resize(_ndim);
    _xold.resize(_ndim);
    return;
}

void Position :: translate(vec delta){
    for(unsigned int i=0; i<_ndim; i++){
        _x(i) = _x(i) + delta(i);
    }
}

void Position :: moveback(){
    _x = _xold;
}

void Position :: acceptmove(){
    _xold = _x;
}

double Position :: getposition(int dim, bool xnew){
    if(xnew) return _x(dim);
    else return _xold(dim);
}

void Position :: setposition(int dim, double position){
    _x(dim) = position;
    return;
}

void Position :: setpositold(int dim, double position){
    _xold(dim) = position;
    return;
}