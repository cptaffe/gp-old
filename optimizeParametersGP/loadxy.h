#ifndef _LOADXY_H
#define _LOADXY_H

#define XYsize 150

class XYclass {
public:
    void loadX(int indexX, int valueX) {
        XY[indexX].x = valueX;
    }
    void loadY(int indexY, long valueY) {
        XY[indexY].y = valueY;
    }
    int getX(int indexX) {
        return XY[indexX].x;
    }
    long getY(int indexY) {
        return XY[indexY].y;
    }
private:
    struct XYdata {

        int x;
        long y;
    };
    typedef struct XYdata XYarray;

    XYarray XY[XYsize +1];

};
#endif
