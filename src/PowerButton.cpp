#include "PowerButton.h"

PowerButton::PowerButton(QWidget *parent) : QWidget(parent)
{
    relayGroupBox = new QGroupBox("unknown device",this);
    onButton = new QPushButton("Turn On");
    offButton = new QPushButton("Turn Off");
    powerLED = new QLedIndicator(this);

    buttonState = new QLabel("state unknown");
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(onButton);
    buttonLayout->addWidget(offButton);

    connect(onButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(offButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    QHBoxLayout *masterLayout = new QHBoxLayout(relayGroupBox);
    masterLayout->addLayout(buttonLayout);
    masterLayout->addWidget(powerLED);
    masterLayout->addWidget(buttonState);
    QVBoxLayout *myLayout = new QVBoxLayout(this);
    myLayout->addWidget(relayGroupBox);
    lastState = -1;
}




PowerButton::~PowerButton()
{

}


void PowerButton::setButtonLabel(const QString theLabel)
{
   relayGroupBox->setTitle(theLabel);
}
void PowerButton::setButtonNumber(int theNumber)
{
    myNumber = theNumber;
}

void PowerButton::setButtonState(int theState)
{
    if(theState)
    {
        if(lastState != 1)
        {
            buttonState->setText("on");
            lastState = 1;
            setLedColor(powerLED, Qt::green);
        }
    }
    else
    {
        if(lastState != 0)
        {
            buttonState->setText("off");
            lastState = 0;
            resetLedColor(powerLED);
        }

    }
}


void PowerButton::buttonClicked()
{
    QPushButton *mySender = (QPushButton *)sender();
    if(mySender == offButton)
    {
        emit powerButtonClicked(myNumber,0);
    }
    else
    {
        emit powerButtonClicked(myNumber,1);
    }

}
