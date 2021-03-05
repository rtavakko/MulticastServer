#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    server(new MulticastServer(this)),
    adapterName(QNetworkInterface::interfaceFromIndex(0).humanReadableName()),
    incomingIP(QHostAddress::LocalHost),
    incomingPort(7000),
    outgoingIP(QHostAddress::LocalHost),
    outgoingPort(8000),
    mainWidget(new QWidget(this)),
    mainLayout(new QBoxLayout(QBoxLayout::Direction::TopToBottom)),
    adapterList(nullptr),
    adapterRefresh(nullptr),
    textIncomingIP(nullptr),
    textIncomingPort(nullptr),
    textOutgoingIP(nullptr),
    textOutgoingPort(nullptr),
    serverSet(nullptr),
    lError(nullptr),
    lCurrentAdapter(nullptr),
    sendTestMessage(nullptr),
    lIncomingTestMessage(nullptr)
{
    ui->setupUi(this);

    //UI setup
    QSize listSize(250,100);
    QSize buttonSize(250,25);
    QSize labelSize(250,25);

    adapterList = new QListWidget(mainWidget);
    adapterList->setMinimumSize(listSize);

    adapterRefresh = new QPushButton(mainWidget);
    adapterRefresh->setMinimumSize(buttonSize);
    adapterRefresh->setText("Refresh Adapters");

    textIncomingIP = new QLineEdit(mainWidget);
    textIncomingIP->setMinimumSize(labelSize);
    textIncomingIP->setInputMask(QString("999.999.999.999"));
    textIncomingIP->setText(incomingIP.toString());

    textIncomingPort = new QLineEdit(mainWidget);
    textIncomingPort->setMinimumSize(labelSize);
    textIncomingPort->setInputMask(QString("9999"));
    textIncomingPort->setText(QString("%1").arg(incomingPort));

    textOutgoingIP = new QLineEdit(mainWidget);
    textOutgoingIP->setMinimumSize(labelSize);
    textOutgoingIP->setInputMask(QString("999.999.999.999"));
    textOutgoingIP->setText(outgoingIP.toString());

    textOutgoingPort = new QLineEdit(mainWidget);
    textOutgoingPort->setMinimumSize(labelSize);
    textOutgoingPort->setInputMask(QString("9999"));
    textOutgoingPort->setText(QString("%1").arg(outgoingPort));

    serverSet = new QPushButton(mainWidget);
    serverSet->setMinimumSize(buttonSize);
    serverSet->setText("Apply");

    lError = new QLabel(mainWidget);
    lError->setMinimumSize(labelSize);
    lError->setText(QString("Error: None"));

    lCurrentAdapter = new QLabel(mainWidget);
    lCurrentAdapter->setMinimumSize(labelSize);
    lCurrentAdapter->setText(QString("Current adapter: %1").arg(adapterName));

    sendTestMessage = new QPushButton(mainWidget);
    sendTestMessage->setMinimumSize(buttonSize);
    sendTestMessage->setText("Send test message");

    lIncomingTestMessage = new QLabel(mainWidget);
    lIncomingTestMessage->setMinimumSize(labelSize);
    lIncomingTestMessage->setText(QString("Incoming test message: None"));

    QLabel* lList = new QLabel(QString("Network adapters:"),mainWidget);
    lList->setMinimumSize(labelSize);
    mainLayout->addWidget(lList,-1,Qt::AlignCenter);
    mainLayout->addWidget(adapterList,-1,Qt::AlignCenter);
    mainLayout->addWidget(adapterRefresh,-1,Qt::AlignCenter);

    QLabel* lIncomingIP = new QLabel(QString("Incoming IP:"),mainWidget);
    lIncomingIP->setMinimumSize(labelSize);
    mainLayout->addWidget(lIncomingIP,-1,Qt::AlignCenter);
    mainLayout->addWidget(textIncomingIP,-1,Qt::AlignCenter);
    QLabel* lIncomingPort = new QLabel(QString("Incoming port:"),mainWidget);
    lIncomingPort->setMinimumSize(labelSize);
    mainLayout->addWidget(lIncomingPort,-1,Qt::AlignCenter);
    mainLayout->addWidget(textIncomingPort,-1,Qt::AlignCenter);
    QLabel* lOutgoingIP = new QLabel(QString("Outgoing IP:"),mainWidget);
    lOutgoingIP->setMinimumSize(labelSize);
    mainLayout->addWidget(lOutgoingIP,-1,Qt::AlignCenter);
    mainLayout->addWidget(textOutgoingIP,-1,Qt::AlignCenter);
    QLabel* lOutgoingPort = new QLabel(QString("Outgoing port:"),mainWidget);
    lOutgoingPort->setMinimumSize(labelSize);
    mainLayout->addWidget(lOutgoingPort,-1,Qt::AlignCenter);
    mainLayout->addWidget(textOutgoingPort,-1,Qt::AlignCenter);

    mainLayout->addWidget(serverSet,-1,Qt::AlignCenter);

    mainLayout->addWidget(lError,-1,Qt::AlignCenter);
    mainLayout->addWidget(lCurrentAdapter,-1,Qt::AlignCenter);

    mainLayout->addWidget(sendTestMessage,-1,Qt::AlignCenter);
    mainLayout->addWidget(lIncomingTestMessage,-1,Qt::AlignCenter);

    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    //UI connections

    //Update list of network adapters
    QObject::connect(adapterRefresh,&QPushButton::clicked,this,[=]()
    {
        QStringList adapters;
        server->refreshNetworkAdapters(adapters);

        adapterList->clear();
        adapterList->addItems(adapters);
    });

    //Update server settings
    QObject::connect(serverSet,&QPushButton::clicked,this,[=]()
    {
        if(adapterList->currentItem())
            adapterName = adapterList->currentItem()->text();

        incomingIP = QHostAddress(textIncomingIP->text());
        incomingPort = static_cast<unsigned int>(textIncomingPort->text().toInt());

        outgoingIP = QHostAddress(textOutgoingIP->text());
        outgoingPort = static_cast<unsigned int>(textOutgoingPort->text().toInt());

        server->setNetworkAdapter(adapterName,
                                  incomingIP,
                                  incomingPort,
                                  outgoingIP,
                                  outgoingPort);
    });

    QObject::connect(server,&MulticastServer::settingError,this,[=](QString error)
    {
        lError->setText(QString("Error: %1").arg(error));
    });
    QObject::connect(server,&MulticastServer::adapterUpdated,this,[=](QString adapterName)
    {
        lCurrentAdapter->setText(QString("Current adapter: %1").arg(adapterName));
    });

    //Message send / receive
    QObject::connect(sendTestMessage,&QPushButton::clicked,this,[=]()
    {
        emit sendMessage(QString("%1:%2:%3").arg(adapterName).arg(incomingIP.toString()).arg(incomingPort));
    });

    QObject::connect(this,&MainWindow::sendMessage,server,&MulticastServer::sendMessage);

    QObject::connect(server,&MulticastServer::receivedMessage,this,[=](QString message)
    {
        lIncomingTestMessage->setText(QString("Incoming test message: @%1: %2").arg(QTime::currentTime().toString()).arg(message));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
