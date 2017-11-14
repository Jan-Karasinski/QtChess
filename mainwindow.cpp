#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DrawBoard();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DrawBoard() {
    auto* scene = new QGraphicsScene(this);
    QGraphicsRectItem* field = nullptr;
    ui->graphicsView->setScene(scene);

    bool color = 1; // true = white, false = black;
    for(int y = 0; y < BoardSizes::MaxColSize; ++y){
        for(int x = 0; x < BoardSizes::MaxRowSize; ++x){
            field = new QGraphicsRectItem(x * BoardSizes::FieldWidth,
                                          y * BoardSizes::FieldHeight,
                                          BoardSizes::FieldWidth,
                                          BoardSizes::FieldHeight);
            if(color) {
                field->setBrush(BoardBrush::White);
            }
            else {
                field->setBrush(BoardBrush::Black);
            }
            color = !color;
            scene->addItem(field);
        }
        // shift colors to right
        color = !color;
    }
}
