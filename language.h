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
    virtual QSyntaxHighlighter *createSyntaxHighlighter() const = 0;
    virtual QCompleter *createCompleter() const = 0;
signals:

public slots:
};

#endif // LANGUAGE_H
