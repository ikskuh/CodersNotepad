#include "generichighlighter.hpp"

GenericHighlighter::GenericHighlighter(QTextDocument *document) :
	QSyntaxHighlighter(document)
{

}

void GenericHighlighter::addRule(const QRegExp &regex, const QTextCharFormat &format)
{
	this->mRules.append({ regex, format });
}

void GenericHighlighter::addBlockRule(const QRegExp &expStart, const QRegExp &expEnd, const QTextCharFormat &format)
{
	this->mBlockRules.append({ expStart, expEnd, format });
}

void GenericHighlighter::highlightBlock(const QString &text)
{
	for(const HighlightingRule &rule : this->mRules)
{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);

	// HACK: Only one block rule is applied
	if(this->mBlockRules.size() > 0)
	{
		BlockHighlightingRule &rule = this->mBlockRules[0];

		int startIndex = 0;
		if (previousBlockState() != 1)
		{
			startIndex = rule.patternStart.indexIn(text);
		}

		while (startIndex >= 0)
		{
			int endIndex = rule.patternEnd.indexIn(text, startIndex);
			int commentLength;
			if (endIndex == -1)
			{
				setCurrentBlockState(1);
				commentLength = text.length() - startIndex;
			}
			else
			{
				commentLength = endIndex - startIndex
						+ rule.patternEnd.matchedLength();
			}
			setFormat(startIndex, commentLength, rule.format);
			startIndex = rule.patternStart.indexIn(text, startIndex + commentLength);
		}
	}
}

































