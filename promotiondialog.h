#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QDialog>

namespace Ui {
    class PromotionDialog;
}

class ChessPiece;
enum class PieceType : char;

class PromotionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PromotionDialog(ChessPiece* piece, QWidget* parent = 0);
    ~PromotionDialog();

    PieceType getType() const noexcept;

private:
    Ui::PromotionDialog *ui;

    ChessPiece* m_piece;
    PieceType m_type;
};

#endif // PROMOTIONDIALOG_H
