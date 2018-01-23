/*-----------------------------------------------------------------------------------------------------
Taranov Alex, 2014 									    SOURCE FILE
QVideoCapture class was created as Qt SIGNAL/SLOT wrapper for OpenCV VideoCapture class
Work with: call openfile(...) or opendevice(...) and on success, object will start to emit
frame_was_captured(const cv::Mat& value) signal with determined period of time.
To stop frame capturing use pause() or close(). Also
class provides some GUI interface to cv::VideoCapture::set(...) function.
------------------------------------------------------------------------------------------------------*/

#include "qvideocapture.h"

QVideoCapture::QVideoCapture(QObject *parent) :
    QObject(parent),
    device_id(0)
{
}

bool QVideoCapture::openfile(const QString &filename)
{
    pt_timer->stop();
    if( m_cvCapture.open( filename.toLocal8Bit().constData() ) )
    {
        deviceFlag = false;
        pt_timer->setInterval( 1000.0 / m_cvCapture.get(cv::CAP_PROP_FPS) ); // CV_CAP_PROP_FPS - m_frame rate
        return true;
    }
    return false;
}

bool QVideoCapture::opendevice(int period) // period should be entered in ms
{
    pt_timer->stop();
    if( m_cvCapture.open( device_id ) )
    {
        deviceFlag = true;        
        pt_timer->setInterval( period );
        emit deviceOpened();
        return true;
    }
    return false;
}

void QVideoCapture::opendefaultdevice()
{
    if(opendevice()) {
        m_cvCapture.set(cv::CAP_PROP_ZOOM, 2.0);
        m_cvCapture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        m_cvCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
        m_cvCapture.set(cv::CAP_PROP_FPS, 30);
    }
}

bool QVideoCapture::set(int propertyID, double value)
{
    return m_cvCapture.set( propertyID, value);
}

bool QVideoCapture::resume()
{
    if( m_cvCapture.isOpened() )
    {
        pt_timer->start();
        return true;
    }
    return false;
}

bool QVideoCapture::start()
{
    if( m_cvCapture.isOpened() ) {
        if(deviceFlag) {
            pt_timer->start();
        } else {
            // TO DO
        }
        return true;
    }
    return false;
}

bool QVideoCapture::close()
{
    if( m_cvCapture.isOpened() )  {
        pt_timer->stop();
        m_cvCapture.release();
        return true;
    }
    return false;
}

bool QVideoCapture::pause()
{
    if( m_cvCapture.isOpened() )   {
        pt_timer->stop();
        return true;
    }
    return false;
}

bool QVideoCapture::read_frame()
{
    if( m_cvCapture.read(m_frame) ) {
        cv::flip(m_frame,m_frame, 1); // horizontal flip to make image perception as mirror reflection
        emit frame_was_captured(m_frame);
        return true;
    } else  {
        this->pause();
        return false;
    }
}

bool QVideoCapture::open_resolutionDialog()
{
    if (m_cvCapture.isOpened() && deviceFlag) {
        QDialog dialog;
        dialog.setFixedSize(198,128);
        dialog.setWindowTitle(tr("Camcorder resolution"));

        QVBoxLayout toplevellayout;
        toplevellayout.setMargin(5);

        QHBoxLayout resLayout;
        resLayout.setMargin(5);
        QComboBox CBresolution;
        CBresolution.addItem("640 x 480"); // 0
        CBresolution.addItem("800 x 600"); // 1
        CBresolution.addItem("1024 x 768"); // 2
        CBresolution.addItem("1280 x 720"); // 3
        CBresolution.addItem("1600 x 1200"); // 4
        CBresolution.addItem("1980 x 1080"); // 5
        CBresolution.setSizeAdjustPolicy(QComboBox::AdjustToContents);
        CBresolution.setCurrentIndex(0);
        QLabel Lresolution("Resolution:");
        resLayout.addWidget(&Lresolution);
        resLayout.addWidget(&CBresolution);

        QHBoxLayout framerateLayout;
        framerateLayout.setMargin(5);
        QComboBox CBframerate;
        CBframerate.addItem("30 fps"); // 0
        CBframerate.addItem("25 fps"); // 1
        CBframerate.addItem("20 fps"); // 2
        CBframerate.addItem("15 fps"); // 3
        CBframerate.addItem("10 fps"); // 4
        CBframerate.addItem("5 fps"); // 5
        CBframerate.setSizeAdjustPolicy(QComboBox::AdjustToContents);
        CBframerate.setCurrentIndex(0);
        QLabel Lframerate("Framerate:");
        framerateLayout.addWidget(&Lframerate);
        framerateLayout.addWidget(&CBframerate);

        QHBoxLayout buttons;
        buttons.setMargin(5);
            QPushButton Baccept;
            Baccept.setText("Accept");
            connect(&Baccept, SIGNAL(clicked()), &dialog, SLOT(accept()));
            QPushButton Bcancel;
            Bcancel.setText("Cancel");
            connect(&Bcancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        buttons.addWidget(&Baccept);
        buttons.addWidget(&Bcancel);

        toplevellayout.addLayout(&resLayout);
        toplevellayout.addLayout(&framerateLayout);
        toplevellayout.addLayout(&buttons);
        dialog.setLayout(&toplevellayout);
        //GUI CONSTRUCTION END//

            if(dialog.exec() == QDialog::Accepted)
            {
                m_cvCapture.set(cv::CAP_PROP_FRAME_WIDTH, CBresolution.currentText().section(" x ",0,0).toDouble());
                m_cvCapture.set(cv::CAP_PROP_FRAME_HEIGHT, CBresolution.currentText().section(" x ",1,1).toDouble());
                m_cvCapture.set(cv::CAP_PROP_FPS, CBframerate.currentText().section(" ",0,0).toDouble());
                pt_timer->setInterval((int)1000/CBframerate.currentText().section(" ",0,0).toDouble());
                return true;
            }
    }
    return false; // false if device was not opened or dialog.exec() == QDialog::Rejected
}

bool QVideoCapture::open_settingsDialog()
{
    if (m_cvCapture.isOpened() && deviceFlag) {
        return m_cvCapture.set(cv::CAP_PROP_SETTINGS,0.0);       
    }
    return false; // false if device was not opened yet
}

bool QVideoCapture::isOpened()
{
    return m_cvCapture.isOpened();
}

QVideoCapture::~QVideoCapture()
{
    delete pt_timer;
}

int QVideoCapture::open_deviceSelectDialog()
{
    QDialog dialog;
    dialog.setFixedSize(256,128);
    dialog.setWindowTitle(tr("Device select dialog"));
    QVBoxLayout layout;
        QGroupBox GBid(tr("Select device from list"));
            QVBoxLayout Lid;
            QComboBox CBid;           
            QList<QByteArray> devices = QCamera::availableDevices();
            for(int i = 0; i < devices.size(); i++)
            {
                CBid.addItem(QCamera::deviceDescription(devices.at(i)), qVariantFromValue(devices.at(i)));
            }
            Lid.addWidget( &CBid );
       GBid.setLayout(&Lid);

       QHBoxLayout Lbuttons;
            QPushButton Baccept(tr("Accept"));
            connect(&Baccept, &QPushButton::clicked, &dialog, &QDialog::accept);
            QPushButton Breject(tr("Reject"));
            connect(&Breject, &QPushButton::clicked, &dialog, &QDialog::reject);
            Breject.setToolTip("Default device will be used if available");
       Lbuttons.addWidget(&Baccept);
       Lbuttons.addWidget(&Breject);
   layout.addWidget(&GBid);
   layout.addLayout(&Lbuttons);
   dialog.setLayout(&layout);

    if( dialog.exec() == QDialog::Accepted )   {
        device_id = CBid.currentIndex();
    }
    return device_id;
}

void QVideoCapture::initializeTimer()
{
    pt_timer = new QTimer();
    pt_timer->setTimerType(Qt::PreciseTimer);
    connect(pt_timer, SIGNAL( timeout() ), this, SLOT( read_frame() )); // makes a connection between timer signal and class slot
}
