#include "qtemperatureprocessor.h"
//----------------------------------------------------------------------------------------------------------
QTemperatureProcessor::QTemperatureProcessor(QObject *parent, quint16 lengthOfData) :
    QObject(parent),
    m_curposObject(0),
    m_curposAmbient(0),
    m_datalength(lengthOfData),
    m_symbolsPosition(0),
    m_afterPointNumbers(0)
{
    v_objectOutput = new qreal[m_datalength];
    v_ambientOutput = new qreal[m_datalength];
    for(quint16 i = 0; i < m_datalength; i++)
    {
        v_objectOutput[i] = 0.0;
        v_ambientOutput[i] = 0.0;
    }
}
//----------------------------------------------------------------------------------------------------------
QTemperatureProcessor::~QTemperatureProcessor()
{
    delete[]  v_objectOutput;
    delete[]  v_ambientOutput;
}

double QTemperatureProcessor::getLastTemperature() const
{
    return v_objectOutput[loop(m_curposObject-1)];
}
//----------------------------------------------------------------------------------------------------------
void QTemperatureProcessor::processData(const QByteArray &input)
{
    char tempSymbol;

    for(quint16 i = 0; i < input.length(); i++)
    {
        tempSymbol = input.at(i);

        if(tempSymbol == 'O')
        {
            m_objectFlag = true;
        }

        if( (tempSymbol != 'O') && (tempSymbol != 'A') && (tempSymbol != ':') && (tempSymbol != '\n') )
        {
            v_symbols[m_symbolsPosition++] = tempSymbol;

            if(m_pointFlag)
            {
                m_afterPointNumbers++;

                if(m_afterPointNumbers == MEASUREMENT_RESOLUTION)
                {
                    qreal tempValue = QByteArray::fromRawData(v_symbols, m_symbolsPosition).toFloat();                    
                    if(m_objectFlag)
                    {
                        emit objtempUpdated(tempValue);
                        v_objectOutput[m_curposObject] = tempValue;
                        m_curposObject = (++m_curposObject) % m_datalength;
                        m_objectFlag = false;
                    }
                    else
                    {
                        emit ambtempUpdated(tempValue);
                        v_ambientOutput[m_curposObject] = tempValue;
                        m_curposAmbient = (++m_curposAmbient) % m_datalength;
                    }
                    m_afterPointNumbers = 0;
                    m_symbolsPosition = 0;
                    m_pointFlag = false;
                }
            }
            if(tempSymbol == '.')
            {
                m_pointFlag = true;
            }
        }
    }
    emit dataProcessed(v_objectOutput, v_ambientOutput, m_datalength);
}
