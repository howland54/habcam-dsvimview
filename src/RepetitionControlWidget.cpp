#include "RepetitionControlWidget.h"

RepetitionControlWidget::RepetitionControlWidget(QWidget *parent) : QWidget(parent)
{
   timerEnabled = false;
   repetitionControlGroupBox = new QGroupBox("repetion control");
   singleFrameButton = new QRadioButton("single frame");
   singleFrameButton->setChecked(true);
   programmedIntervalRadioButton = new QRadioButton("repeat");
   connect(singleFrameButton,SIGNAL(clicked(bool)), this, SLOT(repTypeRadioButtonClicked(bool)));
   connect(programmedIntervalRadioButton,SIGNAL(clicked(bool)), this, SLOT(repTypeRadioButtonClicked(bool)));
   takeButton = new QPushButton("take");
   connect(takeButton,SIGNAL(clicked()), this, SLOT(takePictureSlot()));
   rateSpinBox = new QDoubleSpinBox();
   rateSpinBox->setEnabled(false);
   rateSpinBox->setMinimum(0.10);
   rateSpinBox->setSingleStep(0.01);
   rateSpinBox->setValue(0.25);
   rateSpinBox->setMaximum(30.0);
   rateSpinBoxLabel = new QLabel("secs between images");

   decimationSpinBox = new QSpinBox();
   decimationSpinBox->setMinimum(0);
   decimationSpinBox->setMaximum(5);
   decimationSpinBox->setEnabled(false);

   decimationSpinBoxLabel = new QLabel("decimation: ");
   sendDecimationFactorButton = new QPushButton("change decimation");
   sendDecimationFactorButton->setEnabled(false);
   connect(sendDecimationFactorButton,SIGNAL(clicked()), this, SLOT(sendDecimationFactor()));

   startStopImageTimerButton = new QPushButton("start timer");
   connect(startStopImageTimerButton,SIGNAL(clicked()),this, SLOT(startStopImageTimer()));
   startStopImageTimerButton->setEnabled(false);

   QGridLayout *primaryLayout = new QGridLayout(repetitionControlGroupBox);
   primaryLayout->addWidget(singleFrameButton,0,0);
   primaryLayout->addWidget(programmedIntervalRadioButton,1,0);
   primaryLayout->addWidget(takeButton,2,0);
   primaryLayout->addWidget(rateSpinBox,2,1);
   primaryLayout->addWidget(rateSpinBoxLabel,3,1);
   primaryLayout->addWidget(decimationSpinBox,2,2);
   primaryLayout->addWidget(decimationSpinBoxLabel,3,2);

   primaryLayout->addWidget(startStopImageTimerButton,2,3);
   primaryLayout->addWidget(sendDecimationFactorButton,3,3);
   QVBoxLayout *thisLayout = new QVBoxLayout(this);
   thisLayout->addWidget(repetitionControlGroupBox);


}

void RepetitionControlWidget::sendDecimationFactor()
{
   emit changeDecimation(decimationSpinBox->value());
}

void RepetitionControlWidget::repTypeRadioButtonClicked(bool trueFalse)
{
   QRadioButton * theSendingButton = (QRadioButton *)sender();
   if(singleFrameButton == theSendingButton)
      {
         startStopImageTimerButton->setEnabled(!trueFalse);
         takeButton->setEnabled(trueFalse);
         rateSpinBox->setEnabled(!trueFalse);
         decimationSpinBox->setEnabled(!trueFalse);

      }
   else if(programmedIntervalRadioButton == theSendingButton)
      {
         startStopImageTimerButton->setEnabled(trueFalse);
         rateSpinBox->setEnabled(trueFalse);
         decimationSpinBox->setEnabled(trueFalse);
         takeButton->setEnabled(!trueFalse);
         sendDecimationFactorButton->setEnabled(trueFalse);

      }
}

void RepetitionControlWidget::takePictureSlot()
{
   emit takePictureSignal();
}

void  RepetitionControlWidget::startStopImageTimer()
{
   if(timerEnabled)
      {
         startStopImageTimerButton->setText("start timer");
         timerEnabled = false;
         rateSpinBox->setEnabled(true);
         emit stopRepetition();
      }
   else
      {
         startStopImageTimerButton->setText("stop timer");
         timerEnabled = true;
         rateSpinBox->setEnabled(false);
         emit startRepetition(rateSpinBox->value());
      }

}

void RepetitionControlWidget::setMinMaxRates(double theMin, double theMax)
{
   rateSpinBox->setMinimum(theMin);
   rateSpinBox->setMaximum(theMax);
}

