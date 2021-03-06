#include "promotiondialog.h"
#include "ui_promotiondialog.h"

#include "chess_namespaces.h"
#include "chesspiece.h"
#include "paths.h"

PromotionDialog::PromotionDialog(ChessPiece* piece, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::PromotionDialog),
    m_piece(piece)
{
    ui->setupUi(this);

    const QSize iconSize{ static_cast<int>(BoardSizes::FieldWidth),
                          static_cast<int>(BoardSizes::FieldHeight) };

    setFixedSize(size());
    ui->Knight->setIconSize(iconSize);
    ui->Knight->setText({});

    ui->Bishop->setIconSize(iconSize);
    ui->Bishop->setText({});

    ui->Rook->setIconSize(iconSize);
    ui->Rook->setText({});

    ui->Queen->setIconSize(iconSize);
    ui->Queen->setText({});

    setWindowFlags(windowFlags() &
                   ~Qt::WindowContextHelpButtonHint &
                   ~Qt::WindowCloseButtonHint);

    if(m_piece->m_player == Player::White) {
        ui->Knight->setIcon({ QPixmap{ Paths::White::knight } });
        ui->Bishop->setIcon({ QPixmap{ Paths::White::bishop } });
        ui->Rook->setIcon  ({ QPixmap{ Paths::White::rook   } });
        ui->Queen->setIcon ({ QPixmap{ Paths::White::queen  } });
    }
    else {
        ui->Knight->setIcon({ QPixmap{ Paths::Black::knight } });
        ui->Bishop->setIcon({ QPixmap{ Paths::Black::bishop } });
        ui->Rook->setIcon  ({ QPixmap{ Paths::Black::rook   } });
        ui->Queen->setIcon ({ QPixmap{ Paths::Black::queen  } });
    }

    connect(
        ui->Knight, &QPushButton::clicked,
        [&]() {
            m_type = PieceType::Knight;
            QDialog::close();
        });

    connect(
        ui->Bishop, &QPushButton::clicked,
        [&]() {
            m_type = PieceType::Bishop;
            QDialog::close();
        });

    connect(
        ui->Rook, &QPushButton::clicked,
        [&]() {
            m_type = PieceType::Rook;
            QDialog::close();
        });

    connect(
        ui->Queen, &QPushButton::clicked,
        [&]() {
            m_type = PieceType::Queen;
            QDialog::close();
        });
}

PromotionDialog::~PromotionDialog()
{
    delete ui;
}

PieceType PromotionDialog::getType() const noexcept {
    return m_type;
}
