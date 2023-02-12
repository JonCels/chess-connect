#ifndef PIECEIDENTIFICATION_HEADER_INCLUDED_
  #define PIECEIDENTIFICATION_HEADER_INCLUDED_

  #ifndef COMMON_HEADER_INCLUDED_
    #include "CommonTypes.h"
  #endif

  Piece IdentifyPiece(int sensor);
  char pieceToChar(Piece piece);

#endif