#include "enddialog.h"
#include "ui_enddialog.h"

#include "chess_namespaces.h"

EndDialog::EndDialog(std::pair<WinCondition, Player> t_state,
                     QWidget *parent)
    : QDialog(parent),
      ui(new Ui::EndDialog)
{
    ui->setupUi(this);
    setFixedSize(size());
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    if(t_state.first == WinCondition::Draw) {
        ui->label->setText("The game ended in a draw");
        return;
    }

    auto condition = [&] {
        switch (t_state.first) {
            case WinCondition::Checkmate: {
                return "checkmate";
            }
            case WinCondition::Stalemate: {
                return "stalemate";
            }
            case WinCondition::FiftyMoves: {
                return "fifty moves rule";
            }
            default: {
                QCoreApplication::exit(1);
                return "error";
            }
        }
    }();

    if(t_state.second == Player::White) {
        QString str{ "Whites won by " };
        str.append(std::move(condition));

        ui->label->setText(std::move(str));
    }
    else {
        QString str{ "Blacks won by " };
        str.append(std::move(condition));

        ui->label->setText(std::move(str));
    }
}

EndDialog::~EndDialog()
{
    delete ui;
}
