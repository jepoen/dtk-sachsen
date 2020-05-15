#ifndef TILES_H
#define TILES_H

#include <QList>
#include <QPointF>
#include <QString>
#include <QMap>

enum {TILE_X, TILE_Y};

class TilePos {
private:
    int myDir;
    QString myKey;
    QPointF myUtm; // pair left/right or top/bottom
    QPointF myWgs;

public:
    TilePos():
        myDir(-1), myKey(""), myUtm(QPointF()), myWgs(QPointF())
    {}
    TilePos(int dir, const QString& key, const QPointF& utm, const QPointF& wgs):
        myDir(dir), myKey(key), myUtm(utm), myWgs(wgs)
        {}
    int dir() const { return myDir; }
    QString key() const { return myKey; }
    QPointF utm() const { return myUtm; }
    QPointF wgs() const { return myWgs; }
};

class Tile {
private:
    QPointF myUtm0;
    QPointF myUtm1;
    QPointF myWgs0;
    QPointF myWgs1;
public:
    Tile():
        myUtm0(QPointF()), myUtm1(QPointF()), myWgs0(QPointF(1000, 1000)), myWgs1(QPointF()) {}
    Tile(const QPointF& utm0, const QPointF& utm1, const QPointF& wgs0, const QPointF& wgs1):
        myUtm0(utm0), myUtm1(utm1), myWgs0(wgs0), myWgs1(wgs1) {}
    bool isEmpty() const { return myWgs0.x() < 1000; }
};

class Tiles {
private:
    QString myTileDir;
    QList<TilePos> myXtiles;
    QList<TilePos> myYtiles;
    QMap<QString, Tile> myTiles;

public:
    Tiles();
public:
    QString tileDir() const { return myTileDir; }
    void setTileDir(const QString& tileDir) { myTileDir = tileDir; }
    void addXTile(const QString& key, const QPointF& utm, const QPointF& wgs) {
        myXtiles.append(TilePos(TILE_X, key, utm, wgs));
    }
    void addYTile(const QString& key, const QPointF& utm, const QPointF& wgs) {
        myYtiles.append(TilePos(TILE_Y, key, utm, wgs));
    }
    void addTile(const QString& key, const QPointF& utm0, const QPointF& utm1, const QPointF& wgs0,
                 const QPointF& wgs1) {
        myTiles.insert(key, Tile(utm0, utm1, wgs0, wgs1));
    }
    void clear() {
        myXtiles.clear();
        myYtiles.clear();
        myTiles.clear();
    }
    TilePos xTile(int idx) const;
    TilePos yTile(int idx) const;
    int sizeX() const { return myXtiles.size(); }
    int sizeY() const { return myYtiles.size(); }
    QString keyX(int idx) const  { return myXtiles.at(idx).key(); }
    QString keyY(int idx) const  { return myYtiles.at(idx).key(); }
    Tile tile(const QString& key) const {
        return myTiles.value(key, Tile());
    }
    // lower left boundary
    QPointF wgs0() const;
    // upper right boundary
    QPointF wgs1() const;
};

double geoDist1(const QPointF& p0, const QPointF& p1);
#endif // TILES_H
