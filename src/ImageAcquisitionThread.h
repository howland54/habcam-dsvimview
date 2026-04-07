#ifndef IMAGEACQUISITIONTHREAD_H
#define IMAGEACQUISITIONTHREAD_H
#include <string>

#include <poll.h>

#include <QThread>
#include <QDebug>
#include <QReadWriteLock>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <lcm/lcm.h>
#include <lcm/lcm-cpp.hpp>

#include "../../habcam-lcmtypes/image/image/image_t.hpp"
#include "../../habcam-lcmtypes/image/image/image_parameter_t.hpp"

#include "../../habcam-lcmtypes/marine_sensor/marine_sensor/marineSensorAltimeter_t.hpp"
#include "../../habcam-lcmtypes/marine_sensor/marine_sensor/MarineSensorFathometer_t.hpp"
#include "../../habcam-lcmtypes/marine_sensor/marine_sensor/MarineSensorCtd_t.hpp"
#include "../../habcam-lcmtypes/marine_sensor/marine_sensor/MarineSensorGPS_t.hpp"
#include "../../habcam-lcmtypes/marine_sensor/marine_sensor/MarineSensorAttitudeSensor_t.hpp"



using namespace std;



class ImageAcquisitionThread : public QThread
{
   Q_OBJECT
public:
   explicit ImageAcquisitionThread(int threadNum,QWidget *parent);
   void                                imageSignal();
   void                                setEqualize(bool shouldI);
   void                                setStretch(bool shouldI);
   bool                                getEqualize();
   bool                                getStretch();
   bool                                getHistogramDisplay();
   void                                setHistogramDisplay(bool doIt);
   void                                histogramSignal(QVector<float> theRHist,QVector<float> theGHist,QVector<float> thBHist);
   void                                setRescaleSize(int xSize, int ySize);
   cv::Size                            getDestSize();
   bool                                getisColor();
   void                                setColor(bool isIt);
   void                                setSubscriptionName(char *theName);
   void                                setParameterSubscriptionName(char *theName);
   void                                setActive(bool isIt);

private:

   lcm::Subscription                   *imageSub;
   lcm::Subscription                   *parameterSub;

   lcm::Subscription                   *attitudeSub;
   lcm::Subscription                   *fathometerSub;
   lcm::Subscription                   *altimeterSub;
   lcm::Subscription                   *ctdSub;
   lcm::Subscription                   *calcDepthSubscription;
   lcm::Subscription                   *gpsSubscription;

   int                                 skip;

   char                                subscriptionName[64];

   volatile bool                       stopped;
   int                                 fileno;

   int                                 whichThread;
   bool                                equalize;
   bool                                stretch;
   bool                                getHistogram;
   int                                 xrescaleSize;
   int                                 yrescaleSize;
   bool                                isColor;




protected:
    void    run();


signals:
    void    imUpdate(int theImageNumber);
    void    newHistogram(QVector<float> theRHist,QVector<float> theGHist,QVector<float> theBHist);

public slots:
};

#endif // IMAGEACQUISITIONTHREAD_H
