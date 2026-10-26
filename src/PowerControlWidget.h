#ifndef POWERCONTROLWIDGET_H
#define POWERCONTROLWIDGET_H

#include <QtWidgets>
#define MAX_NUMBER_OF_SWITCHES      8

#include "PowerButton.h"

typedef struct
{
    bool    desiredState;
    bool    actualState;
    char    *attachedTo;
    int     relayNumber;
}switchType;


class PowerControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerControlWidget(QWidget *parent = nullptr);
    void     addAButton(switchType theSwitch, QWidget *myParent);
    void     drawLayout();
    void     setRelayStates(const int8_t *theRelayStates);

private:
    QGroupBox      *powerControlGroupBox;
    PowerButton    *powerButton[MAX_NUMBER_OF_SWITCHES];
    int            relayNumber[MAX_NUMBER_OF_SWITCHES];
    int            numberOfRelays;
    QWidget        *myParent;

signals:


};

#endif // POWERCONTROLWIDGET_H
