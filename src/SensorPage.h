#ifndef SENSORPAGE_H
#define SENSORPAGE_H

#include <QtWidgets>
#include "qledindicator.h"

typedef enum {
  GPS_TIMEOUT,
  CTD_TIMEOUT,
  FATHOMETER_TIMEOUT,
  ALTIMETER_TIMEOUT,
  ATTITUDE_TIMEOUT
} eTimeoutT;
class SensorPage : public QWidget
{
    Q_OBJECT
public:
    explicit    SensorPage(QWidget *parent = nullptr);
    void        setAttitude(double heading, double pitch, double roll);
    void        setCTD(double d, double t);
    void        setGPS(double lat, double lon);
    void        setFathometer(double depth);
    void        setAltimeter(double altitude);
    void        setTimeout(eTimeoutT whichOne, double theTimeout);
private:
    QGroupBox   *ctdGroupBox;
    QLabel      *dLabel;
    QLabel      *dValue;
    QLabel      *tLabel;
    QLabel      *tValue;
    QLabel      *ctdAgeLabel;
    QLabel      *ctdAgeValue;
    QDateTime   lastCTD;
    QLedIndicator *ctdLED;

    QGroupBox   *gpsGroupBox;
    QLabel      *latitudeLabel;
    QLabel      *latitudeValue;
    QLabel      *longitudeLabel;
    QLabel      *longitudeValue;
    QLabel      *gpsAgeLabel;
    QLabel      *gpsAgeValue;
    QDateTime   lastGPS;
    QLedIndicator *gpsLED;

    QGroupBox   *fathometerGroupBox;
    QLabel      *fathometerLabel;
    QLabel      *fathometerValue;
    QLabel      *fathometerAgeLabel;
    QLabel      *fathometerAgeValue;
    QDateTime  lastFathometer;
    QLedIndicator *fathometerLED;

    QGroupBox   *altimeterGroupBox;
    QLabel      *altimeterLabel;
    QLabel      *altimeterValue;
    QLabel      *altimeterAgeLabel;
    QLabel      *altimeterAgeValue;
    QDateTime   lastAltimeter;
    QLedIndicator *altimeterLED;

    QGroupBox   *microstrainGroupBox;
    QLabel      *pitchLabel;
    QLabel      *pitchValue;
    QLabel      *rollLabel;
    QLabel      *rollValue;
    QLabel      *headingLabel;
    QLabel      *headingValue;
    QLabel      *microstrainAgeLabel;
    QLabel      *microstrainAgeValue;
    QDateTime   lastAttitude;
    QLedIndicator *microstrainLED;


    QString     degreeSymbol;

    QTimer      *ageTimer;
    double      gpsTimeoutCriteria;
    double      altimeterTimeoutCriteria;
    double      fathometerTimeoutCriteria;
    double      ctdTimeoutCriteria;
    double      attitudeTimeoutCriteria;




signals:
private slots:
    void    ageTimeout();


};

#endif // SENSORPAGE_H
