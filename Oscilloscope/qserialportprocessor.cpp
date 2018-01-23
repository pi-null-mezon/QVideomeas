#include "qserialportprocessor.h"


QSerialPortProcessor::QSerialPortProcessor(QObject *parent) :
    QObject(parent),
    pt_serialPort(NULL),
    m_deviceFlag(false)
{

}

QSerialPortProcessor::~QSerialPortProcessor()
{
    this->close();
}

bool QSerialPortProcessor::openSerialPortSelectDialog()
{
    QDialog dialog;
    dialog.setWindowTitle(tr("SerialPort select"));
    dialog.setFixedSize(200,128);

    QVBoxLayout layout;

        QGroupBox groupbox(tr("Select port"));
        QVBoxLayout l_groupbox;
            QComboBox combobox;
            QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
            for(quint8 i = 0; i < ports.size(); i++)
            {
                combobox.addItem( ports.at(i).portName() + " - " + ports.at(i).description() );
            }
        l_groupbox.addWidget(&combobox);
        groupbox.setLayout(&l_groupbox);

        QHBoxLayout l_buttons;
            QPushButton b_accept(tr("Accept"));
            connect(&b_accept, SIGNAL(clicked()), &dialog, SLOT(accept()));
            QPushButton b_cancel(tr("Cancel"));
            connect(&b_cancel, SIGNAL(clicked()), &dialog, SLOT(reject()));
        l_buttons.addWidget(&b_accept);
        l_buttons.addWidget(&b_cancel);

     layout.addWidget(&groupbox);
     layout.addLayout(&l_buttons);

     dialog.setLayout(&layout);

     if(dialog.exec() == QDialog::Accepted)
     {
         m_serialInfo = ports[combobox.currentIndex()];
         m_deviceFlag = true;
     }
     else
     {
         m_deviceFlag = false;
     }
     return m_deviceFlag;
}

bool QSerialPortProcessor::open()
{
    this->close();

    if(m_deviceFlag)
    {
        pt_serialPort = new QSerialPort(m_serialInfo, this);
        connect(pt_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
        connect(pt_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleErrors(QSerialPort::SerialPortError)));
        if( pt_serialPort->open(QIODevice::ReadOnly) )
        {
            if( pt_serialPort->setBaudRate(QSerialPort::Baud9600)
                && pt_serialPort->setFlowControl(QSerialPort::NoFlowControl)
                && pt_serialPort->setDataBits(QSerialPort::Data8)
                && pt_serialPort->setParity(QSerialPort::NoParity)
                && pt_serialPort->setStopBits(QSerialPort::OneStop)
                && pt_serialPort->setDataTerminalReady(true) )
            {
                qInfo() << "Serial port has been selected:";
                qInfo() << "portName = " << pt_serialPort->portName();
                qInfo() << "baudRate = " << pt_serialPort->baudRate() << " bps";
                qInfo() << "dataBits = " << pt_serialPort->dataBits() << " bit";
                qInfo() << "flowControl = " << pt_serialPort->flowControl();
                qInfo() << "parity = " << pt_serialPort->parity();
                qInfo() << "stopBits = " << pt_serialPort->stopBits();

                qInfo() << "Default size of the serial port readBuffer = " << pt_serialPort->readBufferSize();
                //pt_serialPort->setReadBufferSize(512);
                //qWarning() << "current size of the serial port readBuffer = " << pt_serialPort->readBufferSize();
                if(pt_serialPort->error() != 0)
                {
                    return false;
                }
                return true;
            }
            else
            {
                qWarning() << "Can not set appropriate settings for the COM port!";
            }
        }
    }
    return false;
}


bool QSerialPortProcessor::opendefault()
{
    QList<QSerialPortInfo> l_ports = QSerialPortInfo::availablePorts();
    qInfo("Follow COM ports have been found:");
    for(int i = 0; i < l_ports.size(); i++) {
        qInfo("%d) %s %s", i, l_ports[i].portName().toLocal8Bit().constData(), l_ports[i].description().toLocal8Bit().constData());
        if(l_ports[i].description().contains("CH340")) {
            m_serialInfo = l_ports[i];
            m_deviceFlag = true;
            break;
        }
    }

    return open();
}


bool QSerialPortProcessor::close()
{
    if(pt_serialPort)
    {
        pt_serialPort->close();
        delete pt_serialPort;
        pt_serialPort = NULL;
        return true;
    }
    return false;
}

void QSerialPortProcessor::handleErrors(QSerialPort::SerialPortError code)
{
    if(code != QSerialPort::NoError) {
        // TO DO...
        qWarning() << code;
    }
}

void QSerialPortProcessor::readData()
{
    m_byteArray = pt_serialPort->readAll() ;
    emit new_data(m_byteArray);
}

