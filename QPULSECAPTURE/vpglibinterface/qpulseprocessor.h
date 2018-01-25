#ifndef QPULSEPROCESSOR_H
#define QPULSEPROCESSOR_H

#include <QObject>
#include <vpg.h>

class QPulseProcessor : public QObject
{
    Q_OBJECT
public:
    explicit QPulseProcessor(QObject *parent = 0);
    ~QPulseProcessor();        

signals:
    void vpgUpdated(const double *_ptr, int _length);
    void hrUpdated(double _hr, double _snr);
    void initialized();

public slots:
    void initialize(double _videofps);
    void enrollFrame(const cv::Mat &_mat);
    void dropTimer();
    void setFaceInPosiiton();
    void setFaceOutOfPosition();

private:
    void __releaseMemory();

    vpg::FaceProcessor *pt_faceproc = NULL;
    vpg::PulseProcessor *pt_pulseproc = NULL;

    bool f_inposition = false;
    int m_countslefttoHRcomputations;
};

#endif // QPULSEPROCESSOR_H
