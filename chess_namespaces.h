#ifndef CHESS_NAMESPACES_H
#define CHESS_NAMESPACES_H

#include <QBrush>
#include <QGraphicsItem>

#include <queue>
#include <memory>

enum class PieceType : char {
    King   = 'K', Queen  = 'Q', Rook = 'R',
    Bishop = 'B', Knight = 'H', Pawn = 'P',
};

enum class Player : char {
    White = 'W', Black = 'B'
};

enum class WinCondition : int {
    Continue = 0, Checkmate, Stalemate, Draw, FiftyMoves
};

enum class MoveType : int {
    Move = 1, Attack, Castle, EnPassant, PromotionMove, PromotionAttack
};

namespace BoardSizes {
    extern const int MaxColCount;
    extern const int MaxRowCount;

    extern const qreal FieldHeight;
    extern const qreal FieldWidth;

    extern const qreal BoardHeight;
    extern const qreal BoardWidth;
}

namespace BoardBrush {
    const QBrush White = QBrush(QColor(Qt::GlobalColor::white));
    const QBrush Black = QBrush(QColor(Qt::GlobalColor::black));
}

class ChessPiece;
class King;

namespace GameStatus {
    extern int uselessMoves; // Fifty moves rule
    extern Player currentPlayer;
    extern std::queue<std::pair<QGraphicsRectItem*, QBrush>> highlighted;

    // pieces detatched from scene
    extern std::vector<std::unique_ptr<ChessPiece>> promotedPieces;

    namespace White {
        extern King* king;
        extern std::vector<ChessPiece*> pieces;
    }
    namespace Black {
        extern King* king;
        extern std::vector<ChessPiece*> pieces;
    }
}

#endif // CHESS_NAMESPACES_H
