#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeView>
#include <QMdiArea>

#include "codeeditor.h"
#include "language.h"

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

    void newFile();
    void loadFile();
    void closeFile();
    void saveFile();
    void saveFileAs();

    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void selectAll();

    void updateFileMenu();
    void updateEditMenu();

    void emptyAction();

    CodeEditor *newEditor();

    CodeEditor *currentEditor();

    template<typename T, typename R, typename... Args>
    QAction *menuItem(
            QMenu *menu,
            const QString &title,
            const QString &hotkey,
            R(T::*fn)(Args...));
private:
    QFont mEditorFont;
    QMdiArea *mMdi;
    QTreeView *mCodeNavigator;
    Language *mLanguage;

    QAction *aSave, *aSaveAs, *aClose;
    QAction *aRedo, *aUndo, *aCopy, *aCut, *aPaste, *aSelectAll;
};

#endif // MAINWINDOW_H
