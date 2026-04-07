#include "LightControlWidget.h"


LightControlWidget::LightControlWidget(QWidget *parent) : QWidget(parent)
{
   lightLabel = new QLabel("unknown light");
   lightSlider = new QSlider();
   lightSlider->setMaximum(100);
   lightSlider->setMinimum(0);
   lightSlider->setOrientation(Qt::Horizontal);
   connect(lightSlider,SIGNAL(valueChanged(int)), this, SLOT(lightValueChange(int)));
   sliderLabel = new QLabel("light intensity");
   sliderValueLabel = new QLabel("0");
   lightTempLabel = new QLabel("temperature: ") ;
   lightTempValueLabel = new QLabel("unknown");
   lightHumidityLabel = new QLabel("humidity: " );
   lightHumidityValueLabel = new QLabel("unknown");
   QGroupBox *lightColorGroupBox = new QGroupBox("light color");
   whiteLightButton = new QRadioButton("white",lightColorGroupBox);
   redLightButton = new QRadioButton("red",lightColorGroupBox);
   bothButton = new QRadioButton("both",lightColorGroupBox);
   receivedPowerLevelLabel = new QLabel("rcvd power level: ");
   receivedPowerLevelValueLabel = new QLabel("unknown");
   receivedModeLabel = new QLabel("rcvd mode: ");
   receivedModeValueLabel = new QLabel("unknown");
   commsSinceLabel = new QLabel("secs since comms: ");
   commsSinceValueLabel = new QLabel("unknown");
   nackCountLabel = new QLabel(" NACK count: ");
   nackCountValueLabel = new QLabel("unknown");
   ushort degree = 0x00b0;
   degreeSymbol = QString::fromUtf16(&degree,1);


   connect(whiteLightButton,SIGNAL(clicked(bool)), this, SLOT(colorChoiceClicked(bool)));
   connect(redLightButton,SIGNAL(clicked(bool)), this, SLOT(colorChoiceClicked(bool)));
   connect(bothButton,SIGNAL(clicked(bool)), this, SLOT(colorChoiceClicked(bool)));

   QVBoxLayout *colorLayout = new QVBoxLayout(lightColorGroupBox);
   colorLayout->addWidget(whiteLightButton);
   colorLayout->addWidget(redLightButton);
   colorLayout->addWidget(bothButton);

   QGridLayout *masterLayout = new QGridLayout(this);
   masterLayout->addWidget(lightLabel,0,0);
   masterLayout->addWidget(sliderLabel,1,0);
   masterLayout->addWidget(lightSlider,1,1,1,1);
   masterLayout->addWidget(sliderValueLabel,1,2,1,1);
   masterLayout->addWidget(lightTempLabel,2,0);
   masterLayout->addWidget(lightTempValueLabel,2,1);
   masterLayout->addWidget(lightHumidityLabel,2,2);
   masterLayout->addWidget(lightHumidityValueLabel,2,3);
   masterLayout->addWidget(receivedPowerLevelLabel,1,3);
   masterLayout->addWidget(receivedPowerLevelValueLabel,1,4);
   masterLayout->addWidget(receivedModeLabel,2,4);
   masterLayout->addWidget(receivedModeValueLabel,2,5);

   masterLayout->addWidget(lightColorGroupBox,3,0,1,2);
   masterLayout->addWidget(commsSinceLabel,3,2);
   masterLayout->addWidget(commsSinceValueLabel,3,3);
   masterLayout->addWidget(nackCountLabel,4,2);
   masterLayout->addWidget(nackCountValueLabel,4,3);




}

void LightControlWidget::setPosition(char *thePosition)
{
   lightLabel->setText((QString) thePosition);
}

void LightControlWidget::lightValueChange(int theNewValue)
{
   sliderValueLabel->setText(QString::number(theNewValue));
   emit changeLightLevel(theNewValue);
}

void LightControlWidget::updateLight( double humidity, double temperature, int lightLevel, int lightMode, double secsSince, int nackCount)
{
   lightHumidityValueLabel->setText(QString::number(humidity) + " %") ;
   lightTempValueLabel->setText(QString::number(temperature) + degreeSymbol) ;
   receivedPowerLevelValueLabel->setText(QString::number(lightLevel));

   if(1 == lightMode)
      {
         receivedModeValueLabel->setText("white") ;
      }
   else if(2 == lightMode)
      {
         receivedModeValueLabel->setText("red") ;
      }
   else
      {
         receivedModeValueLabel->setText(QString::number(lightMode) + " (unknown)"   );
      }
   commsSinceValueLabel->setText(QString::number(secsSince,'f',1));
   nackCountValueLabel->setText(QString::number(nackCount));

}


void LightControlWidget::colorChoiceClicked(bool value)
{
   if(value)
      {
         int theMode = -99;
         QRadioButton *sendingButton = (QRadioButton *)sender( );
         if(sendingButton == whiteLightButton)
            {
               theMode = 1;
            }
         else if(sendingButton == redLightButton)
            {
               theMode = 2;
            }
         else if(sendingButton == bothButton)
            {
               theMode = 3;
            }
         if(-99 != theMode)
            {
               emit changeLightMode(theMode);
            }
      }

}
