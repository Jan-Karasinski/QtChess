#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chesspiece.h"
#include <QGraphicsRectItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DrawBoard();
    PlacePieces({ std::make_tuple(index_to_point(0, 0), QPixmap{":/Pieces/rook_black"}, PieceType::Rook, Player::Black),
                  std::make_tuple(index_to_point(7, 7), QPixmap{":/Pieces/rook_white"}, PieceType::Rook, Player::Black)
                });
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
            field = new QGraphicsRectItem(0, 0,
                                          BoardSizes::FieldWidth,
                                          BoardSizes::FieldHeight);
            field->setPos(x * BoardSizes::FieldWidth,
                          y * BoardSizes::FieldHeight);
            if(color) {
                field->setBrush(BoardBrush::White);
            }
            else {
                field->setBrush(BoardBrush::Black);
            }
            color = !color;
            scene->addItem(std::move(field));
        }
        // shift colors to right
        color = !color;
    }
}

void MainWindow::PlacePieces(std::vector<std::tuple<const QPointF&, const QPixmap&, PieceType, Player>> q) {
    ChessPiece* piece = nullptr;
    auto* scene = ui->graphicsView->scene();
    for(auto row : q) {
        piece = new ChessPiece({0, 0}, std::get<1>(row),
                               std::get<2>(row), std::get<3>(row), scene);
        piece->setPos(std::get<0>(row)); // necessary :v
        piece->setFlag(QGraphicsItem::ItemIsMovable);
        scene->addItem(std::move(piece));
    }
}

QPointF MainWindow::index_to_point(xpos x, ypos y) const noexcept {
    return {x * BoardSizes::FieldWidth, y * BoardSizes::FieldHeight};
}
