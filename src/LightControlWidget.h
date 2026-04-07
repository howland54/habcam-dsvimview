#ifndef LIGHTCONTROLWIDGET_H
#define LIGHTCONTROLWIDGET_H

#include <QtWidgets>

class LightControlWidget : public QWidget
{
   Q_OBJECT
public:
   explicit LightControlWidget(QWidget *parent = nullptr);
   void     setPosition(char *thePosition);
   void     updateLight( double humidity, double temperature, int lightLevel, int lightMode, double secsSince, int nackCount);


private:
   QLabel   *lightLabel;
   QSlider  *lightSlider;
   QLabel   *sliderLabel;
   QLabel   *sliderValueLabel;
   QLabel   *lightTempLabel;
   QLabel   *lightTempValueLabel;
   QLabel   *lightHumidityLabel;
   QLabel   *lightHumidityValueLabel;
   QLabel   *receivedPowerLevelLabel;
   QLabel   *receivedPowerLevelValueLabel;
   QLabel   *receivedModeLabel;
   QLabel   *receivedModeValueLabel;
   QRadioButton   *whiteLightButton;
   QRadioButton   *redLightButton;
   QRadioButton   *bothButton;
   QString  degreeSymbol;
   QLabel   *commsSinceLabel;
   QLabel   *commsSinceValueLabel;
   QLabel   *nackCountLabel;
   QLabel   *nackCountValueLabel;

   int      desiredLightValue;
   double   theTemperature;
   double  theHumidity;



signals:
   void                   changeLightLevel(int level);
   void                   changeLightMode(int mode);


public slots:

   void        lightValueChange(int theNewValue);
   void        colorChoiceClicked(bool value);
};

#endif // LIGHTCONTROLWIDGET_H
