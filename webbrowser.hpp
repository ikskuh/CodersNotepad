#ifndef WEBBROWSER_HPP
#define WEBBROWSER_HPP

#include <QWidget>
#include <QtWebKitWidgets/QWebView>
#include <QLineEdit>

class WebBrowser :
		public QWidget
{
	Q_OBJECT
public:
	explicit WebBrowser(QWidget *parent = 0);

signals:

public slots:

	void navigate();

	void navigateTo(const QUrl &url);

	void search(const QString &what);

private:
	void updateUrlField(const QUrl &);

private:
	QLineEdit *mUrl;
	QWebView *mView;
};

#endif // WEBBROWSER_HPP
