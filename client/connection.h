#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    enum State {Connecting,Ready,Sending,Reading};
    enum SetupState {JustConnected,RecivedRelocationDir};

    Connection();
    SetupState getSetupState();
    State getState();
    void setState(State state);
    void setSetupState(SetupState setupState);

private:
    State state;
    SetupState setupState;

};

#endif // CONNECTION_H
