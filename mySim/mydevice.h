#ifndef MYDEVICE_H
#define MYDEVICE_H

#include <QString>
#include <QStringList>
#include <QRect>
#include <QLineF>

class MyDevice
{
public:
    MyDevice();
    bool isInRangeOf(MyDevice dev2);

    enum Type
    {
        Server,
        Node,
        Mobile
    };

    Type type;
    QRect area;
    int range, taskLength;
    QString name, services;
    bool busy, marked;
};

#endif // MYDEVICE_H
