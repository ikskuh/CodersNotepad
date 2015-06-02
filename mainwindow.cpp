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
#include <QMessageBox>
#include <QShortcut>

#include "genericlanguage.hpp"
#include "tool.h"
#include "webbrowser.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mEditorFont("Monospace"),
    mTools(),
    mLanguages(),
    mMdi(nullptr)
{
    for(QString file : QDir("Languages").entryList(QStringList("*.xml")))
    {
        this->mLanguages.append(GenericLanguage::load("Languages/" + file));
    }
    for(QString file : QDir("Tools").entryList(QStringList("*.xml")))
    {
        this->mTools.append(Tool::load("Tools/" + file));
    }

    for(Tool *tool : this->mTools)
    {
        connect(tool, &Tool::outputEmitted, this, &MainWindow::writeOutput);
    }

    this->mEditorFont.setStyleHint(QFont::TypeWriter);
    this->initMenuBar();
    this->initPanels();
    this->initToolBar();

    this->mMdi = new QMdiArea();
    this->mMdi->setViewMode(QMdiArea::TabbedView);
    //this->mMdi->setDocumentMode(true);
    this->mMdi->setTabsClosable(true);
    connect(this->mMdi, &QMdiArea::subWindowActivated, this, &MainWindow::editorSelected);
    this->setCentralWidget(this->mMdi);
}

MainWindow::~MainWindow()
{

}

void MainWindow::writeOutput(QString output)
{
    if(this->mOutputField != nullptr)
        this->mOutputField->appendPlainText(output);
}

void MainWindow::editorSelected(QMdiSubWindow*)
{
    auto *editor = this->currentEditor();
    if(editor == nullptr) {
        return;
    }

    // TODO: Replace with item model
    if(editor->language() != nullptr)
    {
        for(int i = 0; i < this->mEditorLanguage->count(); i++)
        {
            if(this->mEditorLanguage->itemText(i) == editor->language()->name())
                this->mEditorLanguage->setCurrentIndex(i);
        }
    }

    this->updateToolsMenu();
}

void MainWindow::languageSelected(int idx)
{
    auto *editor = this->currentEditor();
    if(editor == nullptr) {
        return;
    }
    Language *lng = this->mEditorLanguage->itemData(idx).value<Language*>();
    editor->setLanguage(lng);

    this->updateToolsMenu();
}

CodeEditor *MainWindow::currentEditor()
{
    if(this->mMdi == nullptr) {
        return nullptr;
    }
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
    editor->setLanguage(this->mLanguages.first());

    auto *window = this->mMdi->addSubWindow(editor);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();

    return editor;
}

void MainWindow::emptyAction()
{
	QMessageBox(QMessageBox::Information, this->windowTitle(), "Not implemented yet").exec();
}

void MainWindow::newFile()
{
    auto *doc = this->newEditor();
    doc->setPlainText("");
    doc->setWindowTitle("*");
}

void MainWindow::loadFile()
{
    auto fileNames = QFileDialog::getOpenFileNames(
                this,
                "Open File",
                ".",
                "All Files (*)",
                nullptr);
    for(QString fileName : fileNames)
    {
        if(fileName.isNull())
            continue;
        QFile file(fileName);
        if(file.exists() == false)
            continue;

        Language *lng = this->mLanguages.first();
        for(Language *l : this->mLanguages)
        {
            if(l->isFileOfLanguage(file.fileName()))
                lng = l;
        }

        auto *editor = this->newEditor();
        editor->open(fileName);
        editor->setLanguage(lng);
    }
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

void MainWindow::copy()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
    editor->copy();
}

void MainWindow::cut()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
	editor->cut();
}

void MainWindow::paste()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
    editor->paste();
}


void MainWindow::selectAll()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
    editor->selectAll();
}

void MainWindow::undo()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
    editor->undo();
}

void MainWindow::redo()
{
    auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
    editor->redo();
}

void MainWindow::search()
{
	auto *editor = this->currentEditor();
	if(editor == nullptr)
		return;
	if(this->mSearchField->text().length() > 0)
		editor->search(this->mSearchField->text());
}

void MainWindow::searchWeb()
{
	QString text = this->mSearchField->text();
	if(text.length() > 0)
	{
		this->mDockBrowser->setVisible(true);
		if(QUrl(text).scheme().isEmpty()) {
			this->mBrowser->search(this->mSearchField->text());
		} else {
			this->mBrowser->navigateTo(QUrl(text));
		}
	}
}


void MainWindow::startTool(Tool *tool)
{
    auto *editor = this->currentEditor();
    if(editor != nullptr)
        tool->start(editor);
}

void MainWindow::toggleOutput()
{
	this->mDockOutput->setVisible(!this->mDockOutput->isVisible());
	this->aOutput->setChecked(this->mDockOutput->isVisible());
}

void MainWindow::toggleWebBrowser()
{
	this->mDockBrowser->setVisible(!this->mDockBrowser->isVisible());
	this->aBrowser->setChecked(this->mDockBrowser->isVisible());
}

void MainWindow::toggleCodeJumper()
{
	this->mDockJumper->setVisible(!this->mDockJumper->isVisible());
	this->aCodeJumper->setChecked(this->mDockJumper->isVisible());
}

void MainWindow::initPanels()
{
    // Code Jumper
    {
		this->mDockJumper  = new QDockWidget();
		this->mDockJumper->setWindowTitle("Code Jumper");

        auto *tree = new QTreeWidget();
        {
            auto *root = new QTreeWidgetItem();
            root->setText(0, "file-a.c");
            tree->addTopLevelItem(root);

            root = new QTreeWidgetItem();
            root->setText(0, "file-b.c");
            tree->addTopLevelItem(root);
        }
		this->mDockJumper->setWidget(tree);

		this->addDockWidget(Qt::LeftDockWidgetArea, this->mDockJumper);
    }

    // Output
    {
		this->mDockOutput = new QDockWidget();
		this->mDockOutput->setWindowTitle("Output");
		this->mDockOutput->setWindowIcon(QIcon("://Icons/appbar.console.svg"));

        this->mOutputField = new QPlainTextEdit();
        this->mOutputField->setReadOnly(true);

        this->mDockOutput->setWidget(this->mOutputField);

		this->addDockWidget(Qt::BottomDockWidgetArea, this->mDockOutput);
    }

    // Browser
	//*
    {
		this->mDockBrowser = new QDockWidget();
		this->mDockBrowser->setWindowTitle("Browser");
		this->mDockBrowser->setWidget(this->mBrowser = new WebBrowser());
		this->mDockBrowser->setVisible(false);
		this->addDockWidget(Qt::RightDockWidgetArea, this->mDockBrowser);
    }
    //*/
}

template<typename T, typename R>
QAction *MainWindow::menuItem(
		T *menu,
        const QString &title,
        const QString &hotkey,
		const QIcon &icon,
		const R &fn)
{
	QAction *a = menu->addAction(icon, title);
	if(hotkey.isEmpty() == false)
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
	menuItem(fileMenu, "New", "Ctrl+N", QIcon("://Icons/appbar.page.new.svg"), &MainWindow::newFile);
	menuItem(fileMenu, "Open...", "Ctrl+O", QIcon("://Icons/appbar.folder.open.svg"), &MainWindow::loadFile);
    fileMenu->addSeparator();
	this->aSave = menuItem(fileMenu, "Save", "Ctrl+S", QIcon("://Icons/appbar.save.svg"), &MainWindow::saveFile);
	this->aSaveAs = menuItem(fileMenu, "Save As...", "Ctrl+Shift+S", QIcon(), &MainWindow::saveFileAs);
	this->aClose = menuItem(fileMenu, "Close", "", QIcon("://Icons/appbar.close.svg"), &MainWindow::closeFile);
    fileMenu->addSeparator();
	menuItem(fileMenu, "Quit", "Alt+F4", QIcon(), &QWidget::close);

    QMenu *editMenu = menu->addMenu("&Edit");
    connect(
        editMenu, &QMenu::aboutToShow,
        this, &MainWindow::updateEditMenu);
	this->aUndo = menuItem(editMenu, "Undo", "Ctrl+Z", QIcon("://Icons/appbar.undo.svg"), &MainWindow::undo);
	this->aRedo = menuItem(editMenu, "Redo", "Ctrl+Y", QIcon("://Icons/appbar.redo.svg"), &MainWindow::redo);
    editMenu->addSeparator();
	this->aCut = menuItem(editMenu, "Cut", "Ctrl+X", QIcon(), &MainWindow::cut);
	this->aCopy = menuItem(editMenu, "Copy", "Ctrl+C", QIcon(), &MainWindow::copy);
	this->aPaste = menuItem(editMenu, "Paste", "Ctrl+V", QIcon(), &MainWindow::paste);
    editMenu->addSeparator();
	this->aSelectAll = menuItem(editMenu, "Select All", "Ctrl+A", QIcon(), &MainWindow::selectAll);

    this->mToolsMenu = menu->addMenu("&Tools");
    connect(
        this->mToolsMenu, &QMenu::aboutToShow,
        this, &MainWindow::updateToolsMenu);

	QMenu *windowMenu = menu->addMenu("&Window");
	connect(
		windowMenu, &QMenu::aboutToShow,
		this, &MainWindow::updateWindowMenu);
	this->aOutput = menuItem(windowMenu, "Output", "", QIcon("://Icons/appbar.console.svg"), &MainWindow::toggleOutput);
	this->aOutput->setCheckable(true);
	this->aBrowser = menuItem(windowMenu, "Web Browser", "", QIcon("://Icons/appbar.globe.wire.svg"), &MainWindow::toggleWebBrowser);
	this->aBrowser->setCheckable(true);
	this->aCodeJumper = menuItem(windowMenu, "Code Jumper", "", QIcon(), &MainWindow::toggleCodeJumper);
	this->aCodeJumper->setCheckable(true);

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

void MainWindow::updateWindowMenu()
{
	this->aBrowser->setChecked(this->mDockBrowser->isVisible());
	this->aOutput->setChecked(this->mDockOutput->isVisible());
	this->aCodeJumper->setChecked(this->mDockJumper->isVisible());
}

void MainWindow::updateToolsMenu()
{
    auto *editor = this->currentEditor();
    this->mToolsMenu->clear();
    for(Tool *tool : this->mTools)
    {
        if(tool->accepts((editor != nullptr) ? editor->language() : nullptr) == false)
        {
            continue;
        }
        auto *action = this->mToolsMenu->addAction(tool->name());
        action->setEnabled(editor != nullptr);
        action->setShortcut(tool->sequence());
        connect(action, &QAction::triggered, [this,tool]() {
            this->startTool(tool);
        });
    }
}

void MainWindow::initToolBar()
{
    {
        auto *bar = this->addToolBar("File Control");
		menuItem(bar, "New", "", QIcon("://Icons/appbar.page.new.svg"), &MainWindow::newFile);
		menuItem(bar, "Open...", "", QIcon("://Icons/appbar.folder.open.svg"), &MainWindow::loadFile);
		menuItem(bar, "Save", "", QIcon("://Icons/appbar.save.svg"), &MainWindow::saveFile);
        bar->addAction("Save All");
		menuItem(bar, "Close", "", QIcon("://Icons/appbar.close.svg"), &MainWindow::closeFile);
    }
    {
        auto *bar = this->addToolBar("Editor Primitives");
		menuItem(bar, "Undo", "", QIcon("://Icons/appbar.undo.svg"), &MainWindow::undo);
		menuItem(bar, "Redo", "", QIcon("://Icons/appbar.redo.svg"), &MainWindow::redo);
        bar->addSeparator();
        bar->addAction("Cut");
        bar->addAction("Copy");
        bar->addAction("Paste");
    }
    {
        auto *bar = this->addToolBar("Language Options");
        {
            this->mEditorLanguage = new QComboBox();
            connect(
                this->mEditorLanguage, SIGNAL(currentIndexChanged(int)),
                this, SLOT(languageSelected(int)));
            for(Language *lng : this->mLanguages)
            {
                this->mEditorLanguage->addItem(lng->name(), QVariant::fromValue(lng));
            }
            bar->addWidget(this->mEditorLanguage);
        }
		menuItem(bar, "Language Settings", "", QIcon("://Icons/appbar.settings.svg"), &MainWindow::emptyAction);
    }
    {
        auto *bar = this->addToolBar("Search");
        {
			this->mSearchField = new QLineEdit();
			this->mSearchField->setPlaceholderText("Search...");
			this->mSearchField->setMaximumWidth(200);

			auto *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
			QObject::connect(
				shortcut, &QShortcut::activated,
				[this]() {
					this->mSearchField->setFocus();
					this->mSearchField->setText("");
				});

			shortcut = new QShortcut(QKeySequence("Alt+Return"), this->mSearchField);
			QObject::connect(
				shortcut, &QShortcut::activated,
				this, &MainWindow::searchWeb);

			shortcut = new QShortcut(QKeySequence("Return"), this->mSearchField);
			QObject::connect(
				shortcut, &QShortcut::activated,
				this, &MainWindow::search);

			shortcut = new QShortcut(QKeySequence("Escape"), this->mSearchField);
			QObject::connect(
				shortcut, &QShortcut::activated,
				[this]() {
					auto editor = this->currentEditor();
					if(editor != nullptr)
						editor->setFocus();
				});

			bar->addWidget(this->mSearchField);
		}
		menuItem(bar, "Find", "", QIcon("://Icons/appbar.page.search.svg"), &MainWindow::search);
		menuItem(bar, "Find in Web", "", QIcon("://Icons/appbar.globe.wire.svg"), &MainWindow::searchWeb);
    }
}
