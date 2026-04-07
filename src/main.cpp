#include <QtWidgets>

#include <lcm/lcm.h>
#include <lcm/lcm-cpp.hpp>
#include "VimView.h"
#include "Stwinch.h"
#include "ImageAcquisitionThread.h"

pthread_attr_t DEFAULT_ROV_THREAD_ATTR;
ImageAcquisitionThread     *imageProviderThread[MAX_N_OF_CAMERAS];
QReadWriteLock  imageLock[MAX_N_OF_CAMERAS];
cv::Mat    cvImage;
cv::Mat    displayImage[MAX_N_OF_CAMERAS];
char *flyIniFile;

lcm::LCM                            *myLcm;
VimView *theMainWindow;
class MyMainWindow : public QMainWindow
{

private:
   void                       closeEvent(QCloseEvent *event);
   bool                       OKToClose();

};

bool MyMainWindow::OKToClose()
{

   int ret = QMessageBox::warning(this, 	tr("Camera Control"),
                                  tr("Do you really want to quit?"),
                                  QMessageBox::Yes | QMessageBox::Default,
                                  QMessageBox::No | QMessageBox::Escape);
   if (ret == QMessageBox::Yes)
      {
         return true;
      }
   else if (ret == QMessageBox::Cancel)
      {
         return false;
      }

   return false;
}

void MyMainWindow::closeEvent(QCloseEvent *event)
{
   if (MyMainWindow::OKToClose())
      {
         theMainWindow->shutDown();
         event->accept();
      }
   else
      {
         event->ignore();
      }
}

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   char	*enteredIniFileName;
   if(argc > 1)
      {
         enteredIniFileName = strdup((char *)argv[1]);
      }
   else
      {
         enteredIniFileName = strdup("./vimview.ini");

      }
   theMainWindow = new VimView(enteredIniFileName);
   // Let's use an actual QMainWindow construct though.
   MyMainWindow* actualMainWindow = new MyMainWindow();
   actualMainWindow->setCentralWidget(theMainWindow);
   actualMainWindow->setGeometry(theMainWindow->getMyGeometry());

   // Apply the stylesheet?
   QFile file(":/style/style.qss");
   file.open(QFile::ReadOnly);
   QString styleSheet = QLatin1String(file.readAll());
   file.close();
   a.setStyleSheet(styleSheet);
   const QString windowTitle = QString(
            "dsvimview V%1 compiled %2").arg(PROGRAM_VERSION).arg(__DATE__);
   actualMainWindow->setWindowTitle(windowTitle);
   actualMainWindow->show();

   theMainWindow->makeActive(true);


   return a.exec();
}


