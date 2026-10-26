#include "PowerButton.h"

PowerButton::PowerButton(QWidget *parent) : QWidget(parent)
{
    relayGroupBox = new QGroupBox("unknown device",this);
    onButton = new QPushButton("Turn On");
    offButton = new QPushButton("Turn Off");
    buttonState = new QLabel("state unknown");
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(onButton);
    buttonLayout->addWidget(offButton);

    connect(onButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    connect(offButton, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    QHBoxLayout *masterLayout = new QHBoxLayout(relayGroupBox);
    masterLayout->addLayout(buttonLayout);
    masterLayout->addWidget(buttonState);
    QVBoxLayout *myLayout = new QVBoxLayout(this);
    myLayout->addWidget(relayGroupBox);
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
        buttonState->setText("on");
    }
    else
    {
        buttonState->setText("off");

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
