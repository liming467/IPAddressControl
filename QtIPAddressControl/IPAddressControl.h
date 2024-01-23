#pragma once
#include <QFrame>
#include <array>
#include <QRegularExpression>


/// Thanx to https://stackoverflow.com/a/11358560/8524139
class QLineEdit;
class IPAddressControl : public QFrame
{
	Q_OBJECT
		enum
	{
		QTUTL_IP_SIZE = 4,
		MAX_DIGITS = 3
	};

public:
	IPAddressControl(QWidget* parent = 0);
	~IPAddressControl();

	virtual bool eventFilter(QObject* obj, QEvent* event);

	QString getIPString();
	void setIP(QString ipAddr);	

signals:
	void signalTextChanged(QLineEdit* pEdit);

private:
	std::array<QLineEdit*, QTUTL_IP_SIZE> m_pLineEdit;
	void slotTextChanged(QLineEdit* pEdit);
	void moveNextLineEdit(int i);
	void movePrevLineEdit(int i);

	std::array<quint8, QTUTL_IP_SIZE> getIP() const;
	void setIP(std::array<quint8, QTUTL_IP_SIZE> ipAddr);

	bool isAllPositive(const QString& str);
};
