#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QLabel>
#include <QDesktopServices>
#include <QPushButton>
#include <QAction>
#include <QWidget>
#include <QVBoxLayout>
#include <QStatusBar>
#include <QMenu>
#include <QContextMenuEvent>
#include <QByteArray>
#include "qeasyplot.h"
#include "qserialprocessor.h"
#include "qmeasurementstreamprocessor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void createFaceView();
    void createActions();
    void createMenus();
    void createProcessors();

private slots:
    void connectToSensor();
    void onPause();
    void onResume();
    void onHelp();
    void onAbout();
    void startRecord();
    void onRecord(qreal value, bool objectFlag);

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    QWidget *pt_centralWidget;
    QVBoxLayout *pt_centralLayout;
    QEasyPlot *pt_plotterWidget;
    QAction *pt_connectAct;
    QAction *pt_aboutAct;
    QAction *pt_helpAct;
    QAction *pt_exitAct;
    QAction *pt_pauseAct;
    QAction *pt_resumeAct;
    QAction *pt_recordAct;
    QMenu *pt_fileMenu;
    QMenu *pt_helpMenu;
    QSerialProcessor *pt_sensor;
    QMeasurementStreamProcessor *pt_processor;
    QFile m_outputFile;
    QTextStream m_textStream;

};

#endif // MAINWINDOW_H
