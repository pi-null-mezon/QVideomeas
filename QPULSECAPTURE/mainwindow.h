#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//------------------------------------------------------------------------------------------------------

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QLayout>
#include <QLabel>
#include <QThread>
#include <QStatusBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QKeyEvent>
#include <QProcess>
#include <QUrl>

#include <QFile>
#include <QTextStream>

#include "qimagewidget.h"
#include "qopencvprocessor.h"
#include "qvideocapture.h"
#include "qserialportprocessor.h"
#include "qtemperatureprocessor.h"
#include "qeasyplot.h"
#include "qseriesanalyzer.h"
#include "qpulseprocessor.h"

//------------------------------------------------------------------------------------------------------
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);
    void closeEvent(QCloseEvent *_event);

public slots:
    bool openvideofile();
    bool opendevice();
    void opendeviceresolutiondialog();
    void opendevicesettingsdialog();
    void show_about();
    void show_help();
    void showPlot();
    void saveMeasuremets(double _hr, double _snr);
    void setOutputFilename(QString _filename);

protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void __connectToClose();

private:
    void createActions();
    void createMenus();
    void createTimers();
    void createThreads();
    void adjustSettings();    

    QAction *pt_openVideoAct;
    QAction *pt_openDeviceAct;
    QAction *pt_exitAct;
    QAction *pt_aboutAct;
    QAction *pt_helpAct;
    QAction *pt_pauseAct;
    QAction *pt_resumeAct;
    QAction *pt_deviceResAct;
    QAction *pt_deviceSetAct;
    QAction *pt_openMlxDevice;
    QAction *pt_plotAct;
    QAction *pt_fullscreenAct;

    QMenu *pt_fileMenu;
    QMenu *pt_helpMenu;
    QMenu *pt_settingsMenu;
    QMenu *pt_dataMenu;
    QMenu *pt_viewMenu;

    QVideoCapture *pt_videoCapture;
    QThread *pt_videoThread;

    QOpencvProcessor *pt_opencvProcessor;
    QThread *pt_improcThread;

    QSerialPortProcessor *pt_serialProcessor;
    QTemperatureProcessor *pt_temperatureProcessor;

    QPulseProcessor *pt_pulseproc;
    QThread *pt_vpgThread;

    QImageWidget *pt_display;
    QEasyPlot m_tempplot;
    QString m_filename;
};
//------------------------------------------------------------------------------------------------------
#endif // MAINWINDOW_H
