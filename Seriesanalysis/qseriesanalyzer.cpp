#include "qseriesanalyzer.h"

QSeriesAnalyzer::QSeriesAnalyzer(QObject *parent, uint window, uint overlay, real koeff)
    : QObject(parent),
      m_seriesCount(0),
      m_currentIndex(0)
{
    pt_analyzer = new SeriesAnalyzer(window, overlay, koeff);
}

QSeriesAnalyzer::~QSeriesAnalyzer()
{
    delete pt_analyzer;
}

void QSeriesAnalyzer::setWindowWidth(uint value)
{
    pt_analyzer->setWindowsize(value);
}

void QSeriesAnalyzer::setOverlay(uint value)
{
    pt_analyzer->setOverlaysize(value);
}

void QSeriesAnalyzer::setIntervalFactor(real value)
{
    pt_analyzer->setIntervalFactor(value);
}

void QSeriesAnalyzer::updateData(qreal value)
{
    /*pt_analyzer->enrollNextValue(value);
    const int temp = pt_analyzer->getRecordsCount();
    if(m_seriesCount < temp) {
        m_seriesCount = temp;
        m_currentIndex = temp - 1;
        DataSeria tempSeria = pt_analyzer->getRecord(m_currentIndex);
        emit seriesFound(tempSeria, m_seriesCount, m_currentIndex);
        qWarning("Last seria temperature was %f.1 degrees of Celsius", tempSeria.mean);
        if(tempSeria.seriatype == DataSeria::StationarSeria)
            emit stationarSeriaFound(tempSeria.mean);

    }*/

    if(f_enableOutput) {
        v_measurements.push_back(value);
        if(v_measurements.size() > 5) {
            real temp = 0.0;
            for(uint i = 0; i < v_measurements.size(); i++) {
                temp += v_measurements[i];
            }
            temp /= v_measurements.size();
            emit stationarSeriaFound(temp);
        }
    } else {
        v_measurements.clear();
    }
}

void QSeriesAnalyzer::endAnalysis()
{
    pt_analyzer->endAnalysis();
    const int temp = pt_analyzer->getRecordsCount();
    if(m_seriesCount < temp) {
        m_seriesCount = temp;
        m_currentIndex = temp - 1;
        emit seriesFound(pt_analyzer->getRecord(m_currentIndex), m_seriesCount, m_currentIndex);
    }
}

void QSeriesAnalyzer::clearSeriesHistory()
{
    pt_analyzer->clear();
    m_seriesCount = 0;
    m_currentIndex = 0;
}

void QSeriesAnalyzer::stepBackward()
{
    if(m_currentIndex > 0) {
        m_currentIndex--;
        emit seriesFound(pt_analyzer->getRecord(m_currentIndex), m_seriesCount, m_currentIndex);
    }
}

void QSeriesAnalyzer::stepForward()
{
    if(m_currentIndex < (m_seriesCount - 1)) {
        m_currentIndex++;
        emit seriesFound(pt_analyzer->getRecord(m_currentIndex), m_seriesCount, m_currentIndex);
    }
}

void QSeriesAnalyzer::enableOutput()
{
    f_enableOutput = true;
}

void QSeriesAnalyzer::disableOutput()
{
    f_enableOutput = false;
}

