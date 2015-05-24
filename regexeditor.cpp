#include "regexeditor.h"
#include "ui_regexeditor.h"

#include "regexbuilder/builder.h"
#include "regexbuilder/preprocessor.h"
#include "regexbuilder/tokencompat.h"
//#include "regexopt/libregex.hh"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QDirIterator>
#include <QTimer>
#include <QInputDialog>
#include <QStyledItemDelegate>
#include <QTextStream>

#include <fstream>
#include <exception>
#include <sstream>
#include <stdexcept>

RegexEditor::RegexEditor(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegexEditor)
    , highlighter(nullptr)
    , mainDir("")
    , openedFileName("")
{
    ui->setupUi(this);

    ui->fileSelector->setItemDelegate(new QStyledItemDelegate(this));

    QFile file(":/styles/default.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream (&file);
        this->setStyleSheet(stream.readAll());
        file.close();
    }
    else
        qWarning() << "style not set";

    highlighter.reset(new RexClassHighlighter(ui->editor->document()));
    testbedHighlighter.reset(new TestbedHighlighter(ui->testbed->document()));
}

RegexEditor::~RegexEditor()
{
    delete ui;
}
void RegexEditor::on_compile_clicked()
{
    using namespace RegexBuilder;

    // preprocess lhs:
    if (!preprocess())
        return;

    if (preprocessOnly)
        return;

    // build preprocessed output:
    auto builderTokens = convertTokens(tokens);
    Builder builder;
    try {
        builder.compile(builderTokens);
        auto built = builder.execute(ui->mainClass->text().toStdString());

        ui->regexOutput->setPlainText(QString::fromStdString(built));
        testbedHighlighter->setRegex(QString::fromStdString(built));
        testbedHighlighter->rehighlight();
    }
    catch (std::exception const& exc) {
        auto message = QString(exc.what());
        ui->regexOutput->setPlainText(QString("In class: ") + QString::fromStdString(builder.getLastClass()) + QString("\n\n") + message);
    }
}

void RegexEditor::on_directoryChanger_clicked()
{
    on_saveFile_clicked();

    if (mainDir == "")
        // mainDir = QDir::home().absolutePath();
        mainDir = QString::fromStdString(getStdlibPos());

    mainDir = QFileDialog::getExistingDirectory(this, QString("Project Directory"), mainDir);

    if (mainDir == "")
        return;

    ui->fileSelector->clear();

    QDirIterator dirIt(mainDir, QDirIterator::Subdirectories);
    while (dirIt.hasNext()) {
        dirIt.next();
        if (QFileInfo(dirIt.filePath()).isFile())
            if (    QFileInfo(dirIt.filePath()).suffix() == "srx"
                ||  QFileInfo(dirIt.filePath()).suffix() == "hpp"
                ||  QFileInfo(dirIt.filePath()).suffix() == "h")
                ui->fileSelector->addItem(dirIt.filePath().remove(0, mainDir.length() + 1));
    }
}

void RegexEditor::on_fileSelector_currentIndexChanged(const QString &arg1)
{
    on_saveFile_clicked();

    openedFileName = mainDir + "/" + arg1;
    QFile file(openedFileName);
    file.open(QIODevice::ReadOnly);
    ui->editor->setEnabled(true);
    ui->editor->setPlainText(QString::fromStdString(file.readAll().toStdString()));
    file.close();

    // preprocess and try to find a main class macro definition:
    preprocess();
    if (!mainClass.empty())
        ui->mainClass->setText(QString::fromStdString(mainClass));
}

void RegexEditor::on_saveFile_clicked()
{
    if (mainDir != "" && openedFileName != "")
    {
        QString plainText = ui->editor->toPlainText();
        if (plainText.length() == 0 || plainText[plainText.length() - 1] != '\n')
            plainText += "\n";

        QFile file(openedFileName);
        file.open(QIODevice::WriteOnly);
        file.write(ui->editor->toPlainText().toStdString().c_str());
        file.close();
        ui->infoLabel->setText("File saved");
        QTimer::singleShot(3500, this, SLOT(resetInfoLabel()));
    }
}

void RegexEditor::keyPressEvent(QKeyEvent *)
{
}

void RegexEditor::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_S && event->modifiers() & Qt::ControlModifier)
        on_saveFile_clicked();
}

void RegexEditor::resetInfoLabel()
{
    ui->infoLabel->setText("Nothing interesting happening now ;)");
}

bool RegexEditor::preprocess()
{
    preprocessOnly = false;

    auto inputString = ui->editor->toPlainText().toStdString();
    auto libraryPath = getStdlibPos();

    std::vector <boost::wave::preprocess_exception> messages;
    std::unordered_map <std::string, std::vector<std::string>> specialDefinitions;

    if (inputString.empty() || inputString.back() == '\n')
        inputString += "\n";

    try
    {
        std::string output;
        tokens = RegexBuilder::preprocess<std::string::iterator>(std::begin(inputString), std::end(inputString), libraryPath, {mainDir.toStdString()}, messages, specialDefinitions, output);

        if (specialDefinitions.find(RegexBuilder::onlyPreprocessFlag) != std::end(specialDefinitions))
        {
            ui->regexOutput->setPlainText(QString::fromStdString(output));
            preprocessOnly = true;
        }

        auto mainRegexClassIter = specialDefinitions.find(RegexBuilder::mainClassMacroName);
        if (mainRegexClassIter != std::end(specialDefinitions))
            if (!mainRegexClassIter->second.empty())
                ui->mainClass->setText(QString::fromStdString(mainRegexClassIter->second.front()));
    }
    catch (...)
    {
        ui->regexOutput->setPlainText("There has been an unrecoverable error. See the log for further information");
    }

    if (!messages.empty())
    {
        bool justWarnings = true;
        QString lines = "";
        for (auto const& i : messages)
        {
            lines += QString(i.description()) + " in line " + QString::number(i.line_no()) + " in file " + QString(i.file_name()) + "\n";
            if (i.get_severity() > boost::wave::util::severity_warning)
                justWarnings = false;
        }
        ui->warningsAndErrors->setPlainText(lines);
        if (!justWarnings)
        {
            ui->regexOutput->setPlainText("There has been an error. See the log for further information");
            return false;
        }
    }
    else
        ui->warningsAndErrors->setPlainText("");
    return true;
}

std::string RegexEditor::getStdlibPos() const
{
    std::string libpos = "./";
    QFile stdlibpos("./stdlib.txt");
    try
    {
        stdlibpos.open(QIODevice::ReadOnly);
        libpos = stdlibpos.readAll().toStdString();
        stdlibpos.close();
    }
    catch (...) {}

    return libpos;
}

void RegexEditor::on_createFileBtn_clicked()
{
    if (mainDir == "")
        return;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Input File Name"),
                                             tr("File name:"), QLineEdit::Normal,
                                             "new.srx", &ok);

    if (ok)
    {
        auto file = (mainDir + "/" + text).toStdString();
        std::ifstream tester(file);
        if (!tester.good())
        {
            std::ofstream creator(file);
            if (!creator.good())
                return;
        }
        ui->fileSelector->addItem(text);
        ui->fileSelector->setCurrentText(text);
    }
}

void RegexEditor::on_optimize_clicked()
{
    /*
    try {
        unsigned pos=0;

        regexopt_choices tree = RegexOptParse(ui->regexOutput->toPlainText().toStdString(), pos);

        std::stringstream sstr;
        DumpRegexOptTree(sstr, tree);
        ui->regexOutput->setPlainText(QString::fromStdString(sstr.str()));
    }
    catch(const char *s)
    {
        ui->regexOutput->setPlainText(s);
    }
    catch(const std::string& s)
    {
        ui->regexOutput->setPlainText(QString::fromStdString(s));
    }
    */
}
