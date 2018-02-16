#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointF>
#include <tuple>

class ChessPiece;
class King;

enum class PieceType : char;
enum class Player : char;

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

    void PlacePieces(std::vector<std::tuple<const QPixmap&,
                                            PieceType,
                                            const QPointF&,
                                            Player,
                                            bool>>&&);

    void PlacePieces();

    inline QPointF index_to_point(const qreal&, const qreal&) const noexcept;

    void cleanUp() noexcept;

    Ui::MainWindow *ui;

public slots:
    void newGame() noexcept;

protected:
    void showEvent(QShowEvent* event) override;
};

#endif // MAINWINDOW_H
