#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBrush>

namespace BoardSizes {
    const int MaxColSize  = 8;
    const int MaxRowSize  = 8;

    const int FieldHeight = 48;
    const int FieldWidth  = 48;
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

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
