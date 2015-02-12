#ifndef REGEXEDITOR_H
#define REGEXEDITOR_H

#include <QDialog>

#include "rexclasshighlighter.h"
#include "testbedhighlighter.h"

#include <memory>
#include "regexbuilder/tokencompat.h"

namespace Ui {
class RegexEditor;
}

class RegexEditor : public QDialog
{
    Q_OBJECT

public:
    explicit RegexEditor(QWidget *parent = 0);
    ~RegexEditor();

private slots:
    void on_compile_clicked();
    void on_directoryChanger_clicked();
    void on_fileSelector_currentIndexChanged(const QString &arg1);
    void on_saveFile_clicked();

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void resetInfoLabel();

    void on_createFileBtn_clicked();

    void on_optimize_clicked();

private:
    bool preprocess();
    std::string getStdlibPos() const;

private:
    Ui::RegexEditor *ui;

    std::unique_ptr <RexClassHighlighter> highlighter;
    std::unique_ptr <TestbedHighlighter> testbedHighlighter;
    std::vector <WaveToken> tokens;
    std::string mainClass;

    QString mainDir;
    QString openedFileName;
    bool preprocessOnly;
};

#endif // REGEXEDITOR_H
