#ifndef QTEMPERATUREPROCESSOR_H
#define QTEMPERATUREPROCESSOR_H

#include <QObject>
#include <QByteArray>

#define SYMBOLS_COUNT 6 // for the instance XX.XX come to 5 char symbols and YYY.YY to 6, note however that QByteArray(pointer_to_char) will stop conversion only at '\0' symbol!
#define MEASUREMENT_RESOLUTION 2 // after point precision/resolution: NNN.X -> 1 or NNN.YY -> 2, note that by default Arduino sends only 2 symbols after point
//------------------------------------------------------------------------------------------------
class QTemperatureProcessor : public QObject
{
    Q_OBJECT
public:
    explicit QTemperatureProcessor(QObject *parent = 0, quint16 lengthOfData = 64);
    ~QTemperatureProcessor();
    double getLastTemperature() const;

signals:
    void dataProcessed(const qreal *pointer1, const qreal *pointer2, quint16 length);
    void objtempUpdated(qreal value);
    void ambtempUpdated(qreal value);

public slots:
    void processData(const QByteArray& input);

private:
    int loop(int difference) const;

private:
    qreal *v_objectOutput;
    qreal *v_ambientOutput;
    quint16 m_curposObject;
    quint16 m_curposAmbient;
    quint16 m_datalength;
    quint8 m_afterPointNumbers;
    bool m_objectFlag;
    bool m_pointFlag;
    char v_symbols[SYMBOLS_COUNT];
    quint8 m_symbolsPosition;

};
//------------------------------------------------------------------------------------------------
inline int QTemperatureProcessor::loop(int difference) const
{
    return((m_datalength + (difference % m_datalength)) % m_datalength);
}
//------------------------------------------------------------------------------------------------

#endif // QTEMPERATUREPROCESSOR_H
