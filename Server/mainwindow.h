#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QUuid>
#include <QList>
#include <QRandomGenerator>

class Sensor {
public:
    Sensor(const QString &name, const QUuid &uuid) : name(name), id(uuid) {}

    QString getDeviceInfo() const {
        return QString("ID: %1, Name: %2, Status: %3")
                .arg(id.toString())
                .arg(name)
                .arg(QRandomGenerator::global()->bounded(2) ? "Да" : "Нет");
    }

private:
    QString name;
    QUuid id;
};

class BiometricScanner {
public:
    BiometricScanner(const QString &name, const QUuid &uuid)
        : name(name), id(uuid) {}

    QString getDeviceInfo() const {
        return QString("ID: %1, Name: %2, Status: Готов к проверке")
                .arg(id.toString())
                .arg(name);
    }

    bool checkBiometricData(const QByteArray &biometricData, const QByteArray &referenceData) {
        return QRandomGenerator::global()->bounded(2) == 0;
    }

private:
    QString name;
    QUuid id;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startServer();
    void stopServer();
    void handleNewConnection();
    void handleClientRequest();

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QList<QTcpSocket *> clients;
    QList<Sensor*> sensors;
    QList<BiometricScanner*> scanners;

    void initializeDevices();
};

#endif // MAINWINDOW_H
