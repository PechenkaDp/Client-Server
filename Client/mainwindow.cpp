#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(ui->requestButton, &QPushButton::clicked, this, &MainWindow::requestDeviceData);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::handleServerResponse);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::connectToServer() {
    socket->connectToHost("localhost", 10001);
    if (socket->waitForConnected(5000)) {
        ui->statusLabel->setText("Подключение установлено");
        ui->requestButton->setEnabled(true);
    } else {
        ui->statusLabel->setText("Ошибка подключения");
    }
}

void MainWindow::disconnectFromServer() {
    socket->disconnectFromHost();
    ui->statusLabel->setText("Подключение разорвано");
    ui->requestButton->setEnabled(false);
}

void MainWindow::requestDeviceData() {
    socket->write("GET_DEVICES\n");
}

void MainWindow::handleServerResponse() {
    while (socket->canReadLine()) {
        QString response = QString::fromUtf8(socket->readLine()).trimmed();
        ui->deviceListWidget->addItem(response);
    }
}
