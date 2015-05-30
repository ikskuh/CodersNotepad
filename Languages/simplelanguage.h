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

    virtual QCompleter *createCompleter() const override;
};

#endif // SIMPLELANGUAGE_H
