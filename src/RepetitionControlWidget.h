#ifndef REPETITIONCONTROLWIDGET_H
#define REPETITIONCONTROLWIDGET_H

#include <QtWidgets>

class RepetitionControlWidget : public QWidget
{
   Q_OBJECT
public:
   explicit RepetitionControlWidget(QWidget *parent = 0);
   void     setMinMaxRates(double theMin, double theMax);

private:
   QGroupBox      *repetitionControlGroupBox;
   QRadioButton   *singleFrameButton;
   QRadioButton   *programmedIntervalRadioButton;
   QPushButton    *takeButton;
   QDoubleSpinBox *rateSpinBox;
   QLabel         *rateSpinBoxLabel;
   QSpinBox       *decimationSpinBox;
   QLabel         *decimationSpinBoxLabel;
   QPushButton    *sendDecimationFactorButton;



   QPushButton    *startStopImageTimerButton;

   bool           timerEnabled;

signals:
   void           takePictureSignal();
   void           startRepetition(double interval);
   void           stopRepetition();
   void           changeDecimation(int theDecimation);


public slots:

private slots:
   void           repTypeRadioButtonClicked(bool trueFalse);
   void           startStopImageTimer();
   void           takePictureSlot();
   void           sendDecimationFactor();
};

#endif // REPETITIONCONTROLWIDGET_H
