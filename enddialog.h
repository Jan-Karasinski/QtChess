#ifndef CHECKMATEDIALOG_H
#define CHECKMATEDIALOG_H

#include <QDialog>

namespace Ui {
    class EndDialog;
}

enum class Player : char;
enum class WinCondition : int;

class EndDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EndDialog(std::pair<WinCondition, Player> t_state,
                       QWidget *parent = 0);
    ~EndDialog();

private:
    Ui::EndDialog *ui;
};

#endif // CHECKMATEDIALOG_H
