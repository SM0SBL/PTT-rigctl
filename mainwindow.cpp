// COPYRIGHT AND PERMISSION NOTICE

// Copyright (c) 2020 - 2021, Bjorn Langels, <sm0sbl@langelspost.se>
// All rights reserved.

// Permission to use, copy, modify, and distribute this software for any purpose
// with or without fee is hereby granted, provided that the above copyright
// notice and this permission notice appear in all copies.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

// Except as contained in this notice, the name of a copyright holder shall not
// be used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization of the copyright holder.

#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QBuffer>
#include <QSettings>
#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Setup a program Icon
    setWindowIcon(QIcon(":/icon/Images/PTT.ico"));

    // Setup Key TCP connection
    keyPortStatus = false;
    tcpKeySocket = new QTcpSocket(this);
    tcpKeySocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(tcpKeySocket,
            SIGNAL(readyRead()),
            this,
            SLOT(readyReadKeyTcp()));

    // Setup serial port for key up/down detection
    keySerialPort = new QSerialPort(this);
    connect(keySerialPort,
            SIGNAL(readyRead()),
            this,
            SLOT(readyReadKeySerial()));

    // Setupt a 100 millisecond timer to poll key serial port regularely
    QTimer* timer = new QTimer(this);
    timer->connect(timer,
                   SIGNAL(timeout()),
                   this,
                   SLOT(msEvent()));
    timer->start(100); //, Qt::PreciseTimer, timeout());

    // initialize the list of available COM-ports
    updateComPortList();

    // Setup colors to key connect buttons
    ui->ConnectToKeyNetwork->setStyleSheet("background-color: red;");
    ui->ConnectToKeyPort->setStyleSheet("background-color: red;");
    ui->keyButton->setStyleSheet("background-color: lightgray;");

    // Load settings from file system from last open session
    loadSettings();
}

MainWindow::~MainWindow()
{
    // Save all settings from this session before closing
    saveSettings();
    tcpKeySocket->close();
    keySerialPort->close();
    delete ui;
}

void MainWindow::updateComPortList()
{
    QList<QSerialPortInfo> comDevices = QSerialPortInfo::availablePorts();
    ui->keyPortDevice->clear();
    this->ui->keyPortDevice->addItem("Select Key port");
    foreach (QSerialPortInfo i, comDevices) {
        this->ui->keyPortDevice->addItem(i.portName()+" "+i.description());
    }
}

void MainWindow::loadSettings() {

    QSettings settings(SettingsPath + "/" + SettingsFile, QSettings::IniFormat);
    settings.beginGroup("MAIN");
    quint32 val = settings.value("keyNetPort", "").toUInt();
    ui->keyNetPort->setValue(val);
    QString str = settings.value("keyIP", "").toString();
    ui->keyIP->setText(str);
    str = settings.value("KeyPort", "").toString();
    ui->KeyPortName->setText(str);
    str = settings.value("KeyInput", "").toString();
    if ( !QString::compare(str, "CTS") ) {
        ui->KeyOnCTS->setChecked(true);
        ui->KeyOnDSR->setChecked(false);
    } else if ( !QString::compare(str, "DSR") ) {
        ui->KeyOnCTS->setChecked(false);
        ui->KeyOnDSR->setChecked(true);
    }
    str = settings.value("KeyInvert", "").toString();
    if ( !QString::compare(str, "Inverted") ) {
        ui->keyPortInvert->setChecked(true);
    } else {
        ui->keyPortInvert->setChecked(false);
    }
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings settings(SettingsPath + "/" + SettingsFile, QSettings::IniFormat);
    settings.beginGroup("MAIN");
    settings.setValue("keyNetPort",  ui->keyNetPort->value());
    settings.setValue("keyIP",  ui->keyIP->text());
    settings.setValue("KeyPort",  ui->KeyPortName->text().toLatin1());
    if ( ui->KeyOnCTS->isChecked() ) {
        settings.setValue("KeyInput",  "CTS");
    } else {
        settings.setValue("KeyInput",  "DSR");
    }
    if ( ui->keyPortInvert->isChecked() ) {
        settings.setValue("KeyInvert",  "Inverted");
    } else {
        settings.setValue("KeyInvert",  "NotInverted");
    }
    settings.endGroup();
}

void MainWindow::msEvent()
{
    if ( keyPortStatus ) {
        bool KeyIsDown = false;
            QSerialPort::PinoutSignals pinoutSignals = keySerialPort->pinoutSignals();
            if ( ui->KeyOnDSR->isChecked() ) {
               KeyIsDown = (pinoutSignals & QSerialPort::DataSetReadySignal ? true : false);
             }
            if ( ui->KeyOnCTS->isChecked() ) {
               KeyIsDown = (pinoutSignals & QSerialPort::ClearToSendSignal ? true : false);
            }
            if ( keyPortInverted )
                KeyIsDown = !KeyIsDown;
            if ( KeyIsDown != KeyIsDownLast ) {
                if ( KeyIsDown ) {
                    qDebug() << "KeyIsDown";
                    KeyDown();
                } else {
                    KeyUp();
                    qDebug() << "KeyIsUp";
                }
                KeyIsDownLast = KeyIsDown;
            }
    }
}

void MainWindow::on_keyButton_pressed()
{
    QStringList s;
        MainWindow::KeyDown();
}

void MainWindow::on_keyButton_released()
{
    MainWindow::KeyUp();
}


void MainWindow::KeyUp()
{
    QByteArray Data;
    Data.append("T 0\n");
    tcpKeySocket->write(Data.data());
    tcpKeySocket->waitForBytesWritten(1);
    ui->keyButton->setStyleSheet("background-color: lightgray;");

}

void MainWindow::KeyDown()
{
    QByteArray Data;
    Data.append("T 1\n");
    tcpKeySocket->write(Data.data());
    tcpKeySocket->waitForBytesWritten(1);
    ui->keyButton->setStyleSheet("background-color: red;");
}

void MainWindow::readyReadKeySerial()
{
    //qDebug() << "readyReadKeySerial() called";
}

    void MainWindow::readyReadKeyTcp()
{
    // when data comes in
    QByteArray buffer;
    char str[100];
    int strcnt = sizeof(str);

    buffer.clear();
    while ( tcpKeySocket->bytesAvailable() ) {
        tcpKeySocket->read(str, 1);
        if ( strcnt-- > 0) {
            buffer.append(str, 1);
        }

    }
    sscanf(buffer.data(), "%s", str);
    if ( !strcmp(str, "RPRT") ) {
        qDebug() << "RPRT received";
    } else {
        qDebug() << "Unknown data received:"<<buffer.data();
    }
}


void MainWindow::on_KeyPortName_textChanged(const QString &arg1)
{
    ui->ConnectToKeyPort->setChecked(false);
    keyPort = ui->KeyPortName->text().toLatin1();
}

void MainWindow::on_ShowComPortList_clicked()
{
    updateComPortList();
}


void MainWindow::on_keyPortInvert_stateChanged(int arg1)
{
    keyPortInverted = (arg1 != 0);
}

void MainWindow::on_keyPortDevice_currentIndexChanged(const QString &arg1)
{
    QString qstr;
    QStringList list = arg1.split(" ");
    qstr = list[0];
    ui->KeyPortName->setText(qstr);

}



void MainWindow::on_ConnectToKeyNetwork_clicked()
{
    if ( tcpKeySocket->openMode() != 0 ) {
        tcpKeySocket->close();
    } else {
        tcpKeySocket->connectToHost(ui->keyIP->text(), ui->keyNetPort->value());
    }
    if ( tcpKeySocket->openMode() != 0 ) {
        ui->ConnectToKeyNetwork->setStyleSheet("background-color: green;");
    } else {
        ui->ConnectToKeyNetwork->setStyleSheet("background-color: red;");
    }
}

void MainWindow::on_ConnectToKeyPort_clicked()
{
    if ( keyPortStatus ) {
        keySerialPort->close();
        keyPortStatus = false;
        ui->ConnectToKeyPort->setStyleSheet("background-color: red;");
    } else {
        keySerialPort->setPortName(ui->KeyPortName->text());
        keySerialPort->setBaudRate(QSerialPort::Baud115200);
        keySerialPort->setParity(QSerialPort::Parity::NoParity);
        keySerialPort->setDataBits(QSerialPort::DataBits::Data8);
        keySerialPort->setStopBits(QSerialPort::StopBits::OneStop);
        keySerialPort->setFlowControl(QSerialPort::FlowControl::UnknownFlowControl);
        keyPortStatus = keySerialPort->open(QSerialPort::OpenModeFlag::ReadWrite);
        if ( keyPortStatus ) {
            keySerialPort->write("The serial port is open!");
            ui->ConnectToKeyPort->setStyleSheet("background-color: green;");
        }
    }

}
