#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
	void on_ptnGetIp_clicked()
	{
		auto ipAddr = ui.IPAddrControl->getIPString();
		information(ipAddr);
	}

private:
    Ui::MainWindowClass ui;

	inline void information(QString content, QString title = "Warning")
	{
		QMessageBox::information(nullptr, title, content);
	}
};
