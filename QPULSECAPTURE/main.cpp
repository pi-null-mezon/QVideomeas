#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

//------------Logging facilities-----------
#include <QDateTime>
#include <QTextStream>
#include <QFile>

QTextStream *p_logstream = NULL;
QFile *p_logfile = NULL;

void logOut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stderr, "%s\n", localMsg.constData());

    if(p_logstream) {
            switch (type) {
            case QtDebugMsg:
                *p_logstream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " [Debug]: " << localMsg << "\n\r";
                break;
            case QtInfoMsg:
                *p_logstream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " [Info]: " << localMsg << "\n\r";
                break;
            case QtWarningMsg:
                *p_logstream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " [Warning]: " << localMsg << "\n\r";
                break;
            case QtCriticalMsg:
                *p_logstream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " [Critical]: " << localMsg << "\n\r";
                break;
            case QtFatalMsg:
                *p_logstream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << " [Fatal]: " << localMsg << "\n\r";
                abort();
            }
            p_logstream->flush();

        // Check if logfile size exceeds approx. 100 MB
        if(p_logfile){
            if(p_logfile->size() > 100000000) { // in bytes
                QString _filename =p_logfile->fileName();
                p_logfile->close(); // explicitly close logfile before remove
                QFile::remove(_filename); // delete from the hard drive
                delete p_logfile; // clean memory
                p_logfile = new QFile(_filename);
                if(p_logfile->open(QIODevice::Append)) {
                    p_logstream->setDevice(p_logfile);
                    *p_logstream << "======================================================\n\r";
                }
            }
        }
    }
}
//-----------------------------------------


// This application should mimic to this legacy call:
// >> QPULSECAPTURE embedded C:/temp/meas.csv 0
// argv[1] should be embedded (unused in this version, was used to indicate that temperature sensor presents)
// argv[2] is a filename for the output file
// argv[3] is a number of COM port to communicate with temperature sensor (unused in this version, smart search has been implemented)

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create logfile in the directory where .exe resides-------------
    QTextStream _logstream;
    QString _logfilename = app.applicationDirPath().append("/logfile.txt");
    QFile *_plogfile = new QFile(_logfilename, NULL);
    if(_plogfile->open(QIODevice::Append)) {
        qInstallMessageHandler(logOut);
        _logstream.setDevice(_plogfile);
        _logstream << "======================================================\n\r";
        p_logfile = _plogfile;
        p_logstream = &_logstream;
    } else {
        qWarning("Can not create log file, check permissions!");
    }
    //-----------------------------------------------------------------

    QString _outputfilename;
    if(argc < 3) {
        qCritical("At least three command line arguments are required to start! Application will be started in demo mode.");
        //return -1;
    } else {
        _outputfilename = QString(argv[2]);
        qInfo("Measurements will be saved in %s", _outputfilename.toLocal8Bit().constData());
    }

    QTranslator qtranslator;
    if(qtranslator.load("Russian"))
        app.installTranslator(&qtranslator);

    MainWindow window;
    window.setOutputFilename(_outputfilename);
    window.show();

    return app.exec();
}
