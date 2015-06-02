#include <QFile>
#include <QDebug>
#include <QtXml/QDomDocument>
#include "genericlanguage.hpp"
#include "generichighlighter.hpp"

GenericLanguage::GenericLanguage(QString id, QObject *parent) :
    Language(id, parent)
{

}

QSyntaxHighlighter *GenericLanguage::createSyntaxHighlighter() const
{
	auto *highlighter = new GenericHighlighter();
	for(const GenericHighlighter::HighlightingRule &rule : this->mRules)
	{
		highlighter->addRule(rule.pattern, rule.format);
	}
	for(const GenericHighlighter::BlockHighlightingRule &rule : this->mBlockRules)
	{
		highlighter->addBlockRule(rule.patternStart, rule.patternEnd, rule.format);
	}
	return highlighter;
}

QCompleter *GenericLanguage::createCompleter(QTextDocument *) const
{
	return new QCompleter(this->mKeywords);
}

static QTextCharFormat toFormat(const QDomElement &element)
{
	QTextCharFormat format;

	if(element.hasAttribute("foreground"))
		format.setForeground(QBrush(QColor(element.attribute("foreground"))));
	if(element.hasAttribute("background"))
		format.setBackground(QBrush(QColor(element.attribute("background"))));
	if(element.hasAttribute("italic"))
		format.setFontItalic(element.attribute("italic") == "true");
	if(element.hasAttribute("weight"))
	{
		QString weight = element.attribute("weight");
		if(weight == "light")
			format.setFontWeight(QFont::Light);
		else if(weight == "normal")
			format.setFontWeight(QFont::Normal);
		else if(weight == "demibold")
			format.setFontWeight(QFont::DemiBold);
		else if(weight == "bold")
			format.setFontWeight(QFont::Bold);
		else if(weight == "black")
			format.setFontWeight(QFont::Black);
	}
	return format;
}

GenericLanguage *GenericLanguage::load(const QString &fileName)
{
	QFile file(fileName);
	if(file.open(QFile::ReadOnly | QFile::Text) == false)
		return nullptr;
	QDomDocument doc;
	if(doc.setContent(&file) == false)  {
		file.close();
		return nullptr;
	}
	file.close();

	auto root = doc.firstChildElement();
	if(root.tagName() != "language")
		return nullptr;

	qDebug() << "Load language" << root.attribute("name");

    auto *language = new GenericLanguage(root.attribute("id"));
	language->mName = root.attribute("name");
	language->mExtensions = QRegExp(root.attribute("extension"), Qt::CaseInsensitive, QRegExp::WildcardUnix);

	auto nodes = root.childNodes();
	for(int i = 0; i < nodes.count(); i++)
	{
		QDomNode node = nodes.at(i);
		if(node.isElement() == false)
		{
			continue;
		}
		QDomElement element = node.toElement();
		if(element.tagName() == "format")
		{
			QString type = element.attribute("type");
			if(type == "keywords")
			{
				QStringList items = element.text().split(QRegExp("\\s+"), QString::SkipEmptyParts);
				language->mKeywords.append(items);

				auto format = toFormat(element);
				for(const QString &word : items)
				{
					QRegExp expr("\\b" + word + "\\b");
					language->mRules.append({expr, format});
				}
			}
			else
			{
				QRegExp expr(element.text());
				language->mRules.append({expr, toFormat(element)});
			}
		}
		else if(element.tagName() == "block")
		{
			QRegExp start(element.attribute("start")), end(element.attribute("end"));
			language->mBlockRules.append({start, end, toFormat(element)});
		}
		else
		{
			qDebug() << "Unknown tag:" << element.tagName();
		}
	}
	return language;
}
