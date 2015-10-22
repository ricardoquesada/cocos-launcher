/****************************************************************************
Copyright 2015 Chukong Technologies

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>
#include <QProcessEnvironment>


// function taken from here:
// http://stackoverflow.com/a/26991243/1119460
static bool fileExists(const QString& path)
{
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return checkFile.exists() && checkFile.isFile();
}

QString PreferencesDialog::findCocosPath()
{
    QSettings settings("org.cocos2d-x","Cocos2d Console GUI");
    QString settingsPath = settings.value("cocos_console_path").toString();

    if (fileExists(settingsPath + "/cocos"))
        return settingsPath;

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
    {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process.setProcessEnvironment(env);

        process.setProcessChannelMode(QProcess::MergedChannels);

        QStringList stringList;
        stringList << "-l" << "-c" << "which cocos";
        process.start("/bin/bash", stringList);

        if (process.waitForFinished(5000))
        {
            auto filepath = process.readAllStandardOutput();
            if (fileExists(filepath))
            {
                settings.setValue("cocos_path", filepath);
                return filepath;
            }
        }
    }
#endif // Q_OS_OSX

    QString ret = "";

    QString cwd = QDir::currentPath();
    if (fileExists(cwd + "/cocos"))
        ret = cwd;

    else if (fileExists(cwd + "/../../cocos"))
        ret = cwd + "/../../";

   // Some wild guesses
    else if (fileExists(QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin/cocos"))
        ret = QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin";

    else if (fileExists(QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin/cocos"))
        ret = QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin";

    else
    {
        auto list = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
        foreach(const auto& element, list)
        {
            fileExists(element + "/Cocos/bin/cocos");
            ret = element + "/Cocos/bin/cocos";
            break;
        }
    }

    if (!ret.isEmpty())
    {
        ret = QFileInfo(ret).canonicalFilePath();
        settings.setValue("cocos_path", ret);
    }
    return ret;
}

QString PreferencesDialog::findSDKBOXPath()
{
    QSettings settings("org.cocos2d-x","Cocos2d Console GUI");
    QString settingsPath = settings.value("sdkbox_path").toString();

    if (fileExists(settingsPath + "/sdkbox"))
        return settingsPath;

#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
    {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process.setProcessEnvironment(env);

        process.setProcessChannelMode(QProcess::MergedChannels);

        QStringList stringList;
        stringList << "-l" << "-c" << "which sdkbox";
        process.start("/bin/bash", stringList);

        if (process.waitForFinished(5000))
        {
            auto filepath = process.readAllStandardOutput();
            if (fileExists(filepath))
            {
                settings.setValue("sdkbox_path", filepath);
                return filepath;
            }
        }
    }
#endif // Q_OS_OSX

    QString ret = "";
    QString cwd = QDir::currentPath();

    if (fileExists(cwd + "/sdkbox"))
        ret = cwd;

    else if (fileExists(cwd + "/../../sdkbox"))
        ret = cwd + "/../../";

   // Some wild guesses
    else if (fileExists(QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin/sdkbox"))
        ret = QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin";

    else if (fileExists(QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin/sdkbox"))
        ret = QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin";

    else
    {
        auto list = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
        foreach(const auto& element, list)
        {
            fileExists(element + "/Cocos/bin/sdkbox");
            ret = element + "/Cocos/bin/sdkbox";
            break;
        }
    }

    if (!ret.isEmpty())
    {
        ret = QFileInfo(ret).canonicalFilePath();
        settings.setValue("sdkbox_path", ret);
    }
    return ret;
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    ui->setupUi(this);
    ui->lineEdit_cocos->setText(PreferencesDialog::findCocosPath());
    ui->lineEdit_sdkbox->setText(PreferencesDialog::findSDKBOXPath());

    // report error in case there is an error
//    on_lineEdit_cocos_editingFinished();
//    on_lineEdit_sdkbox_editingFinished();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_directoryButton_cocos_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 tr("Cocos2D Console Path"),
                                                 ui->lineEdit_cocos->text());

    if (!filename.isEmpty())
        ui->lineEdit_cocos->setText(filename);
}

void PreferencesDialog::on_buttonBox_accepted()
{
    _settings.setValue("cocos_console_path", ui->lineEdit_cocos->text());
    _settings.setValue("sdkbox_path", ui->lineEdit_sdkbox->text());
}

void PreferencesDialog::on_lineEdit_cocos_editingFinished()
{
    // try to find "cocos". If not, report it as red
    auto cocos_path = ui->lineEdit_cocos->text();
    QFileInfo fi(cocos_path + "/cocos");
    if (!fi.exists())
        ui->label_error->setText(tr("<font color='red'>Invalid path. 'cocos' not found</font>"));
    else
        ui->label_error->setText("");
}

void PreferencesDialog::on_directoryButton_sdkbox_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 tr("SDKBOX Path"),
                                                 ui->lineEdit_sdkbox->text());

    if (!filename.isEmpty())
        ui->lineEdit_sdkbox->setText(filename);
}

void PreferencesDialog::on_lineEdit_sdkbox_editingFinished()
{
    // try to find "sdkbox". If not, report it as red
    auto sdkbox_path = ui->lineEdit_sdkbox->text();
    QFileInfo fi(sdkbox_path + "/sdkbox");
    if (!fi.exists())
        ui->label_error->setText(tr("<font color='red'>Invalid path. 'sdkbox' not found</font>"));
    else
        ui->label_error->setText("");
}
