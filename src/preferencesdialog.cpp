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


// function taken from here:
// http://stackoverflow.com/a/26991243/1119460
static bool fileExists(const QString& path)
{
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}

QString PreferencesDialog::findCocosPath()
{
    QString ret = "";

    // current directory
    QString cwd = QDir::currentPath();
    QString settingsPath = QSettings("org.cocos2d-x","Cocos2d Console GUI").value("cocos_console_path").toString();

    if (fileExists(settingsPath + "/cocos"))
        return settingsPath;

    else if (fileExists(cwd + "/cocos"))
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
        ret = QFileInfo(ret).canonicalFilePath();
    return ret;
}

QString PreferencesDialog::findSDKBOXPath()
{
    QString ret = "";

    // current directory
    QString cwd = QDir::currentPath();
    QString settingsPath = QSettings("org.cocos2d-x","Cocos2d Console GUI").value("sdkbox_path").toString();

    if (fileExists(settingsPath + "/sdkbox"))
        return settingsPath;

    else if (fileExists(cwd + "/sdkbox"))
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
            fileExists(element + "/Cocos/bin/cocos");
            ret = element + "/Cocos/bin/cocos";
            break;
        }
    }

    if (!ret.isEmpty())
        ret = QFileInfo(ret).canonicalFilePath();
    return ret;
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    ui->setupUi(this);

    auto defaultDir = _settings.value("cocos_console_path").toString();
    if (defaultDir.isEmpty())
        defaultDir = PreferencesDialog::findCocosPath();
    ui->lineEdit_cocos->setText(defaultDir);

    defaultDir = _settings.value("sdkbox_path").toString();
    if (defaultDir.isEmpty())
        defaultDir = PreferencesDialog::findCocosPath();
    ui->lineEdit_sdkbox->setText(defaultDir);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_directoryButton_cocos_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 "Cocos2D Console Path",
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
        ui->label_error->setText("<font color='red'>Invalid path. 'cocos' not found</font>");
    else
        ui->label_error->setText("");
}

void PreferencesDialog::on_directoryButton_sdkbox_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 "SDKBOX Path",
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
        ui->label_error->setText("<font color='red'>Invalid path. 'sdkbox' not found</font>");
    else
        ui->label_error->setText("");
}
