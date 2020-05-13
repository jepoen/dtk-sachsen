#include <cmath>
#include "tiles.h"

Tiles::Tiles()
{}

TilePos Tiles::xTile(int idx) const {
    if (idx < 0 || idx >= myXtiles.size()) return TilePos();
    return myXtiles[idx];
}

TilePos Tiles::yTile(int idx) const {
    if (idx < 0 || idx >= myYtiles.size()) return TilePos();
    return myYtiles[idx];
}

QPointF Tiles::wgs0() const {
    if (myXtiles.size() == 0 || myYtiles.size() == 0) return QPointF();
    return QPointF(myXtiles.at(0).wgs().x(), myYtiles.at(0).wgs().y());
}

QPointF Tiles::wgs1() const {
    if (myXtiles.size() == 0 || myYtiles.size() == 0) return QPointF();
    return QPointF(myXtiles.at(myXtiles.size()-1).wgs().y(), myYtiles.at(myYtiles.size()-1).wgs().x());
}

double geoDist1(const QPointF& p0, const QPointF& p1) {
    double f = 1.0/298.257223563; // Abplattung
    double a = 6378.137; // radius
    double ff, gg, l;
    double s, c, cosl, sinl, cosf, sinf, cosg, sing;
    double w, d, r, h1, h2;
    double dist;

    if (p0 == p1) return 0.0;
    ff = 0.5*(p0.y()+p1.y())*M_PI/180.0;
    gg = 0.5*(p1.y()-p0.y())*M_PI/180.0;
    l = 0.5*(p1.x()-p0.x())*M_PI/180.0;
    cosl = cos(l)*cos(l);
    sinl = sin(l)*sin(l);
    cosf = cos(ff)*cos(ff);
    sinf = sin(ff)*sin(ff);
    cosg = cos(gg)*cos(gg);
    sing = sin(gg)*sin(gg);
    s = sing*cosl+cosf*sinl;
    c = cosg*cosl+sinf*sinl;
    w = atan(sqrt(s/c));
    d = 2*w*a;
    r = sqrt(s*c)/w;
    h1 = (3*r-1)/(2*c);
    h2 = (3*r+1)/(2*s);
    dist = d*(1+f*h1*sinf*cosg-f*h2*cosf*sing);
    return dist;
}

