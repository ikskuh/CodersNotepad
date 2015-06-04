#include <QDebug>
#include <QFile>
#include <QtXml>
#include "tool.h"

Tool::Tool(QObject *parent) :
    QObject(parent)
{

}


Tool::Tool(const QString &name, QObject *parent) :
    QObject(parent),
    mName(name)
{

}

extern QString osName;

bool Tool::isSupported()
{
	return this->mOSList.exactMatch(osName);
}

static QString prepare(QString str, CodeEditor *context)
{
	QString result = str;
	result = result.replace("%s", "%%");
	result = result.replace("%s", context->textCursor().selectedText());
	// TODO: Add missing placeholders
	// %w => Current word
	// %l => Current line
	// %c => Current column
	// %? => Message Box Parameter
	// %{...} => Environment Variables, Placeholders, ...
	result = result.replace("%f", context->fileName());
	result = result.replace("%n", QFileInfo(context->fileName()).fileName());
    {
		auto dir = QFileInfo(context->fileName()).absoluteDir();
		result = result.replace("%d", dir.path());
    }
    return result;
}

void Tool::printExitCode(int code)
{
	emit this->outputEmitted("Exit Code: " + QString::number(code));
}

void Tool::start(CodeEditor *context)
{
	if(this->mSaveOnRun) {
		context->save();
		if(context->fileName().isEmpty()) {
			return;
		}
	}

    auto *proc = new QProcess(this);
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
	connect(proc, SIGNAL(finished(int)), this, SLOT(printExitCode(int)));
    connect(proc, &QProcess::readyReadStandardOutput, this, &Tool::flushOutput);
    connect(proc, &QProcess::readyReadStandardError, this, &Tool::flushError);

	QStringList args;
	for(QString arg : this->mArguments)
	{
		args << prepare(arg, context);
	}

	proc->setArguments(args);
	proc->setWorkingDirectory(prepare(this->mWorkingDirectory, context));
	proc->setProgram(prepare(this->mFileName, context));

	this->outputEmitted(">" + QFileInfo(proc->program()).fileName() + " " + proc->arguments().join(' '));
    proc->start();
}

void Tool::flushOutput()
{
	QProcess *proc = qobject_cast<QProcess*>(sender());
    emit this->outputEmitted(QString(proc->readAllStandardOutput()));
}

void Tool::flushError()
{
	QProcess *proc = qobject_cast<QProcess*>(sender());
	emit this->outputEmitted(QString(proc->readAllStandardError()));
}

Tool *Tool::load(const QString &fileName)
{
    QFile file(fileName);
    if(file.open(QFile::ReadOnly | QFile::Text) == false)
        return nullptr;
    QDomDocument doc;
    if(doc.setContent(&file) == false)  {
        file.close();
        return nullptr;
    }
    file.close();

    auto root = doc.firstChildElement();
    if(root.tagName() != "tool")
        return nullptr;

    qDebug() << "Load tool" << root.attribute("name");

    auto *tool = new Tool();
    tool->mName = root.attribute("name");
    tool->mLanguages = QRegExp(root.attribute("languages").split(';').join('|'));
	tool->mOSList = QRegExp(root.attribute("os").split(';').join('|'));
	tool->mSaveOnRun = root.attribute("save-file") == "true";
    tool->mWorkingDirectory = ".";

    auto nodes = root.childNodes();
    for(int i = 0; i < nodes.count(); i++)
    {
        QDomNode node = nodes.at(i);
        if(node.isElement() == false)
        {
            continue;
        }
        QDomElement element = node.toElement();
        if(element.tagName() == "filename")
        {
            tool->mFileName = element.text();
        }
        else if(element.tagName() == "directory")
        {
            tool->mWorkingDirectory = element.text();
        }
        else if(element.tagName() == "arguments")
        {
			auto children = element.childNodes();
			for(int j = 0; j < children.count(); j++)
			{
				if(children.at(j).isElement() == false) {
					continue;
				}
				QDomElement child = children.at(j).toElement();
				if(child.tagName() == "arg")
					tool->mArguments.push_back(child.text());
				else
					qDebug() << "Invalid argument node:" << child.tagName();

			}
        }
        else if(element.tagName() == "shortcut")
        {
            tool->mSequence = QKeySequence(element.text());
        }
        else
        {
            qDebug() << "Unknown tag:" << element.tagName();
        }
    }
    return tool;
}
