#include "VimView.h"
#include "../../dsvimlib/include/IniFile.h"
#include "ImageAcquisitionThread.h"

/* 26 Jan 2024 jch attempted to change all references to 16bit collection to 8 bit, tried to reflect change to sparton vice microstrain (just a label) */

extern ImageAcquisitionThread       *imageProviderThread[MAX_N_OF_CAMERAS];
extern QReadWriteLock               imageLock[MAX_N_OF_CAMERAS];
extern cv::Mat                      cvImage;
extern cv::Mat                      displayImage[MAX_N_OF_CAMERAS];

extern lcm::LCM                            *myLcm;
int                                 globalImageSkipValue;

VimView::VimView(char	*startup_file_name)

{

   nOfCameras = 0;
   imageSkipvalue = 0;
   isActive = false;
   recordingPaused = false;
   double microstrainTimeoutCriteria, ctdTimeoutCriteria, altimeterTimeoutCriteria, fathometerTimeoutCriteria, gpsTimeoutCriteria;
   IniFile	iniFile;
   char *subscriptionNames[MAX_N_OF_CAMERAS];
   lastImageDateTime = QDateTime::currentDateTimeUtc().addSecs(-1000);
   globalImageSkipValue = 1;
   numberOfSwitches = 0;
   powerControlWidget = nullptr;
   // NCD data arrives on the LCM thread; queue it over to the GUI thread
   connect(this, SIGNAL(ncdDataReceived(QByteArray,bool,int)), this, SLOT(updateNCDData(QByteArray,bool,int)), Qt::QueuedConnection);
   int systemParameterSize;

   myLcm = new lcm::LCM("udpm://239.255.76.67:7667?ttl=0");
   if (!myLcm)
      {
         printf("lcm_create() failed\n");
         exit (EXIT_FAILURE);
      }
   else
      {
         qDebug() << "lcm_create() succeeded";
      }
   int	startupReturnCode = iniFile.openIni(startup_file_name);
   if(startupReturnCode != GOOD_INI_FILE_READ)
      {
         return ;
      }
   else
      {
         systemParameterControl = new SystemParameterControl();
         connect(systemParameterControl,SIGNAL(emitSkipValue(int)),this, SLOT(setSkipValue(int)));

         for (int switchNumber = 0; switchNumber < MAX_NUMBER_OF_SWITCHES; switchNumber++)
         {
            char switchLabel[255];
            snprintf(switchLabel,254,"RELAY_%d",switchNumber+1 );
            char *candidateSwitchName = iniFile.readString("NCD_IO",switchLabel, "NO_SWITCH");
            if(!strcmp(candidateSwitchName,"NO_SWITCH"))
                {
                    free(candidateSwitchName);
                    continue;
                }
            // readString returns a strdup'ed string, so just take ownership of it
            switches[numberOfSwitches].attachedTo = candidateSwitchName;
            switches[numberOfSwitches].relayNumber = switchNumber;
            numberOfSwitches++;
         }

         microstrainTimeoutCriteria = iniFile.readDouble("GENERAL", "MICROSTRAIN_TIMEOUT", DEFAULT_MICROSTRAIN_TIMEOUT);
         gpsTimeoutCriteria = iniFile.readDouble("GENERAL", "GPS_TIMEOUT", DEFAULT_GPS_TIMEOUT);
         fathometerTimeoutCriteria = iniFile.readDouble("GENERAL", "FATHOMETER_TIMEOUT", DEFAULT_FATHOMETER_TIMEOUT);
         altimeterTimeoutCriteria = iniFile.readDouble("GENERAL", "ALTIMETER_TIMEOUT", DEFAULT_ALTIMETER_TIMEOUT);
         ctdTimeoutCriteria = iniFile.readDouble("GENERAL", "CTD_TIMEOUT", DEFAULT_CTD_TIMEOUT);

         useColorConstancy = iniFile.readDouble("GENERAL", "USE_CONSTANCY", true);
         systemParameterControl->setUseConstancy(useColorConstancy);

         sensorPage = new SensorPage();
         sensorPage->show();
         sensorPage->setTimeout(GPS_TIMEOUT, gpsTimeoutCriteria);
         sensorPage->setTimeout(CTD_TIMEOUT, ctdTimeoutCriteria);
         sensorPage->setTimeout(FATHOMETER_TIMEOUT, fathometerTimeoutCriteria);
         sensorPage->setTimeout(ALTIMETER_TIMEOUT, altimeterTimeoutCriteria);
         sensorPage->setTimeout(ATTITUDE_TIMEOUT, microstrainTimeoutCriteria);

         connect(systemParameterControl,SIGNAL(hideImageDisplay(int, bool)), this, SLOT(imageHide(int, bool)));
         for(int cameraNumber = 0; cameraNumber < MAX_N_OF_CAMERAS; cameraNumber++)
            {
               char cameraLabel[32];
               snprintf(cameraLabel,32,"CAMERA_%d",cameraNumber+1);
               char *channelName = iniFile.readString(cameraLabel,"COLOR_CHANNEL_NAME","NONAME");

               char *thisSN = iniFile.readString(cameraLabel,"SERIAL_NUMBER", NO_SERIAL_NUMBER);
               if(!strcmp(thisSN,NO_SERIAL_NUMBER))
                  {
                     free(thisSN );
                     free(channelName);
                  }
               else
                  {
                     subscriptionNames[nOfCameras] = channelName;
                     vimCameraControl[nOfCameras] = new VimCameraControl(nOfCameras,(QString)thisSN, this);
                     free(thisSN);
                     int xSize = iniFile.readInt(cameraLabel,"X_DISPLAY_SIZE", 612);
                     systemParameterSize = xSize;
                     int ySize = iniFile.readInt(cameraLabel,"Y_DISPLAY_SIZE", 512);
                     setDisplaySize(nOfCameras,xSize,ySize);
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeAutoExposure(bool,double, double, int)), this, SLOT(changeAutoExposure(bool,double, double, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeExposureValue(double,int)), this, SLOT(changeExposureValue(double,int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(sendAutoGain(bool,double, double, int)), this, SLOT(changeAutoGain(bool,double, double, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeCameraGain(double,int)), this, SLOT(changeGainValue(double,int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(toggleBinning(bool,int)),this,SLOT(toggleBinning(bool,int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeEqualization(bool,int)), this, SLOT(changeThisEqualization(bool, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeStretch(bool,int)), this, SLOT(changeThisStretch(bool, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeHistogramDisplay(bool,int)), this, SLOT(changeHistogramDisplay(bool, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(takeAPicture(int)), this, SLOT(takeAPicture(int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(startTimedStills(double, int)), this, SLOT(startTimedStills(double, int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(stopTimedStills(int)), this, SLOT(stopTimedStills(int)));
                     connect(vimCameraControl[nOfCameras],SIGNAL(changeDecimationFactor(int)), this, SLOT(changeDecimationFactor(int)));
                     // connect(systemParameterControl,SIGNAL(emitCameraInterval(int)),this,SLOT(sendGardaRateChange(int)));
                     connect(systemParameterControl,SIGNAL(pauseRec(bool)),this,SLOT(pauseRecording(bool)));
                     connect(systemParameterControl,SIGNAL(emitConstancy(bool)),this,SLOT(changeConstancySetting(bool)));

                     connect(systemParameterControl,SIGNAL(showWinch(bool)), this, SLOT(showWinchFly(bool)));
                     connect(systemParameterControl,SIGNAL(showSensors(bool)), this, SLOT(showSensors(bool)));
                     imageArea[nOfCameras] = new QLabel();
                     imageArea[nOfCameras]->setFixedHeight(ySize);
                     imageArea[nOfCameras]->setFixedWidth(xSize);
                     vimCameraControl[nOfCameras]->setFixedWidth(xSize);
                     imageArea[nOfCameras]->setFrameStyle(QFrame::StyledPanel);
                     equalizeDisplay[nOfCameras] = false;
                     histogramDisplay[nOfCameras] = false;
                     nOfCameras++;
                  }

            }
         //systemParameterControl->setFixedWidth(systemParameterSize);


      }

   powerControlWidget = new PowerControlWidget (this);
   for(int relayNumber = 0; relayNumber < numberOfSwitches; relayNumber++)
    {
        powerControlWidget->addAButton(switches[relayNumber]) ;
        // the button keeps its own copy of the name
        free(switches[relayNumber].attachedTo);
        switches[relayNumber].attachedTo = nullptr;
    }
   connect(powerControlWidget, SIGNAL(powerButtonClicked(int,int)), this, SLOT(powerButtonClicked(int,int)));
   powerControlWidget->drawLayout();
   iniFile.closeIni();
   QVBoxLayout *masterLayout = new QVBoxLayout(this);

   QHBoxLayout *imagePairLayout = new QHBoxLayout();
   for(int thisCamera = 0; thisCamera < nOfCameras; thisCamera++)
      {
         QVBoxLayout *imageLayout = new QVBoxLayout();
         displayImage[thisCamera] = cv::Mat(imageDisplayHeight[thisCamera],imageDisplayWidth[thisCamera],CV_8UC3);
         imageLayout->addWidget(imageArea[thisCamera]);
         imageLayout->addWidget(vimCameraControl[thisCamera]);

         imagePairLayout->addLayout(imageLayout);
         imagePairLayout->setAlignment(Qt::AlignLeft);
         qDebug() << "starting image acquisition thread...";
         imageProviderThread[thisCamera] = new ImageAcquisitionThread(thisCamera,this);
         imageProviderThread[thisCamera]->setRescaleSize(imageDisplayWidth[thisCamera],imageDisplayHeight[thisCamera]);
         if(strncmp(subscriptionNames[thisCamera],"NONAME",6))
            {
               imageProviderThread[thisCamera]->setSubscriptionName(subscriptionNames[thisCamera]);
            }
         else
            {
               char theSubscriptionName[256];
               //snprintf(theSubscriptionName,255,"Vim%0d",thisCamera);
               if(0 == thisCamera)
                 {
                   snprintf(theSubscriptionName,255,"LeftColor");
                 }
               else
                 {
                   snprintf(theSubscriptionName,255,"RightColor");
                 }
               imageProviderThread[thisCamera]->setSubscriptionName(theSubscriptionName);
            }
         free(subscriptionNames[thisCamera]);
         qRegisterMetaType < QVector<float> >("QVector<float>");

         // connect before subscribing: thread 0 may already be dispatching, and a frame must not signal into nothing
         connect(imageProviderThread[thisCamera],SIGNAL(imUpdate(int)),this,SLOT(updateImage(int)));
         connect(imageProviderThread[thisCamera],SIGNAL(newHistogram(QVector<float>,QVector<float>,QVector<float>)),vimCameraControl[thisCamera],SLOT(drawHistogram(QVector<float>,QVector<float>,QVector<float>)));

         imageProviderThread[thisCamera]->subscribe();
         imageProviderThread[thisCamera]->start();

      }
   currentTimeLabel = new QLabel("current time: ");
   masterLayout->addLayout(imagePairLayout);
   masterLayout->addWidget(systemParameterControl);
   masterLayout->addWidget(powerControlWidget);

   masterLayout->addWidget(currentTimeLabel);
   stWinch = new StWinch(startup_file_name);
   stWinch->show();

   imgReceiveTimer = new QTimer();
   imgReceiveTimer->setInterval(1000);
   imgReceiveTimer->connect(imgReceiveTimer,SIGNAL(timeout()),this, SLOT(checkImgReceipt()));
   imgReceiveTimer->start();


}


void VimView::setAltitude(double theAltitude)
{
   stWinch->setAltitude(theAltitude);
   sensorPage->setAltimeter(theAltitude);
   storedAltitude = theAltitude;
}

void VimView::setNCDData(const ncd_io::NcdIoState_t *ncdData)
{
   // called from the LCM thread: copy the states, the message buffer won't outlive the callback
   QByteArray relayStates(reinterpret_cast<const char *>(ncdData->relay_state), sizeof(ncdData->relay_state));
   emit ncdDataReceived(relayStates, !ncdData->checksum_ok, ncdData->error_code);
}

void VimView::updateNCDData(QByteArray theRelayStates, bool checksumError, int errorCode)
{
   if(!powerControlWidget)
   {
      return;
   }
   if(theRelayStates.size() >= MAX_NUMBER_OF_SWITCHES)
   {
      powerControlWidget->setRelayStates(reinterpret_cast<const int8_t *>(theRelayStates.constData()));
   }
   powerControlWidget->setErrorStates(checksumError, errorCode);
}


void VimView::setCalcDepth(double theCalcDepth)
{
   stWinch->setCalcDepth(theCalcDepth);
}

void VimView::setFathometer(double theFathometerValue)
{
   stWinch->setFathometer(theFathometerValue);
   sensorPage->setFathometer(theFathometerValue);
}

void VimView::setFishDepth(double theDepthValue)
{
   stWinch->setFishDepth(theDepthValue);
   storedDepth = theDepthValue;
   double calculatedDepth = theDepthValue + storedAltitude;
   stWinch->setCalcDepth(calculatedDepth);
}

void VimView::setCTD(double theDepth, double theT)
{
  stWinch->setFishDepth(theDepth);
  storedDepth = theDepth;
  double calculatedDepth = theDepth + storedAltitude;
  stWinch->setCalcDepth(calculatedDepth);
  sensorPage->setCTD(theDepth, theT);
}

void VimView::setFishAttitude(double heading, double pitch, double roll)
{
  if(this)
  {
      sensorPage->setAttitude(heading, pitch, roll);
  }
}

void VimView::setGPS(double latitude, double longitude)
{
  sensorPage->setGPS(latitude, longitude);

}

void VimView::checkImgReceipt()
{
   QDateTime nowTime = QDateTime::currentDateTimeUtc();
   if(lastImageDateTime.msecsTo(nowTime) > IMG_RECEIPT_TIMEOUT)
      {
         systemParameterControl->setAcqLightOK(false);
      }
   else
      {
         systemParameterControl->setAcqLightOK(true);
      }
   QString currentTimeString = nowTime.toString("yyyy/MM/dd hh:mm:ss");
   currentTimeLabel->setText(currentTimeString);

}

void VimView::imageHide(int whichImage, bool hideOrNot)
{
   if(whichImage < 0 || whichImage >= nOfCameras)
      {
         return;
      }
   imageArea[whichImage]->setVisible(hideOrNot) ;
   vimCameraControl[whichImage]->setVisible(hideOrNot);
}

void VimView::makeActive(bool activeState)
{
    isActive = activeState;
}

void VimView::pauseRecording(bool pauseIt)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "RECORDING";
   if(pauseIt)
      {
         imageParameter.value = "0" ;
      }
   else
      {
         imageParameter.value = "1" ;
      }
   imageParameter.cameraNumber = 0;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}
void VimView::setRecordingActallyPaused(bool isIt)
{
   actualRecordingPauseState = isIt;
   systemParameterControl->setRecordingActualState(isIt);
  // vimCameraControl[1]->setRecordingActualState(isIt);
}

void VimView::showWinchFly(bool showIt)
{
   if(showIt)
      {
         stWinch->show();
      }
   else
      {
         stWinch->hide();
      }
}

void VimView::showSensors(bool showIt)
{
   if(showIt)
      {
         sensorPage->show();
      }
   else
      {
         sensorPage->hide();
      }
}


void VimView::setDisplaySize(int cameraNumber, int width, int height)
{
   imageDisplayWidth[cameraNumber] = width;
   imageDisplayHeight[cameraNumber] = height;
}

void VimView::setExposureActual(double theExposure, int theCamera)
{

   if(isActive && theCamera >= 0 && theCamera < nOfCameras)
      {
         vimCameraControl[theCamera]->setExposureActual(theExposure);
      }
}

void VimView::setGainActual(double theGain, int theCamera)
{

   if(isActive && theCamera >= 0 && theCamera < nOfCameras)
      {
         vimCameraControl[theCamera]->setGainActual(theGain);
      }
}

void VimView::setTriggerSource(int theSource, int theCamera)
{

   if(isActive && theCamera >= 0 && theCamera < nOfCameras)
      {
         vimCameraControl[theCamera]->setTriggerSource(theSource);
      }
}

void VimView::startTimedStills(double theInterval, int cameraNumber)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "REPETION_INTERVAL";
   imageParameter.value = QString::number(theInterval,'f',2).toStdString();
   imageParameter.cameraNumber = (unsigned char) cameraNumber;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);
}

void VimView::stopTimedStills(int theCameraNumber)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "REPETION_STOP";
   imageParameter.value = "1";
   imageParameter.cameraNumber = (unsigned char) theCameraNumber;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);
}

void VimView::changeThisEqualization(bool equalize, int theCamera)
{
   equalizeDisplay[theCamera] = equalize;
   imageProviderThread[theCamera]->setEqualize(equalize);
}
void VimView::changeThisStretch(bool stretch, int theCamera)
{
   stretchDisplay[theCamera] = stretch;
   imageProviderThread[theCamera]->setStretch(stretch);
}

void VimView::changeHistogramDisplay(bool doIt, int theCamera)
{
   histogramDisplay[theCamera] = doIt;
   imageProviderThread[theCamera]->setHistogramDisplay(doIt);
}

void VimView::toggleBinning(bool binning, int cameraNumber)
{
   if(binning)
      {


      }
   else
      {


      }
   image::image_parameter_t imageParameter;
   imageParameter.key = "BINNING";
   imageParameter.value = QString::number(binning).toStdString();
   imageParameter.cameraNumber = (unsigned char) cameraNumber;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}

void VimView::setWriteSuccess(bool isItSuccessful)
{
   systemParameterControl->setWriteSuccess(isItSuccessful);
}
void VimView::changeGainValue(double newGain, int theCamera)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "GAIN";
   imageParameter.value = QString::number(newGain,'f',1).toStdString();
   imageParameter.cameraNumber = theCamera;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}

void VimView::changeConstancySetting(bool newConstancy)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "CONSTANCY";
   if(newConstancy)
   {
    imageParameter.value = "1" ;
   }
   else
   {
    imageParameter.value = "0" ;
   }
   imageParameter.cameraNumber = 1;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}




void VimView::changeExposureValue(double newExposure, int theCamera)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "EXPOSURE";
   imageParameter.value = QString::number(newExposure * 1000.0,'f',1).toStdString();
   imageParameter.cameraNumber = theCamera;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}

void VimView::takeAPicture(int theCamera)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "STILL";
   imageParameter.value = "1";
   imageParameter.cameraNumber = theCamera;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);
}
void VimView::changeAutoGain(bool isAuto, double theMin, double theMax, int cameraNumber)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "AUTO_GAIN " + std::to_string(theMin ) + " " + std::to_string(theMax );
   if(isAuto)
      {
         imageParameter.value = "1";
      }
   else
      {
         imageParameter.value = "0";
      }
   imageParameter.cameraNumber = cameraNumber;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}

void VimView::changeAutoExposure(bool isAuto, double theMin, double theMax, int cameraNumber)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "AUTO_EXPOSURE " + std::to_string(theMin * 1000.0) + " " + std::to_string(theMax *1000.0);
   if(isAuto)
      {
         imageParameter.value = "1";
      }
   else
      {
         imageParameter.value = "0";
      }
   imageParameter.cameraNumber = cameraNumber;
   myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}


void VimView::changeDecimationFactor(int theFactor)
{
   image::image_parameter_t imageParameter;
   imageParameter.key = "DECIMATION";
   imageParameter.value = std::to_string(theFactor);
   for(int thisCamera = 0; thisCamera < nOfCameras; thisCamera++)
      {
         imageParameter.cameraNumber = thisCamera;
         myLcm->publish("COMMAND_PARAMETERS",&imageParameter);
      }
}

QRect VimView::getMyGeometry()
{
   QRect myGeometry = geometry();
   return myGeometry;
}




void    VimView::updateImage(int theImage)
{
   //imageArea[0]->setPixmap(QPixmap::fromImage(QImage(displayImage[0].data, displayImage[0].cols, displayImage[0].rows, displayImage[0].step, QImage::Format_RGB888)));
   lastImageDateTime = QDateTime::currentDateTimeUtc();
   // take our own reference to the frame; the callback swaps in new frames rather than writing into this one
   imageLock[theImage].lockForRead();
   cv::Mat frame = displayImage[theImage];
   imageLock[theImage].unlock();

   if(frame.empty())
      {
         return;
      }
   // the app only handles colour imagery; displayImage is always BGR (IS_COLOR in the ini is ignored)
   // fromImage copies the pixels, so frame can go once this returns
   imageArea[theImage]->setPixmap(QPixmap::fromImage(QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888)));
}

void VimView::setSkipValue(int theValue)
{
   imageSkipvalue = theValue+1;
   globalImageSkipValue = imageSkipvalue;
}

void    VimView::powerButtonClicked(int whichButton, int theState)
{
    image::image_parameter_t imageParameter;
    imageParameter.key = "POWER_BUTTON";
    if(theState)
       {
          imageParameter.value = "1" ;
       }
    else
       {
          imageParameter.value = "0" ;
       }
    imageParameter.cameraNumber = whichButton;
    myLcm->publish("COMMAND_PARAMETERS",&imageParameter);

}
void VimView::shutDown()
{
   for(int thisCamera = 0; thisCamera < nOfCameras; thisCamera++)
      {
         imageProviderThread[thisCamera]->requestStop();
      }
   for(int thisCamera = 0; thisCamera < nOfCameras; thisCamera++)
      {
         imageProviderThread[thisCamera]->wait();
      }
   for(int thisCamera = 0; thisCamera < nOfCameras; thisCamera++)
      {
         imageProviderThread[thisCamera]->unsubscribeAll();
      }

   stWinch->close();
   sensorPage->close();
   emit closing();
}

VimView::~VimView()
{

}
