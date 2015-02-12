#ifndef TESTBEDHIGHLIGHTER_H
#define TESTBEDHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class TestbedHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    TestbedHighlighter(QTextDocument *parent, QString regexString = "[]*");

    void setRegex(QString regexString);

protected:
    void highlightBlock(const QString& text);

private:
    QRegExp testRegex;
    QTextCharFormat testFormat;
};

#endif // TESTBEDHIGHLIGHTER_H
