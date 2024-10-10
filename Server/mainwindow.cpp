#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    server = new QTcpServer(this);
    initializeDevices();

    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startServer);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::stopServer);
    connect(server, &QTcpServer::newConnection, this, &MainWindow::handleNewConnection);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::initializeDevices() {
    for (int i = 0; i < 5; ++i) {
        sensors.append(new Sensor("Сенсор №" + QString::number(i + 1), QUuid::createUuid()));
        scanners.append(new BiometricScanner("Биометрический сканнер №" + QString::number(i + 1), QUuid::createUuid()));
    }
}

void MainWindow::startServer() {
    if (server->listen(QHostAddress::Any, 10001)) {
        ui->statusLabel->setText("Сервер запущен на порту 10001");
        ui->startButton->setEnabled(false);
        ui->stopButton->setEnabled(true);
    } else {
        ui->statusLabel->setText("Ошибка запуска сервера");
    }
}

void MainWindow::stopServer() {
    for (QTcpSocket *clientSocket : clients) {
        clientSocket->disconnectFromHost();
        clientSocket->waitForDisconnected();
    }
    clients.clear();

    server->close();
    ui->statusLabel->setText("Сервер остановлен");
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
}

void MainWindow::handleNewConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &MainWindow::handleClientRequest);

    ui->deviceListWidget->addItem("Подключен новый клиент");
}

void MainWindow::handleClientRequest() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    while (clientSocket->canReadLine()) {
        QString request = QString::fromUtf8(clientSocket->readLine()).trimmed();
        ui->deviceListWidget->addItem("Запрос от клиента: " + request);

        if (request == "GET_DEVICES") {
            QStringList deviceInfoList;
            for (auto sensor : sensors) {
                deviceInfoList << sensor->getDeviceInfo();
            }
            for (auto scanner : scanners) {
                deviceInfoList << scanner->getDeviceInfo();
            }
            clientSocket->write(deviceInfoList.join("\n").toUtf8() + "\n");
        } else {
            clientSocket->write("Неизвестный запрос\n");
        }
    }
}
