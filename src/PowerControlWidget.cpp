#include "PowerControlWidget.h"

PowerControlWidget::PowerControlWidget(QWidget *parent) : QWidget(parent)
{
    powerControlGroupBox = new QGroupBox("power control");
    numberOfRelays = 0;
    myParent = parent;
    errorLED = new QLedIndicator(this);
    checksumLED = new QLedIndicator(this);
    errorLabel = new QLabel("Timeout Error: ");
    checksumLabel = new QLabel("Checksum Error: ");
    clearErrorsButton = new QPushButton("Clear Errors");
    connect(clearErrorsButton, SIGNAL(clicked()),this, SLOT(clearErrorsButtonClicked()));
    checksumErrorSet = false;
    timeoutErrorSet = false;


}

void PowerControlWidget::addAButton(switchType theSwitch, QWidget *myParent)
{

    powerButton[numberOfRelays] = new PowerButton();
    connect(powerButton[numberOfRelays],SIGNAL(powerButtonClicked(int,int)),myParent ,SLOT(powerButtonClicked(int,int)));
    powerButton[numberOfRelays]->setButtonLabel(theSwitch.attachedTo);
    powerButton[numberOfRelays]->setButtonNumber(numberOfRelays);
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
            checksumLED->setOffColor1(Qt::red);
            checksumLED->setOffColor2(Qt::red);
            checksumLED->repaint();
            checksumErrorSet = true;
        }
    }
    if(NCD_IO_ERROR_TIMEOUT == errorCode)
    {
        if(!timeoutErrorSet)
        {
            errorLED->setOffColor1(Qt::red);
            errorLED->setOffColor2(Qt::red);
            errorLED->repaint();
            timeoutErrorSet = true;
        }
    }
}
void PowerControlWidget::clearErrorsButtonClicked()
{
    checksumLED->setOffColor1(QColor(0,28,0));
    checksumLED->setOffColor2(QColor(0,128,0));
    checksumLED->repaint();

    errorLED->setOffColor1(QColor(0,28,0));
    errorLED->setOffColor2(QColor(0,128,0));
    errorLED->repaint();


    checksumErrorSet = false;
    timeoutErrorSet = false;
}
