#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    enum State {Connecting,Ready,Sending,Reading};
    enum SetupState {JustConnected,Finished};

    Connection();
    SetupState getSetupState();
    State getState();
    void setState(State state);
    void setSetupState(SetupState setupState);
    void setIV(QByteArray IV);
    QByteArray getIV();

private:
    State state;
    SetupState setupState;
    QByteArray IV;

};

#endif // CONNECTION_H
