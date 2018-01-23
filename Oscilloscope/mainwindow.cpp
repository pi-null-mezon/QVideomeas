#include "mainwindow.h"
#include <QMenuBar>
#include <QMessageBox>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define LAYOUT_MARGIN 5
//-------------------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumWidth(WINDOW_WIDTH/2);
    setMinimumHeight(WINDOW_HEIGHT/2);

    createFaceView();
    createActions();
    createMenus();
    createProcessors();

    statusBar()->showMessage("Context menu available on right click");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
}
//-------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    pt_sensor->close();
    pt_plotterWidget->set_externalArrays(NULL, NULL, 0);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::createFaceView()
{
    pt_centralWidget = new QWidget(this);
    pt_centralLayout = new QVBoxLayout();
    pt_centralLayout->setMargin(LAYOUT_MARGIN);
    setCentralWidget(pt_centralWidget);
    pt_centralWidget->setLayout(pt_centralLayout);
    pt_plotterWidget = new  QEasyPlot(this, "Count", "Temperature, °C");
    pt_centralLayout->addWidget(pt_plotterWidget);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::createActions()
{
    pt_exitAct = new QAction(tr("E&xit"), this);
    pt_exitAct->setStatusTip(tr("Exit from application"));
    connect(pt_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    pt_pauseAct = new QAction(tr("&Pause"), this);
    pt_pauseAct->setStatusTip(tr("Pause data capture"));
    connect(pt_pauseAct, SIGNAL(triggered()), this, SLOT(onPause()));

    pt_resumeAct = new QAction(tr("&Resume"), this);
    pt_resumeAct->setStatusTip(tr("Resume data capture"));
    connect(pt_resumeAct, SIGNAL(triggered()), this, SLOT(onResume()));

    pt_connectAct = new QAction(tr("&Connect"), this);
    pt_connectAct->setStatusTip(tr("Try to open connection with sensor"));
    connect(pt_connectAct, SIGNAL(triggered()), this, SLOT(connectToSensor()));

    pt_aboutAct = new QAction(tr("&About"), this);
    pt_aboutAct->setStatusTip(tr("Open application about window"));
    connect(pt_aboutAct, SIGNAL(triggered()), this, SLOT(onAbout()));

    pt_helpAct = new QAction(tr("&Help"), this);
    pt_helpAct->setStatusTip(tr("Open help link"));
    connect(pt_helpAct, SIGNAL(triggered()), this, SLOT(onHelp()));

    pt_recordAct = new QAction(tr("&Record"), this);
    pt_recordAct->setStatusTip(tr("Start new record or stop previous record"));
    connect(pt_recordAct, SIGNAL(triggered()), this, SLOT(startRecord()));
    pt_recordAct->setCheckable(true);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::createMenus()
{
    pt_fileMenu = menuBar()->addMenu(tr("&File"));
    pt_fileMenu->addAction(pt_connectAct);
    pt_fileMenu->addAction(pt_recordAct);
    pt_helpMenu = menuBar()->addMenu(tr("&Help"));
    pt_helpMenu->addAction(pt_helpAct);
    pt_helpMenu->addAction(pt_aboutAct);
}
//-------------------------------------------------------------------------------------------------
void MainWindow::createProcessors()
{
    pt_sensor = new QSerialProcessor(this);

    pt_processor = new QMeasurementStreamProcessor(this, 64);
    connect(pt_sensor, SIGNAL(new_data(QByteArray)), pt_processor, SLOT(processData(QByteArray)));
    connect(pt_processor, SIGNAL(dataProcessed(const qreal*,const qreal*,quint16)), pt_plotterWidget, SLOT(set_externalArrays(const qreal*,const qreal*,quint16)));
    connect(pt_processor, SIGNAL(temperatureMeasured(qreal,bool)), pt_plotterWidget, SLOT(updateStrings(qreal,bool)));
}
//-------------------------------------------------------------------------------------------------
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu tempMenu;
    tempMenu.addAction(pt_connectAct);
    tempMenu.addSeparator();
    tempMenu.addAction(pt_pauseAct);
    tempMenu.addAction(pt_resumeAct);
    tempMenu.exec(event->globalPos());
}
//-------------------------------------------------------------------------------------------------
void MainWindow::connectToSensor()
{
    if(pt_sensor->openSerialPortSelectDialog())
    {
        pt_sensor->open();
    }
}
//-------------------------------------------------------------------------------------------------
void MainWindow::onPause()
{
    pt_sensor->close();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::onResume()
{
    pt_sensor->open();
}
//-------------------------------------------------------------------------------------------------
void MainWindow::onHelp()
{
    QDesktopServices::openUrl(QUrl("https://github.com/pi-null-mezon/QArduinoTEMP.git", QUrl::TolerantMode));
}
//-------------------------------------------------------------------------------------------------
void MainWindow::onAbout()
{
    QDialog aboutDialog;
    aboutDialog.setWindowTitle("About " + QString(APP_NAME));
    aboutDialog.setFixedSize(256, 156);
    QVBoxLayout layout;
    layout.setMargin(5);
    aboutDialog.setLayout(&layout);

    QHBoxLayout buttonsLayout;
    buttonsLayout.setMargin(5);
    QPushButton helpButton(tr("Help"));
    connect(&helpButton, SIGNAL(clicked()), this, SLOT(onHelp()));
    QPushButton closeButton(tr("Close"));
    connect(&closeButton, SIGNAL(clicked()), &aboutDialog, SLOT(close()));
    buttonsLayout.addWidget(&helpButton);
    buttonsLayout.addWidget(&closeButton);

    QLabel appName( QString(APP_NAME) + " v" + QString(APP_VERSION) );
    appName.setAlignment(Qt::AlignCenter);
    appName.setFrameStyle(QFrame::Sunken | QFrame::Box);
    layout.addWidget(&appName);
    QLabel appAuthor( "Designer: " + QString(APP_AUTHOR) );
    layout.addWidget(&appAuthor, 0 , Qt::AlignCenter);
    QLabel appOrg(QString(APP_COMPANY) + " " + QString(APP_RELEASE_YEAR));
    layout.addWidget(&appOrg, 0 , Qt::AlignCenter);
    QLabel appEmail("<a href='mailto:pi-null-mezon@yandex.ru?subject=QArduinoTEMP'>Contact us at pi-null-mezon@yandex.ru</a>");
    appEmail.setOpenExternalLinks(true);
    layout.addWidget(&appEmail, 0 , Qt::AlignCenter);

    layout.addLayout(&buttonsLayout);
    aboutDialog.exec();
}

void MainWindow::startRecord()
{
    if(!m_outputFile.isOpen()) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "records/record.txt", tr("Text (*.txt)"));
        m_outputFile.setFileName(fileName);
        while(!m_outputFile.open(QIODevice::WriteOnly)) {
            QMessageBox msg(QMessageBox::Warning, APP_NAME, tr("Can not open file to write"), QMessageBox::Open | QMessageBox::Close, this);
            if(msg.exec() == QMessageBox::Open) {
                QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"), "records/record.txt", tr("Text (*.txt)"));
                m_outputFile.setFileName(fileName);
            } else {
                pt_recordAct->setChecked(false);
                break;
            }
        }
        if(m_outputFile.isOpen()) {
            m_textStream.setDevice(&m_outputFile);
            m_textStream << "mlx2arduino record on " << QDateTime::currentDateTime().toString("dd.MM.yyyy") << "\nhh.mm.ss.zzz\tObjectT, °C\tAmbientT, °C\n";
            connect(pt_processor, SIGNAL(temperatureMeasured(qreal,bool)), this, SLOT(onRecord(qreal,bool)));
            pt_recordAct->setChecked(true);
        }
    } else {
        disconnect(pt_processor, SIGNAL(temperatureMeasured(qreal,bool)), this, SLOT(onRecord(qreal,bool)));
        m_outputFile.close();
        pt_recordAct->setChecked(false);
    }
}

void MainWindow::onRecord(qreal value, bool objectFlag)
{
    if(objectFlag) {
        m_textStream << QDateTime::currentDateTime().toString("hh.mm.ss.zzz") + "\t" + QString::number(value, 'f', 2) + "\t";
    } else {
        m_textStream << QString::number(value, 'f', 2) + "\n";
    }
}
