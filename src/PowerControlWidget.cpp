#include "PowerControlWidget.h"

PowerControlWidget::PowerControlWidget(QWidget *parent) : QWidget(parent)
{
    powerControlGroupBox = new QGroupBox("power control");
    numberOfRelays = 0;
    myParent = parent;

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
    QVBoxLayout *thisLayout = new QVBoxLayout(this);
    thisLayout->addWidget(powerControlGroupBox);
}

void PowerControlWidget::setRelayStates(const int8_t *theRelayStates)
{
    for(int thisRelay = 0; thisRelay < numberOfRelays; thisRelay++)
    {
        powerButton[thisRelay]->setButtonState(theRelayStates[relayNumber[thisRelay]]);
    }
}
