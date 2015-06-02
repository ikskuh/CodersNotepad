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

static QString prepare(QString str, CodeEditor *context, bool escape = true)
{
    QString result = str;
    QString q = escape ? "\"" : "";
    result = result.replace("%s", q + context->textCursor().selectedText() + q);
    result = result.replace("%f", q + context->fileName() + q);
    result = result.replace("%n", q + QFileInfo(context->fileName()).fileName() + q);
    {
        auto dir = QFileInfo(context->fileName()).absoluteDir();
        dir.cdUp();
        result = result.replace("%d", q + dir.path() + q);
    }
    return result;
}

void Tool::start(CodeEditor *context)
{
    qDebug() << "Start" << this->name() << "with file" << context->fileName();

    auto *proc = new QProcess(this);
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    connect(proc, &QProcess::readyReadStandardOutput, this, &Tool::flushOutput);
    connect(proc, &QProcess::readyReadStandardError, this, &Tool::flushError);

    proc->setNativeArguments(prepare(this->mArguments, context));
    proc->setWorkingDirectory(prepare(this->mWorkingDirectory, context, false));
    proc->setProgram(prepare(this->mFileName, context));

    qDebug() << proc->workingDirectory() << proc->program() << proc->nativeArguments();

    proc->start();
}

void Tool::flushOutput()
{
    QProcess *proc = qobject_cast<QProcess*>(sender());
    qDebug() << proc;
    emit this->outputEmitted(QString(proc->readAllStandardOutput()));
}

void Tool::flushError()
{
    QProcess *proc = qobject_cast<QProcess*>(sender());
    qDebug() << proc;
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
            tool->mArguments = element.text();
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
