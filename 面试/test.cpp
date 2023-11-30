#include "test.h"
#include <iostream>

int Rectangle::getPerimeter(int length, int widget, int r)
{
    this->length = length;
    this->widget = widget;
   

    int ret = ((length + widget) << 2);

    return ret;
}

int Rectangle::getArea(int length, int widget)
{
    this->length = length;
    this->widget = widget;

    int ret = 0;

    ret = (length * widget);

    return ret;
}

int Square::getPerimeter(int length, int widget, int r)
{
    this->length = length;
    this->widget = NULL;

    return 0;
}
