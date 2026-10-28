#include "PowerControlWidget.h"

PowerControlWidget::PowerControlWidget(QWidget *parent) : QWidget(parent)
{
    powerControlGroupBox = new QGroupBox("power control");
    numberOfRelays = 0;
    errorLED = new QLedIndicator(this);
    checksumLED = new QLedIndicator(this);
    errorLabel = new QLabel("Timeout Error: ");
    checksumLabel = new QLabel("Checksum Error: ");
    clearErrorsButton = new QPushButton("Clear Errors");
    connect(clearErrorsButton, SIGNAL(clicked()),this, SLOT(clearErrorsButtonClicked()));
    checksumErrorSet = false;
    timeoutErrorSet = false;


}

void PowerControlWidget::addAButton(switchType theSwitch)
{
    if(numberOfRelays >= MAX_NUMBER_OF_SWITCHES)
    {
        qWarning() << "PowerControlWidget: no room for relay" << theSwitch.relayNumber << theSwitch.attachedTo;
        return;
    }
    powerButton[numberOfRelays] = new PowerButton();
    // pass the clicks on, the owner connects to our powerButtonClicked
    connect(powerButton[numberOfRelays],SIGNAL(powerButtonClicked(int,int)),this ,SIGNAL(powerButtonClicked(int,int)));
    powerButton[numberOfRelays]->setButtonLabel(theSwitch.attachedTo);
    powerButton[numberOfRelays]->setButtonNumber(theSwitch.relayNumber);
    relayNumber[numberOfRelays] = theSwitch.relayNumber;
    numberOfRelays++;
}
void PowerControlWidget::drawLayout()
{
    QHBoxLayout *relayLayout = new QHBoxLayout(powerControlGroupBox);
    for(int thisRelay = 0; thisRelay < numberOfRelays; thisRelay++)
    {
        relayLayout->addWidget(powerButton[thisRelay]) ;
    }
    QHBoxLayout *errorLayout = new QHBoxLayout();
    errorLayout->addWidget(errorLabel);
    errorLayout->addWidget(errorLED);
    errorLayout->addWidget(checksumLabel);
    errorLayout->addWidget(checksumLED);
    errorLayout->addWidget(clearErrorsButton);
    errorLayout->addStretch(5);

    QVBoxLayout *thisLayout = new QVBoxLayout(this);
    thisLayout->addWidget(powerControlGroupBox);
    thisLayout->addLayout(errorLayout);
}

void PowerControlWidget::setRelayStates(const int8_t *theRelayStates)
{
    for(int thisRelay = 0; thisRelay < numberOfRelays; thisRelay++)
    {
        powerButton[thisRelay]->setButtonState(theRelayStates[relayNumber[thisRelay]]);
    }
}
void  PowerControlWidget::setErrorStates(bool  checksumError, int errorCode)
{
    if(checksumError)
    {
        if(!checksumErrorSet){
            setLedColor(checksumLED, Qt::red);
            checksumErrorSet = true;
        }
    }
    if(NCD_IO_ERROR_TIMEOUT == errorCode)
    {
        if(!timeoutErrorSet)
        {
            setLedColor(errorLED, Qt::red);
            timeoutErrorSet = true;
        }
    }
}
void PowerControlWidget::clearErrorsButtonClicked()
{
    resetLedColor(checksumLED);
    resetLedColor(errorLED);


    checksumErrorSet = false;
    timeoutErrorSet = false;
}
