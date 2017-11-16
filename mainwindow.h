#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBrush>
#include <vector>
#include <utility>
#include <string>
#include <QPointF>

typedef const qreal xpos;
typedef const qreal ypos;

enum class PieceType : char {
    King   = 'K', Queen  = 'Q', Rook = 'R',
    Bishop = 'B', Knight = 'H', Pawn = 'P',
    Empty  = 'E'
};

enum class Player : char {
    White = 'W', Black = 'B'
};

namespace BoardSizes {
    constexpr int multiply(int i, int j) {
        return i * j;
    }

    const int MaxColSize  = 8;
    const int MaxRowSize  = 8;

    const int FieldHeight = 48;
    const int FieldWidth  = 48;

    const int BoardHeight = multiply(MaxColSize, FieldHeight);
    const int BoardWidth  = multiply(MaxRowSize, FieldWidth);
}

namespace BoardBrush {
    const QBrush White = QBrush(QColor(Qt::GlobalColor::white));
    const QBrush Black = QBrush(QColor(Qt::GlobalColor::black));
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

    void PlacePieces(std::vector<std::tuple<const QPointF&, const QPixmap&, PieceType, Player>>);

    QPointF index_to_point(xpos, ypos) const noexcept;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
