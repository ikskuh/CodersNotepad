#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeView>
#include <QMdiArea>
#include <QDockWidget>

#include "codeeditor.h"
#include "language.h"
#include "webbrowser.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void initMenuBar();
    void initPanels();
    void initToolBar();

	// File
    void newFile();
    void loadFile();
    void closeFile();
    void saveFile();
    void saveFileAs();

	// Edit
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void selectAll();

	// Window
	void toggleOutput();
	void toggleWebBrowser();
	void toggleCodeJumper();

    void updateFileMenu();
    void updateEditMenu();
	void updateWindowMenu();

    void emptyAction();

	void focusSearch();
	void search();
	void searchWeb();

    CodeEditor *newEditor();

    CodeEditor *currentEditor();

	template<typename T, typename R>
    QAction *menuItem(
			T *menu,
            const QString &title,
            const QString &hotkey,
			const QIcon &icon,
			const R &fn);
private:
    QFont mEditorFont;
    QMdiArea *mMdi;
    QTreeView *mCodeNavigator;
    Language *mLanguage;
	QLineEdit *mSearchField;
	WebBrowser *mBrowser;
	QDockWidget *mDockBrowser, *mDockJumper, *mDockOutput;

    QAction *aSave, *aSaveAs, *aClose;
    QAction *aRedo, *aUndo, *aCopy, *aCut, *aPaste, *aSelectAll;
	QAction *aCodeJumper, *aBrowser, *aOutput;
};

#endif // MAINWINDOW_H
