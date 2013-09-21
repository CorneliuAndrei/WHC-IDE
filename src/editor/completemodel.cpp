#include "completemodel.h"

CompleteModel::CompleteModel(MdiTextEditor *parent) :
    parent(parent)
{
    thread = new QThread();
    worker = new ModelFromScope(this, parent);
    worker->moveToThread(thread);

    connect(parent, SIGNAL(getModel(int)), this, SLOT(slotGetModel(int)));
    connect(this, SIGNAL(gotModel(QStringListModel*)), parent,
            SLOT(slotGotModel(QStringListModel*)));
    connect(this, SIGNAL(requestModel(int)), worker, SLOT(slotGetModel(int)));
    connect(worker, SIGNAL(gotModel(QStringListModel*)), this,
            SLOT(slotObtainedModel(QStringListModel*)));

    thread->start();
}

CompleteModel::~CompleteModel()
{
    worker->deleteLater();
    thread->deleteLater();
}

void CompleteModel::slotGetModel(int position)
{
    emit requestModel(position);
}

void CompleteModel::slotObtainedModel(QStringListModel *model)
{
    emit gotModel(model);
}

ModelFromScope::ModelFromScope(CompleteModel *parent, MdiTextEditor *editor) :
    parent(parent), editor(editor)
{
    connect(parent, SIGNAL(requestModel(int)), this, SLOT(slotGetModel(int)));
}

void ModelFromScope::slotGetModel(int position)
{
    emit gotModel(modelFromScope(position));
}

QStringListModel *ModelFromScope::modelFromScope(int position)
{
    //Don't know what it does but it was here before
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#endif

    if(position < 0)
        return NULL;

    QString text = editor->toPlainText();

    if(position > text.size())
        return NULL;

    QSet<QString> foundWords;
    QRegExp regex("[a-zA-Z_][a-zA-Z0-9_]*");
    int currentPos = regex.indexIn(text);
    QStringList words;

    while (currentPos != -1 && currentPos < position)
    {
        if(inScopeOf(currentPos, position))
            foundWords.insert(regex.cap());
        currentPos = regex.indexIn(text, currentPos + regex.matchedLength());
    }
    foreach(QString word, foundWords)
    {
        if(word.size() > 3)
            words<<word;
    }
    words.sort();

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    QStringListModel *result = new QStringListModel(words);
    result->moveToThread(QApplication::instance()->thread());
    return result;
}

bool ModelFromScope::inScopeOf(int a, int b)
{
    if(a > b || a < 0 || b < 0)
        return false;

    QString text = editor->toPlainText();

    if(a >= text.size() || b > text.size())
        return false;

    int levelDiff = 0;

    for(int i = a; i < b; i++)
        if(text.at(i) == '{')
            levelDiff++;
        else if(text.at(i) == '}')
            levelDiff--;

    if(levelDiff < 0)
        return false;
    else
        return true;
}
