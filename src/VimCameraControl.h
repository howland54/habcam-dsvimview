#ifndef FLYCAMERACONTROL_H
#define FLYCAMERACONTROL_H

#include <QtWidgets>
#include "qcustomplot.h"
#include "RepetitionControlWidget.h"
#include "cameradefines.h"

class VimCameraControl : public QWidget
{
   Q_OBJECT
public:
   VimCameraControl(int cameraNumber, QString cameraName, QWidget *parent);
   void                    setExposureActual(double theExposure);
   void                    setGainActual(double theExposure);
   void                    setIsColor(bool isIt);
   bool                    getIsColor();
   void                    setTriggerSource(int theSource);

private:
   QCheckBox               *autoGainCheckBox;
   QCheckBox               *autoExposureCheckBox;
   QSlider                 *changeGainSlider;
   QGroupBox               *autoExposureGroupBox;
   QSlider                 *changeExposureSlider;
   QDoubleSpinBox          *autoExposureSpinLow;
   QDoubleSpinBox          *autoExposureSpinHigh;
   QLabel                  *lowExposureLabel;
   QLabel                  *highExposureLabel;
   QLabel                  *actualGainLabel;
   QLabel                  *receivedGainLabel;
   QDoubleSpinBox          *autoGainSpinLow;
   QDoubleSpinBox          *autoGainSpinHigh;
   QLabel                  *lowGainLabel;
   QLabel                  *highGainLabel;
   QGroupBox               *autoGainGroupBox;
   QLabel                  *actualExposureLabel;
   QLabel                  *receivedExposureLabel;
   QLabel                  *cameraNameLabel;
   QCheckBox               *cameraIsBadCheckbox;
   QCheckBox               *histogramEqualizationCheckBox;
   QCheckBox               *histogramStretchCheckBox;
   QCustomPlot             *imageHistogramPlot;
   QCheckBox               *showHistogramCheckBox;
   RepetitionControlWidget *repetitionControlWidget;




   int                     myCameraNumber;
   double                  desiredGain;
   double                  autoMinGain;
   double                  autoMaxGain;
   double                  autoMinExposure;
   double                  autoMaxExposure;
   double                  desiredExposure;


   QVector<double>         greyShades;
   bool                    isColor;




signals:
   void                   changeExposureValue(double newExposure, int theCamera);
   void                   changeCameraGain(double newGain, int theCamera);
   void                   sendAutoGain(bool isAuto, double min, double max, int theCamera);
   void                   toggleBinning(bool bin, int theCamera);
   void                   changeAutoExposure(bool isAuto, double min, double max, int theCamera);
   void                   changeEqualization(bool equalize, int theCamera);
   void                   changeStretch(bool stretch, int theCamera);
   void                   changeHistogramDisplay(bool doIt, int theCamera);
   void                   takeAPicture(int theCamera);
   void                   startTimedStills(double theInterval, int cameraNumber);
   void                   stopTimedStills(int cameraNumber);
   void                   changeDecimationFactor(int theFactor);


public slots:

private slots:

   void                   changeExposureValue();
   void                   changeGainValue();
   void                   changeCameraExposure(int newExposure );
   void                   changeCameraGain(int newExposure);
   void                   changeAutoExposure(bool isAuto);
   void                   changeAutoGain(bool isAuto);
   void                   toggleBinning(bool binning);
   void                   changeHistogramEqualization(bool equalize);
   void                   changeHistogramStretch(bool equalize);
   void                   showHistogram(bool show);
   void                   drawHistogram(QVector<float> RhistToDraw,QVector<float> GistToDraw,QVector<float> BhistToDraw);
   void                   autoGainValueChanged(double theValue);
   void                   autoExposureValueChanged(double theValue);
   void                   startRepetition(double theInterval);
   void                   stopRepetition();
   void                   takePictureSlot();
   void                   changeDecimation(int theDecimation);
   void                   cameraIsBadClicked(bool itIs);


};

#endif // FLYCAMERACONTROL_H
