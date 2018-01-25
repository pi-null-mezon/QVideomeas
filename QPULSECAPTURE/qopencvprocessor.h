/*------------------------------------------------------------------------------------------------------
Taranov Alex, 2014									     HEADER FILE
Class that wraps opencv functions into Qt SIGNAL/SLOT interface
The simplest way to use it - rewrite appropriate section in QOpencvProcessor::custom_algorithm(...) slot
------------------------------------------------------------------------------------------------------*/

#ifndef QOPENCVPROCESSOR_H
#define QOPENCVPROCESSOR_H
//------------------------------------------------------------------------------------------------------

#include <QObject>
#include <opencv2/opencv.hpp>

//------------------------------------------------------------------------------------------------------

#define LENGTH_OF_FACE_RECTS_VECTOR 8

class QOpencvProcessor : public QObject
{
    Q_OBJECT
public:
    explicit QOpencvProcessor(QObject *parent = 0);

signals:
    void frame_was_processed(const cv::Mat& value, double frame_period); //should be emited in the end of each frame processing
    void objectFound(const cv::Mat &objImg, double frame_period);
    void statusChanged(QString string);
    void rightPosition();
    void outofFrame();

public slots:
    void custom_algorithm(const cv::Mat& input);    // just a template of how a program logic should work
    void update_timeCounter();                       // use it in the beginning of any time-measurement operations
    void setRect(const cv::Rect &input_rect);       // set m_cvrect
    void searchFace(const cv::Mat &img);
    bool loadClassifier(const std::string &fileName);


private:
    int64 m_timeCounter;                        // stores time of application/computer start
    double m_framePeriod;                             // stores time of frame processing
    cv::Rect m_cvRect;                                // this rect is used by process_rectregion_pulse slot
    cv::CascadeClassifier m_classifier;

    cv::Rect v_faceRects[LENGTH_OF_FACE_RECTS_VECTOR];
    quint8 m_facesPos;
    quint8 m_framesWithoutFace;
    bool m_beginFlag;
    void __updateFaceRects(const cv::Rect &rect);
    cv::Rect __getMeanFaceRect() const;

    bool __isRectInsideField(const cv::Rect &rect, const cv::Rect &outRect, const cv::Rect &inRect) const;
    cv::Rect __portionRectfromCenter(const cv::Rect &rect, float portionX, float portionY) const;
};

//------------------------------------------------------------------------------------------------------
#endif // QOPENCVPROCESSOR_H
