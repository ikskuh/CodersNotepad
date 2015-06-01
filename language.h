#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QObject>
#include <QSyntaxHighlighter>
#include <QCompleter>

/**
 * @brief Documentation comment.
 */
class Language : public QObject
{
    Q_OBJECT
protected:
    explicit Language(QObject *parent = 0);
public:
	/**
	 * @brief Creates a syntax highlighter for the language.
	 * @return
	 */
    virtual QSyntaxHighlighter *createSyntaxHighlighter() const = 0;

	/**
	 * @brief Creates a QCompleter for a given QTextDocument.
	 * @param document The document that should provide completion information for the completer.
	 * @return
	 */
	virtual QCompleter *createCompleter(QTextDocument *document) const = 0;
signals:

public slots:
};

#endif // LANGUAGE_H
