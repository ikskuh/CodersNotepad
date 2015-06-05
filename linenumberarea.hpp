#ifndef LINENUMBERAREA_HPP
#define LINENUMBERAREA_HPP

#include <QWidget>
#include "codeeditor.h"

class LineNumberArea : public QWidget
{
public:
	LineNumberArea(CodeEditor *editor) : QWidget(editor) {
		codeEditor = editor;
	}

	virtual QSize sizeHint() const override {
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	virtual void paintEvent(QPaintEvent *event) override {
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	CodeEditor *codeEditor;
};

#endif // LINENUMBERAREA_HPP
