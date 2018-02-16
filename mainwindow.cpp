#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGraphicsRectItem>
#include <QGraphicsItem>

#include "chess_namespaces.h"
#include "chesspiece.h"
#include "paths.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionNew_game, &QAction::triggered,
            this, &MainWindow::newGame);

    ui->graphicsView->setFixedHeight(static_cast<int>(BoardSizes::BoardHeight));
    ui->graphicsView->setFixedWidth(static_cast<int>(BoardSizes::BoardWidth));

    setFixedSize(size());

    DrawBoard();

    //PlacePieces();

#if 1 // test
    PlacePieces({
    //std::make_tuple(QPixmap{ Paths::Black::knight }, PieceType::Knight, index_to_point(5, 7), Player::Black, 1),
    std::make_tuple(QPixmap{ Paths::Black::queen  }, PieceType::Queen,  index_to_point(4, 1), Player::Black, 1),
    std::make_tuple(QPixmap{ Paths::Black::king   }, PieceType::King,   index_to_point(5, 1), Player::Black, 1),
    std::make_tuple(QPixmap{ Paths::Black::bishop }, PieceType::Bishop, index_to_point(6, 1), Player::Black, 1),
    //std::make_tuple(QPixmap{ Paths::Black::pawn   }, PieceType::Pawn,   index_to_point(2, 5), Player::Black, 1),

    std::make_tuple(QPixmap{ Paths::White::king   }, PieceType::King,   index_to_point(5, 6), Player::White, 1),
    std::make_tuple(QPixmap{ Paths::White::queen  }, PieceType::Queen,  index_to_point(4, 8), Player::White, 1),
    std::make_tuple(QPixmap{ Paths::White::pawn   }, PieceType::Pawn,   index_to_point(1, 2), Player::White, 1),
    //std::make_tuple(QPixmap{ Paths::White::rook   }, PieceType::Rook,   index_to_point(8, 8), Player::White, 1)
    });
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::DrawBoard() {
    auto* scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    bool color = 1; // true = white, false = black;
    for(int y = 0; y < BoardSizes::MaxColCount; ++y){
        for(int x = 0; x < BoardSizes::MaxRowCount; ++x){
            auto* field = new QGraphicsRectItem(0, 0,
                                                BoardSizes::FieldWidth,
                                                BoardSizes::FieldHeight);
            field->setPos(x * BoardSizes::FieldWidth,
                          y * BoardSizes::FieldHeight);

            field->setBrush(color ? BoardBrush::White :
                                    BoardBrush::Black);

            color = !color;
            scene->addItem(std::move(field));
        }
        // shift colors to right
        color = !color;
    }
}

void MainWindow::PlacePieces(
        std::vector<std::tuple<const QPixmap&,
                               PieceType,
                               const QPointF&,
                               Player,
                               bool // first move
                             >>&& q)
{
    auto* scene = ui->graphicsView->scene();

    constexpr const size_t pixmap    = 0;
    constexpr const size_t piecetype = 1;
    constexpr const size_t qpointf   = 2;
    constexpr const size_t player    = 3;
    constexpr const size_t firstMove = 4;

    for(const auto& row : q) {
        ChessPiece* item = ChessPiece::Create(std::get<pixmap>(row),
                                              std::get<piecetype>(row),
                                              std::get<qpointf>(row),
                                              std::get<player>(row),
                                              scene,
                                              std::get<firstMove>(row));
        if(std::get<player>(row) == Player::White) {
            if(std::get<piecetype>(row) == PieceType::King) {
                GameStatus::White::king = static_cast<King*>(item);
            }
            GameStatus::White::pieces.push_back(item);
        }
        else {
            if(std::get<piecetype>(row) == PieceType::King) {
                GameStatus::Black::king = static_cast<King*>(item);
            }
            GameStatus::Black::pieces.push_back(item);
        }

        scene->addItem(item);
    }
}

void MainWindow::PlacePieces() {
    const std::array<std::tuple<const QPixmap&, PieceType, const QPointF&, Player>, 32> q{{
        std::make_tuple(QPixmap{":/Pieces/rook_black"},   PieceType::Rook,   index_to_point(1, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/knight_black"}, PieceType::Knight, index_to_point(2, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/bishop_black"}, PieceType::Bishop, index_to_point(3, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/queen_black"},  PieceType::Queen,  index_to_point(4, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/king_black"},   PieceType::King,   index_to_point(5, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/bishop_black"}, PieceType::Bishop, index_to_point(6, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/knight_black"}, PieceType::Knight, index_to_point(7, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/rook_black"},   PieceType::Rook,   index_to_point(8, 1), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(1, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(2, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(3, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(4, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(5, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(6, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(7, 2), Player::Black),
        std::make_tuple(QPixmap{":/Pieces/pawn_black"},   PieceType::Pawn,   index_to_point(8, 2), Player::Black),

        std::make_tuple(QPixmap{":/Pieces/rook_white"},   PieceType::Rook,   index_to_point(1, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/knight_white"}, PieceType::Knight, index_to_point(2, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/bishop_white"}, PieceType::Bishop, index_to_point(3, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/queen_white"},  PieceType::Queen,  index_to_point(4, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/king_white"},   PieceType::King,   index_to_point(5, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/bishop_white"}, PieceType::Bishop, index_to_point(6, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/knight_white"}, PieceType::Knight, index_to_point(7, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/rook_white"},   PieceType::Rook,   index_to_point(8, 8), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(1, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(2, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(3, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(4, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(5, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(6, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(7, 7), Player::White),
        std::make_tuple(QPixmap{":/Pieces/pawn_white"},   PieceType::Pawn,   index_to_point(8, 7), Player::White)
    }};

    auto* scene = ui->graphicsView->scene();

    constexpr const size_t pixmap    = 0;
    constexpr const size_t piecetype = 1;
    constexpr const size_t qpointf   = 2;
    constexpr const size_t player    = 3;

    for(const auto& row : q) {
        ChessPiece* item = ChessPiece::Create(std::get<pixmap>(row),
                                              std::get<piecetype>(row),
                                              std::get<qpointf>(row),
                                              std::get<player>(row),
                                              scene);
        if(std::get<player>(row) == Player::White) {
            if(std::get<piecetype>(row) == PieceType::King) {
                GameStatus::White::king = static_cast<King*>(item);
            }
            GameStatus::White::pieces.push_back(item);
        }
        else {
            if(std::get<piecetype>(row) == PieceType::King) {
                GameStatus::Black::king = static_cast<King*>(item);
            }
            GameStatus::Black::pieces.push_back(item);
        }

        scene->addItem(item);
    }
}

inline QPointF MainWindow::index_to_point(const qreal& x,
                                          const qreal& y) const noexcept
{
    return {(x-1) * BoardSizes::FieldWidth, (y-1) * BoardSizes::FieldHeight};
}

void MainWindow::cleanUp() noexcept {
    for(auto* piece : GameStatus::White::pieces) {
        piece->m_scene->removeItem(piece);
        delete piece;
    }
    GameStatus::White::pieces.clear();

    for(auto* piece : GameStatus::Black::pieces) {
        piece->m_scene->removeItem(piece);
        delete piece;
    }
    GameStatus::Black::pieces.clear();

    GameStatus::White::king = nullptr;
    GameStatus::Black::king = nullptr;

    GameStatus::currentPlayer = Player::White;

    GameStatus::promotedPieces.clear();
}

void MainWindow::newGame() noexcept {
    cleanUp();
    PlacePieces();
}

void MainWindow::showEvent(QShowEvent* event) {
    ui->graphicsView->centerOn({BoardSizes::BoardHeight / 2,
                                BoardSizes::BoardWidth  / 2});
    QWidget::showEvent(event);
}
