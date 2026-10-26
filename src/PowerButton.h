#ifndef CONTROLBUTTON_H
#define CONTROLBUTTON_H

#include <QtWidgets>

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
   QLabel       *buttonLabel;
   QLabel       *buttonState;
   int          myNumber;


signals:
   void powerButtonClicked(int,int);
protected:

};

#endif // CONTROLBUTTON_H
