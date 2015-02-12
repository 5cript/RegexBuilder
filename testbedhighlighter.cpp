#include "testbedhighlighter.h"

TestbedHighlighter::TestbedHighlighter(QTextDocument *parent, QString regexString)
    : QSyntaxHighlighter(parent)
    , testRegex(regexString)
{
    testFormat.setBackground(Qt::darkYellow);
}

void TestbedHighlighter::setRegex(QString regexString)
{
    testRegex = QRegExp(regexString);
}

void TestbedHighlighter::highlightBlock(const QString &text)
{
    int index = testRegex.indexIn(text);
    while (index >= 0)
    {
        int length = testRegex.matchedLength();
        setFormat(index, length, testFormat);
        index = testRegex.indexIn(text, index + length);

        // regex that match nothing won't cause endless loops anymore:
        if (length == 0)
            break;
    }
}

