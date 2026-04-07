#ifndef SYSTEMPARAMETERCONTROL_H
#define SYSTEMPARAMETERCONTROL_H

#include <QtWidgets>
#include "qledindicator.h"

class SystemParameterControl : public QWidget
{
   Q_OBJECT
public:
   explicit SystemParameterControl(QWidget *parent = nullptr);
   void                    setRecordingActualState(bool theState);
   void                    setWriteSuccess(bool isItSuccessful);
   void                    setAcqLightOK(bool isItOK);
   void                    setUseConstancy(bool useIt);


private:
   QCheckBox               *showWinchFlyCheckBox;
   QCheckBox               *showSensorsCheckBox;
   QCheckBox               *pauseRecordingCheckBox;
   QCheckBox               *useColorConstancyCheckBox;

   QLedIndicator           *recordingLEDIndicator;
   QLedIndicator           *writeStatusLEDIndicator;
   QLedIndicator           *imgRecieveLEDIndicator;
   QLabel                  *recordStatusLabel;
   QLabel                  *writeStatusLabel;
   QLabel                  *imgRecieveStatusLabel;
   QSlider                 *rateSlider;
   QLabel                  *rateLabel;

   double                  cameraHz;

   QCheckBox               *showPortCheckBox;
   QCheckBox               *showStbdCheckBox;

   QSpinBox                *imageSkipSpinner;
   QLabel                  *imageSkipLabel;

signals:

   void                   emitCameraInterval(int theInterval);
   void                   pauseRec(bool pauseIt);
   void                   showWinch(bool showIt);
   void                   showSensors(bool showIt);
   void                   emitSkipValue(int theSkipValue);
   void                   hideImageDisplay(int which,bool hideOrNot) ;
   void                   emitConstancy(bool newConstancy);



public slots:

private slots:
   //void                   gardaRateChanged(int rate);
   //void                   sendGardaRateChange();
   void                   pauseRecording(bool pauseIt);
   void                   winchflyShow(bool showIt);
   void                   sensorsShow(bool showIt);
   void                   skipValueChanged(int theNewValue);
   void                   hideImage(bool hideOrNot);
   void                   toggleUseConstancy(bool useOrNot);


};

#endif // SYSTEMPARAMETERCONTROL_H
