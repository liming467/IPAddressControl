#include "IPAddressControl.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIntValidator>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
IPAddressControl::IPAddressControl(QWidget* parent) : QFrame(parent)
{
	//setFrameShape(QFrame::StyledPanel);
	setFrameShape(QFrame::HLine);
	setFrameShadow(QFrame::Sunken);
	
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setSpacing(1);
	setLayout(layout);

	QHBoxLayout* ipLineEdit = new QHBoxLayout(this);
	layout->addLayout(ipLineEdit);

	//底部添加水平线
	QFrame* line = new QFrame(this);
	line->setFrameShape(QFrame::HLine);
	line->setFrameShadow(QFrame::Sunken);
	line->setLineWidth(1);
	layout->addWidget(line);
	
	ipLineEdit->setContentsMargins(0, 0, 1, 0);
	ipLineEdit->setSpacing(0);

	for (int i = 0; i != QTUTL_IP_SIZE; ++i)
	{
		if (i != 0)
		{
			QLabel* pDot = new QLabel(".", this);
			ipLineEdit->addWidget(pDot);
			ipLineEdit->setStretch(ipLineEdit->count(), 0);
		}

		m_pLineEdit.at(i) = new QLineEdit(this);
		QLineEdit* pEdit = m_pLineEdit.at(i);
		pEdit->installEventFilter(this);
		//pEdit->setMinimumWidth(20);

		QPalette palette = pEdit->palette();
		palette.setBrush(QPalette::Base, Qt::transparent);
		pEdit->setPalette(palette);

		ipLineEdit->addWidget(pEdit);
		ipLineEdit->setStretch(ipLineEdit->count(), 1);

		pEdit->setFrame(false);
		pEdit->setAlignment(Qt::AlignCenter);

		QFont font = pEdit->font();
		font.setStyleHint(QFont::Monospace);
		font.setFixedPitch(true);
		pEdit->setFont(font);

		QRegExp rx("^(0|[1-9]|[1-9][0-9]|1[0-9][0-9]|2([0-4][0-9]|5[0-5]))$");
		QValidator* validator = new QRegExpValidator(rx, pEdit);
		pEdit->setValidator(validator);
	}

	//设置控件最大宽度
	setMaximumWidth(40 * QTUTL_IP_SIZE);

	connect(this, &IPAddressControl::signalTextChanged, this, &IPAddressControl::slotTextChanged, Qt::QueuedConnection);
}

IPAddressControl::~IPAddressControl()
{
}

void IPAddressControl::slotTextChanged(QLineEdit* pEdit)
{
	for (unsigned int i = 0; i != QTUTL_IP_SIZE; ++i)
	{
		if (pEdit == m_pLineEdit.at(i))
		{
			if ((pEdit->text().size() == MAX_DIGITS && pEdit->text().size() == pEdit->cursorPosition())
				|| (pEdit->text() == "0"))
			{
				// auto-move to next item
				if (i + 1 != QTUTL_IP_SIZE)
				{
					m_pLineEdit.at(i + 1)->setFocus();
					m_pLineEdit.at(i + 1)->selectAll();
				}
			}
		}
	}
}

bool IPAddressControl::eventFilter(QObject* obj, QEvent* event)
{
	bool bRes = QFrame::eventFilter(obj, event);

	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent* pEvent = dynamic_cast<QKeyEvent*>(event);
		if (pEvent)
		{
			for (unsigned int i = 0; i != QTUTL_IP_SIZE; ++i)
			{
				QLineEdit* pEdit = m_pLineEdit[i];
				if (pEdit == obj)
				{
					switch (pEvent->key())
					{
					case Qt::Key_Left:
						if (pEdit->cursorPosition() == 0)
						{
							// user wants to move to previous item
							movePrevLineEdit(i);
						}
						break;

					case Qt::Key_Right:
						if (pEdit->text().isEmpty() || (pEdit->text().size() == pEdit->cursorPosition()))
						{
							// user wants to move to next item
							moveNextLineEdit(i);
						}
						break;

					case Qt::Key_0:
						if (pEdit->text().isEmpty() || pEdit->text() == "0")
						{
							pEdit->setText("0");
							// user wants to move to next item
							moveNextLineEdit(i);
						}
						emit signalTextChanged(pEdit);
						break;

					case Qt::Key_Backspace:
						if (pEdit->text().isEmpty() || pEdit->cursorPosition() == 0)
						{
							// user wants to move to previous item
							movePrevLineEdit(i);
						}
						break;

					case Qt::Key_Comma:
					case Qt::Key_Period:
						moveNextLineEdit(i);
						break;

					default:
						emit signalTextChanged(pEdit);
						break;
					}
				}
			}
		}
	}

	return bRes;
}

std::array<quint8, IPAddressControl::QTUTL_IP_SIZE> IPAddressControl::getIP() const
{
	std::array<quint8, QTUTL_IP_SIZE> ipAddr;
	std::transform(m_pLineEdit.cbegin(), m_pLineEdit.cend(), ipAddr.begin(),
		[](const QLineEdit* lineEdit) -> quint8 { return lineEdit->text().toUInt(); });
	return ipAddr;
}

QString IPAddressControl::getIPString()
{
	QString res = "";
	auto ipAddr = getIP();
	res.sprintf("%d.%d.%d.%d", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	return res;
}

void IPAddressControl::setIP(QString ipAddr)
{
	auto numArray = ipAddr.split(QRegExp("[.]+"), QString::SkipEmptyParts);
	if (numArray.size() == 4)
	{
		std::array<quint8, IPAddressControl::QTUTL_IP_SIZE> ipArray;
		for (int i=0;i< IPAddressControl::QTUTL_IP_SIZE;i++)
		{
			if (isAllPositive(numArray[i]))
			{
				ipArray[i] = numArray[i].toInt();
			}
			else return;			
		}
		setIP(ipArray);
	}
}

void IPAddressControl::setIP(std::array<quint8, IPAddressControl::QTUTL_IP_SIZE> ipAddr)
{
	for (auto i = 0; i != QTUTL_IP_SIZE; ++i)
	{
		m_pLineEdit.at(i)->setText(QString::number(ipAddr.at(i)));
	}
}

void IPAddressControl::moveNextLineEdit(int i)
{
	if (i + 1 != QTUTL_IP_SIZE)
	{
		m_pLineEdit.at(i + 1)->setFocus();
		m_pLineEdit.at(i + 1)->setCursorPosition(0);
		m_pLineEdit.at(i + 1)->selectAll();
	}
}

void IPAddressControl::movePrevLineEdit(int i)
{
	if (i != 0)
	{
		m_pLineEdit.at(i - 1)->setFocus();
		m_pLineEdit.at(i - 1)->setCursorPosition(m_pLineEdit[i - 1]->text().size());
		// m_pLineEdit[i-1]->selectAll();
	}
}

bool IPAddressControl::isAllPositive(const QString& str)
{
	bool ok;
	int num = str.toInt(&ok);
	return (ok && num >= 0);
}