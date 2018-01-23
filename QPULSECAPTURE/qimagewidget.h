/*--------------------------------------------------------------------------------------------
Taranov Alex, 2014                              		                   HEADER FILE
The Develnoter [internet], Marcelo Mottalli, 2014
This class is a descendant of QWidget with QImageWidget::updateImage(...) slot that constructs
QImage instance from cv::Mat image. The QImageWidget should be used as widget for video display  
 * ------------------------------------------------------------------------------------------*/

#ifndef QIMAGEWIDGET_H
#define QIMAGEWIDGET_H
//------------------------------------------------------------------------------------------------------
#ifdef USE_OPENGL_WIDGETS
    #include <QOpenGLWidget>
    #include <QSurfaceFormat>
#else
    #include <QWidget>
#endif
#include <QImage>
#include <QPen>
#include <QPainter>
#include <QPainterPath>
#include <QColor>
#include <QMouseEvent>

#include "opencv2/opencv.hpp"
//------------------------------------------------------------------------------------------------------

class QImageWidget : public WIDGET_CLASS
{
    Q_OBJECT

public:
    explicit QImageWidget(QWidget *parent = 0);
signals:
    //void selectionUpdated(const cv::Rect &value);
public slots:
    void updateImage(const cv::Mat &image, double frame_period); // takes cv::Mat image and converts it to the appropriate Qt QImage format
    void updateTemperature(qreal _value);
    void updateHR(double _hr, double _snr);
    void updateVPGSignal(const double *_signal, int _length);
    void updateStatus(QString _str);
    void increaseStringShift();
    void decreaseStringShift();    

private:
    void __updateViewRect();  // calculates m_viewRect by means of m_cvMat dimensions proportion
    void __drawTemperature(QPainter &painter, const QRect &input_rect);
    void __drawSelection(QPainter &painter);
    void __drawHeadContour(QPainter &painter, const QRect &input_rect);
    void __drawStatus(QPainter &painter, const QRect &input_rect);
    void __drawVPGSignal(QPainter &painter, const QRect &input_rect);
    void __drawHR(QPainter &painter, const QRect &input_rect);


    QImage m_qtImage;       // stores current QImage instance
    cv::Mat m_cvMat;        // stores current cv::Mat instance
    int x0;                 // mouse coordinate X
    int y0;                 // mouse coordinate Y
    QRect m_aimrect;        // stores current rectangle region on widget
    QRect m_viewRect;       // a rect inside widget's rect with image proportion of dimensions
    QRect m_selectionRect;  // a rect that stores user's last selection on m_viewRect
    cv::Rect m_Rect;        // a rect that stores coordinates of selected region on m_cvMat
    QColor m_infoColor;
    QColor m_selectionColor;
    QString m_statusString;
    qreal m_temperature = 0.0;
    int m_stringShift;    
    const double *pt_vpg = NULL;
    int m_vpglength = 0;
    double m_hr = -1.0;
    double m_snr = -1.0;
    QString m_frameproperties;

protected:
    void paintEvent(QPaintEvent *);
    //void mousePressEvent(QMouseEvent* event);
    //void mouseMoveEvent(QMouseEvent *event);
};

//------------------------------------------------------------------------------------------------------
#endif // QIMAGEWIDGET_H
