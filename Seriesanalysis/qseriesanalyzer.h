#ifndef QSERIESANALYZER_H
#define QSERIESANALYZER_H

#include <QObject>
#include "seriesanalyzer.h"

class QSeriesAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit QSeriesAnalyzer(QObject *parent = 0, uint window = 10, uint overlay = 2, real koeff = 3.0);
    ~QSeriesAnalyzer();
signals:
    void seriesFound(const DataSeria &seria, uint total, uint current);
    void stationarSeriaFound(qreal meanValue);

public slots:
    void setWindowWidth(uint value);
    void setOverlay(uint value);
    void setIntervalFactor(real value);
    void updateData(qreal value);
    void endAnalysis();
    void clearSeriesHistory();
    void stepBackward();
    void stepForward();
    void enableOutput();
    void disableOutput();

private:
    SeriesAnalyzer *pt_analyzer;
    uint m_seriesCount;
    uint m_currentIndex;

    bool f_enableOutput;
    std::vector<real> v_measurements;
};

#endif // QSERIESANALYZER_H
