#pragma once

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>

class ThreadWorker : public QObject
{
    Q_OBJECT

public:
    explicit ThreadWorker(qintptr ID) : socketDescriptor_(ID) {}

public slots:
    void start();
    void readyRead();
    void disconnected();
    void sendGameState(const std::string& state);

signals:
    void error(QTcpSocket::SocketError socketerror);
    void connectionLost(const int sockId);
    void moveCommand(int sockId, float x, float y);

private:
    QTcpSocket* socket_;
    QDataStream in;
    qintptr socketDescriptor_;
};