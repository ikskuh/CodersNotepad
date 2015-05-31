#include "mainwindow.h"

#include <QDockWidget>
#include <QMenuBar>
#include <QFont>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QFileDialog>
#include <QTextStream>
#include <QToolBar>
#include <QComboBox>
#include <QTreeWidget>
#include <QLineEdit>

//#include <QtWebKitWidgets/QWebView>

#include "Languages/simplelanguage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mEditorFont("Monospace")
{
    this->mEditorFont.setStyleHint(QFont::TypeWriter);
    this->initMenuBar();
    this->initPanels();
    this->initToolBar();

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

void MainWindow::emptyAction()
{

}


void MainWindow::copy()
{
    auto *editor = this->currentEditor();
    editor->copy();
}

void MainWindow::cut()
{
    auto *editor = this->currentEditor();
    editor->cut();
}

void MainWindow::paste()
{
    auto *editor = this->currentEditor();
    editor->paste();
}


void MainWindow::selectAll()
{
    auto *editor = this->currentEditor();
    editor->selectAll();
}

void MainWindow::undo()
{
    auto *editor = this->currentEditor();
    editor->undo();
}

void MainWindow::redo()
{
    auto *editor = this->currentEditor();
    editor->redo();
}

void MainWindow::initPanels()
{
    // Code Jumper
    {
        auto *codePanel  = new QDockWidget();
        codePanel->setWindowTitle("Code Jumper");

        auto *tree = new QTreeWidget();
        {
            auto *root = new QTreeWidgetItem();
            root->setText(0, "file-a.c");
            tree->addTopLevelItem(root);

            root = new QTreeWidgetItem();
            root->setText(0, "file-b.c");
            tree->addTopLevelItem(root);
        }
        codePanel->setWidget(tree);

        this->addDockWidget(Qt::LeftDockWidgetArea, codePanel);
    }

    // Output
    {
        auto *outputPanel = new QDockWidget();
        outputPanel->setWindowTitle("Output");

        auto *output = new QTextEdit();
        output->setReadOnly(true);
        output->setText(">cat data.dat");

        outputPanel->setWidget(output);

        this->addDockWidget(Qt::BottomDockWidgetArea, outputPanel);
    }

    // Browser
    /*
    {
        auto *browserPanel = new QDockWidget();
        browserPanel->setWindowTitle("Browser");

        auto *view = new QWebView();
        view->load(QUrl("http://mq32.de"));
        browserPanel->setWidget(view);

        this->addDockWidget(Qt::RightDockWidgetArea, browserPanel);
    }
    //*/
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

    QMenu *editMenu = menu->addMenu("&Edit");
    connect(
        editMenu, &QMenu::aboutToShow,
        this, &MainWindow::updateEditMenu);
    this->aUndo = menuItem(editMenu, "Undo", "Ctrl+Z", &MainWindow::undo);
    this->aRedo = menuItem(editMenu, "Redo", "Ctrl+Y", &MainWindow::redo);
    editMenu->addSeparator();
    this->aCut = menuItem(editMenu, "Cut", "Ctrl+X", &MainWindow::cut);
    this->aCopy = menuItem(editMenu, "Copy", "Ctrl+C", &MainWindow::copy);
    this->aPaste = menuItem(editMenu, "Paste", "Ctrl+V", &MainWindow::paste);
    editMenu->addSeparator();
    this->aSelectAll = menuItem(editMenu, "Select All", "Ctrl+A", &MainWindow::selectAll);

    QMenu *toolsMenu = menu->addMenu("&Tools");

    QMenu *windowMenu = menu->addMenu("&Window");

    QMenu *helpMenu = menu->addMenu("&Help");
}

void MainWindow::updateFileMenu()
{
    auto *editor = this->currentEditor();

    this->aSave->setEnabled(editor != nullptr);
    this->aSaveAs->setEnabled(editor != nullptr);
    this->aClose->setEnabled(editor != nullptr);
}

void MainWindow::updateEditMenu()
{
    auto *editor = this->currentEditor();

    this->aUndo->setEnabled((editor != nullptr) && (editor->document()->isUndoAvailable()));
    this->aRedo->setEnabled((editor != nullptr) && (editor->document()->isRedoAvailable()));
    this->aCut->setEnabled((editor != nullptr) && (editor->textCursor().hasSelection()));
    this->aCopy->setEnabled((editor != nullptr) && (editor->textCursor().hasSelection()));
    this->aPaste->setEnabled((editor != nullptr) && (editor->canPaste()));
    this->aSelectAll->setEnabled(editor != nullptr);
}

void MainWindow::initToolBar()
{
    {
        auto *bar = this->addToolBar("File Control");
        bar->addAction("New");
        bar->addAction("Open...");
        bar->addAction("Save");
        bar->addAction("Save All");
        bar->addAction("Close");
    }
    {
        auto *bar = this->addToolBar("Editor Primitives");
        bar->addAction("Undo");
        bar->addAction("Redo");
        bar->addSeparator();
        bar->addAction("Cut");
        bar->addAction("Copy");
        bar->addAction("Paste");
    }
    {
        auto *bar = this->addToolBar("Language Options");
        {
            auto *box = new QComboBox();
            box->addItem("C/C++");
            box->addItem("C#");
            box->addItem("Lua");
            box->addItem("Vala");
            box->addItem("SolidMarkup");
            bar->addWidget(box);
        }
        bar->addAction("Settings");
    }
    {
        auto *bar = this->addToolBar("Search");
        {
            auto *edit = new QLineEdit();
            edit->setPlaceholderText("Search...");
            edit->setMaximumWidth(200);
            bar->addWidget(edit);
        }
        bar->addAction("Find");
        bar->addAction("Find Web");
    }
}
