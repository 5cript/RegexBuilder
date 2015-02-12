#include "rexclasshighlighter.h"

RexClassHighlighter::RexClassHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // keywords
    keywordFormat.setForeground(QColor(0x00A0E6));
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bregex\\b" << "\\bcapture\\b" << "\\batomic\\b"
                    << "\\bgroup\\b" << "\\bset\\b" << "\\bcontains\\b"
                    << "\\braw\\b" << "\\bnegate\\b" << "\\binsensitive\\b";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(0xDE7500));
    rule.pattern = QRegExp("~[A-Za-z_][_A-Za-z0-9]*");
    rule.format = classFormat;
    highlightingRules.append(rule);

    preproFormat.setForeground(QColor(0x6BD235));
    rule.pattern = QRegExp("^#.*");
    rule.format = preproFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(QColor(0x35D224));
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(QColor(0x35D224));

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void RexClassHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0)
    {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
