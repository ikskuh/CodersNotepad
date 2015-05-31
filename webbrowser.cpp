#include "webbrowser.hpp"

#include <QVBoxLayout>
#include <QToolBar>
#include <QUrlQuery>
#include <QDebug>

WebBrowser::WebBrowser(QWidget *parent) : QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	{
		this->mView = new QWebView();
		this->mView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		this->mView->load(QUrl("http://mq32.de"));
		connect(this->mView, &QWebView::urlChanged, this, &WebBrowser::updateUrlField);

		auto *bar = new QToolBar("Navigation");
		bar->setMaximumHeight(32);
		bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		{
			bar->addAction(QIcon("://Icons/appbar.navigate.previous.svg"), "Back", this->mView, SLOT(back()));
			bar->addAction(QIcon("://Icons/appbar.navigate.next.svg"), "Forward", this->mView, SLOT(forward()));
			bar->addAction(QIcon("://Icons/appbar.refresh.svg"), "Refresh", this->mView, SLOT(reload()));
			this->mUrl = new QLineEdit();
			this->mUrl->setPlaceholderText("http://...");
			connect(this->mUrl, &QLineEdit::returnPressed, this, &WebBrowser::navigate);
			bar->addWidget(this->mUrl);
			bar->addAction(QIcon("://Icons/appbar.arrow.right.svg"), "Navigate", this, SLOT(navigate()));
		}
		layout->addWidget(bar);
		layout->addWidget(this->mView);
	}
	this->setLayout(layout);
}

void WebBrowser::updateUrlField(const QUrl &url)
{
	this->mUrl->setText(url.toString());
}

void WebBrowser::navigate()
{
	QUrl url(this->mUrl->text());
	if(url.scheme().isEmpty())
		url.setScheme("http");
	this->navigateTo(url);
}

void WebBrowser::navigateTo(const QUrl &url)
{
	this->mView->load(url);
}

void WebBrowser::search(const QString &what)
{
	QUrlQuery query;
	query.addQueryItem("q", what);

	this->navigateTo(QUrl("http://www.google.com/search?" + query.toString()));
}
