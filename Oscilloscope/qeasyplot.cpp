#include <QPainter>
#include <QPainterPath>
#include <QColorDialog>
#include <QFontDialog>
#include <QLinearGradient>
#include <QBrush>
#include "qeasyplot.h"

#define DIMENSION_STEP 3

QEasyPlot::QEasyPlot(QWidget *parent) :
    QWidget(parent)
{
    set_defaultValues();
}

QEasyPlot::QEasyPlot(QWidget *parent, QString nameOfXaxis, QString nameOfYaxis) :
    QWidget(parent),
    m_X_axisName(nameOfXaxis),
    m_Y_axisName(nameOfYaxis)
{
    set_defaultValues();
}

void QEasyPlot::set_externalArrays(const qreal *pointer1, const qreal *pointer2, quint16 length)
{
    pt_Array1 = pointer1;
    pt_Array2 = pointer2;
    m_ArrayLength = length;
    if(m_DrawRegime != QEasyPlot::PhaseRegime)
    {
        set_horizontal_Borders(0, length-1);
    }
    update();
}


void QEasyPlot::set_defaultValues()
{
    pt_Array1 = NULL;
    pt_Array2 = NULL;
    m_ArrayLength = 0;
    //visual appearance
    m_textMargin = 3;
    m_backgroundColor = QColor(85, 85, 85);
    m_tracePen = QPen( Qt::NoBrush, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
    m_tracePen.setColor( Qt::green );
    m_coordinatePen = QPen( Qt::NoBrush, 0.35, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin );
    m_coordinatePen.setColor( Qt::white );
    m_coordinateSystemFont = QFont("Calibri", 10, QFont::DemiBold, true );
    m_DrawRegime = QEasyPlot::TraceRegime;

    set_X_Ticks(21);
    set_Y_Ticks(11);
    set_coordinatesPrecision(0, 2);
    set_horizontal_Borders(0, 63);
    set_vertical_Borders(-70.0, 280.0);

    f_enablegradient = false;
    f_drawsecondarray = true;
}

void QEasyPlot::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), m_backgroundColor);
    painter.setRenderHint(QPainter::Antialiasing);
    if(f_enablegradient) {
        draw_gradient(painter);
    }
    draw_strings(painter);
    draw_externalArray(painter);
    draw_coordinateSystem(painter);
}

void QEasyPlot::draw_coordinateSystem(QPainter &painter)
{
    painter.setPen(m_coordinatePen);
    painter.setFont(m_coordinateSystemFont);

    qreal widgetXstep = (qreal)width() / (m_XTicks - 1);
    qreal widgetYstep = (qreal)height() / (m_YTicks - 1);

    qreal tempCoordinate = widgetYstep;
    for(quint16 i = 1; i < m_YTicks - 1; i++)
    {
        painter.drawLine(0, tempCoordinate, width(), tempCoordinate);
        painter.drawText(m_textMargin, tempCoordinate - m_textMargin, QString::number((m_topBorder - i*m_yStep), 'f', m_coordinatesYPrecision));
        tempCoordinate += widgetYstep;
    }
    tempCoordinate = widgetXstep;
    for(quint16 i = 1; i < m_XTicks - 1; i++)
    {
        painter.drawLine(tempCoordinate, 0, tempCoordinate, height());
        painter.drawText(tempCoordinate + m_textMargin, height() - m_textMargin, QString::number((m_leftBorder + i*m_xStep), 'f', m_coordinatesXPrecision));
        tempCoordinate += widgetXstep;
    }
   painter.drawLine(convertXY(0.0,m_topBorder),convertXY(0.0,m_bottomBorder));
   painter.drawLine(convertXY(m_leftBorder,0.0),convertXY(m_rightBorder,0.0));

   painter.drawStaticText(m_textMargin, m_textMargin, m_Y_axisName);
   painter.drawStaticText(m_textMargin, height() - 2.5*m_coordinateSystemFont.pointSize() - m_textMargin , m_X_axisName);
}

QPointF QEasyPlot::convertXY(qreal x, qreal y) const
{
    return QPointF( (x - m_leftBorder)*width()/(m_rightBorder - m_leftBorder), (m_topBorder - y)*height()/(m_topBorder - m_bottomBorder) );
}

void QEasyPlot::draw_externalArray(QPainter &painter)
{
    if((pt_Array1 != NULL) && (pt_Array2 != NULL))
    {
        QPainterPath path1;
        QPainterPath path2;
        switch(m_DrawRegime)
        {
            case QEasyPlot::TraceRegime:
                m_tracePen.setColor(Qt::green);
                painter.setPen(m_tracePen);
                path1.moveTo(convertXY( 0.0 , pt_Array1[0] ));
                for(quint32 i = 1; i < m_ArrayLength; i++)
                {
                    path1.lineTo( convertXY( i, pt_Array1[i] ) );
                }
                painter.drawPath(path1);
                if(f_drawsecondarray == true) {
                    m_tracePen.setColor(Qt::yellow);
                    painter.setPen(m_tracePen);
                    path2.moveTo(convertXY( 0.0 , pt_Array2[0] ));
                    for(quint32 i = 1; i < m_ArrayLength; i++)
                    {
                        path2.lineTo( convertXY( i, pt_Array2[i] ) );
                    }
                    painter.drawPath(path2);
                }
            break;
            case QEasyPlot::FilledTraceRegime:
                path1.moveTo(convertXY( 0.0 , 0.0 )); // only for filling, from zero
                path1.lineTo(convertXY( 0.0 , pt_Array1[0] ));
                for(quint32 i = 1; i < m_ArrayLength; i++)
                {
                    path1.lineTo( convertXY( i, pt_Array1[i] ) );
                }
                path1.lineTo(convertXY( m_ArrayLength-1, 0.0 )); // only for filling, to zero
                painter.fillPath(path1, Qt::Dense3Pattern);
                painter.drawPath(path1);
            break;
            case QEasyPlot::PhaseRegime:
                path1.moveTo(convertXY( pt_Array1[0] , pt_Array1[DIMENSION_STEP] ));
                for(quint32 i = 1; i < (m_ArrayLength - DIMENSION_STEP); i++)
                {
                    path1.lineTo( convertXY( pt_Array1[i], pt_Array1[i + DIMENSION_STEP ]  ) );
                }
                painter.drawPath(path1);
            break;
        }
        //pt_Array1 = NULL;
        //pt_Array2 = NULL;
    }
}

bool QEasyPlot::set_horizontal_Borders(qreal left_value, qreal right_value)
{
    if(right_value > left_value)
    {
        m_rightBorder = right_value;
        m_leftBorder = left_value;
        update_X_TicksStep();
        return true;
    }
    return false;
}

bool QEasyPlot::set_vertical_Borders(qreal bottom_value, qreal top_value)
{
    if(top_value > bottom_value)
    {
        m_topBorder = top_value;
        m_bottomBorder = bottom_value;
        update_Y_TicksStep();
        return true;
    }
    return false;
}

void QEasyPlot::update_X_TicksStep()
{
    m_xStep = (m_rightBorder - m_leftBorder) / (m_XTicks - 1);
}

void QEasyPlot::update_Y_TicksStep()
{
    m_yStep = (m_topBorder - m_bottomBorder) / (m_YTicks - 1);
}

void QEasyPlot::set_coordinatesPrecision(quint8 value_for_x, quint8 value_for_y)
{
   m_coordinatesXPrecision = value_for_x;
   m_coordinatesYPrecision = value_for_y;
}

bool QEasyPlot::set_X_Ticks(quint16 value)
{
    if(value > 2)
    {
        m_XTicks = value;
        update_X_TicksStep();
        return true;
    }
    return false;
}

bool QEasyPlot::set_Y_Ticks(quint16 value)
{
    if(value > 2)
    {
        m_YTicks = value;
        update_Y_TicksStep();
        return true;
    }
    return false;
}

void QEasyPlot::set_coordinatePen(const QPen &pen,const QColor &color)
{
    m_coordinatePen = pen;
    m_coordinatePen.setColor(color);
}

void QEasyPlot::set_tracePen(const QPen &pen, const QColor &color)
{
    m_tracePen = pen;
    m_tracePen.setColor(color);
}

bool QEasyPlot::open_colorSelectDialog_for(VisualEntity value)
{
    QColor default_color;
    switch(value)
    {
        case QEasyPlot::Background:
            default_color = QColor(55,55,55);
            break;
        case QEasyPlot::Coordinates:
            default_color = QColor(Qt::white);
            break;
        case QEasyPlot::Trace:
            default_color = QColor(Qt::green);
            break;
        default:
            default_color = Qt::red;
    }
    QColor result_color = QColorDialog::getColor(default_color, this);
    if(result_color.isValid())
    {
        switch(value)
        {
            case QEasyPlot::Background:
                m_backgroundColor = result_color;
                break;
            case QEasyPlot::Coordinates:
                m_coordinatePen.setColor( result_color );
                break;
            case QEasyPlot::Trace:
                m_tracePen.setColor( result_color );
                break;
        }
        return true;
    }
    return false;
}

bool QEasyPlot::open_fontSelectDialog()
{
    bool result_flag;
    QFont result_font = QFontDialog::getFont(&result_flag, m_coordinateSystemFont, this);
    if(result_flag)
    {
        m_coordinateSystemFont = result_font;
        return true;
    }
    return false;
}

void QEasyPlot::set_axis_names(const QString &name_for_x, const QString &name_for_y)
{
    m_X_axisName = QStaticText(name_for_x);
    m_Y_axisName = QStaticText(name_for_y);
}

void QEasyPlot::set_DrawRegime(DrawRegime value)
{
    m_DrawRegime = value;
}

bool QEasyPlot::open_traceColorDialog()
{
    return open_colorSelectDialog_for(Trace);
}

bool QEasyPlot::open_backgroundColorDialog()
{
    return open_colorSelectDialog_for(Background);
}

bool QEasyPlot::open_coordinatesystemColorDialog()
{
    return open_colorSelectDialog_for(Coordinates);
}

void QEasyPlot::updateStrings(qreal value, bool objectMeasurementFlag)
{
    if(objectMeasurementFlag)
        m_objectTempString = "Obj: " + QString::number(value, 'f', 2) + " °C";
    else
        m_ambientTempString = "Amb: " + QString::number(value, 'f', 2) + " °C";
}

void QEasyPlot::draw_strings(QPainter &painter)
{
    if(!m_objectTempString.isEmpty() && !m_ambientTempString.isEmpty() ) {
        int fontSize = width()/40;
        painter.setFont( QFont("Tahoma", fontSize, QFont::DemiBold, true) );

        int startX = 3*width()/4;
        int startY = 2*fontSize;
        painter.setPen(Qt::green);
        painter.drawText(startX, startY, m_objectTempString );
        startY += startY;
        painter.setPen(Qt::yellow);
        painter.drawText(startX, startY, m_ambientTempString );
    }
}

void QEasyPlot::draw_gradient(QPainter &painter)
{
    quint8 opacity = 100;
    QLinearGradient gradient(convertXY(0.0,33.3), convertXY(0.0,29.3));
    gradient.setColorAt(0.0, QColor(0,255,0, opacity));
    gradient.setColorAt(0.5, QColor(255,255,0, opacity));
    gradient.setColorAt(1.0, QColor(255,0,0, opacity));
    gradient.setSpread(QGradient::ReflectSpread);

    QBrush brush(gradient);
    painter.fillRect(rect(),brush);
}

void QEasyPlot::setEnableGradient(bool value)
{
    f_enablegradient = value;
}

void QEasyPlot::setDrawSecondArray(bool value)
{
    f_drawsecondarray = value;
}

