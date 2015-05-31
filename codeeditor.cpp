#include "codeeditor.h"
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QDebug>
#include <QFile>
#include <QTextStream>

CodeEditor::CodeEditor(QWidget *parent) :
    QPlainTextEdit(parent),
    mDirty(false),
    mLanguage(nullptr),
    mHighlighter(nullptr),
    mCompleter(nullptr)
{
    connect(
        this,
        &CodeEditor::textChanged,
        this,
        &CodeEditor::makeDirty);
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::makeDirty()
{
    this->setDirty(true);
}

void CodeEditor::setDirty(bool dirty)
{
    this->mDirty = dirty;
    QString postfix = "";
    if(this->mDirty) {
        postfix = "*";
    }

    if(this->mFileName.isEmpty()) {
        this->setWindowTitle("*");
    } else {
        this->setWindowTitle(QFileInfo(this->mFileName).fileName() + postfix);
    }
}

void CodeEditor::save(const QString &name)
{
    this->mFileName = name;
    this->save();
}

void CodeEditor::save()
{
    QFile file(this->mFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream stream(&file);
        stream << this->document()->toPlainText();
        this->setDirty(false);
    } else {
        this->setDirty(true);
    }
    file.close();
}

void CodeEditor::open(const QString &name)
{
    QFile file(name);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        this->setPlainText(stream.readAll());

        this->mFileName = name;
        this->setDirty(false);
    }
    file.close();
}

void CodeEditor::setLanguage(Language *language)
{
    if (this->mCompleter != nullptr) {
        delete this->mCompleter;
    }
    if(this->mHighlighter != nullptr) {
        delete this->mHighlighter;
    }

    this->mLanguage = language;

    if (this->mLanguage == nullptr)
        return;

    this->mCompleter = this->mLanguage->createCompleter();
    this->mCompleter->setParent(this);
    this->mCompleter->setWidget(this);
    this->mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    this->mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(
        this->mCompleter, SIGNAL(activated(QString)),
        this, SLOT(insertCompletion(QString)));

    this->mHighlighter = this->mLanguage->createSyntaxHighlighter();
    this->mHighlighter->setParent(this);
    this->mHighlighter->setDocument(this->document());
}

void CodeEditor::insertCompletion(const QString& completion)
{
    if (this->mCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    int extra = completion.length() - this->mCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void CodeEditor::focusInEvent(QFocusEvent *e)
{
    if (this->mLanguage != nullptr)
        this->mCompleter->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent *e)
{
    QCompleter *c = nullptr;
    if(this->mLanguage != nullptr)
        c = this->mCompleter;

    if (c && c->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space); // CTRL+E
    if (!c || !isShortcut) // do not process the shortcut when we have a completer
        QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!c || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3
                      || eow.contains(e->text().right(1)))) {
        c->popup()->hide();
        return;
    }

    if (completionPrefix != c->completionPrefix()) {
        c->setCompletionPrefix(completionPrefix);
        c->popup()->setCurrentIndex(c->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(c->popup()->sizeHintForColumn(0)
                + c->popup()->verticalScrollBar()->sizeHint().width());
    c->complete(cr); // popup it up!
}
