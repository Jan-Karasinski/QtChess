#include "chesspiece.h"
#include "mainwindow.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QLibraryInfo>
#include <QGraphicsScene>
#include <algorithm>

enum class FieldState : int
{
    Empty = 0, Friend = 1, Enemy = 2
};

struct fieldInfo
{
    // 0 = empty
    // 1 = friend
    // 2 = enemy
    FieldState state = FieldState::Empty;

    ChessPiece* enemy = nullptr;

    // usual validation
    bool resolve(QGraphicsScene* scene) {
        switch (state) {
            case FieldState::Enemy: {
                scene->removeItem(enemy);
            }
            case FieldState::Empty: {
                return true;
            }
            default: {
                return false;
            }
        }
    }

    bool operator ==(FieldState i) const noexcept{
        return state == i;
    }

    bool operator !=(FieldState i) const noexcept{
        return state != i;
    }

    fieldInfo(FieldState s)
        : state(s)
    {
    }

    fieldInfo(FieldState s, ChessPiece* p)
        : state(s), enemy(p)
    {
    }

};

bool isEqual(qreal a, qreal b) {
    return std::abs(a-b) <= std::abs(std::min(a, b)) * std::numeric_limits<double>::epsilon();
}

bool lessEqual(qreal a, qreal b) {
    return a-b <= 0.0 * std::numeric_limits<double>::epsilon();
}

bool greaterEqual(qreal a, qreal b) {
    return a-b >= 0.0 * std::numeric_limits<double>::epsilon();
}

bool lessThan(qreal a, qreal b) {
    return a-b < 0.0 * std::numeric_limits<double>::epsilon();
}

bool greaterThan(qreal a, qreal b) {
    return a-b > 0.0 * std::numeric_limits<double>::epsilon();
}

template<typename T>
fieldInfo checkField(QList<T> list, ChessPiece* plPiece) {
    list.removeOne(list.last());
    list.removeOne(plPiece);

    // make sure at most one piece exist on field
    Q_ASSERT_X(list.size() < 2, "checkField", "more than 1 piece at field");

    if(list.size()) {
        ChessPiece* piece = dynamic_cast<ChessPiece*>(list.first());
        if(!piece) {
            return {FieldState::Empty};
        }
        else if(piece->player == plPiece->player) {
            return {FieldState::Friend};
        }
        else {
            return {FieldState::Enemy, piece};
        }
    }
    return {FieldState::Empty};
}

ChessPiece* canAttack( QGraphicsItem* field, ChessPiece* plPiece, const QGraphicsScene* scene) {
    QPointF rect = {field->pos().x() + 0.5*BoardSizes::FieldWidth,
                    field->pos().y() + 0.5*BoardSizes::FieldHeight};
    auto list = scene->items(rect, Qt::ContainsItemBoundingRect);
    list.removeOne(field);
    list.removeOne(plPiece);
#if 0
    auto piece = std::find_if(std::begin(list), std::end(list), [player](QGraphicsItem* item) {
            ChessPiece* piece = dynamic_cast<ChessPiece*>(item);
            if(piece && piece->player != player){
                return true;
            }
            return false;
    });
#endif
    ChessPiece* piece = nullptr;
    for(auto* item : list) {
        piece = dynamic_cast<ChessPiece*>(item);
        if(piece && piece->player != plPiece->player){
            break;
        }
    }
    if(piece) {
        return dynamic_cast<ChessPiece*>(piece);
    }
    return nullptr;
    //return piece == std::end(list) ? nullptr : dynamic_cast<ChessPiece*>(*piece);
    // return pointer to valid piece(for delete after attack) or nullptr
}

ChessPiece::ChessPiece(const QPointF& point, const QPixmap& pmap, PieceType t, Player p, QGraphicsScene* s)
    : QGraphicsPixmapItem(pmap),
      lastPos(point),
      type(t),
      player(p),
      scene(s)
{
    setPos(point);
    setFlag(QGraphicsItem::ItemIsMovable);
}

ChessPiece* ChessPiece::Create(PieceType t, const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s) {
    switch (t) {
        case PieceType::Pawn:
            return new Pawn(point, pmap, p, s);

        case PieceType::Knight:
            return new Knight(point, pmap, p, s);

        case PieceType::Bishop:
            return new Bishop(point, pmap, p, s);

        case PieceType::Rook:
            return new Rook(point, pmap, p, s);

        case PieceType::Queen:
            return new Queen(point, pmap, p, s);

        case PieceType::King:
            return new King(point, pmap, p, s);

        default:
            return nullptr;
    }
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }
        setZValue(zValue() + 1);
        highlight();
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        dehighlight();

        if(!goodMove()) {
            setPos(lastPos);
        }
        else {
            setPos(scene->items(
                        {pos().x() + 0.5*BoardSizes::FieldWidth,
                         pos().y() + 0.5*BoardSizes::FieldHeight}
                        ).last()->pos());
            lastPos = pos();
            GameStatus::currentPlayer = player == Player::White ? Player::Black : Player::White;
        }
        setZValue(zValue() - 1);
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void ChessPiece::dehighlight() {
    QPair<QGraphicsRectItem*, QBrush> pair;
    while (!GameStatus::highlighted.empty()) {
        pair = GameStatus::highlighted.dequeue();
        pair.first->setBrush(pair.second);
    }
}

Pawn::Pawn(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Pawn, p, s)
{
}

void Pawn::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }

        setZValue(zValue() + 1);
        highlight();
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void Pawn::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        dehighlight();

        if(!goodMove()) {
            setPos(lastPos);
        }
        else {
            setPos(scene->items(
                        {pos().x() + 0.5*BoardSizes::FieldWidth,
                         pos().y() + 0.5*BoardSizes::FieldHeight}
                        ).last()->pos());
            lastPos = pos();
            firstMove = 0;
            GameStatus::currentPlayer = player == Player::White ? Player::Black : Player::White;
        }
        setZValue(zValue() - 1);
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void Pawn::highlight() {
    if(!firstMove) {
        if(player == Player::White) {
            // field in front and not top row
            if(lastPos.y() <  BoardSizes::FieldHeight ||
               lessThan(lastPos.y(), BoardSizes::FieldHeight))
            {
                return;
            }

            // left top, attack-only
            if(lastPos.x() > 0 ||
               greaterThan(lastPos.x(), 0))
            {
                auto list = scene->items(
                                {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // middle top, move-only
            if(lastPos.y() > 0 ||
               greaterThan(lastPos.y(), 0))
            {
                auto list = scene->items(
                                {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Empty) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                auto list = scene->items(
                                {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }
        }
        else {
            // field in front and not last row
            if(lastPos.y() >=  BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
               greaterEqual(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                return;
            }

            // left bottom, attack-only
            if(lastPos.x() > 0 ||
               greaterThan(lastPos.x(), 0))
            {
                auto list = scene->items(
                                {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // middle bottom, move-only
            if(lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
               lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                auto list = scene->items(
                                {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Empty) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                auto list = scene->items(
                                {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }
        }
    }
    else {
        if(player == Player::White) {
            // field in front and not top row
            if(lastPos.y() <  BoardSizes::FieldHeight ||
               lessThan(lastPos.y(), BoardSizes::FieldHeight))
            {
                return;
            }

            // middle top, move-only
            {
                auto list = scene->items(
                                {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Empty) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);

                    // second in front
                    auto list = scene->items(
                                    {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 1.5*BoardSizes::FieldHeight},
                                     Qt::ContainsItemBoundingRect
                                    );
                    fieldInfo state = checkField(list, this);

                    if(state == FieldState::Empty) {
                        field = static_cast<QGraphicsRectItem*>(list.last());

                        GameStatus::highlighted.push_back({field, field->brush()});
                        field->setBrush(BoardBrush::Highlight);
                    }
                }
            }

            // left top, attack-only
            if(lastPos.x() > 0 ||
               greaterThan(lastPos.x(), 0))
            {
                auto list = scene->items(
                                {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                auto list = scene->items(
                                {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                 lastPos.y() - 0.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

        }
        else {
            // field in front and not last row
            if(lastPos.y() >=  BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
               greaterEqual(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                return;
            }

            // middle bottom, move-only
            {
                auto list = scene->items(
                                {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Empty) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);

                    // second in front
                    auto list = scene->items(
                                    {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 2.5*BoardSizes::FieldHeight},
                                     Qt::ContainsItemBoundingRect
                                    );
                    fieldInfo state = checkField(list, this);

                    if(state == FieldState::Empty) {
                        field = static_cast<QGraphicsRectItem*>(list.last());

                        GameStatus::highlighted.push_back({field, field->brush()});
                        field->setBrush(BoardBrush::Highlight);
                    }
                }
            }

            // left bottom, attack-only
            if(lastPos.x() > 0 ||
               greaterThan(lastPos.x(), 0))
            {
                auto list = scene->items(
                                {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                auto list = scene->items(
                                {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                 lastPos.y() + 1.5*BoardSizes::FieldHeight},
                                 Qt::ContainsItemBoundingRect
                                );
                fieldInfo state = checkField(list, this);

                if(state == FieldState::Enemy) {
                    QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }
        }
    }
}

bool Pawn::goodMove() {
    const QPointF& newPos = scene->items({pos().x() + 0.5*BoardSizes::FieldWidth,
                                          pos().y() + 0.5*BoardSizes::FieldHeight
                                        }).last()->pos();

    if(newPos.x() > BoardSizes::BoardWidth  ||
       newPos.y() > BoardSizes::BoardHeight ||
       newPos.x() < 0 ||
       newPos.y() < 0)
    {
        return false;
    }

    if(!firstMove){
        if(player == Player::White) {
            if(GameStatus::whiteKing->inDanger()) {
                return false;
            }

            // if not in front of
            if(newPos.y() != lastPos.y() - BoardSizes::FieldHeight ||
               !isEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight))
            {
                return false;
            }

            // move forward
            if(newPos.x() == lastPos.x() ||
               isEqual(newPos.x(), lastPos.x()))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Empty) {
                    return true;
                }

            }
            // move to left
            else if(newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }
            // move to right
            else if(newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }
        }
        else {
            if(GameStatus::blackKing->inDanger()) {
                return false;
            }

            // if not in front of???
            if(newPos.y() != lastPos.y() + BoardSizes::FieldHeight ||
               !isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight))
            {
                return false;
            }

            // move forward
            if(newPos.x() == lastPos.x() ||
               isEqual(newPos.x(), lastPos.x()))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Empty) {
                    return true;
                }

            }
            // move to left
            else if(newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }
            // move to right
            else if(newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }
        }
    }
    else {
        if(player == Player::White) {
            // move to left
            if((newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth))  &&
               (newPos.y() == lastPos.y() - BoardSizes::FieldHeight ||
                isEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight)))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }

            // move to right
            else if((newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                     isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth)) &&
                    (newPos.y() == lastPos.y() - BoardSizes::FieldHeight ||
                     isEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight)))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }

            else if(newPos.x() == lastPos.x() ||
                    isEqual(newPos.x(), lastPos.x()))
            {
                // first in front
                if(newPos.y() == lastPos.y() - BoardSizes::FieldHeight ||
                   isEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight))
                {
                    fieldInfo field = checkField(scene->items(
                                        {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                         lastPos.y() - 0.5*BoardSizes::FieldHeight}
                                        ), this);
                    if(field.state == FieldState::Empty) {
                            return true;
                    }
                }

                // second in front
                else if(newPos.y() == lastPos.y() - 2*BoardSizes::FieldHeight ||
                        isEqual(newPos.y(), lastPos.y() - 2*BoardSizes::FieldHeight))
                {
                    fieldInfo field = checkField(scene->items(
                                        {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                         lastPos.y() - 1.5*BoardSizes::FieldHeight}
                                        ), this);
                    if(field.state == FieldState::Empty) {
                            return true;
                    }
                }
            }
        }
        else {
            // move to left
            if((newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth)) &&
               (newPos.y() == lastPos.y() + BoardSizes::FieldHeight ||
                isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight)))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }
            // move to right
            else if((newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                     isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth)) &&
                    (newPos.y() == lastPos.y() + BoardSizes::FieldHeight ||
                     isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight)))
            {
                fieldInfo field = checkField(scene->items(
                                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                    ), this);
                if(field.state == FieldState::Enemy) {
                        scene->removeItem(field.enemy);
                        return true;
                }
            }

            else if(newPos.x() == lastPos.x() ||
                    isEqual(newPos.x(), lastPos.x()))
            {
                // first in front
                if(newPos.y() == lastPos.y() + BoardSizes::FieldHeight ||
                   isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight))
                {
                    fieldInfo field = checkField(scene->items(
                                        {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                         lastPos.y() + 1.5*BoardSizes::FieldHeight}
                                        ), this);
                    if(field.state == FieldState::Empty) {
                            return true;
                    }
                }

                // second in front
                else if(newPos.y() == lastPos.y() + 2*BoardSizes::FieldHeight ||
                        isEqual(newPos.y(), lastPos.y() + 2*BoardSizes::FieldHeight))
                {
                    fieldInfo field = checkField(scene->items(
                                        {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                                         lastPos.y() + 2.5*BoardSizes::FieldHeight}
                                        ), this);
                    if(field.state == FieldState::Empty) {
                            return true;
                    }
                }
            }
        }
    }

    return false;
}

Knight::Knight(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Knight, p, s)
{
}

void Knight::highlight() {
    // left & top
    if((lastPos.x() >= 2*BoardSizes::FieldWidth ||
        greaterEqual(lastPos.x(), 2*BoardSizes::FieldWidth)) &&
       (lastPos.y() > 0 ||
        greaterThan(lastPos.y(), 0)))
    {
        auto list = scene->items(
                            {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                             lastPos.y() - 0.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // left & bottom
    if((lastPos.x() >= 2*BoardSizes::FieldWidth ||
        greaterEqual(lastPos.x(), 2*BoardSizes::FieldWidth)) &&
       (lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
        lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                             lastPos.y() + 1.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // top & left
    if((lastPos.x() >= BoardSizes::FieldWidth ||
        greaterEqual(lastPos.x(), BoardSizes::FieldWidth)) &&
       (lastPos.y() >= 2*BoardSizes::FieldHeight ||
        greaterEqual(lastPos.y(), 2*BoardSizes::FieldWidth)) )
    {
        auto list = scene->items(
                            {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                             lastPos.y() - 1.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // top & right
    if((lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
        lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth)) &&
       (lastPos.y() >= 2*BoardSizes::FieldHeight ||
        greaterEqual(lastPos.y(), 2*BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                             lastPos.y() - 1.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // right & top
    if((lastPos.x() < BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth ||
        lessThan(lastPos.x(), BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth)) &&
       (lastPos.y() >= BoardSizes::FieldHeight ||
        greaterEqual(lastPos.y(), BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                             lastPos.y() - 0.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // right & bottom
    if((lastPos.x() < BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth ||
        lessThan(lastPos.x(), BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth)) &&
       (lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
        lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                             lastPos.y() + 1.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // bottom & left
    if((lastPos.x() >= BoardSizes::FieldWidth ||
        greaterEqual(lastPos.x(), lastPos.x() - BoardSizes::FieldWidth)) &&
       (lastPos.y() < BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight ||
        lessThan(lastPos.y(), BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                             lastPos.y() + 2.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // bottom & right
    if((lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
        lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth)) &&
       (lastPos.y() < BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight ||
        lessThan(lastPos.y(), BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight)))
    {
        auto list = scene->items(
                            {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                             lastPos.y() + 2.5*BoardSizes::FieldHeight}
                            );
        auto state =  checkField(list, this);
        if(state == FieldState::Empty || state == FieldState::Enemy) {
            QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

}

bool Knight::goodMove() {
    if(player == Player::White && GameStatus::whiteKing->inDanger()) {
        return false;
    }
    else if(GameStatus::blackKing->inDanger()){
        return false;
    }

    const QPointF& newPos = scene->items({pos().x() + 0.5*BoardSizes::FieldWidth,
                                          pos().y() + 0.5*BoardSizes::FieldHeight
                                        }).last()->pos();
    // left & top
    if((newPos.x() == lastPos.x() - 2*BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() - 2*BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() - BoardSizes::FieldHeight ||
        isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth)))
    {
        return checkField(scene->items(
                    {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // left & bottom
    if((newPos.x() == lastPos.x() - 2*BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() - 2*BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() + BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // top & left
    if((newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() - 2*BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() - 2*BoardSizes::FieldWidth)) )
    {
        return checkField(scene->items(
                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                     lastPos.y() - 1.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // top & right
    if((newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() - 2*BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() - 2*BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                     lastPos.y() - 1.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // right & top
    if((newPos.x() == lastPos.x() + 2*BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() + 2*BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() - BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // right & bottom
    if((newPos.x() == lastPos.x() + 2*BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() + 2*BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() + BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // bottom & left
    if((newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() + 2*BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() + 2*BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                     lastPos.y() + 2.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    // bottom & right
    if((newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
        isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth)) &&
       (newPos.y() == lastPos.y() + 2*BoardSizes::FieldHeight ||
        isEqual(newPos.y(), lastPos.y() + 2*BoardSizes::FieldHeight)))
    {
        return checkField(scene->items(
                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                     lastPos.y() + 2.5*BoardSizes::FieldHeight}
                    ), this).resolve(scene);
    }

    return 0;
}

Bishop::Bishop(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Bishop, p, s)
{
}

void Bishop::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        dehighlight();

        if(!goodMove())
        {
            setPos(lastPos);
        }
        else {
            setPos(scene->items({pos().x() + (BoardSizes::FieldWidth/2),
                                 pos().y() + (BoardSizes::FieldHeight/2)
                                }).last()->pos());
            lastPos = pos();
            //GameStatus::currentPlayer = player == Player::White ? Player::Black : Player::White;
        }
        setZValue(zValue() - 1);
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void Bishop::highlight() {
    // left-top
    {
        qreal x = lastPos.x() - 0.5*BoardSizes::FieldWidth;
        qreal y = lastPos.y() - 0.5*BoardSizes::FieldHeight;

        while((x >= 0 || greaterEqual(x, 0)) &&
              (y >= 0 || greaterEqual(y, 0)))
        {
            auto list = scene->items({x, y});
            auto state =  checkField(list, this);
            if(state == FieldState::Empty || state == FieldState::Enemy) {
                QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                GameStatus::highlighted.push_back({field, field->brush()});
                field->setBrush(BoardBrush::Highlight);
            }
            else break;
            x -= BoardSizes::FieldWidth;
            y -= BoardSizes::FieldHeight;
        }
    }

    // right-top
    {
        qreal x = lastPos.x() + 1.5*BoardSizes::FieldWidth;
        qreal y = lastPos.y() - 0.5*BoardSizes::FieldHeight;

        while((y >= 0 || greaterEqual(y, 0)) &&
              (x < BoardSizes::BoardWidth || lessThan(x, BoardSizes::BoardWidth)))
        {
            auto list = scene->items({x, y});
            auto state =  checkField(list, this);
            if(state == FieldState::Empty || state == FieldState::Enemy) {
                QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                GameStatus::highlighted.push_back({field, field->brush()});
                field->setBrush(BoardBrush::Highlight);
            }
            else break;
            x += BoardSizes::FieldWidth;
            y -= BoardSizes::FieldHeight;
        }
    }

    // left-bottom
    {
        qreal x = lastPos.x() - 0.5*BoardSizes::FieldWidth;
        qreal y = lastPos.y() + 1.5*BoardSizes::FieldHeight;

        while((x >= 0 || greaterEqual(x, 0)) &&
              (y < BoardSizes::BoardHeight|| lessThan(y, BoardSizes::BoardHeight)))
        {
            auto list = scene->items({x, y});
            auto state =  checkField(list, this);
            if(state == FieldState::Empty || state == FieldState::Enemy) {
                QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                GameStatus::highlighted.push_back({field, field->brush()});
                field->setBrush(BoardBrush::Highlight);
            }
            else break;
            x -= BoardSizes::FieldWidth;
            y += BoardSizes::FieldHeight;
        }
    }

    // right-bottom
    {
        qreal x = lastPos.x() + 1.5*BoardSizes::FieldWidth;
        qreal y = lastPos.y() + 1.5*BoardSizes::FieldHeight;

        while((x < BoardSizes::BoardWidth || lessThan(x, BoardSizes::BoardWidth)) &&
              (y < BoardSizes::BoardHeight || lessThan(y, BoardSizes::BoardHeight)))
        {
            auto list = scene->items({x, y});
            auto state =  checkField(list, this);
            if(state == FieldState::Empty || state == FieldState::Enemy) {
                QGraphicsRectItem* field = static_cast<QGraphicsRectItem*>(list.last());

                GameStatus::highlighted.push_back({field, field->brush()});
                field->setBrush(BoardBrush::Highlight);
            }
            else break;
            x += BoardSizes::FieldWidth;
            y += BoardSizes::FieldHeight;
        }
    }
}

bool Bishop::goodMove()
{
    return 1;
}

Rook::Rook(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Rook, p, s)
{
}

void Rook::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        qDebug() << "LMB pressed at " << pos();

        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }

    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void Rook::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        qDebug() << "LMB released at " << pos();
        if(pos().x() + (BoardSizes::FieldWidth/2)  > BoardSizes::BoardWidth  ||
           pos().y() + (BoardSizes::FieldHeight/2) > BoardSizes::BoardHeight ||
           pos().x() + (BoardSizes::FieldWidth/2)  < 0 ||
           pos().y() + (BoardSizes::FieldHeight/2) < 0)
        {
            setPos(lastPos);
        }
        else {
            setPos(scene->items({pos().x() + (BoardSizes::FieldWidth/2),
                                 pos().y() + (BoardSizes::FieldHeight/2)
                                }).last()->pos());
            lastPos = pos();
        }
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void Rook::highlight() {

}

bool Rook::goodMove()
{
    return 0;
}

Queen::Queen(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Queen, p, s)
{
}

void Queen::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        qDebug() << "LMB pressed at " << pos();

        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }

    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void Queen::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        qDebug() << "LMB released at " << pos();
        if(pos().x() + (BoardSizes::FieldWidth/2)  > BoardSizes::BoardWidth  ||
           pos().y() + (BoardSizes::FieldHeight/2) > BoardSizes::BoardHeight ||
           pos().x() + (BoardSizes::FieldWidth/2)  < 0 ||
           pos().y() + (BoardSizes::FieldHeight/2) < 0)
        {
            setPos(lastPos);
        }
        else {
            setPos(scene->items({pos().x() + (BoardSizes::FieldWidth/2),
                                 pos().y() + (BoardSizes::FieldHeight/2)
                                }).last()->pos());
            lastPos = pos();
        }
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void Queen::highlight() {

}

bool Queen::goodMove()
{
    return 0;
}

King::King(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::King, p, s)
{
}

bool King::inDanger() {
    return false; // temporary
}

void King::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        qDebug() << "LMB pressed at " << pos();

        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }

    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void King::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        qDebug() << "LMB released at " << pos();
        if(pos().x() + (BoardSizes::FieldWidth/2)  > BoardSizes::BoardWidth  ||
           pos().y() + (BoardSizes::FieldHeight/2) > BoardSizes::BoardHeight ||
           pos().x() + (BoardSizes::FieldWidth/2)  < 0 ||
           pos().y() + (BoardSizes::FieldHeight/2) < 0)
        {
            setPos(lastPos);
        }
        else {
            setPos(scene->items({pos().x() + (BoardSizes::FieldWidth/2),
                                 pos().y() + (BoardSizes::FieldHeight/2)
                                }).last()->pos());
            lastPos = pos();
        }
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void King::highlight() {

}

bool King::goodMove()
{
    return 0;
}
