#ifndef SIMPLELANG_H
#define SIMPLELANG_H

#include <QSyntaxHighlighter>

class SimpleLangHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SimpleLangHighlighter(QTextDocument *doc = nullptr);

protected:
    void highlightBlock(const QString &text) override;

signals:

public slots:
private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // SIMPLELANG_H
