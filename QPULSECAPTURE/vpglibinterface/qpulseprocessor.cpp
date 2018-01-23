#include "qpulseprocessor.h"
#include <QCoreApplication>

QPulseProcessor::QPulseProcessor(QObject *parent) : QObject(parent)
{
    std::string cascadefilename = QCoreApplication::applicationDirPath().append("/haarcascades/haarcascade_frontalface_alt2.xml").toStdString();;
    #ifdef HAARCASCADES_PATH // it is debug option
        cascadefilename = std::string(HAARCASCADES_PATH) + std::string("haarcascade_frontalface_alt2.xml");
    #endif
    pt_faceproc = new vpg::FaceProcessor(cascadefilename);
    if(pt_faceproc->empty()) {
        qCritical("QPulseProcessor: can not load classifier for the face detection!");
    }


    cv::VideoCapture _videocapture;
    double _frametime = -1.0;
    if(_videocapture.open(0)) {
        _videocapture.set(cv::CAP_PROP_ZOOM, 2.0);
        _videocapture.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        _videocapture.set(cv::CAP_PROP_FRAME_HEIGHT, 360);
        _videocapture.set(cv::CAP_PROP_FPS, 30);
        qInfo("Video source calibration... Please wait...");
        _frametime = pt_faceproc->measureFramePeriod(&_videocapture);
    }
    if(_frametime > 0.0) {
        qInfo("Video source has been calibrated, frametime %.2f ms", _frametime);
        pt_pulseproc = new vpg::PulseProcessor(_frametime);
    } else {
        qWarning("Video source can not be calibrated for some reasons!");
        pt_pulseproc = new vpg::PulseProcessor(33.0);
    }

    m_countslefttoHRcomputetions = pt_pulseproc->getLength();
}

QPulseProcessor::~QPulseProcessor()
{
    delete pt_faceproc;
    delete pt_pulseproc;
}

void QPulseProcessor::enrollFrame(const cv::Mat &_mat)
{
    double _value, _time;
    pt_faceproc->enrollImage(_mat,_value,_time);

    if(f_inposition) {
        pt_pulseproc->update(_value, _time);
        emit vpgUpdated(pt_pulseproc->getSignal(), pt_pulseproc->getLength());
        m_countslefttoHRcomputetions--;
        int _count = pt_pulseproc->getLastPos();
        if( (_count % (pt_pulseproc->getLength()/7) == 0) && m_countslefttoHRcomputetions < 0) {
            double _hr = pt_pulseproc->computeFrequency();
            double _snr = pt_pulseproc->getSNR();
            if(_snr > 3.0) {
                emit hrUpdated(_hr, _snr);
            }
        }
    }
}

void QPulseProcessor::dropTimer()
{
    pt_faceproc->dropTimer();
}

void QPulseProcessor::setFaceInPosiiton()
{
    f_inposition = true;
}

void QPulseProcessor::setFaceOutOfPosition()
{
    f_inposition = false;
    m_countslefttoHRcomputetions = pt_pulseproc->getLength();
}
