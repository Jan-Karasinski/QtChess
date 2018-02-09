#include "chesspiece.h"
#include "movements.h"
#include "promotiondialog.h"
#include "paths.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <algorithm>

template<typename C, typename V>
bool contains(C&& t_container, V&& t_value) {
    return std::end(t_container) != std::find(std::begin(t_container),
                                              std::end(t_container),
                                              t_value);
}

QPointF getFixedPos(const QPointF& pos) noexcept {
    // offset to middle of piece
    constexpr const qreal offsetX = .5*BoardSizes::FieldWidth;
    constexpr const qreal offsetY = .5*BoardSizes::FieldHeight;
    return {pos.x() + offsetX - std::fmod(pos.x() + offsetX, BoardSizes::FieldWidth),
            pos.y() + offsetY - std::fmod(pos.y() + offsetY, BoardSizes::FieldHeight)};
}

enum class FieldState : int
{
    Empty = 0, Friend = 1, Enemy = 2, InvalidField = 3
};

struct fieldInfo
{
    FieldState state = FieldState::Empty;

    ChessPiece* piece = nullptr;

    bool operator ==(FieldState i) const noexcept {
        return state == i;
    }

    bool operator !=(FieldState i) const noexcept {
        return state != i;
    }

    fieldInfo(FieldState s = FieldState::Empty)
        : state(s)
    {
    }

    fieldInfo(FieldState s, ChessPiece* p)
        : state(s), piece(p)
    {
    }

};

fieldInfo checkField(const QPointF& pos,
                     const ChessPiece* plPiece,
                     QGraphicsScene* scene)
{
    if(pos.x() < 0 || pos.x() >= BoardSizes::BoardWidth ||
       pos.y() < 0 || pos.y() >= BoardSizes::BoardHeight)
    {
        return {FieldState::InvalidField};
    }

    // list of elements in middle of field
    auto list = scene->items({pos.x() + .5*BoardSizes::FieldWidth,
                              pos.y() + .5*BoardSizes::FieldHeight});

    // make sure at most one piece exist on field
    Q_ASSERT_X(list.size() <= 2, "TESTcheckField",
                                 "more than 1 piece at field");

    if(list.size() == 1) { // contains only field
        return {FieldState::Empty};
    }

    auto* piece = dynamic_cast<ChessPiece*>(list.constFirst());
    if(piece) {
        if(piece->m_player == plPiece->m_player) {
            return {FieldState::Friend, piece};
        }
        else {
            return {FieldState::Enemy, piece};
        }
    }

    return {FieldState::InvalidField};
}


ChessPiece::ChessPiece(const QPixmap& t_pixMap, PieceType t_type,
                       const QPointF& t_point, Player t_player,
                       QGraphicsScene* t_scene, bool t_firstMove) noexcept
    : QGraphicsPixmapItem(t_pixMap),
      m_type(t_type),
      m_lastPos(t_point),
      m_player(t_player),
      m_scene(t_scene),
      m_firstMove(t_firstMove)
{
    setPos(t_point);
    setFlag(QGraphicsItem::ItemIsMovable);
    setZValue(ChessPiece::defaultZValue);
}

ChessPiece* ChessPiece::Create(const QPixmap& t_pixMap, PieceType t_type,
                               const QPointF& t_point, Player t_player,
                               QGraphicsScene* t_scene, bool t_firstMove) noexcept
{
    switch (t_type) {
        case PieceType::Pawn:
            return new Pawn(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Knight:
            return new Knight(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Bishop:
            return new Bishop(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Rook:
            return new Rook(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Queen:
            return new Queen(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::King:
            return new King(t_pixMap, t_point, t_player, t_scene, t_firstMove);
    }

    return nullptr;
}

ChessPiece* ChessPiece::Create(PieceType t_type,
                               const QPointF& t_point, Player t_player,
                               QGraphicsScene* t_scene, bool t_firstMove) noexcept
{
    auto t_pixMap = [&]() -> QPixmap {
            switch (t_type) {
                case PieceType::Pawn: {
                    if(t_player == Player::White) {
                        return {Paths::White::pawn};
                    }
                    else {
                        return {Paths::Black::pawn};
                    }
                }

                case PieceType::Knight: {
                    if(t_player == Player::White) {
                        return {Paths::White::knight};
                    }
                    else {
                        return {Paths::Black::knight};
                    }
                }

                case PieceType::Bishop: {
                    if(t_player == Player::White) {
                        return {Paths::White::bishop};
                    }
                    else {
                        return {Paths::Black::bishop};
                    }
                }

                case PieceType::Rook: {
                    if(t_player == Player::White) {
                        return {Paths::White::rook};
                    }
                    else {
                        return {Paths::Black::rook};
                    }
                }

                case PieceType::Queen: {
                    if(t_player == Player::White) {
                        return {Paths::White::queen};
                    }
                    else {
                        return {Paths::Black::queen};
                    }
                }

                case PieceType::King: {
                    if(t_player == Player::White) {
                        return {Paths::White::king};
                    }
                    else {
                        return {Paths::Black::king};
                    }
                }
            }
    }();

    switch (t_type) {
        case PieceType::Pawn:
            return new Pawn(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Knight:
            return new Knight(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Bishop:
            return new Bishop(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Rook:
            return new Rook(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::Queen:
            return new Queen(t_pixMap, t_point, t_player, t_scene, t_firstMove);

        case PieceType::King:
            return new King(t_pixMap, t_point, t_player, t_scene, t_firstMove);
    }

    return nullptr;
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* t_event) {
    if(t_event->button() == Qt::LeftButton) {
        if(GameStatus::currentPlayer != m_player) {
            t_event->ignore();
        }
        else {
            setZValue(zValue() + 1);

            findValidMoves();

            highlight();
        }
    }

    QGraphicsPixmapItem::mousePressEvent(t_event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* t_event) {
    if(t_event->button() == Qt::LeftButton) {
        dehighlight();
        setZValue(zValue() - 1);

        const QPointF piecePos = getFixedPos(pos());

        auto move = std::find(std::begin(m_moves), std::end(m_moves), piecePos);

        if(move != std::end(m_moves)) {
            m_firstMove = false;

            if((*move)->type == MoveType::PromotionAttack ||
               (*move)->type == MoveType::PromotionMove)
            { // prevents jumping to top-left corner after promotion
                QGraphicsPixmapItem::mouseReleaseEvent(t_event);
            }

            (*move)->exec(); // if promotion - pawn gets deleted

            if(isGameOver()) {
                ChessPiece::checkmate();
            }
            else {
                ChessPiece::nextTurn();
            }
        }
        else {
            t_event->ignore();
            setPos(m_lastPos);
        }

        ChessPiece::m_moves.clear();
    }

    QGraphicsPixmapItem::mouseReleaseEvent(t_event);
}

void ChessPiece::highlight() {
    constexpr const qreal offsetX = .5*BoardSizes::FieldWidth;
    constexpr const qreal offsetY = .5*BoardSizes::FieldHeight;

    for(const auto& move : m_moves) {
        auto list = m_scene->items({move->m_coordinates.x() + offsetX,
                                    move->m_coordinates.y() + offsetY});

        auto* field = static_cast<QGraphicsRectItem*>(list.last());

        GameStatus::highlighted.emplace(field, field->brush());
        field->setBrush(move->getHightlightColor());
    }
}

void ChessPiece::dehighlight() {
    while (!GameStatus::highlighted.empty()) {
        auto pair = GameStatus::highlighted.front();

        pair.first->setBrush(pair.second);

        GameStatus::highlighted.pop();
    }
}

bool ChessPiece::isGameOver() const noexcept {
    // if count of available moves of enemies == 0
    // (cannot protect their king or king cannot flee)

    return std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            return enemy->haveValidMoves();
                        });
}

void ChessPiece::checkmate() noexcept {
    std::for_each(std::begin(GameStatus::White::pieces),
                  std::end(GameStatus::White::pieces),
                  [&](ChessPiece* piece) {
                      piece->setEnabled(false);
                  });
    std::for_each(std::begin(GameStatus::Black::pieces),
                  std::end(GameStatus::Black::pieces),
                  [&](ChessPiece* piece) {
                      piece->setEnabled(false);
                  });
}

void ChessPiece::nextTurn() noexcept {
    if(GameStatus::currentPlayer == Player::White) {
        GameStatus::currentPlayer = Player::Black;
    }
    else {
        GameStatus::currentPlayer = Player::White;
    }
}

std::vector<std::unique_ptr<Movement>> ChessPiece::m_moves;

inline void ChessPiece::addMove(Movement* t_movetype) {
    ChessPiece::m_moves.emplace_back(t_movetype);
}

//

Pawn::Pawn(const QPixmap& t_pixMap, const QPointF& t_point,
           Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
    : ChessPiece(t_pixMap, PieceType::Pawn,
                 t_point, t_player, t_scene, t_firstMove)
{
}

bool Pawn::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          const QPointF& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // pawn cannot be blocked, parameters ignored
    auto posToCheck = [&] () -> std::array<const QPointF, 2> {
        if(m_player == Player::White) {
            return {{
                // right top
                {m_lastPos.x() + BoardSizes::FieldWidth,
                 m_lastPos.y() - BoardSizes::FieldHeight},
                // left top
                {m_lastPos.x() - BoardSizes::FieldWidth,
                 m_lastPos.y() - BoardSizes::FieldHeight}
            }};
        }
        else {
            return {{
                // right bottom
                {m_lastPos.x() + BoardSizes::FieldWidth,
                 m_lastPos.y() + BoardSizes::FieldHeight},
                // left bottom
                {m_lastPos.x() - BoardSizes::FieldWidth,
                 m_lastPos.y() + BoardSizes::FieldHeight}
            }};
        }
    }();

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(pos == t_targetPos ||
           (state == FieldState::Enemy &&
            state.piece->m_lastPos == t_targetPos))
        {
            return true;
        }
    }

    return false;
}

bool Pawn::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          std::vector<QPointF>&& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // pawn cannot be blocked, parameters ignored
    auto posToCheck = [&] () -> std::array<QPointF, 2> {
        if(m_player == Player::White) {
            return {{
                // right top
                {m_lastPos.x() + BoardSizes::FieldWidth,
                 m_lastPos.y() - BoardSizes::FieldHeight},
                // left top
                {m_lastPos.x() - BoardSizes::FieldWidth,
                 m_lastPos.y() - BoardSizes::FieldHeight}
            }};
        }
        else {
            return {{
                // right bottom
                {m_lastPos.x() + BoardSizes::FieldWidth,
                 m_lastPos.y() + BoardSizes::FieldHeight},
                // left bottom
                {m_lastPos.x() - BoardSizes::FieldWidth,
                 m_lastPos.y() + BoardSizes::FieldHeight}
            }};
        }
    }();

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(pos == t_targetPos ||
           (state == FieldState::Enemy &&
            state.piece->m_lastPos == t_targetPos))
        {
            return true;
        }
    }

    return false;
}

bool Pawn::haveValidMoves() const noexcept {
    const qreal direction = m_player == Player::White ? -BoardSizes::FieldHeight : BoardSizes::FieldHeight;

    const QPointF middle  {m_lastPos.x(),                          m_lastPos.y() + direction},
            secondMiddle  {middle.x(),                             middle.y()    + direction},
            left          {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction},
            leftEPAttack  {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()},
            leftEPDest    {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction},
            right         {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction},
            rightEPAttack {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()},
            rightEPDest   {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction};


    // middle, move only
    {
        if(checkField(middle, this, m_scene) == FieldState::Empty &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            return enemy->canAttackField(m_king->m_lastPos, middle, m_lastPos);
                        })
        ) {
            return true;
        }
    }

    // left, attack only
    {
        auto state = checkField(left, this, m_scene);

        if(state == FieldState::Enemy &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            if(enemy->m_lastPos == left)
                                return false;
                            return enemy->canAttackField(m_king->m_lastPos, left, m_lastPos);
                        })
        ) {
            return true;
        }
    }

    // left en passant, attack only
    {
        auto attackedPosStatus    = checkField(leftEPAttack, this, m_scene); // pos of enemy
        auto destinationPosStatus = checkField(leftEPDest,   this, m_scene);

        if(destinationPosStatus == FieldState::Empty &&
           attackedPosStatus == FieldState::Enemy &&
           attackedPosStatus.piece->m_type == PieceType::Pawn &&
           static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            if(enemy->m_lastPos == leftEPAttack)
                                return false;
                            return enemy->canAttackField(m_king->m_lastPos, leftEPDest, {m_lastPos, leftEPAttack});
                         })
        ) {
            return true;
        }
    }

    // right, attack only
    {
        auto state = checkField(right, this, m_scene);

        if(state == FieldState::Enemy &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            if(enemy->m_lastPos == right)
                                return false;
                            return enemy->canAttackField(m_king->m_lastPos, right, m_lastPos);
                        }))
        {
            return true;
        }
    }

    // right en passant, attack only
    {
        auto attackedPosStatus    = checkField(rightEPAttack, this, m_scene); // pos of enemy
        auto destinationPosStatus = checkField(rightEPDest, this, m_scene);

        if(destinationPosStatus == FieldState::Empty &&
           attackedPosStatus == FieldState::Enemy &&
           attackedPosStatus.piece->m_type == PieceType::Pawn &&
           static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            if(enemy->m_lastPos == rightEPAttack)
                                return false;
                            return enemy->canAttackField(m_king->m_lastPos, rightEPDest, {m_lastPos, rightEPAttack});
                        }))
        {
            return true;
        }
    }

    return false;
}

void Pawn::promote() {
    auto type = [&]() -> PieceType {
        PromotionDialog dialog(this);
        dialog.exec();
        return dialog.getType();
    }();

    // delete piece from scene and container
    // add piece to scene and container

    auto newPiece = ChessPiece::Create(type, m_lastPos, m_player, m_scene, false);

    auto& pieces = m_player == Player::White ? GameStatus::White::pieces:
                                               GameStatus::Black::pieces;

    pieces.erase(std::find(std::begin(pieces), std::end(pieces), this));
    m_scene->removeItem(this);


    pieces.push_back(newPiece);
    m_scene->addItem(newPiece);

    // scene no longer owns this piece
    GameStatus::promotedPieces.emplace_back(this);
}

size_t Pawn::findValidMoves() noexcept {
    const qreal direction = m_player == Player::White ? -BoardSizes::FieldHeight :
                                                        BoardSizes::FieldHeight;

    const QPointF middle { m_lastPos.x(),                          m_lastPos.y() + direction },
            secondMiddle { middle.x(),                             middle.y()    + direction },
            left         { m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction },
            leftEPAttack { m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() },
            leftEPDest   { m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction },
            right        { m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction },
            rightEPAttack{ m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() },
            rightEPDest  { m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction };


    // middle, move only
    {
        if(checkField(middle, this, m_scene) == FieldState::Empty &&
           !m_king->inCheckAfterMove(middle, m_lastPos)
        ) {
            // if last field, save as promotion
            if(middle.y() <  BoardSizes::FieldHeight ||
               middle.y() >= BoardSizes::BoardHeight - BoardSizes::FieldHeight)
            {
                addMove(new PromotionMove(this, middle));
            }
            else {
                addMove(new Move(this, middle));
            }

            // second middle, move only
            if(m_firstMove &&
               checkField(secondMiddle, this, m_scene) == FieldState::Empty &&
               !m_king->inCheckAfterMove(secondMiddle, m_lastPos)
            ) {
                addMove(new EnPassantMove(this, secondMiddle));
            }
        }
    }

    // left, attack only
    {
        auto state = checkField(left, this, m_scene);

        if(state == FieldState::Enemy &&
           !m_king->inCheckAfterMove(left, m_lastPos)
        ) {
            if(left.y() <  BoardSizes::FieldHeight ||
               left.y() >= BoardSizes::BoardHeight - BoardSizes::FieldHeight)
            {
                addMove(new PromotionAttack(this, state.piece));
            }
            else {
                addMove(new Attack(this, state.piece));
            }
        }
    }

    // left en passant, attack only
    {
        auto attackedPosStatus    = checkField(leftEPAttack, this, m_scene); // pos of enemy
        auto destinationPosStatus = checkField(leftEPDest,   this, m_scene);

        if(destinationPosStatus == FieldState::Empty &&
           attackedPosStatus == FieldState::Enemy &&
           attackedPosStatus.piece->m_type == PieceType::Pawn &&
           static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
           !m_king->inCheckAfterMove(leftEPDest, {m_lastPos, leftEPAttack})
        ) {
            addMove(new EnPassantAttack(this, attackedPosStatus.piece, leftEPDest));
        }
    }

    // right, attack only
    {
        auto state = checkField(right, this, m_scene);

        if(state == FieldState::Enemy &&
           !m_king->inCheckAfterMove(right, m_lastPos)
        ) {
            if(right.y() <  BoardSizes::FieldHeight ||
               right.y() >= BoardSizes::BoardHeight - BoardSizes::FieldHeight)
            {
                addMove(new PromotionAttack(this, state.piece));
            }
            else {
                addMove(new Attack(this, state.piece));
            }
        }
    }

    // right en passant, attack only
    {
        auto attackedPosStatus    = checkField(rightEPAttack, this, m_scene); // pos of enemy
        auto destinationPosStatus = checkField(rightEPDest, this, m_scene);

        if(destinationPosStatus == FieldState::Empty &&
           attackedPosStatus == FieldState::Enemy &&
           attackedPosStatus.piece->m_type == PieceType::Pawn &&
           static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
           !m_king->inCheckAfterMove(rightEPDest, {m_lastPos, rightEPAttack})
        ) {
            addMove(new EnPassantAttack(this, attackedPosStatus.piece, rightEPDest));
        }
    }

    return m_moves.size();
}

//

Knight::Knight(const QPixmap& t_pixMap, const QPointF& t_point,
               Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
        : ChessPiece(t_pixMap, PieceType::Knight,
                     t_point, t_player, t_scene, t_firstMove)
{
}

bool Knight::canAttackField(const QPointF& t_targetPos,
                            const QPointF& t_newDefenderPos,
                            const QPointF& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // knight cannot be blocked, parameters ignored
    std::array<const QPointF, 8> posToCheck{{
            // top left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},
            // top right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},

            // right top
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right bottom
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},

            // bottom left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},
            // bottom right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},

            // left top
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // left bottom
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight}
    }};

    for(const auto& pos : posToCheck) {
        if(pos != t_targetPos) {
            continue;
        }

        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Enemy ||
           state == FieldState::Empty)
        {
            return true;
        }
    }

    return false;
}

bool Knight::canAttackField(const QPointF& t_targetPos,
                            const QPointF& t_newDefenderPos,
                            std::vector<QPointF>&& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // knight cannot be blocked, parameters ignored
    std::array<const QPointF, 8> posToCheck{{
            // top left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},
            // top right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},

            // right top
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right bottom
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},

            // bottom left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},
            // bottom right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},

            // left top
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // left bottom
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight}
    }};

    for(const auto& pos : posToCheck) {
        if(pos != t_targetPos) {
            continue;
        }

        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Enemy ||
           state == FieldState::Empty)
        {
            return true;
        }
    }

    return false;
}

bool Knight::haveValidMoves() const noexcept {
    std::array<const QPointF, 8> posToCheck{{
            // top left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},
            // top right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},

            // right top
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right bottom
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},

            // bottom left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},
            // bottom right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},

            // left top
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // left bottom
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight}
    }};

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Friend ||
           state == FieldState::InvalidField)
        {
            continue;
        }

        if(!m_king->inCheckAfterMove(pos, m_lastPos)) {
            return true;
        }
    }

    return false;
}

size_t Knight::findValidMoves() noexcept {
    std::array<const QPointF, 8> posToCheck{{
            // top left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},
            // top right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - 2*BoardSizes::FieldHeight},

            // right top
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right bottom
            {m_lastPos.x() + 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},

            // bottom left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},
            // bottom right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + 2*BoardSizes::FieldHeight},

            // left top
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // left bottom
            {m_lastPos.x() - 2*BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight}
    }};

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Friend ||
           state == FieldState::InvalidField
        ) {
            continue;
        }

        if(!m_king->inCheckAfterMove(pos, m_lastPos)) {
            if(state == FieldState::Enemy) {
                addMove(new Attack(this, state.piece));
            }
            else if(state == FieldState::Empty) {
                addMove(new Move(this, pos));
            }
        }
    }

    return m_moves.size();
}

//

Bishop::Bishop(const QPixmap& t_pixMap, const QPointF& t_point,
               Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
    : ChessPiece(t_pixMap, PieceType::Bishop,
                 t_point, t_player, t_scene, t_firstMove)
{
}

bool Bishop::canAttackField(const QPointF& t_targetPos,
                            const QPointF& t_newDefenderPos,
                            const QPointF& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // if target lies next to this piece return true
    if(m_lastPos != t_targetPos &&
       std::abs(m_lastPos.x() - t_targetPos.x()) <= BoardSizes::FieldWidth &&
       std::abs(m_lastPos.y() - t_targetPos.y()) <= BoardSizes::FieldHeight)
    {
        return true;
    }

    // if not on diagonal relative to last pos
    if(std::round(std::abs(t_targetPos.x() - m_lastPos.x()) / BoardSizes::FieldWidth) !=
       std::round(std::abs(t_targetPos.y() - m_lastPos.y()) / BoardSizes::FieldHeight)
    ) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // target on left side of the board
    if(t_targetPos.x() < m_lastPos.x()) {
        // left-top
        if(t_targetPos.y() < m_lastPos.y()){
            QPointF leftTop{m_lastPos.x() - BoardSizes::FieldWidth,
                            m_lastPos.y() - BoardSizes::FieldHeight};
            while(leftTop.x() > t_targetPos.x() &&
                  leftTop.y() > t_targetPos.y())
            {
                if(leftTop != t_ignoredPos &&
                        (leftTop == t_newDefenderPos ||
                         checkField(leftTop, this, m_scene) != FieldState::Empty)
                ) {
                   return false;
                }

                leftTop.rx() -= BoardSizes::FieldWidth;
                leftTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // left-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF leftBottom{m_lastPos.x() - BoardSizes::FieldWidth,
                               m_lastPos.y() + BoardSizes::FieldHeight};
            while(leftBottom.x() > t_targetPos.x() &&
                  leftBottom.y() < t_targetPos.y())
            {
                if(leftBottom != t_ignoredPos &&
                        (leftBottom == t_newDefenderPos ||
                         checkField(leftBottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                leftBottom.rx() -= BoardSizes::FieldWidth;
                leftBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // target on right side of the board
    else if(t_targetPos.x() > m_lastPos.x()) {
        // right-top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF rightTop{m_lastPos.x() + BoardSizes::FieldWidth,
                             m_lastPos.y() - BoardSizes::FieldHeight};
            while(rightTop.x() < t_targetPos.x() &&
                  rightTop.y() > t_targetPos.y())
            {
                if(rightTop != t_ignoredPos &&
                        (rightTop == t_newDefenderPos ||
                         checkField(rightTop, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                rightTop.rx() += BoardSizes::FieldWidth;
                rightTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // right-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF rightBottom{m_lastPos.x() + BoardSizes::FieldWidth,
                                m_lastPos.y() + BoardSizes::FieldHeight};
            while(rightBottom.x() < t_targetPos.x() &&
                  rightBottom.y() < t_targetPos.y())
            {
                if(rightBottom != t_ignoredPos &&
                        (rightBottom == t_newDefenderPos ||
                         checkField(rightBottom, this, m_scene) != FieldState::Empty)
                ) {
                      return false;
                }

                rightBottom.rx() += BoardSizes::FieldWidth;
                rightBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    return false;
}

bool Bishop::canAttackField(const QPointF& t_targetPos,
                            const QPointF& t_newDefenderPos,
                            std::vector<QPointF>&& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // if not on diagonal relative to last pos
    if(std::round(std::abs(t_targetPos.x() - m_lastPos.x()) / BoardSizes::FieldWidth) !=
       std::round(std::abs(t_targetPos.y() - m_lastPos.y()) / BoardSizes::FieldHeight)
    ) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // target on left side of the board
    if(t_targetPos.x() < m_lastPos.x()) {
        // left-top
        if(t_targetPos.y() < m_lastPos.y()){
            QPointF leftTop{m_lastPos.x() - BoardSizes::FieldWidth,
                            m_lastPos.y() - BoardSizes::FieldHeight};
            while(leftTop.x() > t_targetPos.x() &&
                  leftTop.y() > t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, leftTop) &&
                        (leftTop == t_newDefenderPos ||
                         checkField(leftTop, this, m_scene) != FieldState::Empty)
                ) {
                   return false;
                }

                leftTop.rx() -= BoardSizes::FieldWidth;
                leftTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // left-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF leftBottom{m_lastPos.x() - BoardSizes::FieldWidth,
                               m_lastPos.y() + BoardSizes::FieldHeight};
            while(leftBottom.x() > t_targetPos.x() &&
                  leftBottom.y() < t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, leftBottom) &&
                        (leftBottom == t_newDefenderPos ||
                         checkField(leftBottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                leftBottom.rx() -= BoardSizes::FieldWidth;
                leftBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // target on right side of the board
    else if(t_targetPos.x() > m_lastPos.x()) {
        // right-top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF rightTop{m_lastPos.x() + BoardSizes::FieldWidth,
                             m_lastPos.y() - BoardSizes::FieldHeight};
            while(rightTop.x() < t_targetPos.x() &&
                  rightTop.y() > t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, rightTop) &&
                        (rightTop == t_newDefenderPos ||
                         checkField(rightTop, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                rightTop.rx() += BoardSizes::FieldWidth;
                rightTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // right-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF rightBottom{m_lastPos.x() + BoardSizes::FieldWidth,
                                m_lastPos.y() + BoardSizes::FieldHeight};
            while(rightBottom.x() < t_targetPos.x() &&
                  rightBottom.y() < t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, rightBottom) &&
                        (rightBottom == t_newDefenderPos ||
                         checkField(rightBottom, this, m_scene) != FieldState::Empty)
                ) {
                      return false;
                }

                rightBottom.rx() += BoardSizes::FieldWidth;
                rightBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    return false;
}

bool Bishop::haveValidMoves() const noexcept {
    // left top diagonal
    {
        QPointF leftTopDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                m_lastPos.y() - BoardSizes::FieldHeight};

        while(leftTopDiagonal.x() >= 0 &&
              leftTopDiagonal.y() >= 0)
        {
            if(!m_king->inCheckAfterMove(leftTopDiagonal, m_lastPos)) {
                return true;
            }

            leftTopDiagonal.rx() -= BoardSizes::FieldWidth;
            leftTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right bottom diagonal
    {
        QPointF rightBottomDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                    m_lastPos.y() + BoardSizes::FieldHeight};

        while(rightBottomDiagonal.x() < BoardSizes::BoardWidth &&
              rightBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!m_king->inCheckAfterMove(rightBottomDiagonal, m_lastPos)) {
                return true;
            }

            rightBottomDiagonal.rx() += BoardSizes::FieldWidth;
            rightBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }
    // right top diagonal
    {
        QPointF rightTopDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                 m_lastPos.y() - BoardSizes::FieldHeight};

        while(rightTopDiagonal.x() < BoardSizes::BoardWidth &&
              rightTopDiagonal.y() >= 0)
        {
            if(!m_king->inCheckAfterMove(rightTopDiagonal, m_lastPos)) {
                return true;
            }

            rightTopDiagonal.rx() += BoardSizes::FieldWidth;
            rightTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // left bottom diagonal
    {
        QPointF leftBottomDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                   m_lastPos.y() + BoardSizes::FieldHeight};

        while(leftBottomDiagonal.x() >= 0 &&
              leftBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!m_king->inCheckAfterMove(leftBottomDiagonal, m_lastPos)) {
                return true;
            }

            leftBottomDiagonal.rx() -= BoardSizes::FieldWidth;
            leftBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }

    return false;
}

size_t Bishop::findValidMoves() noexcept {
    // left top diagonal
    {
        QPointF leftTopDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                m_lastPos.y() - BoardSizes::FieldHeight};

        while(leftTopDiagonal.x() >= 0 &&
              leftTopDiagonal.y() >= 0)
        {
            if(!validateField(leftTopDiagonal)) {
                break;
            }

            leftTopDiagonal.rx() -= BoardSizes::FieldWidth;
            leftTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right bottom diagonal
    {
        QPointF rightBottomDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                    m_lastPos.y() + BoardSizes::FieldHeight};

        while(rightBottomDiagonal.x() < BoardSizes::BoardWidth &&
              rightBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!validateField(rightBottomDiagonal)) {
                break;
            }

            rightBottomDiagonal.rx() += BoardSizes::FieldWidth;
            rightBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }
    // right top diagonal
    {
        QPointF rightTopDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                 m_lastPos.y() - BoardSizes::FieldHeight};

        while(rightTopDiagonal.x() < BoardSizes::BoardWidth &&
              rightTopDiagonal.y() >= 0)
        {
            if(!validateField(rightTopDiagonal)) {
                break;
            }

            rightTopDiagonal.rx() += BoardSizes::FieldWidth;
            rightTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // left bottom diagonal
    {
        QPointF leftBottomDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                   m_lastPos.y() + BoardSizes::FieldHeight};

        while(leftBottomDiagonal.x() >= 0 &&
              leftBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!validateField(leftBottomDiagonal)) {
                break;
            }

            leftBottomDiagonal.rx() -= BoardSizes::FieldWidth;
            leftBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }

    return m_moves.size();
}

bool Bishop::validateField(const QPointF& t_field) noexcept {
    auto state = checkField(t_field, this, m_scene);

    if(state == FieldState::Friend ||
       state == FieldState::InvalidField
    ) {
        return false;
    }

    if(!m_king->inCheckAfterMove(t_field, m_lastPos)) {
        if(state == FieldState::Enemy) {
            addMove(new Attack(this, state.piece));
            return false;
        }
        else if (state == FieldState::Empty) {
            addMove(new Move(this, t_field));
        }
    }

    return true;
}

//

Rook::Rook(const QPixmap& t_pixMap, const QPointF& t_point,
           Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
    : ChessPiece(t_pixMap, PieceType::Rook,
                 t_point, t_player, t_scene, t_firstMove)
{
}

bool Rook::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          const QPointF& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // vertical
    if(t_targetPos.x() == m_lastPos.x()) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top = {m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight};

            while(top.y() > t_targetPos.y()) {
                if(top != t_ignoredPos &&
                        (top == t_newDefenderPos ||
                         checkField(top, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                top.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF bottom = {m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight};

            while(bottom.y() < t_targetPos.y()) {
                if(bottom != t_ignoredPos &&
                        (bottom == t_newDefenderPos ||
                         checkField(bottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                bottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // horizontal
    else if(t_targetPos.y() == m_lastPos.y()) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left = {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()};

            while(left.x() > t_targetPos.x()) {
                if(left != t_ignoredPos &&
                        (left == t_newDefenderPos ||
                         checkField(left, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                left.rx() -= BoardSizes::FieldWidth;
            }

            return true;
        }

        // right
        else if(t_targetPos.x() > m_lastPos.x()) {
            QPointF right = {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()};

            while(right.x() < t_targetPos.x()) {
                if(right != t_ignoredPos &&
                        (right == t_newDefenderPos ||
                         checkField(right, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                right.rx() += BoardSizes::FieldWidth;
            }

            return true;
        }
    }

    return false;
}

bool Rook::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          std::vector<QPointF>&& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // vertical
    if(t_targetPos.x() == m_lastPos.x()) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top = {m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight};

            while(top.y() > t_targetPos.y()) {
                if(!::contains(t_ignoredPos, top) &&
                        (top == t_newDefenderPos ||
                         checkField(top, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                top.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // bottom
        else if(t_targetPos.y() < m_lastPos.y()) {
            QPointF bottom = {m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight};

            while(bottom.y() < t_targetPos.y()) {
                if(!::contains(t_ignoredPos, bottom) &&
                        (bottom == t_newDefenderPos ||
                         checkField(bottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                bottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // horizontal
    else if(t_targetPos.y() == m_lastPos.y()) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left = {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()};

            while(left.x() > t_targetPos.x()) {
                if(!::contains(t_ignoredPos, left) &&
                        (left == t_newDefenderPos ||
                         checkField(left, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                left.rx() -= BoardSizes::FieldWidth;
            }

            return true;
        }

        // right
        else if(t_targetPos.x() > m_lastPos.x()) {
            QPointF right = {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()};

            while(right.x() < t_targetPos.x()) {
                if(!::contains(t_ignoredPos, right) &&
                        (right == t_newDefenderPos ||
                         checkField(right, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                right.rx() += BoardSizes::FieldWidth;
            }

            return true;
        }
    }

    return false;
}

bool Rook::haveValidMoves() const noexcept {
    // top straight
    {
        QPointF topStraight{m_lastPos.x(),
                            m_lastPos.y() - BoardSizes::FieldHeight};

        while(topStraight.y() >= 0) {
            if(!m_king->inCheckAfterMove(topStraight, m_lastPos)) {
                return true;
            }

            topStraight.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right straight
    {
        QPointF rightStraight{m_lastPos.x() + BoardSizes::FieldWidth,
                              m_lastPos.y()};

        while(rightStraight.x() < BoardSizes::BoardWidth) {
            if(!m_king->inCheckAfterMove(rightStraight, m_lastPos)) {
                return true;
            }


            rightStraight.rx() += BoardSizes::FieldWidth;
        }
    }
    // bottom straight
    {
        QPointF bottomStraight{m_lastPos.x(),
                               m_lastPos.y() + BoardSizes::FieldHeight};

        while(bottomStraight.y() < BoardSizes::BoardHeight) {
            if(!m_king->inCheckAfterMove(bottomStraight, m_lastPos)) {
                return true;
            }


            bottomStraight.ry() += BoardSizes::FieldHeight;
        }
    }
    // left straight
    {
        QPointF leftStraight{m_lastPos.x() - BoardSizes::FieldWidth,
                             m_lastPos.y()};

        while(leftStraight.x() >= 0) {
            if(!m_king->inCheckAfterMove(leftStraight, m_lastPos)) {
                return true;
            }

            leftStraight.rx() -= BoardSizes::FieldWidth;
        }
    }

    return false;
}

size_t Rook::findValidMoves() noexcept {
    // top straight
    {
        QPointF topStraight{m_lastPos.x(),
                            m_lastPos.y() - BoardSizes::FieldHeight};

        while(topStraight.y() >= 0) {
            if(!validateField(topStraight)) {
                break;
            }

            topStraight.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right straight
    {
        QPointF rightStraight{m_lastPos.x() + BoardSizes::FieldWidth,
                              m_lastPos.y()};

        while(rightStraight.x() < BoardSizes::BoardWidth) {
            if(!validateField(rightStraight)) {
                break;
            }

            rightStraight.rx() += BoardSizes::FieldWidth;
        }
    }
    // bottom straight
    {
        QPointF bottomStraight{m_lastPos.x(),
                               m_lastPos.y() + BoardSizes::FieldHeight};

        while(bottomStraight.y() < BoardSizes::BoardHeight) {
            if(!validateField(bottomStraight)) {
                break;
            }

            bottomStraight.ry() += BoardSizes::FieldHeight;
        }
    }
    // left straight
    {
        QPointF leftStraight{m_lastPos.x() - BoardSizes::FieldWidth,
                             m_lastPos.y()};

        while(leftStraight.x() >= 0) {
            if(!validateField(leftStraight)) {
                break;
            }

            leftStraight.rx() -= BoardSizes::FieldWidth;
        }
    }

    return m_moves.size();
}

bool Rook::validateField(const QPointF& t_field) noexcept {
    auto state = checkField(t_field, this, m_scene);

    if(state == FieldState::Friend ||
       state == FieldState::InvalidField
    ) {
        return false;
    }

    if(!m_king->inCheckAfterMove(t_field, m_lastPos)) {
        if(state == FieldState::Enemy) {
            addMove(new Attack(this, state.piece));
            return false;
        }
        else if (state == FieldState::Empty) {
            addMove(new Move(this, t_field));
        }
    }

    return true;
}

//

Queen::Queen(const QPixmap& t_pixMap, const QPointF& t_point,
             Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
    : ChessPiece(t_pixMap, PieceType::Queen,
                 t_point, t_player, t_scene, t_firstMove)
{
}

bool Queen::canAttackField(const QPointF& t_targetPos,
                           const QPointF& t_newDefenderPos,
                           const QPointF& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // vertical
    if(t_targetPos.x() == m_lastPos.x()) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top = {m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight};

            while(top.y() > t_targetPos.y()) {
                if(top != t_ignoredPos &&
                        (top == t_newDefenderPos ||
                         checkField(top, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                top.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF bottom = {m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight};

            while(bottom.y() < t_targetPos.y()) {
                if(bottom != t_ignoredPos &&
                        (bottom == t_newDefenderPos ||
                         checkField(bottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                bottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // horizontal
    else if(t_targetPos.y() == m_lastPos.y()) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left = {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()};

            while(left.x() > t_targetPos.x()) {
                if(left != t_ignoredPos &&
                        (left == t_newDefenderPos ||
                         checkField(left, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                left.rx() -= BoardSizes::FieldWidth;
            }

            return true;
        }

        // right
        else if(t_targetPos.x() > m_lastPos.x()) {
            QPointF right = {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()};

            while(right.x() < t_targetPos.x()) {
                if(right != t_ignoredPos &&
                        (right == t_newDefenderPos ||
                         checkField(right, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                right.rx() += BoardSizes::FieldWidth;
            }

            return true;
        }
    }

    // if not on diagonal relative to last pos
    if(std::round(std::abs(t_targetPos.x() - m_lastPos.x()) / BoardSizes::FieldWidth) !=
       std::round(std::abs(t_targetPos.y() - m_lastPos.y()) / BoardSizes::FieldHeight)
    ) {
        return false;
    }

    // target on left side of the board
    if(t_targetPos.x() < m_lastPos.x()) {
        // target in left-top
        if(t_targetPos.y() < m_lastPos.y()){
            QPointF leftTop{m_lastPos.x() - BoardSizes::FieldWidth,
                            m_lastPos.y() - BoardSizes::FieldHeight};
            while(leftTop.x() > t_targetPos.x() &&
                  leftTop.y() > t_targetPos.y())
            {
                if(leftTop != t_ignoredPos &&
                        (leftTop == t_newDefenderPos ||
                         checkField(leftTop, this, m_scene) != FieldState::Empty)
                ) {
                   return false;
                }

                leftTop.rx() -= BoardSizes::FieldWidth;
                leftTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // left-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF leftBottom{m_lastPos.x() - BoardSizes::FieldWidth,
                               m_lastPos.y() + BoardSizes::FieldHeight};
            while(leftBottom.x() > t_targetPos.x() &&
                  leftBottom.y() < t_targetPos.y())
            {
                if(leftBottom != t_ignoredPos &&
                        (leftBottom == t_newDefenderPos ||
                         checkField(leftBottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                leftBottom.rx() -= BoardSizes::FieldWidth;
                leftBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // target on right side of the board
    else if(t_targetPos.x() > m_lastPos.x()) {
        // target in right-top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF rightTop{m_lastPos.x() + BoardSizes::FieldWidth,
                             m_lastPos.y() - BoardSizes::FieldHeight};
            while(rightTop.x() < t_targetPos.x() &&
                  rightTop.y() > t_targetPos.y())
            {
                if(rightTop != t_ignoredPos &&
                        (rightTop == t_newDefenderPos ||
                         checkField(rightTop, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                rightTop.rx() += BoardSizes::FieldWidth;
                rightTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // target in right-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF rightBottom{m_lastPos.x() + BoardSizes::FieldWidth,
                                m_lastPos.y() + BoardSizes::FieldHeight};
            while(rightBottom.x() < t_targetPos.x() &&
                  rightBottom.y() < t_targetPos.y())
            {
                if(rightBottom != t_ignoredPos &&
                        (rightBottom == t_newDefenderPos ||
                         checkField(rightBottom, this, m_scene) != FieldState::Empty)
                ) {
                      return false;
                }

                rightBottom.rx() += BoardSizes::FieldWidth;
                rightBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    return false;
}

bool Queen::canAttackField(const QPointF& t_targetPos,
                           const QPointF& t_newDefenderPos,
                           std::vector<QPointF>&& t_ignoredPos) const
{
    if(m_lastPos == t_newDefenderPos) {
        return false;
    }

    // check path to t_targetPos starting from
    // the first field between lastPos and t_targetPos

    // vertical
    if(t_targetPos.x() == m_lastPos.x()) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top = {m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight};

            while(top.y() > t_targetPos.y()) {
                if(!::contains(t_ignoredPos, top) &&
                        (top == t_newDefenderPos ||
                         checkField(top, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                top.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // bottom
        else if(t_targetPos.y() < m_lastPos.y()) {
            QPointF bottom = {m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight};

            while(bottom.y() < t_targetPos.y()) {
                if(!::contains(t_ignoredPos, bottom) &&
                        (bottom == t_newDefenderPos ||
                         checkField(bottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                bottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // horizontal
    else if(t_targetPos.y() == m_lastPos.y()) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left = {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()};

            while(left.x() > t_targetPos.x()) {
                if(!::contains(t_ignoredPos, left) &&
                        (left == t_newDefenderPos ||
                         checkField(left, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                left.rx() -= BoardSizes::FieldWidth;
            }

            return true;
        }

        // right
        else if(t_targetPos.x() > m_lastPos.x()) {
            QPointF right = {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()};

            while(right.x() < t_targetPos.x()) {
                if(!::contains(t_ignoredPos, right) &&
                        (right == t_newDefenderPos ||
                         checkField(right, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                right.rx() += BoardSizes::FieldWidth;
            }

            return true;
        }
    }

    // if not on diagonal relative to last pos
    if(std::round(std::abs(t_targetPos.x() - m_lastPos.x()) / BoardSizes::FieldWidth) !=
       std::round(std::abs(t_targetPos.y() - m_lastPos.y()) / BoardSizes::FieldHeight))
    {
        return false;
    }

    // target on left side of the board
    if(t_targetPos.x() < m_lastPos.x()) {
        // target in left-top
        if(t_targetPos.y() < m_lastPos.y()){
            QPointF leftTop{m_lastPos.x() - BoardSizes::FieldWidth,
                            m_lastPos.y() - BoardSizes::FieldHeight};
            while(leftTop.x() > t_targetPos.x() &&
                  leftTop.y() > t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, leftTop) &&
                        (leftTop == t_newDefenderPos ||
                         checkField(leftTop, this, m_scene) != FieldState::Empty)
                ) {
                   return false;
                }

                leftTop.rx() -= BoardSizes::FieldWidth;
                leftTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // left-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF leftBottom{m_lastPos.x() - BoardSizes::FieldWidth,
                               m_lastPos.y() + BoardSizes::FieldHeight};
            while(leftBottom.x() > t_targetPos.x() &&
                  leftBottom.y() < t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, leftBottom) &&
                        (leftBottom == t_newDefenderPos ||
                         checkField(leftBottom, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                leftBottom.rx() -= BoardSizes::FieldWidth;
                leftBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    // target on right side of the board
    else if(t_targetPos.x() > m_lastPos.x()) {
        // target in right-top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF rightTop{m_lastPos.x() + BoardSizes::FieldWidth,
                             m_lastPos.y() - BoardSizes::FieldHeight};
            while(rightTop.x() < t_targetPos.x() &&
                  rightTop.y() > t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, rightTop) &&
                        (rightTop == t_newDefenderPos ||
                         checkField(rightTop, this, m_scene) != FieldState::Empty)
                ) {
                    return false;
                }

                rightTop.rx() += BoardSizes::FieldWidth;
                rightTop.ry() -= BoardSizes::FieldHeight;
            }

            return true;
        }

        // target in right-bottom
        else if(t_targetPos.y() > m_lastPos.y()) {
            QPointF rightBottom{m_lastPos.x() + BoardSizes::FieldWidth,
                                m_lastPos.y() + BoardSizes::FieldHeight};
            while(rightBottom.x() < t_targetPos.x() &&
                  rightBottom.y() < t_targetPos.y())
            {
                if(!::contains(t_ignoredPos, rightBottom) &&
                        (rightBottom == t_newDefenderPos ||
                         checkField(rightBottom, this, m_scene) != FieldState::Empty)
                ) {
                      return false;
                }

                rightBottom.rx() += BoardSizes::FieldWidth;
                rightBottom.ry() += BoardSizes::FieldHeight;
            }

            return true;
        }
    }

    return false;
}

bool Queen::haveValidMoves() const noexcept {
    // left top diagonal
    {
        QPointF leftTopDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                m_lastPos.y() - BoardSizes::FieldHeight};

        while(leftTopDiagonal.x() >= 0 &&
              leftTopDiagonal.y() >= 0)
        {
            if(!m_king->inCheckAfterMove(leftTopDiagonal, m_lastPos)) {
                return true;
            }

            leftTopDiagonal.rx() -= BoardSizes::FieldWidth;
            leftTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right bottom diagonal
    {
        QPointF rightBottomDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                    m_lastPos.y() + BoardSizes::FieldHeight};

        while(rightBottomDiagonal.x() < BoardSizes::BoardWidth &&
              rightBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!m_king->inCheckAfterMove(rightBottomDiagonal, m_lastPos)) {
                return true;
            }

            rightBottomDiagonal.rx() += BoardSizes::FieldWidth;
            rightBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }
    // right top diagonal
    {
        QPointF rightTopDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                 m_lastPos.y() - BoardSizes::FieldHeight};

        while(rightTopDiagonal.x() < BoardSizes::BoardWidth &&
              rightTopDiagonal.y() >= 0)
        {
            if(!m_king->inCheckAfterMove(rightTopDiagonal, m_lastPos)) {
                return true;
            }

            rightTopDiagonal.rx() += BoardSizes::FieldWidth;
            rightTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // left bottom diagonal
    {
        QPointF leftBottomDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                   m_lastPos.y() + BoardSizes::FieldHeight};

        while(leftBottomDiagonal.x() >= 0 &&
              leftBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!m_king->inCheckAfterMove(leftBottomDiagonal, m_lastPos)) {
                return true;
            }

            leftBottomDiagonal.rx() -= BoardSizes::FieldWidth;
            leftBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }

    // top straight
    {
        QPointF topStraight{m_lastPos.x(),
                            m_lastPos.y() - BoardSizes::FieldHeight};

        while(topStraight.y() >= 0) {
            if(!m_king->inCheckAfterMove(topStraight, m_lastPos)) {
                return true;
            }

            topStraight.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right straight
    {
        QPointF rightStraight{m_lastPos.x() + BoardSizes::FieldWidth,
                              m_lastPos.y()};

        while(rightStraight.x() < BoardSizes::BoardWidth) {
            if(!m_king->inCheckAfterMove(rightStraight, m_lastPos)) {
                return true;
            }

            rightStraight.rx() += BoardSizes::FieldWidth;
        }
    }
    // bottom straight
    {
        QPointF bottomStraight{m_lastPos.x(),
                               m_lastPos.y() + BoardSizes::FieldHeight};

        while(bottomStraight.y() < BoardSizes::BoardHeight) {
            if(!m_king->inCheckAfterMove(bottomStraight, m_lastPos)) {
                return true;
            }

            bottomStraight.ry() += BoardSizes::FieldHeight;
        }
    }
    // left straight
    {
        QPointF leftStraight{m_lastPos.x() - BoardSizes::FieldWidth,
                             m_lastPos.y()};

        while(leftStraight.x() >= 0) {
            if(!m_king->inCheckAfterMove(leftStraight, m_lastPos)) {
                return true;
            }

            leftStraight.rx() -= BoardSizes::FieldWidth;
        }
    }

    return false;
}

size_t Queen::findValidMoves() noexcept {
    // left top diagonal
    {
        QPointF leftTopDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                m_lastPos.y() - BoardSizes::FieldHeight};

        while(leftTopDiagonal.x() >= 0 &&
              leftTopDiagonal.y() >= 0)
        {
            if(!validateField(leftTopDiagonal)) {
                break;
            }

            leftTopDiagonal.rx() -= BoardSizes::FieldWidth;
            leftTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right bottom diagonal
    {
        QPointF rightBottomDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                    m_lastPos.y() + BoardSizes::FieldHeight};

        while(rightBottomDiagonal.x() < BoardSizes::BoardWidth &&
              rightBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!validateField(rightBottomDiagonal)) {
                break;
            }

            rightBottomDiagonal.rx() += BoardSizes::FieldWidth;
            rightBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }
    // right top diagonal
    {
        QPointF rightTopDiagonal{m_lastPos.x() + BoardSizes::FieldWidth,
                                 m_lastPos.y() - BoardSizes::FieldHeight};

        while(rightTopDiagonal.x() < BoardSizes::BoardWidth &&
              rightTopDiagonal.y() >= 0)
        {
            if(!validateField(rightTopDiagonal)) {
                break;
            }

            rightTopDiagonal.rx() += BoardSizes::FieldWidth;
            rightTopDiagonal.ry() -= BoardSizes::FieldHeight;
        }
    }
    // left bottom diagonal
    {
        QPointF leftBottomDiagonal{m_lastPos.x() - BoardSizes::FieldWidth,
                                   m_lastPos.y() + BoardSizes::FieldHeight};

        while(leftBottomDiagonal.x() >= 0 &&
              leftBottomDiagonal.y() < BoardSizes::BoardHeight)
        {
            if(!validateField(leftBottomDiagonal)) {
                break;
            }

            leftBottomDiagonal.rx() -= BoardSizes::FieldWidth;
            leftBottomDiagonal.ry() += BoardSizes::FieldHeight;
        }
    }

    // top straight
    {
        QPointF topStraight{m_lastPos.x(),
                            m_lastPos.y() - BoardSizes::FieldHeight};

        while(topStraight.y() >= 0) {
            if(!validateField(topStraight)) {
                break;
            }

            topStraight.ry() -= BoardSizes::FieldHeight;
        }
    }
    // right straight
    {
        QPointF rightStraight{m_lastPos.x() + BoardSizes::FieldWidth,
                              m_lastPos.y()};

        while(rightStraight.x() < BoardSizes::BoardWidth) {
            if(!validateField(rightStraight)) {
                break;
            }

            rightStraight.rx() += BoardSizes::FieldWidth;
        }
    }
    // bottom straight
    {
        QPointF bottomStraight{m_lastPos.x(),
                               m_lastPos.y() + BoardSizes::FieldHeight};

        while(bottomStraight.y() < BoardSizes::BoardHeight) {
            if(!validateField(bottomStraight)) {
                break;
            }

            bottomStraight.ry() += BoardSizes::FieldHeight;
        }
    }
    // left straight
    {
        QPointF leftStraight{m_lastPos.x() - BoardSizes::FieldWidth,
                             m_lastPos.y()};

        while(leftStraight.x() >= 0) {
            if(!validateField(leftStraight)) {
                break;
            }

            leftStraight.rx() -= BoardSizes::FieldWidth;
        }
    }

    return m_moves.size();
}

bool Queen::validateField(const QPointF& t_field) noexcept {
    auto state = checkField(t_field, this, m_scene);

    if(state == FieldState::Friend ||
       state == FieldState::InvalidField
    ) {
        return false;
    }

    if(!m_king->inCheckAfterMove(t_field, m_lastPos)) {
        if(state == FieldState::Enemy) {
            addMove(new Attack(this, state.piece));
            return false;
        }
        else if (state == FieldState::Empty) {
            addMove(new Move(this, t_field));
        }
    }

    return true;
}

//

King::King(const QPixmap& t_pixMap, const QPointF& t_point,
           Player t_player, QGraphicsScene* t_scene, bool t_firstMove)
    : ChessPiece(t_pixMap, PieceType::King,
                 t_point, t_player, t_scene, t_firstMove)
{
}

bool King::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          const QPointF& t_ignoredPos) const
{
    // if target lies next to this piece return true, else false
    return std::abs(m_lastPos.x() - t_targetPos.x()) <= BoardSizes::FieldWidth &&
           std::abs(m_lastPos.y() - t_targetPos.y()) <= BoardSizes::FieldHeight;
}

bool King::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          std::vector<QPointF>&& t_ignoredPos) const
{
    // if target lies next to this piece return true, else false
    return std::abs(m_lastPos.x() - t_targetPos.x()) <= BoardSizes::FieldWidth &&
           std::abs(m_lastPos.y() - t_targetPos.y()) <= BoardSizes::FieldHeight;
}

bool King::haveValidMoves() const noexcept {
    std::array<const QPointF, 8> posToCheck{{
            // left-top
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // top
            {m_lastPos.x(),
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right-top
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y()},
            // right-bottom
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},
            // bottom
            {m_lastPos.x(),
             m_lastPos.y() + BoardSizes::FieldHeight},
            // left-bottom
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},
            // left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y()}
    }};

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Friend ||
           state == FieldState::InvalidField
        ) {
            continue;
        }

        if(!inCheckAfterMove(pos)) {
            return true;
        }
    }

    return false;
}

bool King::inCheckAfterMove(const QPointF& t_newPos,
                            const QPointF& t_oldPos) const noexcept
{
    return std::any_of(std::begin(m_enemyPieces),
                       std::end(m_enemyPieces),
                       [&](const ChessPiece* enemy) {
                           return enemy->canAttackField(m_lastPos, t_newPos, t_oldPos);
                       });
}

bool King::inCheckAfterMove(const QPointF& t_newPos,
                            std::vector<QPointF>&& t_oldPos) const noexcept
{
    return std::any_of(std::begin(m_enemyPieces),
                       std::end(m_enemyPieces),
                       [&](const ChessPiece* enemy) {
                           return enemy->canAttackField(m_lastPos, t_newPos,
                                                        std::forward<decltype(t_oldPos)>(t_oldPos));
                       });
}

size_t King::findValidMoves() noexcept {
    std::array<const QPointF, 8> posToCheck{{
            // left-top
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // top
            {m_lastPos.x(),
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right-top
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() - BoardSizes::FieldHeight},
            // right
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y()},
            // right-bottom
            {m_lastPos.x() + BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},
            // bottom
            {m_lastPos.x(),
             m_lastPos.y() + BoardSizes::FieldHeight},
            // left-bottom
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y() + BoardSizes::FieldHeight},
            // left
            {m_lastPos.x() - BoardSizes::FieldWidth,
             m_lastPos.y()}
    }};

    for(const auto& pos : posToCheck) {
        auto state = checkField(pos, this, m_scene);

        if(state == FieldState::Friend ||
           state == FieldState::InvalidField
        ) {
            continue;
        }

        if(!inCheckAfterMove(pos)) {
            if(state == FieldState::Enemy) {
                addMove(new Attack(this, state.piece));
            }
            else if (state == FieldState::Empty) {
                addMove(new Move(this, pos));
            }
        }
    }

    // castle
    if(m_firstMove && // if enemy cannot attack current pos
       std::none_of(std::begin(m_enemyPieces),
                    std::end(m_enemyPieces),
                    [&](const ChessPiece* enemy) {
                        return enemy->canAttackField(m_lastPos, {-1, -1}, {-1, -1});
                    })
    ) {
        // right
        {
            const QPointF rookPos {
              BoardSizes::BoardWidth - BoardSizes::FieldWidth,
              m_lastPos.y()
            };

            Rook* rightRook = dynamic_cast<Rook*>(checkField(rookPos, this, m_scene).piece);

            if(rightRook && rightRook->m_firstMove) {
                const QPointF rookDest{m_lastPos.x() + BoardSizes::FieldWidth,   m_lastPos.y()};
                const QPointF kingDest{m_lastPos.x() + 2*BoardSizes::FieldWidth, m_lastPos.y()};

                if(checkField(rookDest, this, m_scene) == FieldState::Empty &&
                   checkField(kingDest, this, m_scene) == FieldState::Empty &&
                   std::none_of(std::begin(m_enemyPieces),
                                std::end(m_enemyPieces),
                                [&](const ChessPiece* enemy) {
                                    return enemy->canAttackField(rookDest, {-1, -1}, m_lastPos) ||
                                           enemy->canAttackField(kingDest, {-1, -1}, m_lastPos);
                                })
                ) {
                    addMove(new Castle(this, kingDest, rightRook, rookDest));
                }
            }
        }

        // left
        {
            const QPointF rookPos{ 0, m_lastPos.y() };

            Rook* leftRook = dynamic_cast<Rook*>(checkField(rookPos, this, m_scene).piece);

            if(leftRook && leftRook->m_firstMove) {
                const QPointF rookDest{m_lastPos.x() - BoardSizes::FieldWidth,   m_lastPos.y()};
                const QPointF kingDest{m_lastPos.x() - 2*BoardSizes::FieldWidth, m_lastPos.y()};
                const QPointF emptyPos{m_lastPos.x() - 3*BoardSizes::FieldWidth, m_lastPos.y()};

                if(checkField(rookDest, this, m_scene) == FieldState::Empty &&
                   checkField(kingDest, this, m_scene) == FieldState::Empty &&
                   checkField(emptyPos, this, m_scene) == FieldState::Empty &&
                   std::none_of(std::begin(m_enemyPieces),
                                std::end(m_enemyPieces),
                                [&](const ChessPiece* enemy) {
                                    return enemy->canAttackField(rookDest, {-1, -1}, m_lastPos) ||
                                           enemy->canAttackField(kingDest, {-1, -1}, m_lastPos);
                                })
                ) {
                    addMove(new Castle(this, kingDest, leftRook, rookDest));
                }
            }
        }
    }

    return m_moves.size();
}

bool King::inCheckAfterMove(const QPointF& t_KingNewPos) const noexcept {
    return std::any_of(std::begin(m_enemyPieces),
                       std::end(m_enemyPieces),
                       [&](const ChessPiece* enemy) {
                           return enemy->canAttackField(t_KingNewPos, {-1, -1}, m_lastPos);
                       });
}

//
