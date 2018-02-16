#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include <QGraphicsScene>
#include <memory>

class ChessPiece;
class Pawn;

enum class MoveType : int;

struct Movement
{
    const QPointF& m_coordinates;

    const MoveType m_type;

    virtual void exec() = 0;
    virtual const QBrush& getHightlightColor() const noexcept = 0;

    Movement(const QPointF& t_point, const MoveType t_type) noexcept;

    virtual ~Movement() = default;

    friend bool operator ==(const std::unique_ptr<Movement>& t_move,
                            const QPointF& t_coordinates) noexcept;

    friend bool operator !=(const std::unique_ptr<Movement>& t_move,
                            const QPointF& t_coordinates) noexcept;

protected:
    void movePiece(ChessPiece* t_piece, QPointF t_dest) const noexcept;

    // overload for castling
    void movePiece(ChessPiece* t_fPiece, QPointF t_fDest,
                   ChessPiece* t_sPiece, QPointF t_sDest) const noexcept;
};

struct AttackingType
{
protected:
    void removePiece(ChessPiece* t_enemy);
};

struct Move : public Movement
{
    ChessPiece* m_self;
    const QPointF m_moveDest;

    static const QBrush m_hightlightColor;

    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    Move(ChessPiece* t_self, const QPointF& t_moveDest) noexcept;
};

struct Attack : public Movement, public AttackingType
{
    ChessPiece* m_self;
    ChessPiece* m_enemy;

    static const QBrush m_hightlightColor;

    // set pos of m_self to pos of m_enemy and delete m_enemy from board
    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    Attack(ChessPiece* t_self, ChessPiece* t_enemy) noexcept;
};

struct Castle : public Movement
{
    ChessPiece* m_king;
    const QPointF m_kingDest;

    ChessPiece* m_rook;
    const QPointF m_rookDest;

    static const QBrush m_hightlightColor;

    // set pos of m_self to pos of m_enemy and delete m_enemy from board
    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    Castle(ChessPiece* t_king, const QPointF& t_kingDest,
           ChessPiece* t_rook, const QPointF& t_rookDest) noexcept;
};

struct EnPassantAttack : public Movement, public AttackingType
{
    ChessPiece* m_self;
    ChessPiece* m_enemy;
    const QPointF m_moveDest;

    static const QBrush m_hightlightColor;

    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    EnPassantAttack(ChessPiece* t_self, ChessPiece* t_enemy, const QPointF& t_moveDest) noexcept;
};

struct EnPassantMove : public Movement
{
    Pawn* m_self;
    const QPointF m_moveDest;

    static const QBrush m_hightlightColor;

    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    EnPassantMove(Pawn* t_self, const QPointF& t_moveDest) noexcept;
};

struct PromotionMove : public Movement
{
    Pawn* m_self;
    const QPointF m_moveDest;

    static const QBrush m_hightlightColor;

    // set pos of m_self to pos of m_enemy and delete m_enemy from board
    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    PromotionMove(Pawn* t_self, const QPointF& t_moveDest) noexcept;
};

struct PromotionAttack : public Movement, public AttackingType
{
    Pawn* m_self;
    ChessPiece* m_enemy;

    static const QBrush m_hightlightColor;

    // set pos of m_self to pos of m_enemy and delete m_enemy from board
    void exec() override;

    const QBrush& getHightlightColor() const noexcept override;

    PromotionAttack(Pawn* t_self, ChessPiece* t_enemy) noexcept;
};

#endif // MOVEMENTS_H
