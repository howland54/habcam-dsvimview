#include "LcmThread.h"
#include "ImageAcquisitionThread.h"
#include "VimView.h"

extern ImageAcquisitionThread       *imageProviderThread[MAX_N_OF_CAMERAS];
extern lcm::LCM                     *myLcm;
extern VimView                      *theMainWindow;

namespace
{
class State {
public:

   int usefulVariable;
};
}

// handed to every shared subscription as its user pointer, so it has to outlive them all
static State lcmState;

void marine_sensor_altimeter_t_callback0(const lcm::ReceiveBuffer *rbuf, const std::string& channel,
                                  const marine_sensor::marineSensorAltimeter_t *altimeterData, State *user)
{
   theMainWindow->setAltitude(altimeterData->altitude);

}

void marine_sensor_fathometer_t_callback(const lcm::ReceiveBuffer *rbuf, const std::string& channel,
                                  const marine_sensor::MarineSensorFathometer_t *fathometerData, State *user)
{
   if(channel == "FATHOMETER")
      {
         theMainWindow->setFathometer(fathometerData->depth);
      }
   else if(channel == "CALC_DEPTH")
      {
         theMainWindow->setCalcDepth(fathometerData->depth);
      }
}

void marine_sensor_ctd_t_callback(const lcm::ReceiveBuffer *rbuf, const std::string& channel,
                           const marine_sensor::MarineSensorCtd_t *ctdData, State *user)
{
   theMainWindow->setCTD(ctdData->depth,ctdData->sea_water_temperature );

}

void marine_sensor_attitude_callback(const lcm::ReceiveBuffer *rbuf, const std::string& channel,
                           const marine_sensor::MarineSensorAttitudeSensor_t *msData, State *user)
{
  theMainWindow->setFishAttitude(msData->heading, msData->pitch, msData->roll);
}

void marine_sensor_gps_callback(const lcm::ReceiveBuffer *rbuf, const std::string& channel,
                                const marine_sensor::MarineSensorGPS_t *gpsData, State *user)
{
  theMainWindow->setGPS(gpsData->latitude, gpsData->longitude);
}
void ncd_io_callback(const lcm::ReceiveBuffer *rbuf, const std::string& channel, const ncd_io::NcdIoState_t *ncdData,State *user)
{
    theMainWindow->setNCDData(ncdData);
}


// std::stod throws on bad input, and an exception escaping an LCM callback ends the program
static bool parseParameterValue(const image::image_parameter_t *imageParameter, double &value)
{
   const char *text = imageParameter->value.c_str();
   char *end;
   errno = 0;
   value = strtod(text, &end);
   if(end == text || *end != '\0' || errno == ERANGE)
      {
         qWarning() << "parameter_t_callback: bad value" << imageParameter->value.c_str()
                    << "for" << imageParameter->key.c_str();
         return false;
      }
   return true;
}

void parameter_t_callback (const lcm::ReceiveBuffer *rbuf, const std::string& channel,const image::image_parameter_t *imageParameter, State *user)
{
   // LOGGING, RECORDING and WRITE_STATUS are system wide; only the per-camera keys need a valid camera
   int camNum = imageParameter->cameraNumber;
   bool isCameraKey = ("EXPOSURE" == imageParameter->key || "GAIN" == imageParameter->key || "TRIGGER_SOURCE" == imageParameter->key);
   if(isCameraKey && (camNum < 0 || camNum >= MAX_N_OF_CAMERAS || !imageProviderThread[camNum] || !imageProviderThread[camNum]->getIsActive()))
      {
         return;
      }
   if("EXPOSURE" == imageParameter->key)
      {
         double value;
         if(!parseParameterValue(imageParameter, value))
            {
               return;
            }
         value = value/1000.0;
         theMainWindow->setExposureActual(value, imageParameter->cameraNumber);
      }
   else if("LOGGING" == imageParameter->key)
      {
         double value;
         if(!parseParameterValue(imageParameter, value))
            {
               return;
            }
         if(value < 0)
            {
               theMainWindow->setWriteSuccess(false);
               value += 100.0;
            }
         else
            {
               theMainWindow->setWriteSuccess(true);
               value -= 100.0;
            }
         if(value > 0.0)
            {
               theMainWindow->setRecordingActallyPaused(false);
            }
         else
            {
               theMainWindow->setRecordingActallyPaused(true);
            }

      }
   else if("GAIN" == imageParameter->key)
      {
         double value;
         if(!parseParameterValue(imageParameter, value))
            {
               return;
            }
         theMainWindow->setGainActual(value, imageParameter->cameraNumber);
      }
   else if("RECORDING" == imageParameter->key)
      {
         bool isRecordingPaused;
         if("true" == imageParameter->value)
            {
               isRecordingPaused = false;
            }
         else
            {
               isRecordingPaused = true;
            }
         theMainWindow->setRecordingActallyPaused(isRecordingPaused);
      }
   else if("WRITE_STATUS" == imageParameter->key)
      {
         if("FAIL" == imageParameter->value)
            {
               theMainWindow->setWriteSuccess(false);
            }
         else
            {
               theMainWindow->setWriteSuccess(true);
            }
      }
   else if("TRIGGER_SOURCE" == imageParameter->key)
      {
         if("Software" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(SOFTWARE, imageParameter->cameraNumber);
            }
         else if("FixedRate" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(FIXED_RATE, imageParameter->cameraNumber);
            }
         else if("Line1" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(LINE1, imageParameter->cameraNumber);
            }
         else if("Line2" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(LINE2, imageParameter->cameraNumber);
            }
         else if("Line3" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(LINE3, imageParameter->cameraNumber);
            }
         else if("Line4" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(LINE4, imageParameter->cameraNumber);
            }
         else if("Freerun" == imageParameter->value)
            {
               theMainWindow->setTriggerSource(FREE_RUN, imageParameter->cameraNumber);
            }
         else
            {
               theMainWindow->setTriggerSource(UNKNOWN_TRIGGER_SOURCE, imageParameter->cameraNumber);
            }
      }

}


LcmThread::LcmThread() : QThread()
{
   stopped = false;
   parameterSub = nullptr;
   fathometerSub = nullptr;
   altimeterSub = nullptr;
   ctdSub = nullptr;
   attitudeSub = nullptr;
   calcDepthSubscription = nullptr;
   gpsSubscription = nullptr;
   ncdIOSubscription = nullptr;
}

// call from the GUI thread before start(), see ImageAcquisitionThread::subscribe
void LcmThread::subscribe()
{
   lcmState.usefulVariable = 1;

   parameterSub = myLcm->subscribeFunction("M_STATUS_PARAMETERS",&parameter_t_callback, &lcmState);
   fathometerSub = myLcm->subscribeFunction("FATHOMETER",&marine_sensor_fathometer_t_callback, &lcmState);
   altimeterSub = myLcm->subscribeFunction("ALTIMETER",&marine_sensor_altimeter_t_callback0, &lcmState);
   ctdSub = myLcm->subscribeFunction("CTD",&marine_sensor_ctd_t_callback, &lcmState);
   attitudeSub = myLcm->subscribeFunction("ATTITUDE", marine_sensor_attitude_callback, &lcmState);
   calcDepthSubscription  = myLcm->subscribeFunction("CALC_DEPTH",&marine_sensor_fathometer_t_callback, &lcmState);
   gpsSubscription  = myLcm->subscribeFunction("GPS",&marine_sensor_gps_callback, &lcmState);
   ncdIOSubscription = myLcm->subscribeFunction("NCD_IO",&ncd_io_callback, &lcmState);
}

// the only place myLcm is handled; it dispatches the camera image subscriptions as well
void LcmThread::run()
{
   bool lcmFailing = false;
   // handleTimeout returns after 50 ms with no traffic, so the stop request is always seen
   while (!stopped)
      {
         if(myLcm->handleTimeout(50) < 0)
            {
               // an LCM error returns at once; back off so the loop doesn't spin, and only log the change
               if(!lcmFailing)
                  {
                     qWarning() << "LcmThread: LCM handleTimeout failed, retrying";
                     lcmFailing = true;
                  }
               usleep(50000);
            }
         else if(lcmFailing)
            {
               qWarning() << "LcmThread: LCM handling recovered";
               lcmFailing = false;
            }
      }
}

// call only after wait(), so nothing is inside handleTimeout
void LcmThread::unsubscribeAll()
{
   lcm::Subscription **subscriptions[] = {&parameterSub, &fathometerSub, &altimeterSub, &ctdSub,
                                          &attitudeSub, &calcDepthSubscription, &gpsSubscription, &ncdIOSubscription};
   for(lcm::Subscription **thisSubscription : subscriptions)
      {
         if(*thisSubscription)
            {
               myLcm->unsubscribe(*thisSubscription);
               *thisSubscription = nullptr;
            }
      }
}

void LcmThread::requestStop()
{
   stopped = true;
}
