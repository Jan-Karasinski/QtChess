#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include "mainwindow.h"
#include <QGraphicsPixmapItem>
#include <QPointF>

struct Movement;

using Container = decltype(GameStatus::White::pieces);

QPointF getFixedPos(const QPointF&) noexcept;

class ChessPiece : public QGraphicsPixmapItem
{
public:
    ChessPiece(const QPixmap& t_pixMap, PieceType t_type,
               const QPointF& t_point, Player t_player,
               QGraphicsScene* t_scene, bool t_firstMove = true) noexcept;

    static ChessPiece* Create(const QPixmap&  t_pixMap,
                              PieceType       t_type,
                              const QPointF&  t_point,
                              Player          t_player,
                              QGraphicsScene* t_scene,
                              bool            t_firstMove = true) noexcept;

    static ChessPiece* Create(PieceType       t_type,
                              const QPointF&  t_point,
                              Player          t_player,
                              QGraphicsScene* t_scene,
                              bool            t_firstMove = true) noexcept;

    virtual ~ChessPiece() = default;


    // ignoredPos treated as empty
    virtual bool canAttackField(const QPointF& t_targetPos,
                                const QPointF& t_newDefenderPos,
                                const QPointF& t_ignoredPos) const = 0;

    virtual bool canAttackField(const QPointF& t_targetPos,
                                const QPointF& t_newDefenderPos,
                                std::vector<QPointF>&& t_ignoredPos) const = 0;

    virtual bool haveValidMoves() const noexcept = 0;

    static inline void addMove(Movement* t_movetype);

    bool isGameOver() const noexcept;

    static void checkmate() noexcept;

    static void nextTurn() noexcept;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* t_event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* t_event);

    virtual size_t findValidMoves() noexcept = 0;

    void highlight();
    void dehighlight();

// variables
public:
    static constexpr const qreal defaultZValue = 10;

    static std::vector<std::unique_ptr<Movement>> m_moves;

    const PieceType m_type;
    QPointF         m_lastPos;
    const Player    m_player;
    QGraphicsScene* m_scene;
    bool            m_firstMove{ true };

    King*& m_king{ m_player == Player::White ? GameStatus::White::king :
                                               GameStatus::Black::king };
    Container& m_enemyPieces{ m_player == Player::White ?
                                            GameStatus::Black::pieces :
                                            GameStatus::White::pieces };
};

class Pawn final : public ChessPiece
{
public:
    Pawn(const QPixmap& t_pixMap, const QPointF& t_point,
         Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

    void promote();

    bool enPassant() const noexcept;

    bool m_enPassant{ false };

private:
    size_t findValidMoves() noexcept override;
};

class Knight final : public ChessPiece
{
public:
    Knight(const QPixmap& t_pixMap, const QPointF& t_point,
           Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

private:
    size_t findValidMoves() noexcept override;
};

class Bishop final : public ChessPiece
{
public:
    Bishop(const QPixmap& t_pixMap, const QPointF& t_point,
           Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

private:
    size_t findValidMoves() noexcept override;

    bool validateField(const QPointF& t_field) noexcept;
};

class Rook final : public ChessPiece
{
public:
    Rook(const QPixmap& t_pixMap, const QPointF& t_point,
         Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

private:
    size_t findValidMoves() noexcept override;

    bool validateField(const QPointF& t_field) noexcept;
};

class Queen final : public ChessPiece
{
public:
    Queen(const QPixmap& t_pixMap, const QPointF& t_point,
          Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

private:
    size_t findValidMoves() noexcept override;

    bool validateField(const QPointF& t_field) noexcept;
};

class King final : public ChessPiece
{
public:
    King(const QPixmap& t_pixMap, const QPointF& t_point,
         Player t_player, QGraphicsScene* t_scene, bool t_firstMove = true);

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        const QPointF& t_ignoredPos) const override;

    bool canAttackField(const QPointF& t_targetPos,
                        const QPointF& t_newDefenderPos,
                        std::vector<QPointF>&& t_ignoredPos) const override;

    bool haveValidMoves() const noexcept override;

    bool inCheckAfterMove(const QPointF& t_oldPos,
                          const QPointF& t_newPos) const noexcept;

    // for pawn (en passant)
    bool inCheckAfterMove(const QPointF& t_newPos,
                          std::vector<QPointF>&& t_oldPos) const noexcept;

private:
    size_t findValidMoves() noexcept override;

    // for king
    bool inCheckAfterMove(const QPointF& t_KingNewPos) const noexcept;
};

#endif // CHESSPIECE_H
