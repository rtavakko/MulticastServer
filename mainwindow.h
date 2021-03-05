#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>

#include <QTime>

#include <multicastserver.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //Multicast server
    MulticastServer* server;

    QString adapterName;

    QHostAddress incomingIP;
    unsigned int incomingPort;

    QHostAddress outgoingIP;
    unsigned int outgoingPort;

    //For central widget
    QBoxLayout* mainLayout;
    QWidget* mainWidget;

    //Other widgets
    QListWidget* adapterList;
    QPushButton* adapterRefresh;

    QLineEdit* textIncomingIP;
    QLineEdit* textIncomingPort;
    QLineEdit* textOutgoingIP;
    QLineEdit* textOutgoingPort;

    QPushButton* serverSet;

    QLabel* lError;
    QLabel* lCurrentAdapter;

    QPushButton* sendTestMessage;
    QLabel* lIncomingTestMessage;

signals:
    void sendMessage(QString message);
};

#endif // MAINWINDOW_H
