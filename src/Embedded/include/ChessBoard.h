#ifndef CHESSBOARD_HEADER_INCLUDED_
  #define CHESSBOARD_HEADER_INCLUDED_

  #include "CommonTypes.h"
  
  const int numRows = 8;
  const int numCols = 8;

  extern char FEN[100];

  // 2D array to represent the chess board
  extern Piece currentBoard[numRows][numCols];

  void resetChessBoard();
  void loopChessBoard();
  void LightAllPieces();
  void LightSquare(int row, int col, bool on);
  bool movePiece(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionType = NO_PIECE);
  char* boardToFen();
  void sendFen(char* fen);

#endif