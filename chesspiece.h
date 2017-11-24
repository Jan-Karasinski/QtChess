#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QGraphicsPixmapItem>
#include <QPointF>
#include "mainwindow.h"


class ChessPiece : public QGraphicsPixmapItem
{
public:
    ChessPiece(const QPointF&, const QPixmap&, PieceType, Player, QGraphicsScene*);

    virtual ~ChessPiece() = default;

    static ChessPiece* Create(PieceType, const QPointF&, const QPixmap&, Player, QGraphicsScene*);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent*);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    virtual void highlight() = 0;
    void dehighlight();

    virtual bool goodMove() = 0;

    PieceType type;
    QPointF lastPos;
    QGraphicsScene* scene;

public:
    const Player player;
};

class Pawn : public ChessPiece
{
public:
    Pawn(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;

    bool firstMove = 1;

};

class Knight : public ChessPiece
{
public:
    Knight(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;
};

class Bishop : public ChessPiece
{
public:
    Bishop(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;
};

class Rook : public ChessPiece
{
public:
    Rook(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;
};

class Queen : public ChessPiece
{
public:
    Queen(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;
};

class King : public ChessPiece
{
public:
    King(const QPointF&, const QPixmap&, Player, QGraphicsScene*);

    bool inDanger();

private:
    void mousePressEvent(QGraphicsSceneMouseEvent*) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;

    void highlight() override;

    bool goodMove() override;
};

#endif // CHESSPIECE_H
