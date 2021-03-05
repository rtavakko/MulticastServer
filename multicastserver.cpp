#include "multicastserver.h"

MulticastServer::MulticastServer(QObject *parent,
                                 const QNetworkInterface &adapter,
                                 const QHostAddress &incomingIP,
                                 unsigned int incomingPort,
                                 const QHostAddress &outgoingIP,
                                 unsigned int outgoingPort) :
    QObject(parent),
    networkAdapter(adapter),
    UDPSocket(new QUdpSocket(this)),
    inputIPAddress(incomingIP),
    inputPort(incomingPort),
    outputIPAddress(outgoingIP),
    outputPort(outgoingPort)
{
    QObject::connect(UDPSocket,&QUdpSocket::readyRead,this,&MulticastServer::processData);
    initialize();
}

void MulticastServer::refreshNetworkAdapters(QStringList &list)
{
    //Clear the current list
    list.clear();

    //Go through all network interfaces and filter out usable ones
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if(MulticastServer::isValidNetworkAdapter(interface))
            list.push_back(interface.humanReadableName());
    }
}

QNetworkInterface MulticastServer::getNetworkAdapter(QString adapterName)
{
    //Find a valid network interface of the given name if it exists
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
    {
        if(MulticastServer::isValidNetworkAdapter(interface))
        {
            bool match = (interface.humanReadableName().compare(adapterName) == 0);
            if(match)
                return interface;
        }
    }

    return QNetworkInterface();
}

bool MulticastServer::isValidNetworkAdapter(const QNetworkInterface &adapter)
{
    if(!adapter.isValid())
        return false;

    if(!(adapter.humanReadableName().toUpper().contains("BLUETOOTH") || adapter.humanReadableName().toUpper().contains("VM")))
        return true;

    return false;
}

bool MulticastServer::isValidNetworkAdapter(QString adapterName)
{
    return MulticastServer::getNetworkAdapter(adapterName).isValid();
}

bool MulticastServer::isValidIPAddress(const QHostAddress &IPAddress)
{
    bool valid = (!IPAddress.isNull());
    return valid;
}

const QHostAddress &MulticastServer::getIncomingIPAddress() const
{
    return inputIPAddress;
}

const QHostAddress &MulticastServer::getOutgoingIPAddress() const
{
    return outputIPAddress;
}

unsigned int MulticastServer::getIncomingPort() const
{
    return inputPort;
}

unsigned int MulticastServer::getOutgoingPort() const
{
    return outputPort;
}

bool MulticastServer::setIncomingIPAddress(const QHostAddress &incomingIP)
{
    if(!MulticastServer::isValidIPAddress(incomingIP))
        return false;

    inputIPAddress = incomingIP;

    initialize();
    return true;
}

bool MulticastServer::setOutgoingIPAddress(const QHostAddress &outgoingIP)
{
    if(!MulticastServer::isValidIPAddress(outgoingIP))
        return false;

    outputIPAddress = outgoingIP;

    return true;
}

bool MulticastServer::setIncomingPort(unsigned int incomingPort)
{
    if(incomingPort == 0)
        return false;

    inputPort = incomingPort;

    initialize();
    return true;
}

bool MulticastServer::setOutgoingPort(unsigned int outgoingPort)
{
    if(outgoingPort == 0)
        return false;

    outputPort = outgoingPort;
    return true;
}

bool MulticastServer::setNetworkAdapter(const QNetworkInterface &adapter, const QHostAddress &incomingIP, unsigned int incomingPort, const QHostAddress &outgoingIP, unsigned int outgoingPort)
{
    if(!MulticastServer::isValidNetworkAdapter(adapter))
    {
        emit settingError(QString("Invalid adapter"));
        return false;
    }

    if(!MulticastServer::isValidIPAddress(incomingIP) || !MulticastServer::isValidIPAddress(outgoingIP))
    {
        emit settingError(QString("Invalid IP"));
        return false;
    }

    if(incomingPort == 0 || outgoingPort == 0)
    {
        emit settingError(QString("Invalid port"));
        return false;
    }

    networkAdapter = adapter;

    inputIPAddress = incomingIP;
    inputPort = incomingPort;

    outputIPAddress = outgoingIP;
    outputPort = outgoingPort;

    initialize();
    return true;
}

bool MulticastServer::setNetworkAdapter(QString adapterName, const QHostAddress &incomingIP, unsigned int incomingPort, const QHostAddress &outgoingIP, unsigned int outgoingPort)
{
    //Find a valid network interface of the given name if it exists
    QNetworkInterface interface = MulticastServer::getNetworkAdapter(adapterName);
    return setNetworkAdapter(interface,
                             incomingIP,
                             incomingPort,
                             outgoingIP,
                             outgoingPort);
}

void MulticastServer::initialize()
{
    //Unbind the socket if currently bound
    if(UDPSocket->state() == QAbstractSocket::BoundState)
        UDPSocket->close();

    //Bind the socket to the specified input address and port
    UDPSocket->bind(inputIPAddress,
                    static_cast<quint16>(inputPort));

    UDPSocket->setMulticastInterface(networkAdapter);

    emit adapterUpdated(UDPSocket->multicastInterface().humanReadableName());
}

void MulticastServer::deInitialize()
{
    //Unbind the socket if currently bound
    if(UDPSocket->state() == QAbstractSocket::BoundState)
        UDPSocket->close();
}

void MulticastServer::processData()
{
    while (UDPSocket->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = UDPSocket->receiveDatagram();

        //Process valid packets
        if(!datagram.isNull())
            emit receivedMessage(QString::fromStdString(datagram.data().toStdString()));
    }
}

void MulticastServer::sendMessage(QString message)
{
    //Send single message
    QNetworkDatagram datagram;

    datagram.setData(QByteArray::fromStdString(message.toStdString()));
    datagram.setInterfaceIndex(UDPSocket->multicastInterface().index());
    datagram.setDestination(outputIPAddress,
                            static_cast<quint16>(outputPort));

    UDPSocket->writeDatagram(datagram);
}
