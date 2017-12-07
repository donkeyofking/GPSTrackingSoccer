#include "mainwindow.h"
#include "consts.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  // setting the label-text
  ui->dockWidgetHeim->setWindowTitle(MW_TEAM_HOME_DOCK);
  ui->dockWidgetGegner->setWindowTitle(MW_TEAM_ENEMY_DOCK);

  this->mMannschaftHeim.setUI(this);
  this->mMannschaftGegner.setUI(this);

  this->mMannschaftHeim.setChartWidget(ui->widget);
  this->mMannschaftHeim.setSlider(ui->horizontalSlider);

  this->mMannschaftGegner.setChartWidget(ui->widget);
  this->mMannschaftGegner.setSlider(ui->horizontalSlider);

  createMenusAndActions();

  connect(&mMannschaftHeim, &Mannschaft::playersChanged, this, &MainWindow::reDrawSpielerList);
  connect(&mMannschaftGegner, &Mannschaft::playersChanged, this, &MainWindow::reDrawSpielerList);

  loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenusAndActions()
{
  // File menu
  QMenu* fileMenu = menuBar()->addMenu(MW_FILE_MENU);
  fileMenu->addAction(MW_FILE_MENU_LOAD_TEAM_HOME, this, &MainWindow::showFileOpenDialogMannschaftHeim);
  fileMenu->addAction(MW_FILE_MENU_LOAD_TEAM_ENEMY, this, &MainWindow::showFileOpenDialogMannschaftGegner);
  fileMenu->addSeparator();
  fileMenu->addAction(MW_FILE_MENU_LOAD_PLAYER, this, &MainWindow::showFileOpenDialogAddPlayer);
  fileMenu->addSeparator();
  fileMenu->addAction(MW_FILE_MENU_LOAD_SETTINGS, this, &MainWindow::showSettingsDialog);
  fileMenu->addSeparator();
  fileMenu->addAction(MW_FILE_MENU_EXIT, this, SLOT(close()));

  QMenu* helpMenu = menuBar()->addMenu(MW_HELP_MENU);
  helpMenu->addAction(MW_HELP_MENU_HELP, this, &MainWindow::showHelpMenuDialog);
  helpMenu->addSeparator();
  helpMenu->addAction(MW_HELP_MENU_INFORMATION, this, &MainWindow::showInformationDialog);
}

QStringList MainWindow::showFileOpenDialog()
{
  QFileDialog openDialog(this);
  openDialog.setFileMode(QFileDialog::ExistingFiles);

  QStringList fileNames;
  if (openDialog.exec())
    fileNames = openDialog.selectedFiles();

  return fileNames;
}

void MainWindow::showFileOpenDialogMannschaftHeim()
{
  QStringList fileNames = this->showFileOpenDialog();

  this->mMannschaftHeim.neueSpieler(fileNames);
}

void MainWindow::showFileOpenDialogMannschaftGegner()
{
  QStringList fileNames = this->showFileOpenDialog();

  this->mMannschaftGegner.neueSpieler(fileNames);
}

void MainWindow::showFileOpenDialogAddPlayer()
{
  QMessageBox msgBox;
  msgBox.setText(MW_LOAD_PLAYER_QUESTION);
  msgBox.setInformativeText(MW_LOAD_PLAYER_QUESTION_INFORMATION);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Abort);
  msgBox.setDefaultButton(QMessageBox::Yes);
  msgBox.setButtonText(QMessageBox::Yes, BUTTON_TEXT_YES);
  msgBox.setButtonText(QMessageBox::No, BUTTON_TEXT_NO);
  msgBox.setButtonText(QMessageBox::Abort, BUTTON_TEXT_Abort);
  msgBox.setDefaultButton(QMessageBox::Yes);

  int ret = msgBox.exec();

  switch (ret) {
    case QMessageBox::Yes:
      // Yes was clicked
      this->showFileOpenDialogMannschaftHeim();
      break;
    case QMessageBox::No:
      // No was clicked
      this->showFileOpenDialogMannschaftGegner();
      break;
    default:
      // should never be reached
      break;
  }
}

void MainWindow::reDrawSpielerList()
{
  auto  widget = this->mMannschaftHeim.displaySpieler();
  ui->dockWidgetContentsHeim->setChartWidget(widget);

  widget = this->mMannschaftGegner.displaySpieler();
  ui->dockWidgetContentsGegner->setChartWidget(widget);
}

void MainWindow::showHelpMenuDialog()
{
  QMessageBox::question(this,
                           APPLICATION_NAME_HELP,
                           MW_HELP_TEXT,
                           QMessageBox::Ok);
}

void MainWindow::showInformationDialog()
{
  QMessageBox::information(this,
                           APPLICATION_NAME,
                           MW_ABOUT_TEXT,
                           QMessageBox::Ok);
}

void MainWindow::showSettingsDialog()
{
  SettingsDialog dlg;
  dlg.exec();
}

void MainWindow::loadSettings()
{
#ifdef Q_OS_LINUX
  QSettings settings("." + QApplication::applicationDirPath().left(1) + SETTINGS_FILE_PATH, QSettings::IniFormat);
#elif Q_OS_WIN
  QSettings settings(QApplication::applicationDirPath().left(1) + SETTINGS_FILE_PATH, QSettings::IniFormat);
#endif

  while(!settings.value(SETTINGS_COORDINATES_EXISTS, false).toBool())
  {
    QMessageBox::information(this,
                             APPLICATION_NAME,
                             MW_NO_SETTINGS,
                             QMessageBox::Ok);

    this->showSettingsDialog();
  }
}
