#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBrush>
#include <QQueue>
#include <QPair>
#include <utility>
#include <string>
#include <QPointF>
#include <QGraphicsItem>
#include <memory>
#include <queue>
#include <utility>

typedef const qreal xpos;
typedef const qreal ypos;

class ChessPiece;
class King;

enum class PieceType : char {
    King   = 'K', Queen  = 'Q', Rook = 'R',
    Bishop = 'B', Knight = 'H', Pawn = 'P',
};

enum Player : char {
    White = 'W', Black = 'B'
};

namespace BoardSizes {
    constexpr const int MaxColCount = 8;
    constexpr const int MaxRowCount = 8;

    constexpr const int FieldHeight = 48;
    constexpr const int FieldWidth  = 48;

    constexpr const int BoardHeight = MaxColCount * FieldHeight;
    constexpr const int BoardWidth  = MaxRowCount * FieldWidth;
}

namespace BoardBrush {
    const QBrush White = QBrush(QColor(Qt::GlobalColor::white));
    const QBrush Black = QBrush(QColor(Qt::GlobalColor::black));
}

namespace GameStatus {
    extern Player currentPlayer;
    extern std::queue<std::pair<QGraphicsRectItem*, QBrush>> highlighted;

    // pieces detatched from scene
    extern std::vector<std::unique_ptr<ChessPiece>> promotedPieces;

    namespace White {
        extern King* king;
        extern std::vector<ChessPiece*> pieces;
    }
    namespace Black {
        extern King* king;
        extern std::vector<ChessPiece*> pieces;
    }
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void DrawBoard();

    void PlacePieces(std::vector<std::tuple<const QPixmap&, PieceType, const QPointF&, Player>>);

    void PlacePieces();

    QPointF index_to_point(const qreal&, const qreal&) const noexcept;

    void cleanUp() noexcept;

    void newGame() noexcept;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
