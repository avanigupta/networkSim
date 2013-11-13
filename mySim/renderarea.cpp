#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent) : QWidget(parent)
{
    ok = delDevice = changed = false;
    dev = -1;
    qsrand(time(0));

    connect(this, SIGNAL(jobCompletion(QStringList)), &server, SLOT(updateTable(QStringList)));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));
    timer->start(1000);

    setCursor(Qt::CrossCursor);
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(640, 359);
}

void RenderArea::timeOut()
{
    int i;

    for(i = 0; i <nodeJobs.length(); i++)
    {
        int left = nodeJobs[i].split(":")[4].toInt();
        int total = nodeJobs[i].split(":")[3].toInt();

        if(left < total)
        {
            devices[indices[nodeJobs[i].split(":")[0]]].busy = true;
            devices[indices[nodeJobs[i].split(":")[1]]].busy = true;
            nodeJobs[i] = nodeJobs[i].section(':', 0, 3) + ":" + QString::number(left + 1);
        }
        else
        {
            nodeJobs.removeAt(i);
            clearBusy();
        }
    }

    emit jobCompletion(nodeJobs);
}

int RenderArea::tryAddJob(QString job, int dest, bool &net)
{
    int i, j;
    QList<MyDevice> devs;

    devices[dest].marked = true;

    for(i = 0; i < devices.length(); i++)
    {
        if(i != dest && devices[i].isInRangeOf(devices[dest]) && !devices[i].marked)
            devs.append(devices[i]);
    }

    if(devs.length() > 0)
        net = true;

    for(i = 0; i < devs.length(); i++)
    {
        if(devs[i].type != MyDevice::Mobile)
        {
            j = indices[devs[i].name];

            if(!devices[j].services.isEmpty() &&
                    (devices[j].services + ",").split(',', QString::SkipEmptyParts).
                    contains(job, Qt::CaseInsensitive))
                return j;
            else
            {
                int ret = tryAddJob(job, j, net);

                if(ret == -1 && i < devs.length())
                    continue;
                else
                    return ret;
            }
        }
    }

    return -1;
}

void RenderArea::paintEvent(QPaintEvent *)
{
    if(!ok)
        return;

    if(newDevice)
    {
        QString name;

        device.area = QRect((clickPoint - QPoint(16, 16)), QSize(32, 32));

        do
        {
            name = QInputDialog::getText(this, "Input Value", "Enter the name of the device");
            if(name != "")
            {
                if(indices.contains(name))
                    QMessageBox::information(this, "Error", "Name already exits!");
            }
        } while(indices.contains(name));

        device.name = name;
        device.range = QInputDialog::getInt(this, "Input Value", "Enter the Range");
        device.services = "";
        devices.append(device);
        indices[device.name] = devices.length() - 1;
    }
    newDevice = false;

    if(delDevice)
    {
        for(int i = 0; i < devices.length(); i++)
        {
            if(devices[i].area.contains(clickPoint))
            {
                indices.remove(devices[i].name);
                devices.removeAt(i);
                break;
            }
        }
    }
    delDevice = false;

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing, true);

    foreach(MyDevice dev, devices)
    {
        switch(dev.type)
        {
        case MyDevice::Server:
            pixmap.load(":/images/server.png");
            break;
        case MyDevice::Node:
            pixmap.load(":/images/node.png");
            break;
        case MyDevice::Mobile:
            pixmap.load(":/images/mobile.png");
            break;
        }

        painter.drawPixmap(dev.area, pixmap);
        QString text = dev.name;
        if(dev.busy)
            text += " (busy)";
        painter.drawText(dev.area.topLeft() + QPoint(0, 42), text);
        painter.drawEllipse(dev.area.center(), dev.range, dev.range);
        pixmap.detach();
    }
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    int i;
    clickPoint = event->pos();

    if(event->button() == Qt::LeftButton)
    {
        if(!newDevice && !delDevice)
        {
            for(i = 0; i < devices.length(); i++)
            {
                if(devices[i].area.contains(clickPoint))
                {
                    dev = i;
                    return;
                }
            }
        }
        else
            update();
    }
    else
    {
        for(i = 0; i < devices.length(); i++)
        {
            if(devices[i].area.contains(clickPoint))
            {
                QString input;

                switch(devices[i].type)
                {
                case MyDevice::Server:
                    server.show();
                    break;
                case MyDevice::Node:
                    input = QInputDialog::getText(this, "Input Required", "Enter service(s) list separated by commas",
                                                  QLineEdit::Normal, devices[i].services);
                    if(input != "")
                        devices[i].services = input;
                    break;
                case MyDevice::Mobile:
                    bool net = false;
                    clearMarkings();

                    input = QInputDialog::getText(this, "Input Required", "Enter service needed");
                    if(input != "")
                        devices[i].services = input;

                    int found = tryAddJob(devices[i].services, i, net);
                    if(found != -1)
                    {
                        QString job = devices[found].name + ":" + devices[i].name + ":"
                                + devices[i].services + ":" + QString::number(qrand() % 60) + ":"
                                + QString::number(0);
                        devices[i].busy = devices[found].busy = true;
                        nodeJobs.append(job);
                    }

                    if(!net)
                        QMessageBox::information(this, "Error!", "No network in range!");
                    else if(found == -1)
                        QMessageBox::information(this, "Error!", "No such service found!");
                    break;
                }
                return;
            }
        }
    }
}

void RenderArea::mouseReleaseEvent(QMouseEvent *)
{
    if(dev != -1)
        dev = -1;
}

void RenderArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    QString name;
    int i;

    for(i = 0; i < devices.length(); i++)
    {
        if(devices[i].area.contains(event->pos()))
        {
            do
            {
                name = QInputDialog::getText(this, "Input Value", "Enter the name of the device",
                                             QLineEdit::Normal, devices[i].name);
                if(devices[i].name == name)
                    break;
                if(indices.contains(name))
                        QMessageBox::information(this, "Error", "Name already exits!");
            } while(indices.contains(name));

            indices.remove(devices[i].name);
            indices[name] = i;
            devices[i].name = name;
            devices[i].range = QInputDialog::getInt(this, "Input Value", "Enter the range", devices[i].range);
            break;
        }
    }
}

void RenderArea::mouseMoveEvent(QMouseEvent *event)
{
    if(dev != -1 && !devices[dev].busy)
    {
        if(devices[dev].type == MyDevice::Server && nodeJobs.length() > 0)
            QMessageBox::information(this, "Error!", "Can't move a server while in operation!");
        else
        {
            devices[dev].area = QRect((event->pos() - QPoint(16, 16)), QSize(32, 32));
            update();
        }
    }
}

void RenderArea::addDevice(MyDevice device)
{
    this->device = device;
    newDevice = true;
}

void RenderArea::clear()
{
    devices.clear();
    update();
}

QList<MyDevice> RenderArea::getDevices()
{
    return devices;
}

void RenderArea::setDevices(QList<MyDevice> list)
{
    ok = true;
    newDevice = false;
    devices = list;

    indices.clear();
    for(int i = 0; i < devices.length(); i++)
        indices[devices[i].name] = i;

    update();
}

int RenderArea::devCount()
{
    return devices.length();
}

void RenderArea::setOk()
{
    ok = true;
}

void RenderArea::setDel(bool value)
{
    delDevice = value;
}

void RenderArea::setChanged()
{
    changed = true;
}

void RenderArea::clearMarkings()
{
    for(int i = 0; i < devices.length(); i++)
        devices[i].marked = false;
}

void RenderArea::clearBusy()
{
    for(int i = 0; i < devices.length(); i++)
        devices[i].busy = false;
    update();
}
