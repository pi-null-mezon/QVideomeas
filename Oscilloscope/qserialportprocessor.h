#ifndef QSERIALPORTPROCESSOR_H
#define QSERIALPORTPROCESSOR_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGroupBox>
#include <QDebug>

class QSerialPortProcessor : public QObject
{
    Q_OBJECT
public:
    explicit QSerialPortProcessor(QObject *parent = 0);
    ~QSerialPortProcessor();

signals:
    void new_data(const QByteArray &array);

public slots:
    bool open();
    bool opendefault();
    bool close();
    bool openSerialPortSelectDialog();
    void handleErrors(QSerialPort::SerialPortError code);
    void readData();

private:
    QSerialPort *pt_serialPort;
    QSerialPortInfo m_serialInfo;
    bool m_deviceFlag;
    QByteArray m_byteArray;
};

#endif // QSERIALPORTPROCESSOR_H
