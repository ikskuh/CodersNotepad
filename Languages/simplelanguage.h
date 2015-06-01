#ifndef SIMPLELANGUAGE_H
#define SIMPLELANGUAGE_H

#include <QObject>
#include "../language.h"

class SimpleLanguage :
        public Language
{
    Q_OBJECT
public:
    SimpleLanguage(QObject *parent = nullptr);

    virtual QSyntaxHighlighter *createSyntaxHighlighter() const override;

	virtual QCompleter *createCompleter(QTextDocument *) const override;

private:
	QStringList mKeywords;
	QTextCharFormat mKeywordFormat, mQtFormat, mCommentFormat, mStringFormat, mFunctionFormat, mPreprocessorFormat;
};

#endif // SIMPLELANGUAGE_H
