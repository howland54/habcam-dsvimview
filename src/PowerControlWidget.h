#ifndef POWERCONTROLWIDGET_H
#define POWERCONTROLWIDGET_H

#include <QtWidgets>
#define MAX_NUMBER_OF_SWITCHES      8
#include "qledindicator.h"
#include "PowerButton.h"

#define NCD_IO_ERROR_NONE      0
#define NCD_IO_ERROR_CHECKSUM  1
#define NCD_IO_ERROR_TIMEOUT   2

typedef struct
{
    char    *attachedTo;
    int     relayNumber;
}switchType;


class PowerControlWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerControlWidget(QWidget *parent = nullptr);
    void     addAButton(switchType theSwitch);
    void     drawLayout();
    void     setRelayStates(const int8_t *theRelayStates);
    void     setErrorStates(bool  checksumError, int errorCode);

private:
    QGroupBox      *powerControlGroupBox;
    PowerButton    *powerButton[MAX_NUMBER_OF_SWITCHES];
    int            relayNumber[MAX_NUMBER_OF_SWITCHES];
    int            numberOfRelays;
    QPushButton     *clearErrorsButton;
    QLedIndicator   *errorLED;
    QLedIndicator   *checksumLED;
    QLabel          *errorLabel;
    QLabel          *checksumLabel;
    bool            checksumErrorSet;
    bool            timeoutErrorSet;

private slots:
    void            clearErrorsButtonClicked();

signals:
    void            powerButtonClicked(int whichRelay, int theState);

};

#endif // POWERCONTROLWIDGET_H
