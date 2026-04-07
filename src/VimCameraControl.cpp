#include "VimCameraControl.h"

VimCameraControl::VimCameraControl(int cameraNumber, QString cameraName, QWidget *myParent)
{

   myCameraNumber = cameraNumber;
   QString cameraNameString = "Camera " + QString::number(cameraNumber) + " (" + cameraName + ")";
   cameraNameLabel = new QLabel(cameraNameString);

   autoMinGain = 0.0;
   autoMaxGain = 40.0;  //set for Manta 319
   autoMinExposure = 0.0;
   autoMaxExposure = 10.0;
   isColor = false;

   changeGainSlider = new QSlider();
   changeGainSlider->setOrientation(Qt::Horizontal);
   changeGainSlider->setRange(0,400);
   connect(changeGainSlider,SIGNAL(sliderReleased()),this,SLOT(changeGainValue()));
   connect(changeGainSlider,SIGNAL(valueChanged(int)) ,this,SLOT(changeCameraGain(int)));

   autoGainSpinLow = new QDoubleSpinBox();
   autoGainSpinHigh = new QDoubleSpinBox();
   autoGainSpinLow->setMaximum(40.0);
   autoGainSpinLow->setMinimum(0.0);
   autoGainSpinHigh->setMaximum(40.0);
   autoGainSpinHigh->setMinimum(0.0);
   autoGainSpinHigh->setValue(40.0);
   autoGainSpinLow->setValue(0.0);

   lowGainLabel = new QLabel("min gain");
   highGainLabel = new QLabel("max gain");

   autoGainGroupBox = new QGroupBox;
   QGridLayout *autoGainLayout = new QGridLayout(autoGainGroupBox);
   autoGainLayout->addWidget(autoGainSpinLow,0,0);
   autoGainLayout->addWidget(autoGainSpinHigh,0,1);
   connect(autoGainSpinLow,SIGNAL(valueChanged(double)),this, SLOT(autoGainValueChanged(double)));
   connect(autoGainSpinHigh,SIGNAL(valueChanged(double)),this, SLOT(autoGainValueChanged(double)));
   autoGainLayout->addWidget(lowGainLabel,1,0);
   autoGainLayout->addWidget(highGainLabel,1,1);
   autoGainGroupBox->setVisible(true);

   autoGainCheckBox = new QCheckBox("auto");
   autoGainCheckBox->setChecked(false);
   connect(autoGainCheckBox,SIGNAL(clicked(bool)), this, SLOT(changeAutoGain(bool)));

   actualGainLabel = new QLabel("??");
   receivedGainLabel = new QLabel("rcvd: ??");

   QGroupBox *gainGroupBox = new QGroupBox("gain");
   QGridLayout *gainLayout = new QGridLayout(gainGroupBox);
   gainLayout->addWidget(autoGainCheckBox,0,0);
   gainLayout->addWidget(changeGainSlider,0,1,1,2);

   gainLayout->addWidget(actualGainLabel,0,3);
   gainLayout->addWidget(receivedGainLabel,0,4);
   gainLayout->addWidget(autoGainGroupBox,1,0,2,4);

   autoExposureCheckBox = new QCheckBox("auto");
   autoExposureCheckBox->setChecked(false);
   connect(autoExposureCheckBox,SIGNAL(clicked(bool)), this, SLOT(changeAutoExposure(bool)));
   autoExposureSpinLow = new QDoubleSpinBox();
   autoExposureSpinHigh = new QDoubleSpinBox();
   autoExposureSpinLow->setMaximum(100.0);
   autoExposureSpinLow->setMinimum(0.0);
   autoExposureSpinHigh->setMaximum(100.0);
   autoExposureSpinHigh->setMinimum(0.0);
   autoExposureSpinHigh->setValue(10.0);
   autoExposureSpinLow->setValue(0.0);

   lowExposureLabel = new QLabel("min exp (ms)");
   highExposureLabel = new QLabel("max exp (ms)");


   autoExposureGroupBox = new QGroupBox;
   QGridLayout *autoExposureLayout = new QGridLayout(autoExposureGroupBox);
   autoExposureLayout->addWidget(autoExposureSpinLow,0,0);
   autoExposureLayout->addWidget(autoExposureSpinHigh,0,1);
   connect(autoExposureSpinLow,SIGNAL(valueChanged(double)),this, SLOT(autoExposureValueChanged(double)));
   connect(autoExposureSpinHigh,SIGNAL(valueChanged(double)),this, SLOT(autoExposureValueChanged(double)));
   autoExposureLayout->addWidget(lowExposureLabel,1,0);
   autoExposureLayout->addWidget(highExposureLabel,1,1);
   autoExposureGroupBox->setVisible(true);


   changeExposureSlider = new QSlider();
   changeExposureSlider->setRange(0,1000);
   changeExposureSlider->setOrientation(Qt::Horizontal);
   connect(changeExposureSlider,SIGNAL(sliderReleased()),this,SLOT(changeExposureValue()));
   connect(changeExposureSlider,SIGNAL(valueChanged(int)) ,this,SLOT(changeCameraExposure(int)));


   actualExposureLabel = new QLabel("??");
   receivedExposureLabel = new QLabel("rcvd:  ??");

   QGroupBox *exposureGroupBox = new QGroupBox("exposure");
   QGridLayout *exposureLayout = new QGridLayout(exposureGroupBox);
   exposureLayout->addWidget(autoExposureCheckBox,0,0);
   exposureLayout->addWidget(changeExposureSlider,0,1,1,3);

   exposureLayout->addWidget(actualExposureLabel,0,4);
   exposureLayout->addWidget(receivedExposureLabel,0,5);
   exposureLayout->addWidget(autoExposureGroupBox,1,0,2,5);


   showHistogramCheckBox = new QCheckBox("Show Histogram");
   connect(showHistogramCheckBox,SIGNAL(clicked(bool)), this,SLOT(showHistogram(bool)));

   histogramEqualizationCheckBox = new QCheckBox("Histogram Equalize");
   connect(histogramEqualizationCheckBox,SIGNAL(clicked(bool)), this, SLOT(changeHistogramEqualization(bool)));

   cameraIsBadCheckbox = new QCheckBox("camera bad");
   connect(cameraIsBadCheckbox,SIGNAL(clicked(bool)), this, SLOT(cameraIsBadClicked(bool)));
   //lightLayout->addWidget(cameraIsBadCheckbox,2,0);


   histogramStretchCheckBox = new QCheckBox("Histogram Stretch");
   connect(histogramStretchCheckBox,SIGNAL(clicked(bool)), this, SLOT(changeHistogramStretch(bool)));

   imageHistogramPlot = new QCustomPlot(this);
   for(int greyN = 0; greyN < 256; greyN++)
      {
         greyShades.append((double)greyN);
      }
   imageHistogramPlot->addGraph();
   imageHistogramPlot->graph(0)->setPen(QPen(Qt::red));
   imageHistogramPlot->addGraph();
   imageHistogramPlot->graph(1)->setPen(QPen(Qt::blue));
   imageHistogramPlot->addGraph();
   imageHistogramPlot->graph(2)->setPen(QPen(Qt::green));

   imageHistogramPlot->setVisible(false);

#if 1
   //binningStateLabel = new QLabel("actual Binning: ?");

   repetitionControlWidget = new RepetitionControlWidget(this);
   connect(repetitionControlWidget,SIGNAL(startRepetition(double)),this, SLOT(startRepetition(double)));
   connect(repetitionControlWidget,SIGNAL(stopRepetition()),this, SLOT(stopRepetition()));
   connect(repetitionControlWidget,SIGNAL(takePictureSignal()), this, SLOT(takePictureSlot()));
   connect(repetitionControlWidget,SIGNAL(changeDecimation(int)), this, SLOT(changeDecimation(int)));
#endif

   QGroupBox *titleGroupBox = new QGroupBox();
   QGridLayout *titleLayout = new QGridLayout(titleGroupBox);

   titleLayout->addWidget(cameraNameLabel,0,0);
#if 0
   if(0 == myCameraNumber)
      {
         titleLayout->addWidget(pauseRecordingCheckBox,0,1);
         titleLayout->addWidget(showWinchFlyCheckBox,0,2);
         titleLayout->addLayout(rateLayout,1,0);
      }
#endif
  // titleLayout->addWidget(binningCheckBox,1,0);
   //titleLayout->addWidget(binningStateLabel,1,1);
   titleLayout->addWidget(showHistogramCheckBox,1,1);
   titleLayout->addWidget(histogramEqualizationCheckBox,2,0);
   titleLayout->addWidget(histogramStretchCheckBox,2,1);
#if 0
   titleLayout->addLayout(lightLayout,2,2);
   if(1 == myCameraNumber)
      {
         recordingLEDIndicator->setVisible(false);
         writeStatusLEDIndicator->setVisible(false);
         recordStatusLabel->setVisible(false);
         writeStatusLabel->setVisible(false);
      }
#endif
  // titleLayout->addWidget(showLightControlCheckBox,2,2);

   QVBoxLayout *masterLayout = new QVBoxLayout(this);
   masterLayout->addWidget(imageHistogramPlot);
   masterLayout->addWidget(titleGroupBox);
   QHBoxLayout *imageBrightnessLayout = new QHBoxLayout();
   imageBrightnessLayout->addWidget(gainGroupBox);
   imageBrightnessLayout->addWidget(exposureGroupBox);
   masterLayout->addLayout(imageBrightnessLayout);
   masterLayout->addWidget(repetitionControlWidget);


}

void VimCameraControl::cameraIsBadClicked(bool itIs)
{
   if(itIs)
      {

      }
   else
      {

      }
}




void VimCameraControl::autoGainValueChanged(double theValue)
{
   QDoubleSpinBox *theSendingSpinBox = (QDoubleSpinBox *)sender(  );
   if(autoGainSpinLow == theSendingSpinBox)
      {
         autoGainSpinHigh->setMinimum(theValue);
         autoMinGain = theValue;
      }
   else
      {
         if(theValue < autoGainSpinLow->value())
            {
               autoGainSpinLow->setMinimum(theValue);
            }
         autoMaxGain = theValue;
      }
}

bool VimCameraControl::getIsColor()
{
   return isColor;
}

void VimCameraControl::setIsColor(bool isIt)
{
   isColor = isIt;
}
void VimCameraControl::autoExposureValueChanged(double theValue)
{
   QDoubleSpinBox *theSendingSpinBox = (QDoubleSpinBox *)sender(  );
   if(autoExposureSpinLow == theSendingSpinBox)
      {
         autoExposureSpinHigh->setMinimum(theValue);
         autoMinExposure = theValue;
      }
   else
      {
         if(theValue < autoExposureSpinLow->value())
            {
               autoExposureSpinLow->setMinimum(theValue);
            }
         autoMaxExposure = theValue;
      }
}

void VimCameraControl::showHistogram(bool show)
{

   imageHistogramPlot->setVisible(show);
   emit changeHistogramDisplay(show, myCameraNumber);


}

void VimCameraControl::drawHistogram(QVector<float> RhistToDraw,QVector<float> GhistToDraw,QVector<float> BhistToDraw)
{
   QVector<double> doubleRHist( RhistToDraw.size());
   QVector<double> doubleGHist( GhistToDraw.size());
   QVector<double> doubleBHist( BhistToDraw.size());
   double maxValue = 0;
   double sum = 0.0;
   double maxPercentage = 0.0;
   for(int index = 0; index < RhistToDraw.size(); index++)
      {
         doubleRHist[index] = (double)RhistToDraw.at(index);
         if(doubleRHist[index] > maxValue)
            {
               maxValue = doubleRHist[index];

            }
         sum += doubleRHist[index];
      }
   for(int index = 0; index < RhistToDraw.size(); index++)
      {
         doubleRHist[index] = doubleRHist[index]/sum;
         if(doubleRHist[index] > maxPercentage)
            {
               maxPercentage = doubleRHist[index];
            }
      }

   sum = 0.0;
   for(int index = 0; index < GhistToDraw.size(); index++)
      {
         doubleGHist[index] = (double)GhistToDraw.at(index);
         if(doubleGHist[index] > maxValue)
            {
               maxValue = doubleGHist[index];

            }
         sum += doubleGHist[index];
      }
   for(int index = 0; index < GhistToDraw.size(); index++)
      {
         doubleGHist[index] = doubleGHist[index]/sum;
         if(doubleGHist[index] > maxPercentage)
            {
               maxPercentage = doubleGHist[index];
            }
      }
   sum = 0.0;
   for(int index = 0; index < BhistToDraw.size(); index++)
      {
         doubleBHist[index] = (double)BhistToDraw.at(index);
         if(doubleBHist[index] > maxValue)
            {
               maxValue = doubleBHist[index];

            }
         sum += doubleBHist[index];
      }
   for(int index = 0; index < RhistToDraw.size(); index++)
      {
         doubleBHist[index] = doubleBHist[index]/sum;
         if(doubleBHist[index] > maxPercentage)
            {
               maxPercentage = doubleBHist[index];
            }
      }
   imageHistogramPlot->graph(0)->setData(greyShades, doubleRHist);
   imageHistogramPlot->graph(1)->setData(greyShades, doubleBHist);
   imageHistogramPlot->graph(2)->setData(greyShades, doubleGHist);

   imageHistogramPlot->xAxis->setRange(0, 255);
   imageHistogramPlot->yAxis->setRange(0,maxPercentage );
   imageHistogramPlot->replot();
}


void VimCameraControl::setExposureActual(double theExposure)
{
   receivedExposureLabel->setText("rcvd: " + QString::number(theExposure,'f',1));
}
void VimCameraControl::setGainActual(double theGain)
{
   receivedGainLabel->setText("rcvd: " + QString::number(theGain,'f',1));
}

void VimCameraControl::changeHistogramEqualization(bool equalize)
{
   if(equalize)
      {
         histogramStretchCheckBox->setChecked(false);
      }
   emit changeEqualization(equalize,myCameraNumber);
}

void VimCameraControl::changeHistogramStretch(bool stretch)
{
   if(stretch)
      {
         histogramEqualizationCheckBox->setChecked(false);
      }
   emit changeStretch(stretch,myCameraNumber);
}

void VimCameraControl::changeExposureValue()
{
   emit changeExposureValue(desiredExposure,myCameraNumber);
}

void VimCameraControl::changeGainValue()
{
   emit changeCameraGain(desiredGain, myCameraNumber);
}

void VimCameraControl::changeCameraExposure(int newExposure)
{
   desiredExposure = newExposure/10.0;
   actualExposureLabel->setText(QString::number(desiredExposure,'f',1));
}

void VimCameraControl::changeCameraGain(int newGain)
{
   desiredGain = (double)newGain/10.0;
   actualGainLabel->setText(QString::number(desiredGain,'f',1));
}

void VimCameraControl::changeAutoGain(bool isAuto)
{
   if(isAuto)
      {
         changeGainSlider->setVisible(false);
         actualGainLabel->setVisible(false);
      }
   else
      {
         changeGainSlider->setVisible(true);
         actualGainLabel->setVisible(true);
      }
   emit sendAutoGain(isAuto, autoMinGain,autoMaxGain, myCameraNumber);
}
void VimCameraControl::toggleBinning(bool binning)
{
   emit toggleBinning(binning, myCameraNumber);
}

void VimCameraControl::changeAutoExposure(bool isAuto)
{
   if(isAuto)
      {
         changeExposureSlider->setVisible(false);
         actualExposureLabel->setVisible(false);
      }
   else
      {
         changeExposureSlider->setVisible(true);
         actualExposureLabel->setVisible(true);
      }
   emit changeAutoExposure(isAuto,autoMinExposure,autoMaxExposure,  myCameraNumber);
}
void VimCameraControl::startRepetition(double theInterval)
{
   emit startTimedStills(theInterval,myCameraNumber );
}
void VimCameraControl::stopRepetition()
{
   emit stopTimedStills(myCameraNumber);
}
void VimCameraControl::takePictureSlot()
{
   emit takeAPicture(myCameraNumber);
}

void VimCameraControl::setTriggerSource(int theSource)
{
   if(SOFTWARE != theSource)
      {
         repetitionControlWidget->setEnabled(false) ;
      }
}

void VimCameraControl::changeDecimation(int theDecimation)
{
   emit changeDecimationFactor(theDecimation);
}

