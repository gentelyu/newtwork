#ifndef TEST_H
#define TEST_H


/* 基类 */
//Perimeter and area

class graphics
{
public:

    /* 计算周长 */
    virtual int getPerimeter(int length, int widget, int r) = 0;

    /* 计算面积 */
    virtual int getArea(int length, int widget, int r) = 0;

    


private:
    int length;
    int widget;
    int r;
};


/* 长方形 */
class Rectangle:public graphics
{
public:
    int getPerimeter(int length,int widget, int r );

    int getArea(int length,int widget);


private:
    int length;
    int widget;
};

/* 正方形 */
class Square:public graphics
{
public:
    int getPerimeter(int length,int widget, int r );

    int getArea(int length,int widget);


private:
    int length;
    int widget;
};


/* 圆 */
class Circular: public graphics
{
public:
    int getPerimeter(int length,int widget, int r );

    int getArea(int length,int widget);


private:
    int length;
    int widget;
    int r;



};








#endif