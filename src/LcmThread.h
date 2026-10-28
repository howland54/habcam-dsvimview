#ifndef LCMTHREAD_H
#define LCMTHREAD_H
#include <atomic>

#include <QThread>

#include <lcm/lcm-cpp.hpp>

// handles the shared LCM instance: subscribes the sensor, status and NCD channels and runs the
// one handle loop, which also dispatches the camera image subscriptions. It runs whether or not
// any cameras are configured.
class LcmThread : public QThread
{
   Q_OBJECT
public:
   LcmThread();
   void                                subscribe();
   void                                unsubscribeAll();
   void                                requestStop();

protected:
   void                                run();

private:
   std::atomic<bool>                   stopped;
   lcm::Subscription                   *parameterSub;
   lcm::Subscription                   *fathometerSub;
   lcm::Subscription                   *altimeterSub;
   lcm::Subscription                   *ctdSub;
   lcm::Subscription                   *attitudeSub;
   lcm::Subscription                   *calcDepthSubscription;
   lcm::Subscription                   *gpsSubscription;
   lcm::Subscription                   *ncdIOSubscription;
};

#endif // LCMTHREAD_H
