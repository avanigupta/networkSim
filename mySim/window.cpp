#include "window.h"

QDataStream &operator <<(QDataStream &out, const MyDevice &device)
{
    int x1, y1, x2, y2, type;

    device.area.getCoords(&x1, &y1, &x2, &y2);

    switch(device.type)
    {
    case MyDevice::Server:
        type = 0;
        break;
    case MyDevice::Node:
        type = 1;
        break;
    case MyDevice::Mobile:
        type = 2;
        break;
    }

    out << type <<
           x1 << y1 << x2 << y2 <<
           device.range << device.name << device.services;

    return out;
}

QDataStream &operator >>(QDataStream &in, MyDevice &device)
{
    int x1, y1, x2, y2, type;

    in >> type >>
          x1 >> y1 >> x2 >> y2 >>
          device.range >> device.name >> device.services;

    switch(type)
    {
    case 0:
        device.type = MyDevice::Server;
        break;
    case 1:
        device.type = MyDevice::Node;
        break;
    case 2:
        device.type = MyDevice::Mobile;
        break;
    }

    device.area.setCoords(x1, y1, x2, y2);

    return in;
}

Window::Window()
{
    magic = 0x1911;
    renderArea = new RenderArea;

    QPushButton *servButton = new QPushButton("Add a Server"),
            *locButton = new QPushButton("Add a Local Node"),
            *mobButton = new QPushButton("Add a Mobile Node"),
            *delButton = new QPushButton("Delete a device"),
            *fontButton = new QPushButton("Select Font"),
            *clrButton = new QPushButton("Clear All"),
            *saveButton = new QPushButton("Save Network"),
            *loadButton = new QPushButton("Load Network"),
            *abtButton = new QPushButton("About");

    servButton->setIcon(QIcon(":/images/server.png"));
    locButton->setIcon(QIcon(":/images/node.png"));
    mobButton->setIcon(QIcon(":/images/mobile.png"));
    delButton->setIcon(QIcon(":/images/delete.png"));
    fontButton->setIcon(QIcon(":/images/font.png"));
    clrButton->setIcon(QIcon(":/images/clear.png"));
    saveButton->setIcon(QIcon(":/images/save.png"));
    loadButton->setIcon(QIcon(":/images/load.png"));
    abtButton->setIcon(QIcon(":/images/about.png"));

    connect(servButton, SIGNAL(clicked()), this, SLOT(servClicked()));
    connect(locButton, SIGNAL(clicked()), this, SLOT(locClicked()));
    connect(mobButton, SIGNAL(clicked()), this, SLOT(mobClicked()));
    connect(delButton, SIGNAL(clicked()), this, SLOT(delClicked()));
    connect(fontButton, SIGNAL(clicked()), this, SLOT(fontClicked()));
    connect(clrButton, SIGNAL(clicked()), this, SLOT(clrClicked()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveClicked()));
    connect(loadButton, SIGNAL(clicked()), this, SLOT(loadClicked()));
    connect(abtButton, SIGNAL(clicked()), this, SLOT(abtClicked()));

    QHBoxLayout *buttons = new QHBoxLayout;
    buttons->addWidget(servButton);
    buttons->addWidget(locButton);
    buttons->addWidget(mobButton);
    buttons->addWidget(delButton);
    buttons->addWidget(clrButton);
    buttons->addWidget(fontButton);
    buttons->addWidget(saveButton);
    buttons->addWidget(loadButton);
    buttons->addWidget(abtButton);

    QLabel *heading = new QLabel("Start putting devices here...");
    heading->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(heading, 0, 0, 1, -1);
    mainLayout->addWidget(renderArea, 1, 0, 1, -1);
    mainLayout->addLayout(buttons, 2, 0, 1, -1);

    setLayout(mainLayout);
    setWindowTitle(tr("mySimulator"));
    setWindowIcon(QIcon(":/images/server.png"));
}

void Window::closeEvent(QCloseEvent *)
{
    if(renderArea->devCount() == 0)
        return;

    QMessageBox::StandardButton reply = QMessageBox::information(this, "Save?", "Save the Network?",
                                                                 QMessageBox::Save, QMessageBox::Discard);
    if(reply == QMessageBox::Save)
        saveClicked();
}

void Window::servClicked()
{
    renderArea->setOk();
    renderArea->setDel(false);
    renderArea->setChanged();
    mode = Window::Server;
    handleIt();
}

void Window::locClicked()
{
    renderArea->setOk();
    renderArea->setDel(false);
    renderArea->setChanged();
    mode = Window::Node;
    handleIt();
}

void Window::mobClicked()
{
    renderArea->setOk();
    renderArea->setDel(false);
    renderArea->setChanged();
    mode = Window::Mobile;
    handleIt();
}

void Window::delClicked()
{
    renderArea->setDel(true);
    renderArea->setChanged();
}

void Window::fontClicked()
{
    bool ok;
    renderArea->setFont(QFontDialog::getFont(&ok, QFont("Times", 10), this));
}

void Window::clrClicked()
{
    renderArea->clear();
    setWindowTitle("mySimulator");
}

void Window::saveClicked()
{
    QString path = QFileDialog::getSaveFileName(this, "Select the file to save",
                                                QDir::homePath(), "mySim Networks (*.msn)");
    if(path == "")
        return;

    QFile file(path);
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_0);
    file.open(QIODevice::WriteOnly);

    out << magic;
    QList<MyDevice> devices = renderArea->getDevices();
    foreach(MyDevice device, devices)
    {
        out << device;
    }
    file.close();

    setWindowTitle(QFileInfo(file).baseName() + " - mySimulator");
}

void Window::loadClicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Select the file to save",
                                                QDir::homePath(), "mySim Networks (*.msn)");

    if(path == "")
        return;

    QFile file(path);
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_0);
    file.open(QIODevice::ReadOnly);

    in >> magic;
    if(magic != 0x1911)
    {
        QMessageBox::information(this, "Error!", "Invalid mySim Network File!");
        file.close();
        return;
    }

    QList<MyDevice> devices;
    MyDevice device;
    while(!in.atEnd())
    {
        in >> device;
        devices.append(device);
    }
    file.close();

    setWindowTitle(QFileInfo(file).baseName() + " - mySimulator");
    renderArea->setDevices(devices);
}

void Window::abtClicked()
{
    QMessageBox::information(this, "mySimulator", "Version:: 1.1\nXena says YO!!!");
    QMessageBox::aboutQt(this, "About Qt");
}

void Window::handleIt()
{
    MyDevice device;

    switch(mode)
    {
    case Window::Server:
        device.type = MyDevice::Server;
        break;
    case Window::Node:
        device.type = MyDevice::Node;
        break;
    case Window::Mobile:
        device.type = MyDevice::Mobile;
        break;
    }
    renderArea->addDevice(device);
}
