#include "mainwindow.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QFont>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QTextStream>

#include "Languages/simplelanguage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mEditorFont("Monospace")
{
    this->mEditorFont.setStyleHint(QFont::TypeWriter);
    this->initMenuBar();

    this->mMdi = new QMdiArea();
    this->mMdi->setViewMode(QMdiArea::TabbedView);
    //this->mMdi->setDocumentMode(true);
    this->mMdi->setTabsClosable(true);
    this->setCentralWidget(this->mMdi);

    this->mLanguage = new SimpleLanguage();
}

MainWindow::~MainWindow()
{

}


CodeEditor *MainWindow::currentEditor()
{
    auto *window = this->mMdi->currentSubWindow();
    if(window == nullptr)
        return nullptr;
    return qobject_cast<CodeEditor*>(window->widget());
}

CodeEditor *MainWindow::newEditor()
{
    auto *editor = new CodeEditor();
    editor->setFont(this->mEditorFont);
    editor->setTabStopWidth(20);
    editor->setLanguage(this->mLanguage);

    auto *window = this->mMdi->addSubWindow(editor);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();

    return editor;
}

void MainWindow::newFile()
{
    auto *doc = this->newEditor();
    doc->setPlainText("");
    doc->setWindowTitle("*");
}

void MainWindow::loadFile()
{
    auto fileName = QFileDialog::getOpenFileName(
        this,
        "Open File",
        ".",
        "All Files (*)");
    if(fileName.isNull())
        return;
    QFile file(fileName);
    if(file.exists() == false)
        return;

    auto *editor = this->newEditor();
    editor->open(fileName);
}

void MainWindow::closeFile()
{
    auto *window = this->mMdi->currentSubWindow();
    if(window != nullptr) {
        window->deleteLater();
    }
}

void MainWindow::saveFile()
{
    auto *editor = this->currentEditor();
    if(editor == nullptr)
        return;

    if(editor->fileName().isNull()){
        this->saveFileAs();
    } else {
        editor->save();
    }
}

void MainWindow::saveFileAs()
{
    auto *editor = this->currentEditor();
    if(editor == nullptr)
        return;
    QString fileName = QFileDialog::getSaveFileName(this, "Save file...", ".", "All Files (*)");
    if(fileName.isNull())
        return;
    editor->save(fileName);
}

template<typename T, typename R, typename... Args>
QAction *MainWindow::menuItem(
        QMenu *menu,
        const QString &title,
        const QString &hotkey,
        R(T::*fn)(Args...))
{
    QAction *a = menu->addAction(title);
    if(hotkey.isNull() == false)
        a->setShortcut(QKeySequence(hotkey));
    if(fn != nullptr)
        connect(a, &QAction::triggered, this, fn);
    return a;
}

void MainWindow::initMenuBar()
{
    QMenuBar *menu = this->menuBar();

    QMenu *fileMenu = menu->addMenu("&File");
    connect(
        fileMenu, &QMenu::aboutToShow,
        this, &MainWindow::updateFileMenu);

    menuItem(fileMenu, "New", "Ctrl+N", &MainWindow::newFile);
    menuItem(fileMenu, "Open...", "Ctrl+O", &MainWindow::loadFile);
    fileMenu->addSeparator();
    this->aSave = menuItem(fileMenu, "Save", "Ctrl+S", &MainWindow::saveFile);
    this->aSaveAs = menuItem(fileMenu, "Save As...", "Ctrl+Shift+S", &MainWindow::saveFileAs);
    this->aClose = menuItem(fileMenu, "Close", "", &MainWindow::closeFile);
    fileMenu->addSeparator();
    menuItem(fileMenu, "Quit", "Alt+F4", &QWidget::close);
}

void MainWindow::updateFileMenu()
{
    auto *editor = this->currentEditor();

    this->aSave->setEnabled(editor != nullptr);
    this->aSaveAs->setEnabled(editor != nullptr);
    this->aClose->setEnabled(editor != nullptr);
}
