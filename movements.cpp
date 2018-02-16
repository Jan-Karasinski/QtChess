#include "movements.h"
#include "chess_namespaces.h"
#include "chesspiece.h"

Movement::Movement(const QPointF& t_point, const MoveType t_type) noexcept
    : m_coordinates(t_point), m_type(t_type)
{
}

bool operator ==(const std::unique_ptr<Movement>& t_move,
                 const QPointF& t_coordinates) noexcept
{
    return t_coordinates == t_move->m_coordinates;
}

bool operator !=(const std::unique_ptr<Movement>& t_move,
                 const QPointF& t_coordinates) noexcept
{
    return t_coordinates != t_move->m_coordinates;
}

void Movement::movePiece(ChessPiece* t_piece, QPointF t_dest)  const noexcept {
    if(t_piece->m_type == PieceType::Pawn) {
        GameStatus::uselessMoves = 0;
    }
    else {
        ++GameStatus::uselessMoves;
    }

    t_piece->setPos(t_dest);
    t_piece->m_lastPos = t_dest;
}

void Movement::movePiece(ChessPiece* t_fPiece, QPointF t_fDest,
                         ChessPiece* t_sPiece, QPointF t_sDest) const noexcept
{
    ++GameStatus::uselessMoves;

    t_fPiece->setPos(t_fDest);
    t_fPiece->m_lastPos = t_fDest;

    t_sPiece->setPos(t_sDest);
    t_sPiece->m_lastPos = t_sDest;
}

void AttackingType::removePiece(ChessPiece* t_enemy) {
    GameStatus::uselessMoves = 0;

    auto& pieces = t_enemy->m_player == Player::White ?
                            GameStatus::White::pieces :
                            GameStatus::Black::pieces;
    const auto it = std::find(std::begin(pieces),
                              std::end(pieces),
                              t_enemy);
    if(it != std::end(pieces)) {
        pieces.erase(it);
    }

    t_enemy->m_scene->removeItem(t_enemy);
    delete t_enemy; // neccessary, as no longer owned by scene
}

void Move::exec() {
    movePiece(m_self, m_moveDest);
}

const QBrush& Move::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

Move::Move(ChessPiece* t_self, const QPointF& t_moveDest) noexcept
    : Movement(m_moveDest, MoveType::Move),
      m_self(t_self), m_moveDest(t_moveDest)
{
}


// set pos of m_self to pos of m_enemy and delete m_enemy from board
void Attack::exec() {
    movePiece(m_self, m_enemy->m_lastPos);

    removePiece(m_enemy);
}

const QBrush& Attack::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

Attack::Attack(ChessPiece* t_self, ChessPiece* t_enemy) noexcept
    : Movement(t_enemy->m_lastPos, MoveType::Attack),
      m_self(t_self), m_enemy(t_enemy)
{
}


// set pos of m_self to pos of m_enemy and delete m_enemy from board
void Castle::exec() {
    movePiece(m_king, m_kingDest, m_rook, m_rookDest);
}

const QBrush& Castle::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

Castle::Castle(ChessPiece* t_king, const QPointF& t_kingDest,
               ChessPiece* t_rook, const QPointF& t_rookDest) noexcept
    : Movement(m_kingDest, MoveType::Castle),
      m_king(t_king), m_kingDest(t_kingDest),
      m_rook(t_rook), m_rookDest(t_rookDest)
{
}


void EnPassantAttack::exec() {
    movePiece(m_self, m_moveDest);

    removePiece(m_enemy);
}

const QBrush& EnPassantAttack::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

EnPassantAttack::EnPassantAttack(ChessPiece* t_self, ChessPiece* t_enemy, const QPointF& t_moveDest) noexcept
    : Movement(m_moveDest, MoveType::EnPassant),
      m_self(t_self), m_enemy(t_enemy), m_moveDest(t_moveDest)
{
}

void EnPassantMove::exec() {
    movePiece(m_self, m_moveDest);

    m_self->m_enPassant = true;
}

const QBrush& EnPassantMove::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

EnPassantMove::EnPassantMove(Pawn* t_self, const QPointF& t_moveDest) noexcept
    : Movement(m_moveDest, MoveType::Move), m_self(t_self), m_moveDest(t_moveDest)
{

}

void PromotionMove::exec() {
    movePiece(m_self, m_moveDest);

    m_self->promote();
}

const QBrush& PromotionMove::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

PromotionMove::PromotionMove(Pawn* _self, const QPointF& _moveDest) noexcept
    : Movement(m_moveDest, MoveType::PromotionMove),
      m_self(_self), m_moveDest(_moveDest)
{
}


void PromotionAttack::exec() {
    movePiece(m_self, m_enemy->m_lastPos);

    removePiece(m_enemy);

    m_self->promote();
}

const QBrush& PromotionAttack::getHightlightColor() const noexcept {
    return m_hightlightColor;
}

PromotionAttack::PromotionAttack(Pawn* t_self, ChessPiece* t_enemy) noexcept
    : Movement(t_enemy->m_lastPos, MoveType::PromotionAttack),
      m_self(t_self), m_enemy(t_enemy)
{
}

const QBrush Move::m_hightlightColor            = {Qt::GlobalColor::blue};
const QBrush Attack::m_hightlightColor          = {Qt::GlobalColor::red};
const QBrush Castle::m_hightlightColor          = {QColor(148,0,211)}; // purple
const QBrush EnPassantAttack::m_hightlightColor = {QColor(148,0,211)}; // purple
const QBrush EnPassantMove::m_hightlightColor   = {Qt::GlobalColor::blue};
const QBrush PromotionMove::m_hightlightColor   = {QColor(148,0,211)}; // purple
const QBrush PromotionAttack::m_hightlightColor = {QColor(148,0,211)}; // purple

