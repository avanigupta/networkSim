#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QProgressBar>

class Server : public QWidget
{
    Q_OBJECT

public:
    Server(QWidget *parent = 0);
    ~Server();
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QTableWidget *table;

public slots:
    void updateTable(QStringList jobList);
};

#endif // SERVER_H
