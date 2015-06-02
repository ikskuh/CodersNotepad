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
    explicit Language(QString id, QObject *parent = 0);
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

	/**
	 * @brief Is the given file name a file of the language?
	 * @param fileName
	 * @return
	 */
	virtual bool isFileOfLanguage(const QString &fileName) const = 0;

	/**
	 * @brief Gets the name of the language.
	 * @return
	 */
	virtual QString name() const = 0;

    QString id() const
    {
        return this->mId;
    }
signals:

public slots:
private:
    QString mId;
};

#endif // LANGUAGE_H
