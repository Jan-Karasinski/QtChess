#include "paths.h"

namespace Paths
{
    using namespace std::string_literals;

    namespace White {
        QString pawn  { ":/Pieces/pawn_white.png"   };
        QString knight{ ":/Pieces/knight_white.png" };
        QString bishop{ ":/Pieces/bishop_white.png" };
        QString rook  { ":/Pieces/rook_white.png"   };
        QString queen { ":/Pieces/queen_white.png"  };
        QString king  { ":/Pieces/king_white.png"   };
    }
    namespace Black {
        QString pawn  { ":/Pieces/pawn_black.png"   };
        QString knight{ ":/Pieces/knight_black.png" };
        QString bishop{ ":/Pieces/bishop_black.png" };
        QString rook  { ":/Pieces/rook_black.png"   };
        QString queen { ":/Pieces/queen_black.png"  };
        QString king  { ":/Pieces/king_black"       };
    }
}
