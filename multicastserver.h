#ifndef MULTICASTSERVER_H
#define MULTICASTSERVER_H

#include <QObject>

#include <QUdpSocket>
#include <QNetworkInterface>
#include <QNetworkDatagram>

#include <QDataStream>

class MulticastServer : public QObject
{
    Q_OBJECT
public:
    explicit MulticastServer(QObject *parent = nullptr,
                             const QNetworkInterface& adapter = QNetworkInterface(),
                             const QHostAddress& incomingIP = QHostAddress::LocalHost,
                             unsigned int incomingPort = 7000,
                             const QHostAddress& outgoingIP = QHostAddress::LocalHost,
                             unsigned int outgoingPort = 8000);

    //Static methods for network adapter and IP check
    static void refreshNetworkAdapters(QStringList& list);
    static QNetworkInterface getNetworkAdapter(QString adapterName);

    static bool isValidNetworkAdapter(const QNetworkInterface& adapter);
    static bool isValidNetworkAdapter(QString adapterName);

    static bool isValidIPAddress(const QHostAddress& IPAddress);

    //IP / port access
    const QHostAddress& getIncomingIPAddress() const;
    const QHostAddress& getOutgoingIPAddress() const;

    unsigned int getIncomingPort() const;
    unsigned int getOutgoingPort() const;

    bool setIncomingIPAddress(const QHostAddress& incomingIP);
    bool setOutgoingIPAddress(const QHostAddress& outgoingIP);

    bool setIncomingPort(unsigned int incomingPort);
    bool setOutgoingPort(unsigned int outgoingPort);

    //Network adapter access
    bool setNetworkAdapter(const QNetworkInterface& adapter,
                           const QHostAddress& incomingIP,
                           unsigned int incomingPort,
                           const QHostAddress& outgoingIP,
                           unsigned int outgoingPort);

    bool setNetworkAdapter(QString adapterName,
                           const QHostAddress& incomingIP,
                           unsigned int incomingPort,
                           const QHostAddress& outgoingIP,
                           unsigned int outgoingPort);

protected:
    void initialize();

    void deInitialize();

    QUdpSocket* UDPSocket;

    QNetworkInterface networkAdapter;

    QHostAddress inputIPAddress;
    QHostAddress outputIPAddress;

    unsigned int inputPort;
    unsigned int outputPort;

signals:
    void receivedMessage(QString message);

    void adapterUpdated(QString name);
    void settingError(QString error);

public slots:
    void processData();

    void sendMessage(QString message);
};

#endif // MULTICASTSERVER_H
