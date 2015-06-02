#ifndef GENERICLANGUAGE_HPP
#define GENERICLANGUAGE_HPP

#include <QStringList>
#include <QVector>
#include "language.h"
#include "generichighlighter.hpp"

class GenericLanguage :
		public Language
{
	Q_OBJECT
private:
	QString mName;
	QRegExp mExtensions;
	QStringList mKeywords;
	QVector<GenericHighlighter::HighlightingRule> mRules;
	QVector<GenericHighlighter::BlockHighlightingRule> mBlockRules;
public:
    explicit GenericLanguage(QString id, QObject *parent = 0);

	static GenericLanguage *load(const QString &fileName);

	virtual QSyntaxHighlighter *createSyntaxHighlighter() const override;

	virtual QCompleter *createCompleter(QTextDocument *) const override;

	virtual QString name() const override
	{
		return this->mName;
	}

	virtual bool isFileOfLanguage(const QString &fileName) const override
	{
		return this->mExtensions.exactMatch(fileName);
	}

signals:

public slots:
};

#endif // GENERICLANGUAGE_HPP
