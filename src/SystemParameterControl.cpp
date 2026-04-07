#include "SystemParameterControl.h"

SystemParameterControl::SystemParameterControl(QWidget *parent) : QWidget(parent)
{
   //QGroupBox *parameterGroupBox = new QGroupBox("System Parameters");
   pauseRecordingCheckBox = new QCheckBox("pause recording");
   showWinchFlyCheckBox = new QCheckBox("show Winchfly");
   showSensorsCheckBox = new QCheckBox("show Sensors");
   useColorConstancyCheckBox = new QCheckBox("Use Gray World");
   showWinchFlyCheckBox->setChecked(true);
   showSensorsCheckBox->setChecked(true);
   connect(pauseRecordingCheckBox,SIGNAL(clicked(bool)), this, SLOT(pauseRecording(bool)));
   connect(showWinchFlyCheckBox,SIGNAL(clicked(bool)), this, SLOT(winchflyShow(bool)));
   connect(showSensorsCheckBox,SIGNAL(clicked(bool)), this, SLOT(sensorsShow(bool)));
   connect(useColorConstancyCheckBox, SIGNAL(clicked(bool)), this, SLOT(toggleUseConstancy(bool)));
   recordingLEDIndicator = new QLedIndicator(this);
   writeStatusLEDIndicator = new QLedIndicator(this);
   imgRecieveLEDIndicator = new QLedIndicator(this);
   recordStatusLabel = new QLabel("rec:");
   writeStatusLabel = new QLabel("wr: ");
   imgRecieveStatusLabel = new QLabel("acq: ");

   QGridLayout *lightLayout = new QGridLayout;
   lightLayout->addWidget(recordingLEDIndicator,0,1);
   lightLayout->addWidget(writeStatusLEDIndicator,1,1);
   lightLayout->addWidget(imgRecieveLEDIndicator,2,1);
   lightLayout->addWidget(recordStatusLabel,0,0);
   lightLayout->addWidget(writeStatusLabel,1,0);
   lightLayout->addWidget(imgRecieveStatusLabel,2,0);

   QGroupBox *displayControlGroupBox = new QGroupBox("display");
   showPortCheckBox = new QCheckBox("Show Port");
   showPortCheckBox->setChecked(true);
   showStbdCheckBox = new QCheckBox("Show Stbd");
   showStbdCheckBox->setChecked(true);
   connect(showPortCheckBox,SIGNAL(clicked(bool)), this, SLOT(hideImage(bool)));
   connect(showStbdCheckBox,SIGNAL(clicked(bool)), this, SLOT(hideImage(bool)));
   imageSkipSpinner = new QSpinBox(this);
   imageSkipSpinner->setMinimum(0);
   imageSkipSpinner->setMaximum(10);
   imageSkipSpinner->setValue(0);
   connect(imageSkipSpinner,SIGNAL(valueChanged(int)),this,SLOT(skipValueChanged(int)));
   imageSkipLabel = new QLabel("display: skip 0 images");
   QGridLayout   *displayParameterLayout = new QGridLayout(displayControlGroupBox);
   displayParameterLayout->addWidget(showPortCheckBox,0,0);
   displayParameterLayout->addWidget(showStbdCheckBox,1,0);
   displayParameterLayout->addWidget(imageSkipSpinner,1,1);
   displayParameterLayout->addWidget(imageSkipLabel,0,1);
#if 0
   rateSlider = new QSlider;
   rateSlider->setRange(0,100);
   rateSlider->setOrientation(Qt::Horizontal);
   rateLabel = new QLabel("rate:??? hz");
   QHBoxLayout *rateLayout = new QHBoxLayout();
   rateLayout->addWidget(rateLabel);
   rateLayout->addWidget(rateSlider);
   connect(rateSlider,SIGNAL(sliderMoved(int)),this,SLOT(gardaRateChanged(int)));
   connect(rateSlider,SIGNAL(sliderReleased()),this,SLOT(sendGardaRateChange()));
#endif
   QGridLayout *masterLayout = new QGridLayout(this);
   //masterLayout->addLayout(rateLayout,0,0);
   masterLayout->addWidget(displayControlGroupBox,0,1);
   masterLayout->addWidget(pauseRecordingCheckBox,0,2);
   masterLayout->addLayout(lightLayout,0,3);
   masterLayout->addWidget(showWinchFlyCheckBox,0,4);
   masterLayout->addWidget(showSensorsCheckBox,0,5);
   masterLayout->addWidget(useColorConstancyCheckBox,0,6);
   //QVBoxLayout *thisLayout = new QVBoxLayout(this);
   //thisLayout->addLayout(masterLayout);



}

void SystemParameterControl::hideImage(bool hideOrNot)
{
   QCheckBox *sendingCheckBox = (QCheckBox *)sender();
   if(showPortCheckBox == sendingCheckBox)
      {
         emit hideImageDisplay(0,hideOrNot) ;
      }
   else
      {
         emit hideImageDisplay(1,hideOrNot) ;
      }
}
void SystemParameterControl::setUseConstancy(bool useIt)
{

   useColorConstancyCheckBox->setChecked(useIt);

}

void SystemParameterControl::setRecordingActualState(bool theState)
{
   if(theState)
      {
         recordingLEDIndicator->setOffColor1(Qt::red);
         recordingLEDIndicator->setOffColor2(Qt::red);
      }
   else
      {
         recordingLEDIndicator->setOffColor1(Qt::green);
         recordingLEDIndicator->setOffColor2(Qt::green);
      }
   update();
}

void SystemParameterControl::setWriteSuccess(bool isItSuccessful)
{
   if(isItSuccessful)
      {
         writeStatusLEDIndicator->setOffColor1(Qt::green);
         writeStatusLEDIndicator->setOffColor2(Qt::green);
      }
   else
      {
         writeStatusLEDIndicator->setOffColor1(Qt::red);
         writeStatusLEDIndicator->setOffColor2(Qt::red);
      }
   update();
}

void SystemParameterControl::setAcqLightOK(bool isItOK)
{
   if(isItOK)
      {
         imgRecieveLEDIndicator->setOffColor1(Qt::green);
         imgRecieveLEDIndicator->setOffColor2(Qt::green);
      }
   else
      {
         imgRecieveLEDIndicator->setOffColor1(Qt::red);
         imgRecieveLEDIndicator->setOffColor2(Qt::red);
      }
   update();
}



void SystemParameterControl::skipValueChanged(int theNewSkipValue)
{

  imageSkipLabel->setText("display: skip " + QString::number(theNewSkipValue) + " images");
  emit emitSkipValue(theNewSkipValue);
}

void SystemParameterControl::pauseRecording(bool pauseIt)
{
   emit pauseRec(pauseIt);
}
void SystemParameterControl::winchflyShow(bool showIt)
{
   emit showWinch(showIt);
}

void SystemParameterControl::sensorsShow(bool showIt)
{
   emit showSensors(showIt);
}

void SystemParameterControl::toggleUseConstancy(bool useIt)
{

   emit emitConstancy(useIt);

}

#if 0
void  SystemParameterControl::gardaRateChanged(int rate)
{
   cameraHz =  (double)rate/10.0;
   rateLabel->setText("rate:" + QString::number(cameraHz,'f',2) + " hz");

}

void SystemParameterControl::sendGardaRateChange()
{
   int interval = (int)(1000.0/cameraHz);
   emit emitCameraInterval(interval);

}
#endif
