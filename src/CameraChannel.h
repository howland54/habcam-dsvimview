#ifndef CAMERACHANNEL_H
#define CAMERACHANNEL_H
#include <QObject>
#include <QVector>

#include <opencv2/core/core.hpp>

#include <lcm/lcm-cpp.hpp>

#include "../../habcam-lcmtypes/image/image/image_t.hpp"



using namespace std;



// One per camera: its image subscription, display settings and the signals to the GUI.
// image_t_callback runs on the LcmThread, not on a thread of its own.
class CameraChannel : public QObject
{
   Q_OBJECT
public:
   explicit CameraChannel(int threadNum,QObject *parent = nullptr);
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
   int                                 getCameraNumber();
   bool                                skipThisFrame();
   void                                setSubscriptionName(char *theName);
   void                                subscribe();
   void                                unsubscribeAll();
   void                                setActive(bool isIt);
   bool                                getIsActive();

private:

   lcm::Subscription                   *imageSub;

   int                                 skip;

   char                                subscriptionName[64];

   int                                 whichThread;
   bool                                equalize;
   bool                                stretch;
   bool                                getHistogram;
   int                                 xrescaleSize;
   int                                 yrescaleSize;
   bool                                isActive;




signals:
    void    imUpdate(int theImageNumber);
    void    newHistogram(QVector<float> theRHist,QVector<float> theGHist,QVector<float> theBHist);

public slots:
};

#endif // CAMERACHANNEL_H
