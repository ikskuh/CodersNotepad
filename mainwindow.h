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

    void newFile();
    void loadFile();
    void closeFile();
    void saveFile();
    void saveFileAs();

    void updateFileMenu();

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
};

#endif // MAINWINDOW_H
