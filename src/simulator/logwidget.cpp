#include "logwidget.h"

LogWidget::LogWidget() : QDockWidget()
{
	setWindowTitle("LogWidget");
	setWidget(&textEdit);

	textEdit.setReadOnly(true);
	textEdit.setAcceptRichText(false);
}

LogWidget::~LogWidget()
{
}

void LogWidget::appendLine(QString &line)
{
	textEdit.append(line);
}

void LogWidget::UdpServer(const char *fmt, ...)
{
	char buf[256], *ptr = buf;
	int size = 0, remaining = sizeof (buf) - 7;   // strlen(" (...)")+1

	struct timeval now;
	gettimeofday (&now, 0x0);
	struct tm *tm = localtime (&now.tv_sec);


	size = sprintf (buf, "%02i:%02i:%02i.%06i ",
		tm->tm_hour,
		tm->tm_min,
		tm->tm_sec,
		(int)now.tv_usec);

	ptr += size;
	remaining -= size;

	// now the variable part
	va_list args;

	va_start (args, fmt);
	int inc = vsnprintf (ptr, remaining, fmt, args);
	va_end (args);

	if (inc > remaining)   // `man vsnprintf'
	{
		inc = remaining;
	}

	ptr += inc;
	size += inc;
	remaining -= inc;

	if(remaining < 1)
	{
		size += sprintf (ptr, " (...)");
	}

	QString text(buf);
	appendLine(text);
}
