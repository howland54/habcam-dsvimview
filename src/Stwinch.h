
#ifndef STWINCH_H
#define STWINCH_H

#include <QtWidgets>
#include <QtNetwork>

#define	VERSION_STRING	"1.0"

#define	DEFAULT_WINCH_IP_ADDRESS	"127.0.0.1"
#define	DEFAULT_WINCH_SOCKET		30001
#define	DEFAULT_WINCH_IN_SOCKET		30000

#define	DEFAULT_FLYING_HISTORY	60
#define   MAX_FLYING_HISTORY      300
#define	MAX_N_OF_FLYING_POINTS	MAX_FLYING_HISTORY+1
#define  KNOTS2MPS 0.514

#define	MAX_MESSAGE_LENGTH	1024

#define  NO_FATHOMETER_DATA   -9876.5432
#define  NO_DEPTH_DATA        9876.5432
#define  DEFAULT_DEPTH_FILTER_CONSTANT 0.8
#define  DEFAULT_FATHOMETER_FILTER_CONSTANT  0.8

#define  DEFAULT_MINIMUM_ALTITUDE   0.1
#define  DEFAULT_MAXIMUM_ALTITUDE  9.9
#include <QtWidgets>
#include <qwt_plot.h>


#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
//#include <qwt_legend_item.h>
#include <qwt_knob.h>

#include "qledindicator.h"
#include "compilation.h"

class BottomCurve: public QwtPlotCurve
{
public:
    BottomCurve(const QString &title):
        QwtPlotCurve(title)
    {
#if QT_VERSION >= 0x040000
        setRenderHint(QwtPlotItem::RenderAntialiased);
#endif
    }
    void	setColor(const QColor &color);
    void	setMyBrush(const QColor &color);

    
}; 

class StWinch : public QWidget
{
    Q_OBJECT

public:
    StWinch(char	*startup_file_name);
    void    setAltitude(double theAltitude);
    void    setFathometer(double theFathometerValue);
    void    setFishDepth(double theDepthValue);
    void    setCalcDepth(double theCalcDepthValue);

private slots:
    void	oneHertzUpdate();
    void	sledSocketDataPending();
    void	changeSpeed(int val);
    void  changeDepthFilter(int val);
    void  changeFathometerFilter(int val);
    void  processDepthBounds();
    void  changeAltBounds(double theBound);

private:

    QTimer		*flyingPlotTimer;
    QwtPlot		*flyingPlot;
    QwtLegend   *legend;
    BottomCurve	*bottomCurve;
    BottomCurve	*fishCurve;
    BottomCurve	*computedDepthCurve;
    QLabel		*currentTimeLabel;
    QLabel		*dataLight;
    QLabel		*incomingLabel;
    QLabel		*calcAltimeterLabel;
    QSlider		*speedAdvanceSlider;
    QSlider		*depthFilterSlider;
    QSlider		*fathometerFilterSlider;

    QDoubleSpinBox *minAltSpinBox;
    QDoubleSpinBox *maxAltSpinBox;

    double         minAltBound;
    double         maxAltBound;
    QLedIndicator  *altitudeIndicator;


    QLabel         *minAltLabel;
    QLabel         *maxAltLabel;

    QLineEdit				*yAxisMinEdit;
    QLineEdit				*yAxisMaxEdit;
    QLabel					*minLabel;
    QLabel					*maxLabel;

    double					yMinBound;
    double					yMaxBound;



    QLabel		*speedLabel;
    QLabel		*fathometerFilterLabel;

    QLabel		*depthFilterLabel;


    double	depth[MAX_N_OF_FLYING_POINTS];
    double	altitude[MAX_N_OF_FLYING_POINTS];
    double	bottomDepth[MAX_N_OF_FLYING_POINTS];
    double	time[MAX_N_OF_FLYING_POINTS];
    int		nOfFlyingPlotPoints;
    int      flyingHistory;

    double	fathometerBottomDepth;
    double   filteredFathometer;
    double	fishDepth;
    double   filteredDepth;
    double	calculatedDepth;
    double	calculatedAltimeter;
    double   shipSpeed;

    double   depthFilterConstant;
    double   fathometerFilterConstant;


    QUdpSocket				*winchSocket;
    unsigned short			winchInSocketNumber;

    QGroupBox				*textGroupBox;
    QLabel					*fathometerLabel;
    QLabel					*fishDepthLabel;
    QLabel					*altimeterLabel;


    double					timeData[MAX_FLYING_HISTORY];
    double					ditheredTimeData[MAX_FLYING_HISTORY];
    double					fathometerAdvanceValue;


    
};

#ifdef WINDOWS
extern	StWinch *stW;
#else
extern	StWinch *stW;
#endif

#endif

