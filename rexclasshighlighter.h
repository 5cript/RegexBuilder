#ifndef REXCLASSHIGHLIGHTER_H
#define REXCLASSHIGHLIGHTER_H

#include <QObject>
#include <QSyntaxHighlighter>

class RexClassHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    RexClassHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;

     QTextCharFormat keywordFormat;
     QTextCharFormat classFormat;
     QTextCharFormat singleLineCommentFormat;
     QTextCharFormat multiLineCommentFormat;
     QTextCharFormat preproFormat;
};

#endif // REXCLASSHIGHLIGHTER_H
