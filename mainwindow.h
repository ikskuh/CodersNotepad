#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeView>
#include <QMdiArea>
#include <QList>
#include <QDockWidget>
#include <QComboBox>

#include "codeeditor.h"
#include "language.h"
#include "webbrowser.hpp"
#include "tool.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void languageSelected(int);
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
    void updateToolsMenu();
	void updateWindowMenu();

    void emptyAction();

	void focusSearch();
	void search();
	void searchWeb();

    void startTool(Tool *tool);

    void editorSelected(QMdiSubWindow *);

    void writeOutput(QString output);

	void selectLanguage(Language *);

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
    QList<Tool*> mTools;
    QList<Language*> mLanguages;
    QMdiArea *mMdi;
    QTreeView *mCodeNavigator;
	QLineEdit *mSearchField;
    QComboBox *mEditorLanguage;
	WebBrowser *mBrowser;
    QPlainTextEdit *mOutputField;

	QDockWidget *mDockBrowser, *mDockJumper, *mDockOutput;

    QMenu *mToolsMenu;
    QAction *aSave, *aSaveAs, *aClose;
    QAction *aRedo, *aUndo, *aCopy, *aCut, *aPaste, *aSelectAll;
	QAction *aCodeJumper, *aBrowser, *aOutput;
};

#endif // MAINWINDOW_H
