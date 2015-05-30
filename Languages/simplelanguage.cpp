#include "simplelanguage.h"
#include "simplelanghighlighter.h"

SimpleLanguage::SimpleLanguage(QObject *parent) :
    Language(parent)
{

}



QSyntaxHighlighter *SimpleLanguage::createSyntaxHighlighter() const
{
    return new SimpleLangHighlighter();
}

QCompleter *SimpleLanguage::createCompleter() const
{
    QStringList list;
    list << "break"
         << "virtual"
         << "class"
         << "const";
    return new QCompleter(list);
}
