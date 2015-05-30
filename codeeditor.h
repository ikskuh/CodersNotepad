#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QTextEdit>
#include <QFileInfo>

#include "language.h"

class CodeEditor : public QTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = 0);
    ~CodeEditor();

    void setLanguage(Language *language);

    Language *language() const
    {
        return this->mLanguage;
    }

    QString fileName() const
    {
        return this->mFileName;
    }

    void open(const QString &name);

    void save();

    void save(const QString &name);

    bool isDirty() const
    {
        return this->mDirty;
    }

protected:
    void focusInEvent(QFocusEvent *e) override;

    void keyPressEvent(QKeyEvent *e) override;
private slots:
    void insertCompletion(const QString&);

    QString textUnderCursor() const;

    void makeDirty();

    void setDirty(bool dirty);
signals:

public slots:
private:
    bool mDirty;
    QString mFileName;
    Language *mLanguage;
    QSyntaxHighlighter *mHighlighter;
    QCompleter *mCompleter;
};

#endif // CODEEDITOR_H
