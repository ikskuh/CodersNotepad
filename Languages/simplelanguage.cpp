#include <QTextCharFormat>
#include "simplelanguage.h"
#include "../generichighlighter.hpp"

SimpleLanguage::SimpleLanguage(QObject *parent) :
    Language(parent)
{
	this->mKeywords
			<< "char" << "class" << "const"
			<< "double" << "float" << "enum" << "explicit"
			<< "friend" << "inline" << "int" << "long" << "namespace"
			<< "operator" << "private" << "protected" << "public"
			<< "short" << "signals" << "signed" << "unsigned"
			<< "slots" << "static" << "struct" << "template"
			<< "typedef" << "typename" << "union" << "this"
			<< "virtual" << "override" << "void" << "volatile";




	this->mKeywordFormat.setForeground(Qt::darkBlue);
	this->mKeywordFormat.setFontWeight(QFont::Bold);

	this->mCommentFormat.setForeground(Qt::darkGreen);
	this->mCommentFormat.setFontItalic(true);

	this->mQtFormat.setForeground(Qt::darkMagenta);

	this->mStringFormat.setForeground(Qt::darkGreen);

	this->mFunctionFormat.setForeground(Qt::blue);

	this->mPreprocessorFormat.setForeground(Qt::darkBlue);
}



QSyntaxHighlighter *SimpleLanguage::createSyntaxHighlighter() const
{
	auto *highlighter = new GenericHighlighter();
	for(auto &keyword : this->mKeywords)
	{
		highlighter->addRule(QRegExp("\\b"+keyword+"\\b"), this->mKeywordFormat);
	}
	QRegExp stringExp("\".*\"");
	stringExp.setMinimal(true);
	highlighter->addRule(stringExp, this->mStringFormat);

	highlighter->addRule(QRegExp("\\b[A-Za-z0-9_]+(?=\\()"), this->mFunctionFormat);
	highlighter->addRule(QRegExp("\\bQ[A-Za-z]+\\b"), this->mQtFormat);

	highlighter->addRule(QRegExp("^#.*$"), this->mPreprocessorFormat);

	highlighter->addRule(QRegExp("//[^\n]*"), this->mCommentFormat);
	highlighter->addBlockRule(QRegExp("/\\*"), QRegExp("\\*/"), this->mCommentFormat);

	return highlighter;
}

QCompleter *SimpleLanguage::createCompleter(QTextDocument *) const
{
	return new QCompleter(this->mKeywords);
}
