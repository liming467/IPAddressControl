#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.IPAddrControl->setIP("192.168.0.1");
}

MainWindow::~MainWindow()
{}
