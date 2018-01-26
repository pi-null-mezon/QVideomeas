/*------------------------------------------------------------------------------------------------------
Taranov Alex, 2014									     SOURCE FILE
Class that wraps opencv functions into Qt SIGNAL/SLOT interface
The simplest way to use it - rewrite appropriate section in QOpencvProcessor::custom_algorithm(...) slot
------------------------------------------------------------------------------------------------------*/

#include "qopencvprocessor.h"

//------------------------------------------------------------------------------------------------------

QOpencvProcessor::QOpencvProcessor(QObject *parent):
    QObject(parent)
{
    //Initialization
    m_cvRect.width = 0;
    m_cvRect.height = 0;
    m_timeCounter = cv::getTickCount();
    m_facesPos = 0;
    m_framesWithoutFace = 0;
    m_beginFlag = true;
}

//-----------------------------------------------------------------------------------------------------

void QOpencvProcessor::update_timeCounter()
{
    m_timeCounter = cv::getTickCount();
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::custom_algorithm(const cv::Mat &input)
{
    cv::Mat output(input); // Copy the header and pointer to data of input object
    cv::Mat temp; // temporary object

    //-------------CUSTOM ALGORITHM--------------

    //You can do here almost whatever you wnat...
    cv::cvtColor(input,temp,CV_BGR2GRAY);
    cv::Canny(temp,output, 15, 300);

    //----------END OF CUSTOM ALGORITHM----------

    //---------Drawing of rectangle--------------
    if( (m_cvRect.width > 0) && (m_cvRect.height > 0) )
    {
        cv::rectangle( output , m_cvRect, cv::Scalar(255,255,255), 1, CV_AA); // white color
    }

    //-------------Time measurement--------------
    m_framePeriod = (cv::getTickCount() -  m_timeCounter) * 1000.0 / cv::getTickFrequency(); // result is calculated in milliseconds
    m_timeCounter = cv::getTickCount();

    emit frame_was_processed(output, m_framePeriod);
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::setRect(const cv::Rect &input_rect)
{
     m_cvRect = input_rect;
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::searchFace(const cv::Mat &img)
{
    cv::Mat temp;
    cv::Rect centralRegion, faceDestinationRect, outerRect, innerRect;

    if( (float)img.cols / img.rows > 14.0 / 9.0) {
        if(img.cols >= 1280 && img.rows >= 720) {
            cv::resize(img, temp, cv::Size(640,360), 0.0, 0.0, CV_INTER_AREA);
        } else {
            temp = img;
        }
        cv::Rect imgRect(0,0,temp.cols,temp.rows);
        centralRegion = __portionRectfromCenter(imgRect, 0.5, 1.0) & imgRect;
        faceDestinationRect = __portionRectfromCenter(imgRect, 0.28, 0.5);
        outerRect = __portionRectfromCenter(faceDestinationRect, 1.35f,1.35f);
        innerRect = __portionRectfromCenter(faceDestinationRect, 0.75f, 0.75f);
    } else {
        if(img.cols >= 1024 && img.rows >= 768) {
            cv::resize(img, temp, cv::Size(640,480), 0.0, 0.0, CV_INTER_AREA);
        } else {
            temp = img;
        }
        cv::Rect imgRect(0,0,temp.cols,temp.rows);
        centralRegion = __portionRectfromCenter(imgRect, 0.5, 1.0) & imgRect;
        faceDestinationRect = __portionRectfromCenter(imgRect, 0.35, 0.5);
        outerRect = __portionRectfromCenter(faceDestinationRect, 1.35f,1.35f);
        innerRect = __portionRectfromCenter(faceDestinationRect, 0.75f, 0.75f);
    }


    std::vector<cv::Rect> v_objects;
    m_classifier.detectMultiScale(temp(centralRegion), v_objects, 1.25, 5, CV_HAAR_FIND_BIGGEST_OBJECT, cv::Size(100,100));
    for(uint i = 0; i < v_objects.size(); i++) {
        v_objects[i] += centralRegion.tl();
    }

    if(v_objects.size() != 0) {
        __updateFaceRects(v_objects[0]);
        m_framesWithoutFace = 0;
    } else {
       m_framesWithoutFace++;
    }
    if(m_framesWithoutFace == LENGTH_OF_FACE_RECTS_VECTOR) {
        m_beginFlag = true;
        __updateFaceRects(cv::Rect(0,0,0,0));
        m_beginFlag = true;
    }

    cv::Rect faceRect = __getMeanFaceRect() & cv::Rect(0,0,temp.cols,temp.rows);

    if(faceRect.area() == 0) {
        emit statusChanged( tr("Please, position your face") );
        emit outofFrame();
    } else if(__isRectInsideField(faceRect, outerRect, innerRect)) {
        emit statusChanged( ""/*tr("Right position")*/ );
        emit rightPosition();
    } else {
        emit statusChanged( tr("Face out of frame") );
        emit outofFrame();
    }
    // Visualize measurement frame
    /*cv::rectangle(temp, faceDestinationRect, cv::Scalar(255,255,255), 1, CV_AA);
    cv::rectangle(temp, outerRect, cv::Scalar(0,0,0), 1, CV_AA);
    cv::rectangle(temp, innerRect, cv::Scalar(0,0,0), 1, CV_AA);
    cv::rectangle(temp, faceRect, cv::Scalar(0,255,0), 1, CV_AA);*/

    m_framePeriod = (cv::getTickCount() -  m_timeCounter) * 1000.0 / cv::getTickFrequency(); // result is calculated in milliseconds
    m_timeCounter = cv::getTickCount();
    emit frame_was_processed(img, m_framePeriod);
}

//------------------------------------------------------------------------------------------------------

bool QOpencvProcessor::loadClassifier(const std::string &fileName)
{
    return m_classifier.load(fileName);
}

//------------------------------------------------------------------------------------------------------

void QOpencvProcessor::__updateFaceRects(const cv::Rect &rect)
{
    if(m_beginFlag == true) {
        for(quint8 i = 0; i < LENGTH_OF_FACE_RECTS_VECTOR; i++)
            v_faceRects[i] = rect;
        m_beginFlag = false;
    } else {
        v_faceRects[m_facesPos] = rect;
        m_facesPos = (m_facesPos+1) % LENGTH_OF_FACE_RECTS_VECTOR;
    }
}

//------------------------------------------------------------------------------------------------------

cv::Rect QOpencvProcessor::__getMeanFaceRect() const
{
    qreal x = 0.0;
    qreal y = 0.0;
    qreal w = 0.0;
    qreal h = 0.0;
    for(quint16 i = 0; i < LENGTH_OF_FACE_RECTS_VECTOR; i++)    {
        x += v_faceRects[i].x;
        y += v_faceRects[i].y;
        w += v_faceRects[i].width;
        h += v_faceRects[i].height;
    }
    x /= LENGTH_OF_FACE_RECTS_VECTOR;
    y /= LENGTH_OF_FACE_RECTS_VECTOR;
    w /= LENGTH_OF_FACE_RECTS_VECTOR;
    h /= LENGTH_OF_FACE_RECTS_VECTOR;
    return cv::Rect(x,y,w,h);
}

//------------------------------------------------------------------------------------------------------

bool QOpencvProcessor::__isRectInsideField(const cv::Rect &rect, const cv::Rect &outRect, const cv::Rect &inRect) const
{
    if( (rect.x > outRect.x) && (rect.x < inRect.x) && (rect.y > outRect.y) && (rect.y < inRect.y)
         && (rect.x + rect.width > inRect.x + inRect.width ) && (rect.x + rect.width < outRect.x + outRect.width)
         && (rect.y + rect.height > inRect.y + inRect.height ) && (rect.y + rect.height < outRect.y + outRect.height) )
        return true;
    else
        return false;
}

//------------------------------------------------------------------------------------------------------

cv::Rect QOpencvProcessor::__portionRectfromCenter(const cv::Rect &rect, float portionX, float portionY) const
{
    cv::Rect outputRect = cv::Rect(rect.x, rect.y, rect.width*portionX, rect.height*portionY);
    cv::Point shift = outputRect.br() - rect.br();
    outputRect -= shift/2.0;
    return outputRect;
}


