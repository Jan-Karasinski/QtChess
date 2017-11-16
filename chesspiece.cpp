#include "chesspiece.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QGraphicsScene>

ChessPiece::ChessPiece(const QPointF& point, const QPixmap& pmap, PieceType t, Player p, QGraphicsScene* s)
    : QGraphicsPixmapItem(pmap),
      lastPos(point),
      type(t),
      player(p),
      scene(s)
{

}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton){
        qDebug() << "LMB pressed at " << this->pos();
    }
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if(event->button() & Qt::LeftButton) {
        qDebug() << "LMB released at " << this->pos();
        if(this->pos().x() > BoardSizes::BoardWidth ||
           this->pos().y() > BoardSizes::BoardHeight ||
           this->pos().x() < 0 ||
           this->pos().y() < 0)
        {
            qDebug() << "invalid pos";
            this->setPos(lastPos);
        }
        else {
            qDebug() << "valid pos";
            this->setPos(this->scene->items({this->pos().rx() + (BoardSizes::FieldWidth/2),
                                             this->pos().ry() + (BoardSizes::FieldHeight/2)}
                                            ).last()->pos());
        }
    }
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}
