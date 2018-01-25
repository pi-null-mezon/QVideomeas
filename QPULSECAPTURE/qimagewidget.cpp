/*--------------------------------------------------------------------------------------------
Taranov Alex, 2014                              		               CPP SOURCE FILE
The Develnoter [internet], Marcelo Mottalli, 2014
This class is a descendant of QWidget with QImageWidget::updateImage(...) slot that constructs
QImage instance from cv::Mat image. The QImageWidget should be used as widget for video display  
 * ------------------------------------------------------------------------------------------*/

#include "qimagewidget.h"

//-----------------------------------------------------------------------------------
QImageWidget::QImageWidget(QWidget *parent): WIDGET_CLASS(parent)
{
    #ifdef USE_OPENGL_WIDGETS
        QSurfaceFormat format;
        format.setSamples(3);
        this->setFormat(format);
    #endif
    m_infoColor = QColor(0,250,0);
    m_selectionColor = QColor(0,250,0);
    m_stringShift = 0;
}
//-----------------------------------------------------------------------------------
void QImageWidget::updateImage(const cv::Mat& image, double frame_period)
{
    m_frameproperties = QString::number(image.cols)
                        + "x" + QString::number(image.rows)
                        + "x" + QString::number(frame_period,'f',1) + " ms";
    switch ( image.type() ) {
        case CV_8UC1:
            cv::cvtColor(image, m_cvMat, CV_GRAY2RGB);
            break;
        case CV_8UC3:
            cv::cvtColor(image, m_cvMat, CV_BGR2RGB);
            break;
        case CV_8UC4:
            cv::cvtColor(image, m_cvMat, CV_BGRA2RGB);
            break;
    }
    assert(m_cvMat.isContinuous()); // QImage needs the data to be stored continuously in memory
    m_qtImage = QImage((const uchar*)m_cvMat.data, m_cvMat.cols, m_cvMat.rows, m_cvMat.cols * 3, QImage::Format_RGB888);  // Assign OpenCV's image buffer to the QImage
    update();
}
//------------------------------------------------------------------------------------
void QImageWidget::paintEvent(QPaintEvent* )
{
    QPainter painter(this);
    if(!m_qtImage.isNull()) {
        __updateViewRect();
        QPainter painter( this );        
        painter.fillRect(rect(),Qt::black);
        painter.drawImage(m_viewRect, m_qtImage); // Draw inside widget, the image is scaled to fit the rectangle
        painter.setRenderHint(QPainter::Antialiasing);

        __drawHeadContour(painter, m_viewRect);
        __drawStatus(painter, m_viewRect);
        __drawTemperature(painter, m_viewRect);
        __drawVPGSignal(painter, m_viewRect);
        __drawHR(painter, m_viewRect);
    } else {
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath _path;
        qreal pointsize = (qreal)rect().height() / 20.0;
        QFont font("Calibry", pointsize, QFont::Bold);
        QPen pen(Qt::NoBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setColor(QColor(0,0,0));
        painter.setPen( pen );
        painter.setBrush(Qt::white);
        QString _str = tr("Calibration in progress...");
        _path.addText(0,0,font,_str);
        painter.translate((rect().width() - _str.length()*pointsize),rect().height()/2.0);
        painter.setBrush(Qt::white);
        painter.drawPath(_path);
    }
}
//------------------------------------------------------------------------------------
/*void QImageWidget::mousePressEvent(QMouseEvent *event)
{
    x0 = event->x();
    y0 = event->y();
    m_aimrect.setX( x0 );
    m_aimrect.setY( y0 );

}*/
//--------------------------------------------------------------------------------------
/*void QImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if( event->x() > x0)
        m_aimrect.setWidth(event->x() - x0);
    else
    {
        m_aimrect.setX( event->x() );
        m_aimrect.setWidth( x0 - event->x() );
    }
    if( event->y() > y0)
        m_aimrect.setHeight(event->y() - y0);
    else
    {
        m_aimrect.setY( event->y() );
        m_aimrect.setHeight( y0 - event->y() );
    }
    if( !m_viewRect.isNull() ) {
        m_selectionRect = m_viewRect.intersected( m_aimrect );
        m_Rect.x = ( (qreal)(m_selectionRect.x() - m_viewRect.x())/m_viewRect.width() ) * m_cvMat.cols;
        m_Rect.y = ( (qreal)(m_selectionRect.y() - m_viewRect.y())/m_viewRect.height() ) * m_cvMat.rows;
        m_Rect.width = ( (qreal)m_selectionRect.width()/m_viewRect.width() ) * m_cvMat.cols;
        m_Rect.height = ( (qreal)m_selectionRect.height()/m_viewRect.height() ) * m_cvMat.rows;
    }
    emit selectionUpdated( m_Rect );
}*/
//------------------------------------------------------------------------------------
void QImageWidget::__drawTemperature(QPainter &painter, const QRect &input_rect)
{
    int _alpha = (255 - 5*m_stringShift) >=0 ? (255 - 5*m_stringShift) : 0;

    QPainterPath path;

    qreal pointsize = (qreal)input_rect.height() / 30.0;
    QFont font("Calibry", pointsize, QFont::DemiBold);
    QPen pen(Qt::NoBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setColor(QColor(0,0,0, _alpha));
    painter.setPen( pen );

    quint8 red,green,blue;
    QString status;
    if(m_temperature >= 31.0 && m_temperature <= 35.6) {
        red = 100;
        green = 255;
        blue = 100;
        status = tr("norm");
    } else if(m_temperature > 35.6) {
        red = 255;
        green = 100;
        blue = 100;
        status = tr("fever");
    } else {
        red = 0;
        green = 100;
        blue = 255;
        status = tr("hypotermia");
    }

    painter.setBrush( QColor(255,255,255, _alpha) );
    path.addText(input_rect.x() + 10 - 1.5*m_stringShift, input_rect.y() + font.pointSize() + 10, font, tr("Forehead temperature [°C]: ") + status );
    painter.drawPath(path);

    path = QPainterPath();
    painter.setBrush( QColor(red,green,blue, _alpha) );
    font.setPointSizeF(pointsize*2.0);
    path.addText(input_rect.x() + 10 - 1.5*m_stringShift, input_rect.y() + pointsize + 10 + 1.2*font.pointSize() + 10,  font, QString::number(m_temperature, 'f', 1));
    painter.drawPath(path);
}

//--------------------------------------------------------------------------------------
void QImageWidget::__updateViewRect()
{
    int cols = m_cvMat.cols;
    int rows = m_cvMat.rows;
    if( (cols > 0) && (rows > 0) )
    {
        m_viewRect = rect();
        int width = rect().width();
        int height = rect().height();
        if( ((qreal)cols/rows) > ((qreal)width/height) )
        {
            m_viewRect.setHeight( width * (qreal)rows/cols );
            m_viewRect.moveTop( (height - m_viewRect.height())/2.0 );
        }
        else
        {
            m_viewRect.setWidth( height * (qreal)cols/rows );
            m_viewRect.moveLeft( (width - m_viewRect.width())/2.0 );
        }
    }
    if((m_cvMat.rows > 0) && (m_cvMat.cols > 0))
    {
        m_selectionRect.setX( m_viewRect.x() + (m_viewRect.width() * (qreal)m_Rect.x/m_cvMat.cols) );
        m_selectionRect.setY( m_viewRect.y() + (m_viewRect.height() * (qreal)m_Rect.y/m_cvMat.rows) );
        m_selectionRect.setWidth( (m_viewRect.width() * (qreal)m_Rect.width/m_cvMat.cols) );
        m_selectionRect.setHeight( (m_viewRect.height() * (qreal)m_Rect.height/m_cvMat.rows) );
    }
}
//--------------------------------------------------------------------------------------
void QImageWidget::__drawSelection(QPainter &painter)
{
    if(!m_selectionRect.isNull())
    {
        QPen pen(Qt::NoBrush, 1.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setColor(m_selectionColor);
        painter.setPen( pen );
        painter.drawRect(m_selectionRect);
    }
}
//--------------------------------------------------------------------------------------
void QImageWidget::__drawHeadContour(QPainter &painter, const QRect &input_rect)
{
    painter.setPen(Qt::black);
    painter.setBrush(QColor(15,15,15,156));
    QPainterPath path;
    path.addRect(input_rect);
    QFont font("FilledHeadContours", input_rect.height()); // Font was created by means of https://icomoon.io and http://www.flaticon.com, thank You guys!
    if( (qreal)input_rect.width()/input_rect.height() < 14.0/9.0) {
        path.addText( input_rect.x(), input_rect.y() + 1.35 * input_rect.height() , font, ""); //Variants:   
    } else {
        path.addText( input_rect.x() + input_rect.height() / 4.5, input_rect.y() + 1.35 * input_rect.height() , font, "");
    }
    painter.drawPath(path);

    QRect _footerRect(input_rect.bottomLeft(), QSize(rect().width(), (rect().height() - input_rect.height())/1.9));
    painter.fillRect(_footerRect, Qt::black);
}

//---------------------------------------------------------------------------------------
void QImageWidget::__drawStatus(QPainter &painter, const QRect &input_rect)
{
    QPainterPath path;

    QFont font("Calibry", (qreal)input_rect.height() / 30.0, QFont::DemiBold);
    QPen pen(Qt::NoBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setColor(QColor(0,0,0));
    painter.setPen( pen );
    painter.setBrush(Qt::white);

    path.addText(input_rect.x() + 10, input_rect.y() + input_rect.height() - 20 - font.pointSize()*1.5, font, m_statusString);

    font.setPointSize(font.pointSizeF()/2.0);
    path.addText(input_rect.x() + 10, input_rect.y() + input_rect.height() - 20, font, m_frameproperties);
    painter.drawPath(path);
}

void QImageWidget::__drawVPGSignal(QPainter &painter, const QRect &input_rect)
{
    if(pt_vpg != NULL && m_vpglength > 0) {

        int _alpha = (255 - 5*m_stringShift) >=0 ? (255 - 5*m_stringShift) : 0;

        double startX = 0.2*input_rect.width();
        double stepX = (input_rect.width() - 2.0*startX) / m_vpglength;
        startX += input_rect.x();

        double startY = 0.9*input_rect.height() + input_rect.y();
        double stepY = 0.015*input_rect.width();

        QPainterPath path;
        path.moveTo(startX, (startY+stepY*pt_vpg[0]));
        for(int i = 1; i < m_vpglength; i++) {
            path.lineTo(startX+stepX*i, startY+stepY*pt_vpg[i]);
        }
        painter.setBrush(Qt::NoBrush);
        QPen pen(Qt::NoBrush, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        pen.setColor(QColor(100,250,100, _alpha));
        painter.setPen(pen);
        painter.drawPath(path);
    }
}

void QImageWidget::__drawHR(QPainter &painter, const QRect &input_rect)
{    
    int _alpha = ((255 - 5*m_stringShift) >=0 ? (255 - 5*m_stringShift) : 0);

    QPainterPath path;

    qreal pointsize = (qreal)input_rect.height() / 30.0;
    QFont font("Calibry", pointsize, QFont::DemiBold);
    QPen pen(Qt::NoBrush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    pen.setColor(QColor(0,0,0,_alpha));
    painter.setPen( pen );

    quint8 red,green,blue;
    QString status;
    if(m_hr >= 50.0 && m_hr <= 90.0) {
        red = 100;
        green = 255;
        blue = 100;
        status = tr("norm");
    } else if(m_hr > 90.0) {
        red = 255;
        green = 100;
        blue = 100;
        status = tr("tachycardia");
    } else {
        red = 100;
        green = 100;
        blue = 255;
        status = tr("bradycardia");
    }

    painter.setBrush( QColor(255,255,255, _alpha) );
    path.addText(input_rect.x() + 10 - 1.5*m_stringShift, input_rect.y() + input_rect.height()/4.0 + font.pointSize() + 10, font, tr("Heart rate [bpm]: ") + QString::number(m_snr,'f',1) + tr(" dB"));
    painter.drawPath(path);

    if(m_hr > 0.0 && m_hr < 300.0) {
        path = QPainterPath();
        painter.setBrush( QColor(red,green,blue, _alpha) );
        font.setPointSizeF(pointsize*2.0);
        path.addText(input_rect.x() + 10 - 1.5*m_stringShift, input_rect.y() + input_rect.height()/4.0 + pointsize + 10 + 1.2*font.pointSize() + 10, font, QString::number(std::round(m_hr)));
        painter.drawPath(path);
    }
}
//---------------------------------------------------------------------------------------
void QImageWidget::increaseStringShift()
{
    m_stringShift += 10;
    m_hr = -1.0;
    m_snr = -1.0;
    if(m_stringShift > 255) {        
        m_stringShift = 255;
    }
}
//---------------------------------------------------------------------------------------
void QImageWidget::decreaseStringShift()
{
    m_stringShift -= 10;
    if(m_stringShift <= 0) {
        m_stringShift = 0;
    }
}
//--------------------------------------------------------------------------------------

void QImageWidget::updateTemperature(qreal _value)
{
    m_temperature = _value;
}

void QImageWidget::updateStatus(QString _str)
{
    m_statusString = _str;
}

void QImageWidget::updateVPGSignal(const double *_signal, int _length)
{
    pt_vpg = _signal;
    m_vpglength = _length;
}

void QImageWidget::updateHR(double _hr, double _snr)
{
    m_hr = _hr;
    m_snr = _snr;
}

