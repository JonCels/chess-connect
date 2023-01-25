import './App.css';
import React, {useState} from "react"
import Chessboard from 'chessboardjsx';
import { Chess } from 'chess.js'
import { calculateBestMove, initGame } from "chess-ai";

const container = {
  marginTop: "2rem",
  display: "flex",
  justifyContent: "space-around",
  alignItems: "center"
}

function lichessCloudEval() {
  const fenString = "rnbqkbnr/pppp1ppp/8/4N3/8/8/PPPPPPPP/RNBQKB1R b KQkq - 0 2"
  return new Promise(function (resolve, reject) {
    fetch("https://lichess.org/api/cloud-eval?fen=" + fenString + "&multiPv=" + "3")
      .then((response) => response.json())
      .then((data) => resolve(data["pvs"]));
  }
)}

function App() {

  const chess = new Chess(
    'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
  )  
  const difficulty = 1
  initGame(chess, difficulty)
  const bestMove = calculateBestMove(chess, difficulty);
  console.log(bestMove)
  const topThree = []
  const moveVariations = lichessCloudEval();
  console.log(moveVariations)

  for(let i = 0; i < moveVariations.length; i++) {
    topThree.push(moveVariations[i]["moves"].split(" ").shift())
  }
  console.log(topThree)


  const [fen, setFen] = useState("start")

  return (
    <div className="App" style={container}>
      <Chessboard position={fen}/>
    </div>
  );
}

export default App;
