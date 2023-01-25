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

function lichessCloudEval(fen) {
  return new Promise(function (resolve, reject) {
    fetch("https://lichess.org/api/cloud-eval?fen=" + fen + "&multiPv=" + "3")
      .then((response) => response.json())
      .then((data) => resolve(data["pvs"]));
  }
)}

async function getTopThree(fenString) {
  const topThree = []
  const moveVariations = await lichessCloudEval(fenString);
  for(let i = 0; i < moveVariations.length; i++) {
    topThree.push(moveVariations[i]["moves"].split(" ").shift())
  }
  return topThree
}

function App() {

  const fenString = "rnbqkbnr/pp2pppp/2p5/3p4/3PP3/5P2/PPP3PP/RNBQKBNR b KQkq - 0 3"
  // const chess = new Chess(
  //   'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1'
  // )  
  // const difficulty = 1
  // initGame(chess, difficulty)
  // const bestMove = calculateBestMove(chess, difficulty);
  // console.log(bestMove)

  console.log(getTopThree(fenString))
  const [fen, setFen] = useState(fenString)

  return (
    <div className="App" style={container}>
      <Chessboard position={fen}/>
    </div>
  );
}

export default App;
