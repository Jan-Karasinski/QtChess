#include "chess_namespaces.h"
#include "chesspiece.h"

#include <QGraphicsScene>

namespace BoardSizes {
    constexpr const int MaxColCount = 8;
    constexpr const int MaxRowCount = 8;

    constexpr const qreal FieldHeight = 48;
    constexpr const qreal FieldWidth  = 48;

    constexpr const qreal BoardHeight = MaxColCount * FieldHeight;
    constexpr const qreal BoardWidth  = MaxRowCount * FieldWidth;
}

namespace GameStatus {
    int uselessMoves{ 0 }; // Fifty moves rule
    Player currentPlayer{ Player::White };
    std::queue<std::pair<QGraphicsRectItem*, QBrush>> highlighted;

    // pieces detatched from scene
    std::vector<std::unique_ptr<ChessPiece>> promotedPieces;

    namespace White {
        King* king{ nullptr };
        std::vector<ChessPiece*> pieces;
    }
    namespace Black {
        King* king{ nullptr };
        std::vector<ChessPiece*> pieces;
    }
}
