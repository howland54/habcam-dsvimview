#ifndef POWERBUTTON_H
#define POWERBUTTON_H

#include <QtWidgets>
#include "qledindicator.h"

// the LEDs are never switched on, so they are coloured through their "off" colours
inline void setLedColor(QLedIndicator *theLED, QColor theColor)
{
   theLED->setOffColor1(theColor);
   theLED->setOffColor2(theColor);
   theLED->repaint();
}

// back to the QLedIndicator default dark green
inline void resetLedColor(QLedIndicator *theLED)
{
   theLED->setOffColor1(QColor(0,28,0));
   theLED->setOffColor2(QColor(0,128,0));
   theLED->repaint();
}

class PowerButton : public QWidget
{
   Q_OBJECT

public:
   PowerButton(QWidget *parent = nullptr);
   ~PowerButton();
   void     setButtonLabel(const QString theLabel);
   void     setButtonState(int theState);
   void     setButtonNumber(int theNumber);

private slots:
   void buttonClicked();


private:
   QGroupBox    *relayGroupBox;
   QPushButton  *offButton;
   QPushButton  *onButton;
   QLabel       *buttonState;
   int          myNumber;
   int8_t       lastState;
   QLedIndicator    *powerLED;


signals:
   void powerButtonClicked(int,int);
protected:

};

#endif // POWERBUTTON_H
