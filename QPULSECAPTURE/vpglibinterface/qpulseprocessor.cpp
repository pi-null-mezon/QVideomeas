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
}

QPulseProcessor::~QPulseProcessor()
{
    __releaseMemory();
    delete pt_faceproc;
}

void QPulseProcessor::initialize(double _videofps)
{
    double _frametime_ms;
    if(std::isinf(_videofps)) { // It sometimes appears
        _frametime_ms = 15.0;
    } else {
        _frametime_ms = 1000.0/_videofps;
    }
    __releaseMemory();
    pt_pulseproc = new vpg::PulseProcessor(_frametime_ms);
    m_countslefttoHRcomputations = pt_pulseproc->getLength();
    dropTimer();
    emit initialized();
}

void QPulseProcessor::enrollFrame(const cv::Mat &_mat)
{
    if(pt_pulseproc != NULL) {

        static double _value, _time;
        pt_faceproc->enrollImage(_mat,_value,_time);

        if(f_inposition) {
            pt_pulseproc->update(_value, _time);
            emit vpgUpdated(pt_pulseproc->getSignal(), pt_pulseproc->getLength());
            m_countslefttoHRcomputations--;
            int _count = pt_pulseproc->getLastPos();
            if( (_count % (pt_pulseproc->getLength()/7) == 0) && m_countslefttoHRcomputations < 0) {
                double _hr = pt_pulseproc->computeFrequency();
                double _snr = pt_pulseproc->getSNR();
                if(_snr > 3.0) {
                    emit hrUpdated(_hr, _snr);
                }
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
    m_countslefttoHRcomputations = pt_pulseproc->getLength();
}

void QPulseProcessor::__releaseMemory()
{
    delete pt_pulseproc;
    pt_pulseproc = NULL;
}
