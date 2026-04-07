#include <QtWidgets>
#include <qwt_scale_engine.h>

#include "../../dsvimlib/include/IniFile.h"
#include "Stwinch.h"

// todo
/*
  autoscale y axis
  alert on low altitude
  */
typedef QPalette Palette;
#if 0
class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw(const QDateTime &base):
        baseTime(base)
    {
    }
    virtual QwtText label(double v) const
    {

        double w = v - flyingHistory/2.0 ;

        //QTime upTime = baseTime.addSecs((int)+v).time();
        return QString::number(w,'f',0);
    }
private:
    QDateTime baseTime;
};
#endif
#if 1
void BottomCurve::setColor(const QColor &color)
{
    QColor c = color;
    setPen(c);

}

void BottomCurve::setMyBrush(const QColor &color)
{
    QColor c = color;
    c.setAlpha(50);
    QBrush brush;
    brush.setStyle(Qt::Dense4Pattern);

    setBrush(brush);


}

#endif

StWinch::StWinch(char	*startup_file_name)
{
    IniFile	iniFile;
    int	startupReturnCode = iniFile.openIni(startup_file_name);
    if(startupReturnCode != GOOD_INI_FILE_READ)
        {
            return ;
        }

    calculatedDepth = 0.0;
    fishDepth = -80.0;
    fathometerBottomDepth = -90.0;
    nOfFlyingPlotPoints = 0;
    fathometerAdvanceValue = 0.0;
    shipSpeed = 0.0;
    filteredFathometer = NO_FATHOMETER_DATA;
    filteredDepth = NO_DEPTH_DATA;


    winchInSocketNumber = iniFile.readInt("GENERAL","WINCH_INCOMING_SOCKET",DEFAULT_WINCH_IN_SOCKET);
    flyingHistory = iniFile.readInt("GENERAL","FLYING_HISTORY",DEFAULT_FLYING_HISTORY);
    depthFilterConstant = iniFile.readDouble("GENERAL", "DEPTH_FILTER_CONSTANT",DEFAULT_DEPTH_FILTER_CONSTANT);
    fathometerFilterConstant = iniFile.readDouble("GENERAL", "FATHOMETER_FILTER_CONSTANT",DEFAULT_FATHOMETER_FILTER_CONSTANT);

    minAltBound = iniFile.readDouble("GENERAL", "MINIMUM_ALTITUDE", DEFAULT_MINIMUM_ALTITUDE);
    maxAltBound = iniFile.readDouble("GENERAL", "MAXIMUM_ALTITUDE", DEFAULT_MAXIMUM_ALTITUDE);

    winchSocket = new   QUdpSocket(this);
    winchSocket->bind(winchInSocketNumber);
    connect (winchSocket, SIGNAL(readyRead()),this,SLOT(sledSocketDataPending()));

    iniFile.closeIni();

    flyingPlotTimer	= new QTimer(this);
    connect(flyingPlotTimer,SIGNAL(timeout()),this,SLOT(oneHertzUpdate()));

    yAxisMinEdit = new QLineEdit;
    yAxisMinEdit->setValidator(new QDoubleValidator(-500.0, 500.0, 1, this));
    yAxisMinEdit->setMaxLength(4);
    yAxisMinEdit->setMaximumWidth(40);
    connect(yAxisMinEdit,SIGNAL(editingFinished()),this,SLOT(processDepthBounds()));
    yAxisMaxEdit = new QLineEdit;
    yAxisMaxEdit->setValidator(new QDoubleValidator(-500.0,500.0, 1, this));
    yAxisMaxEdit->setMaxLength(4);
    yAxisMaxEdit->setMaximumWidth(40);
    connect(yAxisMaxEdit,SIGNAL(editingFinished()),this,SLOT(processDepthBounds()));
    minLabel = new QLabel("min depth");
    maxLabel = new QLabel("max depth");



    flyingPlot = new QwtPlot();

    flyingPlot->setStyleSheet("background-color: lightblue;"
                              "color: black;");
    flyingPlot->setMinimumWidth(500);
    flyingPlot->setMinimumHeight(500);

    nOfFlyingPlotPoints = 0;
    QDateTime t;
    flyingPlot->setAxisTitle(QwtPlot::xBottom, "time delta");
    //flyingPlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(t.currentDateTime().addSecs(-flyingHistory).toUTC()));
    //flyingPlot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw(t.currentDateTime().addSecs(-flyingHistory/2.0).toUTC()));
    flyingPlot->setAxisScale(QwtPlot::xBottom,0, flyingHistory);
    flyingPlot->setAxisLabelRotation(QwtPlot::xBottom, -50.0);
    flyingPlot->setAxisLabelAlignment(QwtPlot::xBottom, Qt::AlignLeft | Qt::AlignBottom);

    QwtScaleEngine *yAxisScaleEngine = flyingPlot->axisScaleEngine(QwtPlot::yLeft );

    //yAxisScaleEngine->setAttribute(QwtScaleEngine::Inverted,true);



    bottomCurve = new BottomCurve("Bottom");



    bottomCurve->setColor(Qt::black);
    QBrush brush;
    brush.setStyle(Qt::Dense4Pattern);
    bottomCurve->setBrush(brush);
    bottomCurve->attach(flyingPlot);

    fishCurve = new BottomCurve("Fish");
    fishCurve->setColor(Qt::red);

    fishCurve->attach(flyingPlot);

    computedDepthCurve = new BottomCurve("Computed Bottom");
    computedDepthCurve->setColor(Qt::blue);

    computedDepthCurve->attach(flyingPlot);

    legend = new QwtLegend;
    flyingPlot->insertLegend(legend, QwtPlot::RightLegend);

    for ( int i = 0; i < flyingHistory; i++ )
        {
            timeData[i] = -i;
        }

    flyingPlotTimer	= new QTimer(this);
    connect(flyingPlotTimer,SIGNAL(timeout()),this,SLOT(oneHertzUpdate()));

    dataLight = new QLabel("incoming");
    dataLight->setPixmap(QPixmap(":/images/tinyRedLED.bmp"));

    textGroupBox = new QGroupBox(tr("status data"));
    fathometerLabel = new QLabel(tr("fathometer: "));
    fishDepthLabel = new QLabel(tr("depth: "));
    altimeterLabel = new QLabel(tr("alt: "));

    altitudeIndicator = new QLedIndicator(this);
    altitudeIndicator->setOffColor1(Qt::yellow);
    altitudeIndicator->setOffColor2(Qt::yellow);

    calcAltimeterLabel = new QLabel(tr("calc altimeter: "));
    currentTimeLabel = new QLabel(tr("current time: "));

    minAltSpinBox = new QDoubleSpinBox();
    maxAltSpinBox = new QDoubleSpinBox();

    minAltSpinBox->setMinimum(0.0);
    minAltSpinBox->setMaximum(10.0);
    minAltSpinBox->setSingleStep(0.1);

    minAltSpinBox->setValue(minAltBound);

    maxAltSpinBox->setMinimum(0.0);
    maxAltSpinBox->setMaximum(10.0);
    maxAltSpinBox->setSingleStep(0.1);
    maxAltSpinBox->setValue(maxAltBound);


    minAltLabel = new QLabel("minimum Altitude:");
    maxAltLabel = new QLabel("maximum Altitude:");

    QGridLayout *altBoundsLayout = new QGridLayout;
    altBoundsLayout->addWidget(minAltLabel,0,0);
    altBoundsLayout->addWidget(minAltSpinBox,0,1);
    altBoundsLayout->addWidget(maxAltLabel,1,0);
    altBoundsLayout->addWidget(maxAltSpinBox,1,1);

    connect(minAltSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeAltBounds(double)));
    connect(maxAltSpinBox, SIGNAL(valueChanged(double)), this, SLOT(changeAltBounds(double)));



    speedAdvanceSlider = new QSlider(Qt::Horizontal);
    speedAdvanceSlider->setMinimum(0);
    speedAdvanceSlider->setMaximum(100);
    depthFilterSlider = new QSlider(Qt::Horizontal);
    depthFilterSlider->setMinimum(0);
    depthFilterSlider->setMaximum(100);
    fathometerFilterSlider = new QSlider(Qt::Horizontal);
    fathometerFilterSlider->setMinimum(0);
    fathometerFilterSlider->setMaximum(100);

    QFont theFont = altimeterLabel->font();
    theFont.setBold(true);
    theFont.setPointSize(42);
    altimeterLabel->setFont(theFont);

    theFont.setPointSize(30);
    theFont.setBold(false);
    fishDepthLabel->setFont(theFont);
    fathometerLabel->setFont(theFont);


    connect(speedAdvanceSlider, SIGNAL(valueChanged( int )), this, SLOT(changeSpeed(int )));
    connect(depthFilterSlider, SIGNAL(valueChanged( int )), this, SLOT(changeDepthFilter(int )));
    connect(fathometerFilterSlider, SIGNAL(valueChanged( int )), this, SLOT(changeFathometerFilter(int )));

    speedLabel = new QLabel(tr("ship speed 0 kts"));
    fathometerFilterLabel = new QLabel(tr("fathometer filter"));
    depthFilterLabel = new QLabel(tr("depth filter"));


    QVBoxLayout	*textLayout = new QVBoxLayout;
    textLayout->addWidget(fishDepthLabel);
    QHBoxLayout *altitudeLayout = new QHBoxLayout();
    altitudeLayout->addWidget(altimeterLabel);
    altitudeLayout->addWidget(altitudeIndicator);
    textLayout->addLayout(altitudeLayout);

    textLayout->addWidget(fathometerLabel);


    textLayout->addWidget(calcAltimeterLabel);
    //textLayout->addSpacing(2);

    QHBoxLayout	*sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(speedLabel);
    sliderLayout->addWidget(speedAdvanceSlider);
    speedAdvanceSlider->setValue((int)(shipSpeed * 10));
    speedLabel->setText("ship speed (kt)\t" + QString::number(shipSpeed,'f',1));


    QHBoxLayout	*depthSliderLayout = new QHBoxLayout;
    depthSliderLayout->addWidget(depthFilterLabel);
    depthSliderLayout->addWidget(depthFilterSlider);
    depthFilterSlider->setValue((int)(depthFilterConstant*100));
    depthFilterLabel->setText("depth filter\t" + QString::number(depthFilterConstant,'f',1) );


    QHBoxLayout	*fathometerSliderLayout = new QHBoxLayout;
    fathometerSliderLayout->addWidget(fathometerFilterLabel);
    fathometerSliderLayout->addWidget(fathometerFilterSlider);
    fathometerFilterSlider->setValue((int)(fathometerFilterConstant*100));
    fathometerFilterLabel->setText("fathometer filter\t" + QString::number(fathometerFilterConstant,'f',1) );

    textLayout->addLayout(altBoundsLayout);
    textLayout->addLayout(sliderLayout);
    textLayout->addLayout(fathometerSliderLayout);
    textLayout->addLayout(depthSliderLayout);
    textLayout->addWidget(currentTimeLabel);

    textGroupBox->setLayout(textLayout);

    QVBoxLayout	*parameterLayout = new QVBoxLayout;
    parameterLayout->addWidget(yAxisMaxEdit);
    parameterLayout->addWidget(maxLabel);

    parameterLayout->addStretch(5);
    parameterLayout->addWidget(minLabel);
    parameterLayout->addWidget(yAxisMinEdit);
    QHBoxLayout *mainHLayout = new QHBoxLayout();

    mainHLayout->addLayout(parameterLayout);
    mainHLayout->addWidget(flyingPlot,5);
    mainHLayout->addWidget(textGroupBox);

    setLayout(mainHLayout);


    incomingLabel = new QLabel(tr("incoming data:"));
    flyingPlotTimer->start(1000);

}

void StWinch::setFathometer(double theFathometerValue)
{
   fathometerBottomDepth = (theFathometerValue);
   double trialFath = theFathometerValue;
   if(NO_FATHOMETER_DATA == filteredFathometer)
       {
           filteredFathometer = trialFath;
       }
   else
      {
           filteredFathometer =  fathometerFilterConstant * trialFath  + (1.0 - fathometerFilterConstant)* filteredFathometer;
       }
   fathometerLabel->setText("fathometer: " + QString::number(fathometerBottomDepth,'f',1) + " m");

}

void StWinch::setFishDepth(double theDepthValue)
{
   double trialDepth = theDepthValue;
   fishDepth = trialDepth;
   calculatedAltimeter = calculatedDepth - fishDepth;
   if(NO_DEPTH_DATA == filteredDepth)
       {
           filteredDepth = fishDepth;
       }
   else
      {
           filteredDepth =  depthFilterConstant * fishDepth  + (1.0 - depthFilterConstant)* filteredDepth;
       }
   fishDepthLabel->setText("fish depth: " + QString::number(fishDepth,'f',1) + " m");
}
void StWinch::setCalcDepth(double theCalcDepthValue)
{
   double trialcalc = theCalcDepthValue;
   calculatedDepth = trialcalc;
   calculatedAltimeter = calculatedDepth - fishDepth;
   calcAltimeterLabel->setText("calc depth: " + QString::number(calculatedDepth,'f',1) + " m");

}

void StWinch::setAltitude(double theAltitude)
{
   double trialAlt = theAltitude;
   altimeterLabel->setText("alt: " + QString::number(trialAlt,'f',1) + " m"); // should this really be trialAlt?
   QFont theAltimeterFont = altimeterLabel->font();
   if(trialAlt > maxAltBound)
       {
           altitudeIndicator->setOffColor1(Qt::yellow);
           altitudeIndicator->setOffColor2(Qt::yellow);

       }
   else if(trialAlt < minAltBound)
       {
           altitudeIndicator->setOffColor1(Qt::red);
           altitudeIndicator->setOffColor2(Qt::red);
       }
   else
       {
           altitudeIndicator->setOffColor1(Qt::green);
           altitudeIndicator->setOffColor2(Qt::green);
       }

}
void	StWinch::changeAltBounds(double theBound)
{
    QDoubleSpinBox *theSpinBox = (QDoubleSpinBox *)sender();
    if(theSpinBox == minAltSpinBox)
        {
            minAltBound = theBound;
        }
    else
        {
            maxAltBound = theBound;
        }
}

void	StWinch::changeSpeed( int val)
{
    double myValue = val/10.0;
    if(myValue < 0.01)
        {
            myValue = 0.0 ;
        }

    speedLabel->setText("ship speed " + QString::number(myValue,'f',1) + " knots");

    if(0.0 == myValue)
        {
            fathometerAdvanceValue =  0.0;
        }
    else
        {
            fathometerAdvanceValue = fishDepth/(KNOTS2MPS *(double)myValue);
        }
    shipSpeed = (double)myValue;


}

void	StWinch::changeDepthFilter( int val)
{
    double myValue = val/100.0;
    depthFilterLabel->setText("depth filter\t" + QString::number(myValue,'f',1) );
    depthFilterConstant = myValue;

}

void	StWinch::changeFathometerFilter( int val)
{
    double myValue = val/100.0;
    fathometerFilterLabel->setText("fathometer filter\t" + QString::number(myValue,'f',1) );
    fathometerFilterConstant = myValue;


}

/*  weight incoming data like this

output[ 0 ] = input [ 0 ];
for ( ii=1; ii<count; ii++ )
{
  output[ ii ] = weight * input [ii ] + ( 1.0 - weight ) * output[ ii-1 ];

In addition, while it's not exactly the strict equivalent of the actual formula, the EWMA can be calculated extremely quickly and compactly with the equations:

newEWMA = (1/10)newSample + (9/10)oldEWMA

or, equivalently

newEWMA = (newSample + (N-1)oldEWMA) / N
}

*/

void StWinch::sledSocketDataPending()
{

    QUdpSocket *theSocket = (QUdpSocket *)sender();

    while(theSocket->hasPendingDatagrams()){

            QByteArray	buffer(theSocket->pendingDatagramSize(),0);
            theSocket->readDatagram(buffer.data(),buffer.size());
            double	trialFath,trialDepth,trialcalc,trialAlt;
            int items = sscanf(buffer.data(),"WINCH DEPTH %lf FATHOMETER %lf CALC_DEPTH %lf ALT %lf",&trialDepth,&trialFath,&trialcalc,&trialAlt);
            if(4 == items)
                {

                    fathometerBottomDepth = (trialFath);
                    fishDepth = trialDepth;
                    if(NO_DEPTH_DATA == filteredDepth)
                        {
                            filteredDepth = fishDepth;
                        }
                    else{
                            filteredDepth =  depthFilterConstant * fishDepth  + (1.0 - depthFilterConstant)* filteredDepth;
                        }

                    if(NO_FATHOMETER_DATA == filteredFathometer)
                        {
                            filteredFathometer = trialFath;
                        }
                    else{
                            filteredFathometer =  fathometerFilterConstant * trialFath  + (1.0 - fathometerFilterConstant)* filteredFathometer;
                        }

                    calculatedDepth = trialcalc;
                    calculatedAltimeter = calculatedDepth - fishDepth;

                    fathometerLabel->setText("fathometer: " + QString::number(fathometerBottomDepth,'f',1) + " m");
                    fishDepthLabel->setText("fish depth: " + QString::number(fishDepth,'f',1) + " m");

                    calcAltimeterLabel->setText("calc depth: " + QString::number(calculatedDepth,'f',1) + " m");
                    altimeterLabel->setText("alt: " + QString::number(trialAlt,'f',1) + " m"); // should this really be trialAlt?
                    QFont theAltimeterFont = altimeterLabel->font();
                    if(trialAlt > maxAltBound)
                        {
                            altitudeIndicator->setOffColor1(Qt::yellow);
                            altitudeIndicator->setOffColor2(Qt::yellow);

                        }
                    else if(trialAlt < minAltBound)
                        {
                            altitudeIndicator->setOffColor1(Qt::red);
                            altitudeIndicator->setOffColor2(Qt::red);
                        }
                    else
                        {
                            altitudeIndicator->setOffColor1(Qt::green);
                            altitudeIndicator->setOffColor2(Qt::green);
                        }

                }


        }
}

void StWinch::processDepthBounds()
{
    QLineEdit *theSender = (QLineEdit *)sender();

    if(theSender == yAxisMinEdit)
        {
            QString theText = yAxisMinEdit->text();
            yMinBound = theText.toDouble();
        }

    else if(theSender == yAxisMaxEdit)
        {
            QString theText = yAxisMaxEdit->text();
            yMaxBound = theText.toDouble();
        }
    flyingPlot->setAxisScale(QwtPlot::yLeft, yMinBound, yMaxBound);
    flyingPlot->replot();

}

void	StWinch::oneHertzUpdate()
{

    for ( int i = nOfFlyingPlotPoints; i > 0; i-- )
        {

            if ( i < flyingHistory )
                {
                    depth[i] = depth[i-1];
                    altitude[i] = altitude[i-1];
                    bottomDepth[i] = bottomDepth[i-1]; // where does this data get filled in?

                }
        }
    depth[0] = -fishDepth;
    altitude[0] = -calculatedDepth;
    bottomDepth[0] = -fathometerBottomDepth;

    if ( nOfFlyingPlotPoints < flyingHistory )
        {
            nOfFlyingPlotPoints++;
        }
#if 0
    for ( int j = 0; j < flyingHistory; j++ ){
            timeData[j]++;
        }
#endif

    for ( int j = 0; j < flyingHistory; j++ )
        {
            ditheredTimeData[j] = timeData[j] + fathometerAdvanceValue;
        }



    flyingPlot->setAxisScale(QwtPlot::xBottom,
                             timeData[flyingHistory-1], timeData[0] + fathometerAdvanceValue);
    //flyingPlot->setAxisScale(QwtPlot::yLeft,
    //         -60.0, -20.0);
    bottomCurve->setRawSamples(ditheredTimeData, bottomDepth, nOfFlyingPlotPoints);

    fishCurve->setRawSamples(timeData, depth, nOfFlyingPlotPoints);
    computedDepthCurve->setRawSamples(timeData, altitude, nOfFlyingPlotPoints);
    bottomCurve->setBaseline(yMinBound);
    flyingPlot->replot();

    QDateTime	currentDT;
    QString nowString = currentDT.currentDateTime().toUTC().toString("yyyy/MM/dd hh:mm:ss.zzz");
    currentTimeLabel->setText("current time:" + nowString);

}



