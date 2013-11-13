#include "mydevice.h"

MyDevice::MyDevice()
{
    range = taskLength = 0;
    busy = marked = false;
    services = "";
}

bool MyDevice::isInRangeOf(MyDevice dev2)
{
    int cDist = int(QLineF(QLine(area.center(), dev2.area.center())).length()), radSum = range + dev2.range;

    if(cDist < radSum)
        return true;
    else
        return false;
}
