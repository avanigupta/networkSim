#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QPixmap>
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QList>
#include <QPoint>
#include <QInputDialog>
#include <QTimer>
#include <QMessageBox>
#include <QHash>
#include <time.h>

#include "mydevice.h"
#include "server.h"

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void addDevice(MyDevice);
    void clear();
    void setOk();
    void setDel(bool value);
    void setChanged();
    QList<MyDevice> getDevices();
    void setDevices(QList<MyDevice>);
    int devCount();

protected:
    void paintEvent(QPaintEvent *t);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:
    void jobCompletion(QStringList jobList);

private:
    QPixmap pixmap;
    QPoint clickPoint;
    MyDevice device;
    QList<MyDevice> devices;
    int dev;
    QTimer *timer;
    Server server;
    QStringList nodeJobs;
    bool ok, delDevice, newDevice, changed;
    QHash<QString, int> indices;

    int tryAddJob(QString job, int dest, bool &net);
    void clearMarkings();
    void clearBusy();

public slots:
    void timeOut();
};
#endif // RENDERAREA_H
