#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include "mainwindow.h"


class ChessPiece : public QGraphicsPixmapItem
{
public:
    ChessPiece(const QPointF&, const QPixmap&, PieceType, Player, QGraphicsScene* );

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

private:
    PieceType type;
    QPointF lastPos;
    const Player player;
    QGraphicsScene* scene;
};

#endif // CHESSPIECE_H
