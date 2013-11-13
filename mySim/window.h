#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QtWidgets>
#include <QMessageBox>
#include <QFontDialog>
#include <QFile>
#include <QDataStream>

#include "renderarea.h"

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();
    void handleIt();

    enum Modes
    {
        Server,
        Node,
        Mobile,
        Simulation
    };

public slots:
    void servClicked();
    void locClicked();
    void mobClicked();
    void delClicked();
    void fontClicked();
    void clrClicked();
    void saveClicked();
    void loadClicked();
    void abtClicked();

protected:
    void closeEvent(QCloseEvent *event);

private:
    RenderArea *renderArea;
    Modes mode;
    quint32 magic;
};

#endif // WINDOW_H
