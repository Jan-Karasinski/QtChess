#include "chesspiece.h"
#include "chess_namespaces.h"
#include "movements.h"
#include "paths.h"
#include "promotiondialog.h"
#include "enddialog.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <algorithm>
#include <utility>

namespace {
    template <typename T>
    typename std::enable_if<std::is_floating_point<T>::value, bool>::type
    areEqual(T&& first, T&& second) {
        return static_cast<int>(first) == static_cast<int>(second);
    }

    template<typename C, typename V>
    bool contains(C&& t_container, V&& t_value) {
        return std::end(t_container) != std::find(std::begin(t_container),
                                                  std::end(t_container),
                                                  t_value);
    }

    enum class FieldState : int {
        Empty = 0, Friend = 1, Enemy = 2, InvalidField = 3
    };

    enum class FieldColor : char {
        White = 'W', Black = 'B'
    };


    // offset to middle of piece
    const qreal offsetX{ .5*BoardSizes::FieldWidth };
    const qreal offsetY{ .5*BoardSizes::FieldHeight };

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

        fieldInfo() = default;

        fieldInfo(FieldState s)
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
           pos.y() < 0 || pos.y() >= BoardSizes::BoardHeight
        ) {
            return { FieldState::InvalidField };
        }

        // list of elements in middle of field
        auto list = scene->items({pos.x() + offsetX,
                                  pos.y() + offsetY});

        // make sure at most one piece exist on field
        Q_ASSERT_X(list.size() <= 2, "TESTcheckField",
                                     "more than 1 piece at field");

        if(list.size() == 1) { // contains only field
            return { FieldState::Empty };
        }

        auto* piece = dynamic_cast<ChessPiece*>(list.constFirst());
        if(piece) {
            if(piece->m_player == plPiece->m_player) {
                return { FieldState::Friend, piece };
            }
            else {
                return { FieldState::Enemy, piece };
            }
        }

        return { FieldState::InvalidField };
    }

    inline FieldColor getFieldColor(QPoint t_pos) {
        if(t_pos.x() % 2 == 0) {
            if(t_pos.y() % 2 == 0) {
                return FieldColor::White;
            }
            else {
                return FieldColor::Black;
            }
        }
        else {
            if(t_pos.y() % 2 == 0) {
                return FieldColor::Black;
            }
            else {
                return FieldColor::White;
            }
        }
    }

    QPointF getCenteredPos(const QPointF& pos) noexcept {
        return { pos.x() + offsetX - std::fmod(pos.x() + offsetX, BoardSizes::FieldWidth),
                 pos.y() + offsetY - std::fmod(pos.y() + offsetY, BoardSizes::FieldHeight) };
    }
}

ChessPiece::ChessPiece(const QPixmap&  t_pixMap,
                       PieceType       t_type,
                       const QPointF&  t_point,
                       Player          t_player,
                       QGraphicsScene* t_scene,
                       bool            t_firstMove) noexcept
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

ChessPiece* ChessPiece::Create(const QPixmap&  t_pixMap,
                               PieceType       t_type,
                               const QPointF&  t_point,
                               Player          t_player,
                               QGraphicsScene* t_scene,
                               bool            t_firstMove) noexcept
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

ChessPiece* ChessPiece::Create(PieceType       t_type,
                               const QPointF&  t_point,
                               Player          t_player,
                               QGraphicsScene* t_scene,
                               bool            t_firstMove) noexcept
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

        const QPointF piecePos = getCenteredPos(pos());

        auto move = std::find(std::begin(m_moves), std::end(m_moves), piecePos);

        if(move != std::end(m_moves)) {
            m_firstMove = false;

            for(ChessPiece* piece : m_enemyPieces) {
                if(piece->m_type == PieceType::Pawn) {
                    static_cast<Pawn*>(piece)->m_enPassant = false;
                }
            }

            // prevents next clicked piece from jumping
            // to top-left corner after promotion
            auto moveType = (*move)->m_type;
            if(moveType == MoveType::PromotionAttack ||
               moveType == MoveType::PromotionMove
            ) {
                QGraphicsPixmapItem::mouseReleaseEvent(t_event);
            }

            (*move)->exec(); // if promotion - pawn gets deleted

            auto status = isGameOver();
            if(status.first != WinCondition::Continue) {
                ChessPiece::endGame(status);
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

std::pair<WinCondition, Player> ChessPiece::isGameOver() const noexcept {
    if(GameStatus::uselessMoves >= 100) {
        return { WinCondition::FiftyMoves, m_player };
    }

    // functors for better readability
    const auto& friendlyPieces = [&] {
        if(m_player == Player::White) {
            return GameStatus::White::pieces;
        }
        else {
            return GameStatus::Black::pieces;
        }
    }();

    const auto& enemyKing = [&] {
        if(m_player == Player::White) {
            return GameStatus::Black::king;
        }
        else return GameStatus::White::king;
    }();

    auto kingInCheck = [](const King* king) {
        const std::vector<ChessPiece*>& vec =
            king->m_player == Player::White ?
                GameStatus::Black::pieces :
                GameStatus::White::pieces;

        return std::any_of(std::begin(vec),
                           std::end(vec),
                           [&](const ChessPiece* enemy) {
                               return enemy->canAttackField(king->m_lastPos);
                           });
    };

    auto canMove = [](const std::vector<ChessPiece*>& vec) {
        return std::any_of(std::begin(vec),
                           std::end(vec),
                           [](const ChessPiece* friendly) {
                               // king excluded, as he's not able to mate
                               if(friendly->m_type == PieceType::King) {
                                   return false;
                               }
                               return friendly->haveValidMoves();
                           });
    };
    //

    if(!canMove(friendlyPieces) &&
       !m_king->haveValidMoves()
    ) {
        return { WinCondition::Stalemate, enemyKing->m_player };
    }
    else if(!canMove(m_enemyPieces) &&
            !enemyKing->haveValidMoves()
    ) {
        if(kingInCheck(enemyKing)) { // it's not possible to protect the king
            return { WinCondition::Checkmate, m_player };
        }
        else { // player not able to move, so game ends
            return { WinCondition::Stalemate, m_player };
        }
    }

    auto draw = [&]() {
        // draw conditions:
        // case       one side(friend)         other side(enemy)
        //  1           king                       king
        //
        //  2       king + knight                  king
        //
        //  3       king + n * bishop              king
        //         n > 0 if bishops
        //      have the same field color
        //
        //  4       king + n * bishop          king + m * bishop
        //         n > 0 if bishops           m > 0 if bishops
        //      have the same field color   have the same field color
        //
        // cases 1, 2
        if(m_enemyPieces.size() == 1) { // only king
            // case 1
            if(friendlyPieces.size() == 1) { // only king
                return true;
            }
            // case 2
            else if(friendlyPieces.size() == 2) {
                if(friendlyPieces[0]->m_type == PieceType::Knight ||
                   friendlyPieces[1]->m_type == PieceType::Knight
                ) {
                    return true;
                }
            }
        }

        //
        auto containsOnly = [](const std::vector<ChessPiece*>& vec,
                                PieceType t_type)
        {
            return std::all_of(std::begin(vec),
                               std::end(vec),
                               [&](ChessPiece* piece) {
                                   return piece->m_type == t_type ||
                                          piece->m_type == PieceType::King;
                               });
        };

        auto allHaveSameFieldColor = [](const std::vector<ChessPiece*>& vec) {
            auto firstBishopIT{
                std::find_if(std::begin(vec),
                             std::end(vec),
                             [&](const ChessPiece* piece) {
                                 return piece->m_type == PieceType::Bishop;
                             })
            };

            Q_ASSERT_X(firstBishopIT != std::end(vec),
                       "isGameOver - draw - containsOnly",
                       "Couldn't find a piece that should exist");

            FieldColor color {
                getFieldColor((*firstBishopIT)->m_lastPos.toPoint())
            };

            return
            std::all_of(
                std::begin(vec),
                std::end(vec),
                [&](const ChessPiece* piece) {
                    return piece->m_type == PieceType::King ||
                        getFieldColor(piece->m_lastPos.toPoint()) == color;
                });
        };
        //

        // cases 3, 4
        if(containsOnly(friendlyPieces, PieceType::Bishop) &&
           allHaveSameFieldColor(friendlyPieces)
        ) {
            // case 3
            if(m_enemyPieces.size() == 1) { // only king
                return true;
            }
            // case 4
            if(containsOnly(m_enemyPieces, PieceType::Bishop) &&
               allHaveSameFieldColor(m_enemyPieces)
            ) {
                return true;
            }
        }

        return false;
    };
    if(draw()) {
        return { WinCondition::Draw, m_player }; // player ignored
    }

    return { WinCondition::Continue, m_player }; // player ignored
}

void ChessPiece::endGame(std::pair<WinCondition, Player> t_state) noexcept {
    for(auto* piece : GameStatus::White::pieces) {
        piece->setEnabled(false);
    }

    for(auto* piece : GameStatus::Black::pieces) {
        piece->setEnabled(false);
    }

    EndDialog dialog{ t_state };
    dialog.exec();
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

inline void ChessPiece::addMove(Movement* t_move) {
    ChessPiece::m_moves.emplace_back(t_move);
}

//

Pawn::Pawn(const QPixmap&  t_pixMap,
           const QPointF&  t_point,
           Player          t_player,
           QGraphicsScene* t_scene,
           bool            t_firstMove)
    : ChessPiece(t_pixMap,
                 PieceType::Pawn,
                 t_point,
                 t_player,
                 t_scene,
                 t_firstMove)
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
    const auto direction = m_player == Player::White ? -BoardSizes::FieldHeight :
                                                        BoardSizes::FieldHeight;

    // middle, move only
    {
        const QPointF middle  {m_lastPos.x(), m_lastPos.y() + direction};
        auto state = checkField(middle, this, m_scene);

        if(state == FieldState::Empty &&
           std::none_of(std::begin(m_enemyPieces),
                        std::end(m_enemyPieces),
                        [&](const ChessPiece* enemy) {
                            if(enemy->m_lastPos == middle)
                                return true;
                            return enemy->canAttackField(m_king->m_lastPos,
                                                         middle,
                                                         m_lastPos);
                        })
        ) {
            return true;
        }
    }

    // ordinary attack
    {
        const std::array<QPointF, 2> ordinaryAttack{{
            {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction}, // left
            {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction}  // right
        }};

        for(const auto& point : ordinaryAttack) {
            auto state = checkField(point, this, m_scene);

            if(state == FieldState::Enemy &&
               !m_king->inCheckAfterMove(point, m_lastPos)
            ) {
                return true;
            }
        }
    }

    // en passant
    {
        const std::array<std::array<QPointF, 2>, 2> epMoves{{
            {{
                {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()}, // left ep attack
                {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction} // left ep dest
            }},
            {{
                {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()}, // right ep attack
                {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction} // right ep dest
            }}
        }};

        for(const auto& points : epMoves) {
            auto attackedPosStatus    = checkField(points[0], this, m_scene); // pos of enemy
            auto destinationPosStatus = checkField(points[1], this, m_scene);

            if(destinationPosStatus == FieldState::Empty &&
               attackedPosStatus == FieldState::Enemy &&
               attackedPosStatus.piece->m_type == PieceType::Pawn &&
               static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
               !m_king->inCheckAfterMove(points[1], {m_lastPos, points[0]})
            ) {
                return true;
            }
        }
    }

    return false;
}

size_t Pawn::findValidMoves() noexcept {
    const qreal direction = m_player == Player::White ? -BoardSizes::FieldHeight :
                                                        BoardSizes::FieldHeight;

    // middle, move only
    {
        const QPointF middle { m_lastPos.x(), m_lastPos.y() + direction },
                secondMiddle { m_lastPos.x(), m_lastPos.y() + 2*direction };
        if(checkField(middle, this, m_scene) == FieldState::Empty &&
           !m_king->inCheckAfterMove(middle, m_lastPos)
        ) {
            // if last field, save as promotion
            if(middle.y() <  BoardSizes::FieldHeight ||
               middle.y() >= BoardSizes::BoardHeight - BoardSizes::FieldHeight
            ) {
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

    // ordinary attack
    {
        const std::array<QPointF, 2> ordinaryAttack{{
            {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction}, // left
            {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction}  // right
        }};

        for(const auto& point : ordinaryAttack) {
            auto state = checkField(point, this, m_scene);

            if(state == FieldState::Enemy &&
               !m_king->inCheckAfterMove(point, m_lastPos)
            ) {
                if(point.y() <  BoardSizes::FieldHeight ||
                   point.y() >= BoardSizes::BoardHeight - BoardSizes::FieldHeight
                ) {
                    addMove(new PromotionAttack(this, state.piece));
                }
                else {
                    addMove(new Attack(this, state.piece));
                }
            }
        }
    }

    // en passant
    {
        const std::array<std::array<QPointF, 2>, 2> epMoves{{
            {{
                {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y()}, // left ep attack
                {m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() + direction} // left ep dest
            }},
            {{
                {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y()}, // right ep attack
                {m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() + direction} // right ep dest
            }}
        }};

        for(const auto& points : epMoves) {
            auto attackedPosStatus    = checkField(points[0], this, m_scene); // pos of enemy
            auto destinationPosStatus = checkField(points[1], this, m_scene);

            if(destinationPosStatus == FieldState::Empty &&
               attackedPosStatus == FieldState::Enemy &&
               attackedPosStatus.piece->m_type == PieceType::Pawn &&
               static_cast<Pawn*>(attackedPosStatus.piece)->m_enPassant &&
               !m_king->inCheckAfterMove(points[1], {m_lastPos, points[0]})
            ) {
                addMove(new EnPassantAttack(this, attackedPosStatus.piece, points[1]));
            }
        }
    }

    return m_moves.size();
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

    // scene no longer owns this piece
    GameStatus::promotedPieces.emplace_back(this);
    m_scene->removeItem(this);

    m_scene->addItem(newPiece);
    pieces.push_back(newPiece);
}

//

Knight::Knight(const QPixmap&  t_pixMap,
               const QPointF&  t_point,
               Player          t_player,
               QGraphicsScene* t_scene,
               bool            t_firstMove)
        : ChessPiece(t_pixMap,
                     PieceType::Knight,
                     t_point,
                     t_player,
                     t_scene,
                     t_firstMove)
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
    const std::array<QPointF, 8> posToCheck{{
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
           state == FieldState::Empty
        ) {
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
    const std::array<QPointF, 8> posToCheck{{
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
           state == FieldState::Empty
        ) {
            return true;
        }
    }

    return false;
}

bool Knight::haveValidMoves() const noexcept {
    const std::array<QPointF, 8> posToCheck{{
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
            return true;
        }
    }

    return false;
}

size_t Knight::findValidMoves() noexcept {
    const std::array<QPointF, 8> posToCheck{{
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

Bishop::Bishop(const QPixmap&  t_pixMap,
               const QPointF&  t_point,
               Player          t_player,
               QGraphicsScene* t_scene,
               bool            t_firstMove)
    : ChessPiece(t_pixMap,
                 PieceType::Bishop,
                 t_point,
                 t_player,
                 t_scene,
                 t_firstMove)
{
}

bool Bishop::canAttackField(const QPointF& t_targetPos,
                            const QPointF& t_newDefenderPos,
                            const QPointF& t_ignoredPos) const
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
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF leftTop{m_lastPos.x() - BoardSizes::FieldWidth,
                            m_lastPos.y() - BoardSizes::FieldHeight};
            while(leftTop.x() > t_targetPos.x() &&
                  leftTop.y() > t_targetPos.y()
            ) {
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
                  leftBottom.y() < t_targetPos.y()
            ) {
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
                  rightTop.y() > t_targetPos.y()
            ) {
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
                  rightBottom.y() < t_targetPos.y()
            ) {
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
                  leftTop.y() > t_targetPos.y()
            ) {
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
                  leftBottom.y() < t_targetPos.y()
            ) {
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
                  rightTop.y() > t_targetPos.y()
            ) {
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
                  rightBottom.y() < t_targetPos.y()
            ) {
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
              leftTopDiagonal.y() >= 0
        ) {
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
              rightBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              rightTopDiagonal.y() >= 0
        ) {
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
              leftBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              leftTopDiagonal.y() >= 0
        ) {
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
              rightBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              rightTopDiagonal.y() >= 0
        ) {
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
              leftBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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

Rook::Rook(const QPixmap&  t_pixMap,
           const QPointF&  t_point,
           Player          t_player,
           QGraphicsScene* t_scene,
           bool            t_firstMove)
    : ChessPiece(t_pixMap,
                 PieceType::Rook,
                 t_point,
                 t_player,
                 t_scene,
                 t_firstMove)
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
    if(areEqual(t_targetPos.x(), m_lastPos.x())) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top{ m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight };

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
            QPointF bottom{ m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight };

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
    else if(areEqual(t_targetPos.y(), m_lastPos.y())) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left{ m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() };

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
            QPointF right{ m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() };

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
    if(areEqual(t_targetPos.x(), m_lastPos.x())) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top{ m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight };

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
            QPointF bottom{ m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight };

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
    else if(areEqual(t_targetPos.y(), m_lastPos.y())) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left{ m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() };

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
            QPointF right{ m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() };

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

Queen::Queen(const QPixmap&  t_pixMap,
             const QPointF&  t_point,
             Player          t_player,
             QGraphicsScene* t_scene,
             bool            t_firstMove)
    : ChessPiece(t_pixMap,
                 PieceType::Queen,
                 t_point,
                 t_player,
                 t_scene,
                 t_firstMove)
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
    if(areEqual(t_targetPos.x(), m_lastPos.x())) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top{ m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight };

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
            QPointF bottom{ m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight };

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
    else if(areEqual(t_targetPos.y(), m_lastPos.y())) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left{ m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() };

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
            QPointF right{ m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() };

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
                  leftTop.y() > t_targetPos.y()
            ) {
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
                  leftBottom.y() < t_targetPos.y()
            ) {
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
                  rightTop.y() > t_targetPos.y()
            ) {
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
                  rightBottom.y() < t_targetPos.y()
            ) {
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
    if(areEqual(t_targetPos.x(), m_lastPos.x())) {
        // top
        if(t_targetPos.y() < m_lastPos.y()) {
            QPointF top{ m_lastPos.x(), m_lastPos.y() - BoardSizes::FieldHeight };

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
            QPointF bottom{ m_lastPos.x(), m_lastPos.y() + BoardSizes::FieldHeight };

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
    else if(areEqual(t_targetPos.y(), m_lastPos.y())) {
        // left
        if(t_targetPos.x() < m_lastPos.x()) {
            QPointF left{ m_lastPos.x() - BoardSizes::FieldWidth, m_lastPos.y() };

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
            QPointF right{ m_lastPos.x() + BoardSizes::FieldWidth, m_lastPos.y() };

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
                  leftTop.y() > t_targetPos.y()
            ) {
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
                  leftBottom.y() < t_targetPos.y()
            ) {
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
                  rightTop.y() > t_targetPos.y()
            ) {
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
                  rightBottom.y() < t_targetPos.y()
            ) {
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
              leftTopDiagonal.y() >= 0
        ) {
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
              rightBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              rightTopDiagonal.y() >= 0
        ) {
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
              leftBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              leftTopDiagonal.y() >= 0
        ) {
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
              rightBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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
              rightTopDiagonal.y() >= 0
        ) {
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
              leftBottomDiagonal.y() < BoardSizes::BoardHeight
        ) {
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

King::King(const QPixmap&  t_pixMap,
           const QPointF&  t_point,
           Player          t_player,
           QGraphicsScene* t_scene,
           bool            t_firstMove)
    : ChessPiece(t_pixMap,
                 PieceType::King,
                 t_point,
                 t_player,
                 t_scene,
                 t_firstMove)
{
}

bool King::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          const QPointF& t_ignoredPos) const
{
    // if target is next to this piece return true, else false
    bool isInRange = [&] {
        const int xDistance{
            static_cast<int>(std::abs(m_lastPos.x() - t_targetPos.x()))
        };
        const int yDistance{
            static_cast<int>(std::abs(m_lastPos.y() - t_targetPos.y()))
        };

        bool inXRange{ xDistance <= static_cast<int>(BoardSizes::FieldWidth) };
        bool inYRange{ yDistance <= static_cast<int>(BoardSizes::FieldHeight) };

        return inXRange && inYRange &&
               (xDistance != 0 || yDistance != 0);
    }();
    return isInRange;
}

bool King::canAttackField(const QPointF& t_targetPos,
                          const QPointF& t_newDefenderPos,
                          std::vector<QPointF>&& t_ignoredPos) const
{
    // if target is next to this piece return true, else false
    bool isInRange = [&] {
        const int xDistance{
            static_cast<int>(std::abs(m_lastPos.x() - t_targetPos.x()))
        };
        const int yDistance{
            static_cast<int>(std::abs(m_lastPos.y() - t_targetPos.y()))
        };

        bool inXRange{ xDistance <= static_cast<int>(BoardSizes::FieldWidth) };
        bool inYRange{ yDistance <= static_cast<int>(BoardSizes::FieldHeight) };

        return inXRange && inYRange &&
               (xDistance != 0 || yDistance != 0);
    }();
    return isInRange;
}

bool King::inCheckAfterMove(const QPointF& t_newPos,
                            const QPointF& t_oldPos) const noexcept
{
    return std::any_of(std::begin(m_enemyPieces),
                       std::end(m_enemyPieces),
                       [&](const ChessPiece* enemy) {
                           if(enemy->m_lastPos == t_newPos)
                               return false;
                           return enemy->canAttackField(m_lastPos, t_newPos, t_oldPos);
                       });
}

bool King::inCheckAfterMove(const QPointF& t_newPos,
                            std::vector<QPointF>&& t_oldPos) const noexcept
{
    return std::any_of(std::begin(m_enemyPieces),
                       std::end(m_enemyPieces),
                       [&](const ChessPiece* enemy) {
                           if(enemy->m_lastPos == t_newPos)
                               return false;
                           return enemy->canAttackField(m_lastPos,
                                                        t_newPos,
                       std::forward<decltype(t_oldPos)>(t_oldPos));
                       });
}

bool King::haveValidMoves() const noexcept {
    const std::array<QPointF, 8> posToCheck{{
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

size_t King::findValidMoves() noexcept {
    const std::array<QPointF, 8> posToCheck{{
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
