#include "server.h"

Server::Server(QWidget *parent) : QWidget(parent)
{
    table = new QTableWidget(0, 5);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("Source"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("Destination"));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem("Current Task"));
    table->setHorizontalHeaderItem(3, new QTableWidgetItem("Length"));
    table->setHorizontalHeaderItem(4, new QTableWidgetItem("Progress"));

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(table, 0, 0, 1, 0);

    setLayout(layout);
    setWindowTitle("Server Monitor");
    setWindowIcon(QIcon(":/images/server.png"));
}

Server::~Server()
{
    delete table;
}

QSize Server::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Server::sizeHint() const
{
    return QSize(640, 359);
}

void Server::updateTable(QStringList jobList)
{
    QStringList parts;

    table->clearContents();
    table->setRowCount(0);

    for(int i = 0; i < jobList.length(); i++)
    {
        parts = jobList[i].split(':');

        QProgressBar *bar = new QProgressBar;
        bar->setMinimum(0);
        bar->setMaximum(parts[3].toInt());
        bar->setValue(parts[4].toInt());

        table->insertRow(0);
        table->setItem(0, 0, new QTableWidgetItem(parts[0]));
        table->setItem(0, 1, new QTableWidgetItem(parts[1]));
        table->setItem(0, 2, new QTableWidgetItem(parts[2]));
        table->setItem(0, 3, new QTableWidgetItem(parts[3]));
        table->setCellWidget(0, 4, bar);
    }
}
