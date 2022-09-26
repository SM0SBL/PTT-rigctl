// COPYRIGHT AND PERMISSION NOTICE

// Copyright (c) 2020 - 2021, Björn Langels, <sm0sbl@langelspost.se>
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QSerialPort>
#include <QBuffer>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define QT_NO_DEBUG_OUTPUT

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    // Buttons and fields
    void on_keyButton_released();
    void on_keyButton_pressed();
    void on_KeyPortName_textChanged(const QString &arg1);
    void on_keyPortInvert_stateChanged(int arg1);
    void on_ShowComPortList_clicked();
    void on_keyPortDevice_currentIndexChanged(const QString &arg1);
    void on_ConnectToKeyNetwork_clicked();
    void on_ConnectToKeyPort_clicked();

    // Functions
    void msEvent();
    void readyReadKeyTcp();
    void readyReadKeySerial();
    void KeyUp();
    void KeyDown();

private:
    Ui::MainWindow *ui;
    void loadSettings();
    void saveSettings();
    void sort(QList<QSerialPortInfo> list, int column, Qt::SortOrder order = Qt::AscendingOrder);
    void updateComPortList();

    QString SettingsPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString SettingsFile = "rigctl_ptt.ini";
    QTcpSocket *tcpKeySocket;
    QSerialPort *keySerialPort;
    QByteArray keyPort;
    quint32 hostPort;
    QByteArray hostAddress;
    bool KeyIsDownLast;
    bool keyPortStatus;
    bool keyPortInverted = false;
};
#endif // MAINWINDOW_H
