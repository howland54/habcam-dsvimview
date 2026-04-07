#include "ImageAcquisitionThread.h"
#include "VimView.h"
#include "SensorPage.h"

extern ImageAcquisitionThread       *imageProviderThread[MAX_N_OF_CAMERAS];
extern QReadWriteLock               imageLock[MAX_N_OF_CAMERAS];

//extern cv::Mat    cvImage;
extern cv::Mat    displayImage[MAX_N_OF_CAMERAS];

extern lcm::LCM                            *myLcm;
extern VimView *theMainWindow;
extern VimView *sensorPage;

bool isActive;

//long int imageCount;

char mainWindowName[100];

int leftSkip;
int rightSkip;
extern int                                 globalImageSkipValue;



class State {
public:

   int usefulVariable;
};

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


void parameter_t_callback (const lcm::ReceiveBuffer *rbuf, const std::string& channel,const image::image_parameter_t *imageParameter, State *user)
{
   if(!isActive)
      {
         return;
      }
   if("EXPOSURE" == imageParameter->key)
      {
         double value = std::stod(imageParameter->value);
         value = value/1000.0;
         theMainWindow->setExposureActual(value, imageParameter->cameraNumber);
      }
   else if("LOGGING" == imageParameter->key)
      {
         double value = std::stod(imageParameter->value);
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
         double value = std::stod(imageParameter->value);
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

void image_t_callback (const lcm::ReceiveBuffer *rbuf, const std::string& channel,const image::image_t *image, State *user)
{

   int camNum = 99;
   // qDebug() << "in callback channel = " << channel.c_str();


   if(channel == "LeftColor")
      {
         camNum = 0;
         leftSkip++;
         if(leftSkip % globalImageSkipValue)
            {
               return;
            }
         else
            {
               leftSkip = 0;
            }
      }
   else if(channel == "RightColor")
      {
         camNum = 1;
         rightSkip++;
         if(rightSkip % globalImageSkipValue)
            {

               return;
            }
         else
            {
               rightSkip = 0;
            }
      }
   else
      {
         return;
      }
   imageLock[camNum].lockForWrite();


   cv::Mat    colorImage = cv::Mat(image->height, image->width, CV_8UC3, (void *)image->data.data());
   //cv::Mat    colorImage(image->width, image->height, CV_16UC3);
   //cvImage = cv::Mat(image->height, image->width, CV_8U, (void *)image->data.data(),image->row_stride);
   //cv::Mat colorImage(image->width, image->height, CV_8UC3);
   //cv::Mat colorImage(image->width, image->height, CV_8UC3);
   //cv::cvtColor(inputImage,colorImage,cv::COLOR_BayerBG2RGB,0);
   // equalize the histogram
   cv::Size destSize = imageProviderThread[camNum]->getDestSize();
   bool isColor = imageProviderThread[camNum]->getisColor();
   if(isColor)
      {
        // cv::cvtColor(inputImage,colorImage,cv::COLOR_BayerRG2BGR,0);
          /*cv::namedWindow("windowName"); // Create a window

          cv::imshow("windowName", colorImage); // Show our image inside the created window.

          cv::waitKey(0); // Wait for any keystroke in the window

          cv::destroyWindow("windowName"); //destroy the created window*
*/
      }
   //if(!isColor)
   {
      bool shouldIHistogram = imageProviderThread[camNum]->getHistogramDisplay();
      if(shouldIHistogram)
         {
            int histSize = 256;
            float range[] = {0,256};
            const float *histRange = {range};
            cv::Mat histogram;
            bool uniform = true;
            bool accumulate = false;
            vector<cv::Mat> bgr_planes;
            cv::split( colorImage, bgr_planes );

            cv::Mat b_hist, g_hist, r_hist;

            cv::calcHist(&bgr_planes[0],1,0,cv::Mat(),b_hist, 1,&histSize, &histRange,uniform, accumulate);
            cv::calcHist(&bgr_planes[1],1,0,cv::Mat(),g_hist, 1,&histSize, &histRange,uniform, accumulate);
            cv::calcHist(&bgr_planes[2],1,0,cv::Mat(),r_hist, 1,&histSize, &histRange,uniform, accumulate);

            QVector<float> passableRHist(histSize);
            QVector<float> passableGHist(histSize);
            QVector<float> passableBHist(histSize);
            for(int begin = 0; begin < histSize; begin++)
               {
                  passableRHist[begin] = r_hist.at<float>(begin);
                  passableGHist[begin] = g_hist.at<float>(begin);
                  passableBHist[begin] = b_hist.at<float>(begin);
               }
            imageProviderThread[camNum]->histogramSignal(passableRHist,passableGHist,passableBHist);

         }

      bool shouldIEqualize = imageProviderThread[camNum]->getEqualize();
      bool shouldIStretch = imageProviderThread[camNum]->getStretch();
      if(shouldIEqualize)
         {
            cv::Mat equalized;
            cv::Mat ycrcb;
            cv::cvtColor(colorImage,ycrcb,cv::COLOR_BGR2YCrCb);

            vector<cv::Mat> channels;
            cv::split(ycrcb,channels);

            cv::equalizeHist(channels[0], channels[0]);

            cv:: merge(channels,ycrcb);

            cvtColor(ycrcb,equalized,cv::COLOR_YCrCb2BGR);
            //cv::equalizeHist(inputImage,equalized);
            cv::resize(equalized,displayImage[camNum],destSize,0.0,0.0);
         }
      else if(shouldIStretch)
         {
            cv::Mat stretched;
            cv::Mat ycrcb;
            cv::cvtColor(colorImage,ycrcb,cv::COLOR_BGR2YCrCb);

            vector<cv::Mat> channels;
            cv::split(ycrcb,channels);
            cv::normalize(channels[0], channels[0], 0, 255, cv::NORM_MINMAX);
            cv:: merge(channels,ycrcb);

            cvtColor(ycrcb,stretched,cv::COLOR_YCrCb2BGR);
            cv::resize(stretched,displayImage[camNum],destSize,0.0,0.0);

         }
      else
         {
            cv::resize(colorImage,displayImage[camNum],destSize,0.0,0.0);
         }
   }
   //else
   //{
     //cv::resize(colorImage,displayImage[camNum],destSize,0.0,0.0);
   //}
   imageProviderThread[camNum]->imageSignal();

  // imageCount++;
   //double imageTime = image->utime/1000.0;
   //qDebug() << "got image " << QString::number(imageCount) << "timestamp "<< QString::number(imageTime,'f',3);
   //printf("  %ld timestamp %0.3f\n",imageCount,imageTime);

   return;

}




ImageAcquisitionThread::ImageAcquisitionThread(int threadNum,QWidget *parent) : QThread()
{

   //std::string lcmurl = NULL;

   stopped = false;
  // imageCount = 0;
   whichThread = threadNum;
   fileno = myLcm->getFileno();
   equalize = false;
   stretch = false;
   isColor = false;
   isActive = true;
   if(threadNum == 0)
      {
         leftSkip = 0;
         snprintf(subscriptionName,63,"LeftColor");
      }
   else
      {
         rightSkip = 0;
         snprintf(subscriptionName,63,"RightColor");
      }


   //char subscriptionName[64];
   //char parameterSubName[64];
   //snprintf(subscriptionName,63,"Vim%0d",threadNum);
   //snprintf(parameterSubName,63,"VimParameter%0d",threadNum);

}

void ImageAcquisitionThread::setActive(bool isIt)
{
   isActive = isIt;
}

void ImageAcquisitionThread::setSubscriptionName(char *theName)
{
   snprintf(subscriptionName,63,"%s",theName);
}

cv::Size ImageAcquisitionThread::getDestSize()
{
   cv::Size answer;
   answer.height = yrescaleSize;
   answer.width = xrescaleSize;
   return answer;
}

bool ImageAcquisitionThread::getisColor()
{
   return isColor;
}

void ImageAcquisitionThread::setColor(bool isIt)
{
   isColor = isIt;
}

void ImageAcquisitionThread::setRescaleSize(int xSize, int ySize)
{
   xrescaleSize = xSize;
   yrescaleSize = ySize;
}
void ImageAcquisitionThread::run()
{
   State state;
   state.usefulVariable = 1;

   usleep(2000);

   imageSub = myLcm->subscribeFunction( subscriptionName, &image_t_callback, &state);
   parameterSub = myLcm->subscribeFunction("M_STATUS_PARAMETERS",&parameter_t_callback, &state);
   fathometerSub = myLcm->subscribeFunction("FATHOMETER",&marine_sensor_fathometer_t_callback, &state);
   altimeterSub = myLcm->subscribeFunction("ALTIMETER",&marine_sensor_altimeter_t_callback0, &state);
   ctdSub = myLcm->subscribeFunction("CTD",&marine_sensor_ctd_t_callback, &state);
   attitudeSub = myLcm->subscribeFunction("ATTITUDE", marine_sensor_attitude_callback, &state);
   calcDepthSubscription  = myLcm->subscribeFunction("CALC_DEPTH",&marine_sensor_fathometer_t_callback, &state);
   gpsSubscription  = myLcm->subscribeFunction("GPS",&marine_sensor_gps_callback, &state);


   while (!stopped)
      {
         pollfd theFds;
         theFds.events = POLLIN;
         theFds.fd = fileno;
         poll(&theFds,1,50);
         if(theFds.revents == POLLIN)
            {
               myLcm->handle();
            }
      }
   myLcm->unsubscribe(imageSub);

}

void ImageAcquisitionThread::imageSignal()
{

   {
      emit(imUpdate(whichThread))  ;
   }

}

void ImageAcquisitionThread::histogramSignal(QVector<float> theRHist,QVector<float> theGHist,QVector<float> theBHist)
{
   emit newHistogram(theRHist,theGHist,theBHist);
}


void ImageAcquisitionThread::setEqualize(bool shouldI)
{
   equalize = shouldI;
}
void ImageAcquisitionThread::setStretch(bool shouldI)
{
   stretch = shouldI;
}


bool ImageAcquisitionThread::getEqualize()
{
   return equalize;
}

bool ImageAcquisitionThread::getStretch()
{
   return stretch;
}

bool ImageAcquisitionThread::getHistogramDisplay()
{
   return getHistogram;
}

void ImageAcquisitionThread::setHistogramDisplay(bool doIt)
{
   getHistogram = doIt;
}
