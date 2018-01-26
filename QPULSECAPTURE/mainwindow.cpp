#include "mainwindow.h"
#include <QCoreApplication>

//------------------------------------------------------------------------------------

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    pt_display = new QImageWidget(); // Widgets without a parent are “top level” (independent) widgets. All other widgets are drawn within their parent
    this->setCentralWidget(pt_display);
    //--------------------------------------------------------------
    createThreads();
    //--------------------------------------------------------------
    createActions();
    createMenus();
    adjustSettings();
    //--------------------------------------------------------------
    setMinimumSize(640,480);
    pt_fullscreenAct->trigger();
    statusBar()->showMessage(tr("Context menu is available by right click"));
}

//------------------------------------------------------------------------------------

void MainWindow::adjustSettings()
{
    //--------------------------------------------------------------
    m_tempplot.setParent(this);
    m_tempplot.setWindowFlags(Qt::Window);
    m_tempplot.set_axis_names(tr("Time, quarter of s"), tr("Temperature, °C"));
    m_tempplot.set_vertical_Borders(29.3, 37.3); // degrees of Celsius
    m_tempplot.set_tracePen(QPen(Qt::NoBrush,1.5),QColor(50,50,200));
    m_tempplot.setEnableGradient(true);
    m_tempplot.setDrawSecondArray(false);
    //--------------------------------------------------------------
}

//------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
    __disconnectobjects();
    // Do not move into closeEvent handler because it may cause deadlocks on close
    pt_videoThread->exit();
    pt_videoThread->wait();

    pt_vpgThread->exit();
    pt_vpgThread->wait();

    pt_improcThread->exit();
    pt_improcThread->wait();
}

//------------------------------------------------------------------------------------

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    switch(key){

        case Qt::Key_Escape:
            menuBar()->show();
            statusBar()->show();
            showNormal();
            break;

        case Qt::Key_F:
            menuBar()->hide();
            statusBar()->hide();
            showFullScreen();
            break;
    }
}

void MainWindow::__connectobjects()
{
    __disconnectobjects();

    connect(pt_videoCapture,    SIGNAL(frame_was_captured(cv::Mat)), pt_pulseproc, SLOT(enrollFrame(cv::Mat)), Qt::BlockingQueuedConnection);
    connect(pt_pulseproc,       SIGNAL(vpgUpdated(const double*,int)), pt_display, SLOT(updateVPGSignal(const double*,int)));
    connect(pt_pulseproc,       SIGNAL(hrUpdated(double,double)), pt_display, SLOT(updateHR(double,double)));

    connect(pt_videoCapture,    SIGNAL(frame_was_captured(cv::Mat)), pt_opencvProcessor, SLOT(searchFace(cv::Mat)),Qt::BlockingQueuedConnection);
    connect(pt_opencvProcessor, SIGNAL(frame_was_processed(cv::Mat,double)), pt_display, SLOT(updateImage(cv::Mat,double)),Qt::BlockingQueuedConnection);
    connect(pt_temperatureProcessor, SIGNAL(objtempUpdated(qreal)), pt_display, SLOT(updateTemperature(qreal)));
    connect(pt_opencvProcessor, SIGNAL(statusChanged(QString)), pt_display, SLOT(updateStatus(QString)));
    connect(pt_opencvProcessor, SIGNAL(rightPosition()), pt_display, SLOT(decreaseStringShift()));
    connect(pt_opencvProcessor, SIGNAL(outofFrame()), pt_display, SLOT(increaseStringShift()));
    connect(pt_opencvProcessor, SIGNAL(rightPosition()), pt_pulseproc, SLOT(setFaceInPosiiton()));
    connect(pt_opencvProcessor, SIGNAL(outofFrame()), pt_pulseproc, SLOT(setFaceOutOfPosition()));
    connect(pt_temperatureProcessor, SIGNAL(dataProcessed(const qreal*,const qreal*,quint16)), &m_tempplot, SLOT(set_externalArrays(const qreal*,const qreal*,quint16)));
}

void MainWindow::__disconnectobjects()
{
    disconnect(pt_videoCapture,    SIGNAL(frame_was_captured(cv::Mat)), pt_pulseproc, SLOT(enrollFrame(cv::Mat)));
    disconnect(pt_pulseproc,       SIGNAL(vpgUpdated(const double*,int)), pt_display, SLOT(updateVPGSignal(const double*,int)));
    disconnect(pt_pulseproc,       SIGNAL(hrUpdated(double,double)), pt_display, SLOT(updateHR(double,double)));

    disconnect(pt_videoCapture,    SIGNAL(frame_was_captured(cv::Mat)), pt_opencvProcessor, SLOT(searchFace(cv::Mat)));
    disconnect(pt_opencvProcessor, SIGNAL(frame_was_processed(cv::Mat,double)), pt_display, SLOT(updateImage(cv::Mat,double)));
    disconnect(pt_temperatureProcessor, SIGNAL(objtempUpdated(qreal)), pt_display, SLOT(updateTemperature(qreal)));
    disconnect(pt_opencvProcessor, SIGNAL(statusChanged(QString)), pt_display, SLOT(updateStatus(QString)));
    disconnect(pt_opencvProcessor, SIGNAL(rightPosition()), pt_display, SLOT(decreaseStringShift()));
    disconnect(pt_opencvProcessor, SIGNAL(outofFrame()), pt_display, SLOT(increaseStringShift()));
    disconnect(pt_opencvProcessor, SIGNAL(rightPosition()), pt_pulseproc, SLOT(setFaceInPosiiton()));
    disconnect(pt_opencvProcessor, SIGNAL(outofFrame()), pt_pulseproc, SLOT(setFaceOutOfPosition()));
    disconnect(pt_temperatureProcessor, SIGNAL(dataProcessed(const qreal*,const qreal*,quint16)), &m_tempplot, SLOT(set_externalArrays(const qreal*,const qreal*,quint16)));
}
//------------------------------------------------------------------------------------

void MainWindow::createActions()
{
    pt_openDeviceAct = new QAction(tr("Open &cam"),this);
    pt_openDeviceAct->setStatusTip(tr("Open video capture device"));
    connect(pt_openDeviceAct, SIGNAL(triggered()), this, SLOT(opendevice()));

    pt_openVideoAct = new QAction(tr("Open &file"),this);
    pt_openVideoAct->setStatusTip(tr("Open video file"));
    connect(pt_openVideoAct, SIGNAL(triggered()), this, SLOT(openvideofile()));

    pt_pauseAct = new QAction(tr("&Pause"), this);
    pt_pauseAct->setStatusTip(tr("Stop processing"));
    connect(pt_pauseAct, SIGNAL(triggered()), pt_videoCapture, SLOT(pause()));

    pt_resumeAct = new QAction(tr("&Resume"), this);
    pt_resumeAct->setStatusTip(tr("Resume processing"));
    connect(pt_resumeAct, SIGNAL(triggered()), pt_videoCapture, SLOT(resume()));

    pt_exitAct = new QAction(tr("E&xit"), this);
    pt_exitAct->setStatusTip(tr("Exit from app"));
    connect(pt_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    pt_aboutAct = new QAction(tr("&About"), this);
    pt_aboutAct->setStatusTip(tr("Show application's about info"));
    connect(pt_aboutAct, SIGNAL(triggered()), this, SLOT(show_about()));

    pt_helpAct = new QAction(tr("&Help"), this);
    pt_helpAct->setStatusTip(tr("Show the application's help info"));
    connect(pt_helpAct, SIGNAL(triggered()), this, SLOT(show_help()));

    pt_deviceResAct = new QAction(tr("&Resolution"), this);
    pt_deviceResAct->setStatusTip(tr("Open video capture device resolution dialog"));
    connect(pt_deviceResAct, SIGNAL(triggered()), this, SLOT(opendeviceresolutiondialog()));

    pt_deviceSetAct = new QAction(tr("&Settings"), this);
    pt_deviceSetAct->setStatusTip(tr("Open video capture device settings dialog"));
    connect(pt_deviceSetAct, SIGNAL(triggered()), this, SLOT(opendevicesettingsdialog()));

    pt_openMlxDevice = new QAction(tr("&Open piro"), this);
    pt_openMlxDevice->setStatusTip("Open particular mlx device");
    connect(pt_openMlxDevice, SIGNAL(triggered()), pt_serialProcessor, SLOT(openSerialPortSelectDialog()));

    pt_plotAct = new QAction(tr("&Temperature"), this);
    pt_plotAct->setStatusTip("Show temperature vs time plot in a new window");
    connect(pt_plotAct, SIGNAL(triggered()), this, SLOT(showPlot()));

    pt_fullscreenAct = new QAction(tr("&Fullscreen"), this);
    pt_fullscreenAct->setStatusTip("Show app in full screen mode, use exit key to normalize");
    connect(pt_fullscreenAct, SIGNAL(triggered()), menuBar(), SLOT(hide()));
    connect(pt_fullscreenAct, SIGNAL(triggered()), statusBar(), SLOT(hide()));
    connect(pt_fullscreenAct, SIGNAL(triggered()), this, SLOT(showFullScreen()));
}

//------------------------------------------------------------------------------------

void MainWindow::createMenus()
{
    pt_fileMenu = this->menuBar()->addMenu(tr("&Devices"));
    pt_fileMenu->addAction(pt_openDeviceAct);
    pt_fileMenu->addSeparator();
    pt_fileMenu->addAction(pt_openMlxDevice);
    pt_fileMenu->addSeparator();
    pt_fileMenu->addAction(pt_exitAct);   
    //--------------------------------------------------
    pt_settingsMenu = this->menuBar()->addMenu(tr("&Settings"));
    pt_settingsMenu->addAction(pt_deviceResAct);
    pt_settingsMenu->addAction(pt_deviceSetAct);
    //--------------------------------------------------
    pt_dataMenu = menuBar()->addMenu(tr("D&ata"));
    pt_dataMenu->addAction(pt_plotAct);
    //--------------------------------------------------
    pt_viewMenu = menuBar()->addMenu(tr("&View"));
    pt_viewMenu->addAction(pt_fullscreenAct);
    //--------------------------------------------------
    pt_helpMenu = menuBar()->addMenu(tr("&Help"));
    pt_helpMenu->addAction(pt_helpAct);
    pt_helpMenu->addAction(pt_aboutAct);
}

//------------------------------------------------------------------------------------

void MainWindow::createThreads()
{
    //-------------------Pointers for objects------------------------
    pt_improcThread = new QThread(this);
    pt_opencvProcessor = new QOpencvProcessor();
    std::string cascadeFilename = QCoreApplication::applicationDirPath().append("/haarcascades/haarcascade_frontalface_alt2.xml").toStdString();
    #ifdef HAARCASCADES_PATH // it is debug option
        cascadeFilename = std::string(HAARCASCADES_PATH) + std::string("haarcascade_frontalface_alt2.xml");      
    #endif
    bool _has_classifier_been_loaded = pt_opencvProcessor->loadClassifier(cascadeFilename);
    assert(_has_classifier_been_loaded); // check if classifier has been loaded
    pt_opencvProcessor->moveToThread( pt_improcThread );
    connect(pt_improcThread, SIGNAL(finished()), pt_opencvProcessor, SLOT(deleteLater()));

    //--------------------QVideoCapture------------------------------
    pt_videoThread = new QThread(this);
    pt_videoCapture = new QVideoCapture();
    pt_videoCapture->moveToThread(pt_videoThread);
    connect(pt_videoThread, SIGNAL(started()), pt_videoCapture, SLOT(initializeTimer()));
    connect(pt_videoThread, SIGNAL(finished()), pt_videoCapture, SLOT(close()));
    connect(pt_videoThread, SIGNAL(finished()), pt_videoCapture, SLOT(deleteLater()));   

    //---------------------QSerialPortProcessor and QTemperatureProcessor--------------------
    pt_serialProcessor = new QSerialPortProcessor(this);
    pt_temperatureProcessor = new QTemperatureProcessor(this);
    pt_serialProcessor->opendefault();
    connect(pt_serialProcessor, SIGNAL(new_data(QByteArray)), pt_temperatureProcessor, SLOT(processData(QByteArray)));

    //---------------------QPulseProcessor--------------------------
    pt_vpgThread = new QThread(this);
    pt_pulseproc = new QPulseProcessor();
    pt_pulseproc->moveToThread(pt_vpgThread);
    connect(pt_vpgThread, SIGNAL(finished()), pt_pulseproc, SLOT(deleteLater()));

    // Initialization routine
    connect(pt_videoCapture, SIGNAL(timerInitialized()), pt_videoCapture, SLOT(opendefaultdevice()));
    connect(pt_videoCapture, SIGNAL(deviceFPSChanged()), pt_videoCapture, SLOT(resume()));
    connect(pt_videoCapture, SIGNAL(deviceFPSChanged()), pt_videoCapture, SLOT(measureActualFPS()));
    connect(pt_videoCapture, SIGNAL(fpsMeasured(double)), pt_pulseproc, SLOT(initialize(double)));
    connect(pt_pulseproc, SIGNAL(initialized()), this, SLOT(__connectobjects()));

    //----------------------Thread start-----------------------------
    pt_vpgThread->start();
    pt_improcThread->start();
    pt_videoThread->start();

    //---------------------------------------------------------------
    QTimer::singleShot(8000, this, SLOT(__connectToClose()));
}

//------------------------------------------------------------------------------------

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(pt_pauseAct);
    menu.addAction(pt_resumeAct);
    menu.exec(event->globalPos());
}

//------------------------------------------------------------------------------------

bool MainWindow::openvideofile()
{
    pt_pauseAct->trigger();
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open video file"));
    while( !pt_videoCapture->openfile(fileName) )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open file!"), QMessageBox::Open | QMessageBox::Cancel, this, Qt::Dialog);
        if(msgBox.exec() == QMessageBox::Open)
        {
            fileName = QFileDialog::getOpenFileName(this, tr("Open video file"), "/video.avi", tr("Video (*.avi *.mp4, *.wmv)"));
        }
        else
        {
            return false;
        }
    }
    pt_resumeAct->trigger();
    return true;
}

//------------------------------------------------------------------------------------

bool MainWindow::opendevice()
{
    pt_pauseAct->trigger();
    pt_videoCapture->open_deviceSelectDialog();
    while( !pt_videoCapture->opendevice() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device!"), QMessageBox::Open | QMessageBox::Cancel, this, Qt::Dialog);
        if(msgBox.exec() == QMessageBox::Open)
        {
            pt_videoCapture->open_deviceSelectDialog();
        }
        else
        {
            return false;
        }
    }
    pt_resumeAct->trigger();
    QTimer::singleShot(0,pt_videoCapture,SLOT(measureActualFPS()));
    return true;
}

//------------------------------------------------------------------------------------

void MainWindow::show_about()
{
   QDialog *aboutdialog = new QDialog(); 
   int pSize = aboutdialog->font().pointSize();
   aboutdialog->setWindowTitle("About");
   aboutdialog->setFixedSize(pSize*25,pSize*15);

   QVBoxLayout *templayout = new QVBoxLayout();
   templayout->setMargin(5);

   QLabel *projectname = new QLabel(QString(APP_NAME) +"\t"+ QString(APP_VERS));
   projectname->setFrameStyle(QFrame::Box | QFrame::Raised);
   projectname->setAlignment(Qt::AlignCenter);
   QLabel *projectauthors = new QLabel("Designed by: Alex A. Taranov\n\nDiacareSoft\n2016");
   projectauthors->setWordWrap(true);
   projectauthors->setAlignment(Qt::AlignCenter);
   QLabel *hyperlink = new QLabel("<a href='mailto:pi-null-mezon@yandex.ru?subject=mlx2cam'>pi-null-mezon@yandex.ru");
   hyperlink->setToolTip("Open default mail client");
   hyperlink->setOpenExternalLinks(true);
   hyperlink->setAlignment(Qt::AlignCenter);

   templayout->addWidget(projectname);
   templayout->addWidget(projectauthors);
   templayout->addWidget(hyperlink);

   aboutdialog->setLayout(templayout);
   aboutdialog->exec();

   delete hyperlink;
   delete projectauthors;
   delete projectname;
   delete templayout;
   delete aboutdialog;
}

//------------------------------------------------------------------------------------

void MainWindow::show_help()
{
    if (!QDesktopServices::openUrl(QUrl(QString("https://github.com/pi-null-mezon/QVideoProcessing.git"), QUrl::TolerantMode))) // runs the ShellExecute function on Windows
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not find help"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
}

//-----------------------------------------------------------------------------------

void MainWindow::opendeviceresolutiondialog()
{
    pt_pauseAct->trigger();
    if( !pt_videoCapture->open_resolutionDialog() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device resolution dialog!"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
    pt_resumeAct->trigger();
}

//-----------------------------------------------------------------------------------

void MainWindow::opendevicesettingsdialog()
{
    if( !pt_videoCapture->open_settingsDialog() )
    {
        QMessageBox msgBox(QMessageBox::Information, this->windowTitle(), tr("Can not open device settings dialog!"), QMessageBox::Ok, this, Qt::Dialog);
        msgBox.exec();
    }
}

//----------------------------------------------------------------------------------------

void MainWindow::showPlot()
{
    m_tempplot.show();
}

//----------------------------------------------------------------------------------------

void MainWindow::saveMeasuremets(double _hr, double _snr)
{
    Q_UNUSED(_snr);
    if(m_filename.isEmpty() == false) {
        QFile _file(m_filename);
        if(_file.open(QFile::WriteOnly)) {

            disconnect(pt_pulseproc, SIGNAL(hrUpdated(double, double)), this, SLOT(saveMeasuremets(double, double)));

            QTextStream _textstream(&_file);
            _textstream << QString::number(std::round(_hr)) // heart rate in bpm
                           + " "
                           + QString::number(0) // legacy part (not implemented in new version because accuracy is not good), value for breath rate
                           + " "
                           + QString::number(0) // legacy part (not implemented in new version because accuracy is not good), value for SPO2
                           + " "
                           + QString::number(pt_temperatureProcessor->getLastTemperature(), 'f', 1) // temperature in degrees of Celsius
                           + "\n";

            _textstream.flush();
            _file.close();
            QTimer::singleShot(1000, this, SLOT(close()));
            qInfo("Output file has been saved, wait to close");
        } else {
            qWarning("Can not save measurements, because output file can not be opened, check %s", m_filename.toLocal8Bit().constData());
        }
    }
}

//----------------------------------------------------------------------------------------

void MainWindow::setOutputFilename(QString _filename)
{
    m_filename = _filename;
}

void MainWindow::__connectToClose()
{
    connect(pt_pulseproc, SIGNAL(hrUpdated(double, double)), this, SLOT(saveMeasuremets(double, double)));
}

//----------------------------------------------------------------------------------------



