#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QProcess>
#include <QRegExp>

#include "codeeditor.h"

class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject *parent = 0);
    explicit Tool(const QString &name, QObject *parent = 0);

    static Tool *load(const QString &file);

    QString name() const
    {
        return this->mName;
    }

    void setName(const QString &name)
    {
        this->mName = name;
    }

    bool accepts(Language *language) const
    {
        if(this->mLanguages.isEmpty()) {
            return true;
        }
        if(language == nullptr) {
            return false;
        }
        return this->mLanguages.exactMatch(language->id());
    }

    void start(CodeEditor *context);

signals:
    void outputEmitted(const QString &value);

public slots:

private:
    void flushOutput();
    void flushError();

private:
    QString mName, mFileName, mArguments, mWorkingDirectory;
    QRegExp mLanguages;
};

#endif // TOOL_H
