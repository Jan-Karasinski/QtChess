#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "chesspiece.h"
#include <QGraphicsRectItem>
#include <QLinkedList>
#include "chesspiece.h"

namespace GameStatus {
    Player currentPlayer = Player::White;
    King* whiteKing = nullptr;
    King* blackKing = nullptr;
    QQueue<QPair<QGraphicsRectItem*, QBrush>> highlighted;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DrawBoard();
    PlacePieces({std::make_tuple(PieceType::Rook,   index_to_point(0, 0), QPixmap{":/Pieces/rook_black"},   Player::Black),
                 std::make_tuple(PieceType::Knight, index_to_point(1, 0), QPixmap{":/Pieces/knight_black"}, Player::Black),
                 std::make_tuple(PieceType::Bishop, index_to_point(2, 0), QPixmap{":/Pieces/bishop_black"}, Player::Black),
                 std::make_tuple(PieceType::Queen,  index_to_point(3, 0), QPixmap{":/Pieces/queen_black"},  Player::Black),
                 std::make_tuple(PieceType::King,   index_to_point(4, 0), QPixmap{":/Pieces/king_black"},   Player::Black),
                 std::make_tuple(PieceType::Bishop, index_to_point(5, 0), QPixmap{":/Pieces/bishop_black"}, Player::Black),
                 std::make_tuple(PieceType::Knight, index_to_point(6, 0), QPixmap{":/Pieces/knight_black"}, Player::Black),
                 std::make_tuple(PieceType::Rook,   index_to_point(7, 0), QPixmap{":/Pieces/rook_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(0, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(1, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(2, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(3, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(4, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(5, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(6, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),
                 std::make_tuple(PieceType::Pawn,   index_to_point(7, 1), QPixmap{":/Pieces/pawn_black"},   Player::Black),

                 std::make_tuple(PieceType::Rook,   index_to_point(0, 7), QPixmap{":/Pieces/rook_white"},   Player::White),
                 std::make_tuple(PieceType::Knight, index_to_point(1, 7), QPixmap{":/Pieces/knight_white"}, Player::White),
                 std::make_tuple(PieceType::Bishop, index_to_point(2, 7), QPixmap{":/Pieces/bishop_white"}, Player::White),
                 std::make_tuple(PieceType::Queen,  index_to_point(3, 7), QPixmap{":/Pieces/queen_white"},  Player::White),
                 std::make_tuple(PieceType::King,   index_to_point(4, 7), QPixmap{":/Pieces/king_white"},   Player::White),
                 std::make_tuple(PieceType::Bishop, index_to_point(5, 7), QPixmap{":/Pieces/bishop_white"}, Player::White),
                 std::make_tuple(PieceType::Knight, index_to_point(6, 7), QPixmap{":/Pieces/knight_white"}, Player::White),
                 std::make_tuple(PieceType::Rook,   index_to_point(7, 7), QPixmap{":/Pieces/rook_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(0, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(1, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(2, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(3, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(4, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(5, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(6, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
                 std::make_tuple(PieceType::Pawn,   index_to_point(7, 6), QPixmap{":/Pieces/pawn_white"},   Player::White),
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

void MainWindow::PlacePieces(std::vector<std::tuple<PieceType, const QPointF&, const QPixmap&, Player>> q) {
    auto* scene = ui->graphicsView->scene();
    QGraphicsItem* item = nullptr;
    for(auto row : q) {
        item = ChessPiece::Create(std::get<0>(row), std::get<1>(row),
                                  std::get<2>(row), std::get<3>(row), scene);
        if(std::get<0>(row) == PieceType::King) {
            if(std::get<3>(row) == Player::White) {
                GameStatus::whiteKing = static_cast<King*>(item);
            }
            else {
                GameStatus::blackKing = static_cast<King*>(item);
            }
        }
        scene->addItem(item);
    }
}

QPointF MainWindow::index_to_point(xpos x, ypos y) const noexcept {
    return {x * BoardSizes::FieldWidth, y * BoardSizes::FieldHeight};
}

int BoardSizes::posToIndex(qreal pos, qreal size) noexcept{
    int i = 0;
    while (pos > 0.0) {
        pos -= size;
        ++i;
    }
    return i;
}
