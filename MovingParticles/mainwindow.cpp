#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->SolverButton, SIGNAL(released()), this, SLOT(clickButton()));
    gl = ui->openGLWidget;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clickButton(){
    //reset simulation
    gl->Reset();

    //change solver
    if (solverStatus) solverStatus = false;
    else solverStatus = true;
    gl->changeSolver(solverStatus);

    //update button text
    if (solverStatus) ui->SolverButton->setText(QString("Euler"));
    else ui->SolverButton->setText(QString("Verlet"));
}

