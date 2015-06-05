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
#include <QPainter>
#include <QTextStream>

#include "linenumberarea.hpp"

CodeEditor::CodeEditor(QWidget *parent) :
	QPlainTextEdit(parent),
    mDirty(false),
	mLastSearch(""),
    mLanguage(nullptr),
    mHighlighter(nullptr),
    mCompleter(nullptr)
{
	this->mLineNumberArea = new LineNumberArea(this);
    connect(
        this,
        &CodeEditor::textChanged,
        this,
		&CodeEditor::makeDirty);

	connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
	connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
	connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

	this->updateLineNumberAreaWidth(0);
	this->highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{

}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}

	int space = 6 + fontMetrics().width(QLatin1Char('9')) * digits;

	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy != 0)
		this->mLineNumberArea->scroll(0, dy);
	else
		this->mLineNumberArea->update(0, rect.y(), this->lineNumberAreaWidth(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	this->mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), this->lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);

		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
	}

	setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(this->mLineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();
	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, this->lineNumberAreaWidth() - 3, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
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

	this->mCompleter = this->mLanguage->createCompleter(this->document());
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

	// Automatic Indentation
	if(e->key() == Qt::Key_Return)
	{
		QPlainTextEdit::keyPressEvent(e);

		auto cursor = this->textCursor();
		auto *doc = this->document();
		int pos = cursor.position() - 1;
		int origin = pos;

		if(pos < 0)
			return;

		// Navigate to line start
		while((pos > 0) && (doc->characterAt(pos - 1).toLatin1() != 0))
		{
			pos--;
		}

		int indent = 0;
		for(int i = pos; i <= origin; i++)
		{
			if(doc->characterAt(i) == '\t') {
				indent++;
			} else {
				break;
			}
		}

		// Indent if necessary
		if(indent > 0)
		{
			this->textCursor().insertText(QString("\t").repeated(indent));
		}

		return;
	}

	// Tab Indentation Control
	if((e->key() == Qt::Key_Tab) || (e->key() == Qt::Key_Backtab))
	{
		bool moveBack = (e->key() == Qt::Key_Backtab);
		auto cursor = this->textCursor();
		auto *doc = this->document();

		int start = cursor.selectionStart();
		int end = cursor.selectionEnd();
		if((end == start) && (moveBack == false)) {
			QPlainTextEdit::keyPressEvent(e);
			return;
		}

		// Find line start
		while((start > 0) && (doc->characterAt(start - 1).toLatin1() != 0))
		{
			start--;
		}

		for(int pos = start; pos <= end; pos++)
		{
			if(moveBack == false)
			{
				// Insert tab at line start
				if(doc->characterAt(pos - 1).toLatin1() == 0)
				{
					cursor.setPosition(pos);
					cursor.insertText("\t");
					end += 1;
				}
			}
			else
			{
				// Remove tab at line start
				if((doc->characterAt(pos - 1).toLatin1() == 0) && (doc->characterAt(pos).toLatin1() == '\t'))
				{
					cursor.setPosition(pos);
					cursor.deleteChar();
					end -= 1;
				}
			}
		}

		e->ignore();
		return;
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



void CodeEditor::search(const QString &str)
{
	this->mLastSearch = str;
	if(this->find(str) == false) {
		auto focus = QApplication::focusWidget();
		auto cursor = this->textCursor();
		cursor.setPosition(0, QTextCursor::MoveAnchor);
		this->setTextCursor(cursor);
		this->find(str);

		if(focus != nullptr)
			focus->setFocus();
	}
}

void CodeEditor::searchNext()
{
	this->search(mLastSearch);
}
