#include "chesspiece.h"
#include "mainwindow.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsScene>
#include <algorithm>

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

ChessPiece* canAttack(const QGraphicsItem* field, Player player, const QGraphicsScene* scene) {
    QRectF rect = {field->pos().x(), field->pos().y(), BoardSizes::FieldWidth, BoardSizes::FieldHeight};

    auto list = scene->items(rect);
    auto piece = std::find_if(std::begin(list), std::end(list), [player](QGraphicsItem* item) {
            ChessPiece* piece = dynamic_cast<ChessPiece*>(item);
            if(piece && piece->player != player){
                return true;
            }
            return false;
    });
    return piece == std::end(list) ? nullptr : dynamic_cast<ChessPiece*>(*piece);
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
    QGraphicsRectItem* field = nullptr;
    ChessPiece* piece = nullptr;

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
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() - BoardSizes::FieldWidth/2,
                             lastPos.y() - BoardSizes::FieldHeight/2}
                            ).first());
                if(piece && piece->player != this->player) { // if piece exist and belongs to enemy
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() - BoardSizes::FieldWidth/2,
                                 lastPos.y() - BoardSizes::FieldHeight/2}
                                ).last());
                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // middle top, move-only
            if(lastPos.y() > 0 ||
               greaterThan(lastPos.y(), 0))
            {
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() + BoardSizes::FieldWidth/2,
                             lastPos.y() - BoardSizes::FieldHeight/2}
                            ).first());
                if(!piece) { // if field is empty
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() + BoardSizes::FieldWidth/2,
                                 lastPos.y() - BoardSizes::FieldHeight/2}
                                ).last());
                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() + BoardSizes::FieldWidth/2 + BoardSizes::FieldWidth,
                             lastPos.y() - BoardSizes::FieldHeight/2}
                            ).first());
                if(piece && piece->player != this->player) { // if piece exist and belongs to enemy
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() + BoardSizes::FieldWidth/2 + BoardSizes::FieldWidth,
                                 lastPos.y() - BoardSizes::FieldHeight/2}
                                ).last());
                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }
        }
        else {
            // field in front and not last row
            if(!(lastPos.y() <  BoardSizes::BoardHeight - BoardSizes::FieldHeight) ||
               !lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                return;
            }

            // left bottom, attack-only
            if(lastPos.x() > 0 ||
               greaterThan(lastPos.x(), 0))
            {
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() - BoardSizes::FieldWidth/2,
                             lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                            ).first());
                if(piece && piece->player != this->player) { // if piece exist and belongs to enemy
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() - BoardSizes::FieldWidth/2,
                                 lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                                ).last());
                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // middle bottom, move-only
            if(lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight ||
               lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() + BoardSizes::FieldWidth/2,
                             lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                            ).first());
                if(!piece) {
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() + BoardSizes::FieldWidth/2,
                                 lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                                ).last());
                    GameStatus::highlighted.push_back({field, field->brush()});
                    field->setBrush(BoardBrush::Highlight);
                }
            }

            // right top, attack-only
            if(lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth ||
               lessThan(lastPos.x(), BoardSizes::BoardWidth - BoardSizes::FieldWidth))
            {
                piece = dynamic_cast<ChessPiece*>(scene->items(
                            {lastPos.x() + BoardSizes::FieldWidth/2 + BoardSizes::FieldWidth,
                             lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                            ).first());

                if(piece && piece->player != this->player) { // if piece exist and belongs to enemy
                    field = static_cast<QGraphicsRectItem*>(scene->items(
                                {lastPos.x() + BoardSizes::FieldWidth/2 + BoardSizes::FieldWidth,
                                 lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                                ).last());
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

            // first middle top, move-only
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + BoardSizes::FieldWidth/2,
                         lastPos.y() - BoardSizes::FieldHeight/2}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);

            // second middle top, move-only
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + BoardSizes::FieldWidth/2,
                         lastPos.y() - BoardSizes::FieldHeight/2 - BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
        else {
            // field in front and not last row
            if(!(lastPos.y() <  BoardSizes::BoardHeight - BoardSizes::FieldHeight) ||
               !lessThan(lastPos.y(), BoardSizes::BoardHeight - BoardSizes::FieldHeight))
            {
                return;
            }

            // middle bottom, move-only
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + BoardSizes::FieldWidth/2,
                         lastPos.y() + BoardSizes::FieldHeight/2 + BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);

            // second bottom, move-only
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + BoardSizes::FieldWidth/2,
                         lastPos.y() + BoardSizes::FieldHeight/2 + 2*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
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
        ChessPiece* piece = nullptr;

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
                piece = canAttack(scene->items(
                            {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                             lastPos.y() - 0.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(!piece) {
                        return true;
                }
            }
            // move to left
            else if(newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth))
            {
                piece = canAttack(scene->items(
                            {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                             lastPos.y() - 0.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(piece) {
                        scene->removeItem(piece);
                        return true;
                }
            }
            // move to right
            else if(newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth))
            {
                piece = canAttack(scene->items(
                            {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                             lastPos.y() - 0.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(piece) {
                        scene->removeItem(piece);
                        return true;
                }
            }
        }
        else {
            if(GameStatus::blackKing->inDanger()) {
                return false;
            }

            // if not in front of
            if(newPos.y() != lastPos.y() + BoardSizes::FieldHeight ||
               !isEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight))
            {
                return false;
            }

            // move forward
            if(newPos.x() == lastPos.x() ||
               isEqual(newPos.x(), lastPos.x()))
            {
                piece = canAttack(scene->items(
                            {lastPos.x() + 0.5*BoardSizes::FieldWidth,
                             lastPos.y() + 1.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(!piece) {
                        return true;
                }
            }
            // move to left
            else if(newPos.x() == lastPos.x() - BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() - BoardSizes::FieldWidth))
            {
                piece = canAttack(scene->items(
                            {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                             lastPos.y() + 1.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(piece) {
                        scene->removeItem(piece);
                        return true;
                }
            }
            // move to right
            else if(newPos.x() == lastPos.x() + BoardSizes::FieldWidth ||
                    isEqual(newPos.x(), lastPos.x() + BoardSizes::FieldWidth))
            {
                piece = canAttack(scene->items(
                            {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                             lastPos.y() + 1.5*BoardSizes::FieldHeight}
                            ).last(), player, scene);
                if(piece) {
                        scene->removeItem(piece);
                        return true;
                }
            }
        }
    }
    else {
        if(player == Player::White) {
            // if not in front of
            if(newPos.x() != lastPos.x() ||
               !isEqual(newPos.x(), lastPos.x()))
            {
                return false;
            }

            // if 1st or 2nd field in front
            if((newPos.y() <= lastPos.y() - BoardSizes::FieldHeight ||
               lessEqual(newPos.y(), lastPos.y() - BoardSizes::FieldHeight)) &&
               (newPos.y() >= lastPos.y() - 2*BoardSizes::FieldHeight ||
               greaterEqual(newPos.y(), lastPos.y() - 2*BoardSizes::FieldHeight)))
            {
                return true;
            }
        }
        else {
            // if not in front of
            if(newPos.x() != lastPos.x() ||
               !isEqual(newPos.x(), lastPos.x()))
            {
                return false;
            }

            // if 1st or 2nd field in front
            if((newPos.y() >= lastPos.y() + BoardSizes::FieldHeight ||
               greaterEqual(newPos.y(), lastPos.y() + BoardSizes::FieldHeight)) &&
               (newPos.y() <= lastPos.y() + 2*BoardSizes::FieldHeight ||
               lessEqual(newPos.y(), lastPos.y() + 2*BoardSizes::FieldHeight)))
            {
                return true;
            }
        }
    }

    return false;
}

Knight::Knight(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Knight, p, s)
{
}

//for debug
void Knight::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
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
            //GameStatus::currentPlayer = player == Player::White ? Player::Black : Player::White;
        }
        setZValue(zValue() - 1);
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void Knight::highlight() {
    QGraphicsRectItem* field = nullptr;
    ChessPiece* piece = nullptr;

    // left & top
    if(lastPos.y() >= BoardSizes::FieldHeight &&
       lastPos.x() >= 2*BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                         lastPos.y() - 0.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // left & bottom
    if(lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight &&
       lastPos.x() >= 2*BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() - 1.5*BoardSizes::FieldWidth,
                         lastPos.y() + 1.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // top & left
    if(lastPos.y() >= 2*BoardSizes::FieldHeight &&
       lastPos.x() >= BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                     lastPos.y() - 1.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                         lastPos.y() - 1.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // top & right
    if(lastPos.y() >= 2*BoardSizes::FieldHeight &&
       lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                     lastPos.y() - 1.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                         lastPos.y() - 1.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // right & top
    if(lastPos.y() >= BoardSizes::FieldHeight &&
       lastPos.x() < BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                     lastPos.y() - 0.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                         lastPos.y() - 0.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // right & bottom
    if(lastPos.y() < BoardSizes::BoardHeight - BoardSizes::FieldHeight &&
       lastPos.x() < BoardSizes::BoardWidth - 2*BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                     lastPos.y() + 1.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + 2.5*BoardSizes::FieldWidth,
                         lastPos.y() + 1.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // bottom & left
    if(lastPos.y() < BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight &&
       lastPos.x() >= BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                     lastPos.y() + 2.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() - 0.5*BoardSizes::FieldWidth,
                         lastPos.y() + 2.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }

    // bottom & right
    if(lastPos.y() < BoardSizes::BoardHeight - 2*BoardSizes::FieldHeight &&
       lastPos.x() < BoardSizes::BoardWidth - BoardSizes::FieldWidth)
    {
        piece = dynamic_cast<ChessPiece*>(scene->items(
                    {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                     lastPos.y() + 2.5*BoardSizes::FieldHeight}
                    ).first());
        if(!piece || piece->player != this->player) { // if piece exist and belongs to enemy
            field = static_cast<QGraphicsRectItem*>(scene->items(
                        {lastPos.x() + 1.5*BoardSizes::FieldWidth,
                         lastPos.y() + 2.5*BoardSizes::FieldHeight}
                        ).last());
            GameStatus::highlighted.push_back({field, field->brush()});
            field->setBrush(BoardBrush::Highlight);
        }
    }
}

bool Knight::goodMove()
{
    return 1;
}

Bishop::Bishop(const QPointF& point, const QPixmap& pmap, Player p, QGraphicsScene* s)
    : ChessPiece(point, pmap, PieceType::Bishop, p, s)
{
}

void Bishop::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        qDebug() << "LMB pressed at " << pos();

        if(GameStatus::currentPlayer != player) {
            event->ignore();
            return;
        }

    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void Bishop::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
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

void Bishop::highlight() {

}

bool Bishop::goodMove()
{
    return 0;
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
