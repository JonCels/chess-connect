import './App.css';
import React, {useState} from "react"
import Chessboard from 'chessboardjsx';
import { Chess } from 'chess.js'
import { calculateBestMove, initGame } from "chess-ai";
import axios from "axios";

const getData = (setData) => {
  axios.get("http://localhost:3000/").then(data => {
    console.log(data.data.test)
    setData(data.data.test)
})
}

const container = {
  marginTop: "6rem",
  display: "flex",
  justifyContent: "space-around",
  alignItems: "center"
}

// function lichessCloudEval(fen) {
//   return new Promise(function (resolve, reject) {
//     fetch("https://lichess.org/api/cloud-eval?fen=" + fen + "&multiPv=" + "3")
//       .then((response) => response.json())
//       .then((data) => resolve(data["pvs"]));
//   }
// )}

// async function getTopThree(fenString) {
//   const topThree = []
//   const moveVariations = await lichessCloudEval(fenString);
//   for(let i = 0; i < moveVariations.length; i++) {
//     topThree.push(moveVariations[i]["moves"].split(" ").shift())
//   }
//   return topThree
// }

function App() {

  const fenString = "rnbqkbnr/pp2pppp/2p5/3p4/3PP3/5P2/PPP3PP/RNBQKBNR b KQkq - 0 3"
  const chess = new Chess(fenString)  
  const difficulty = 1
  const [data, setData] = useState()
  const [intervalSet, setIntervalSet] = useState(false)
  initGame(chess, difficulty)
  const bestMove = calculateBestMove(chess, difficulty);
  if (!intervalSet) {
    const myInterval = setInterval(() => getData(setData), 1000)
    setIntervalSet(true)
  }

  // console.log(getTopThree(fenString))
  // const [fen, setFen] = useState(fenString)

  return (
    <div className="App" style={container}>
      {chess.isCheckmate() ?
        <div> 
          <p>Checkmate!</p>
          <Chessboard position={data}/>
        </div>
        : <Chessboard position={data}/>
      }
    </div>
  );
}

export default App;
