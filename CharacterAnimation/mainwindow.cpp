#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gl = ui->openGLWidget;
}

void MainWindow::on_ChangeAerial_toggled(bool status){
    ui->openGLWidget->setViewType(AERIAL);
}

void MainWindow::on_ChangeFollow_toggled(bool status){
    ui->openGLWidget->setViewType(FP);
}

MainWindow::~MainWindow()
{
    delete ui;
}

