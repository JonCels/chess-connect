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

const sendData = (data) => {
  axios.post("http://localhost:3000/", { engineMove: data })
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

  const [data, setData] = useState([])
  // const [bestMove, setBestMove] = useState("")
  // const [currentFenString, setCurrentFenString] = useState("")
  // getData(setData)
  const [intervalSet, setIntervalSet] = useState(false)
  const fenString = data[0]
  const gameState = data[1]
  const mode = data[2]
  const chess = new Chess(fenString) 
  const difficulty = 1
  initGame(chess, difficulty)
  const calculatedMove = calculateBestMove(chess, difficulty)
  // console.log("currentFenString: ", currentFenString)
  console.log("fenString: ", fenString)
  console.log(calculatedMove != "Nf3" && !chess.in_checkmate() && !chess.in_stalemate())
  if (calculatedMove != "Nf3" && !chess.in_checkmate() && !chess.in_stalemate() && mode == "e") {
    // setBestMove(calculatedMove);
    // setCurrentFenString(fenString)
    let index = fenString.indexOf(" ")
    sendData(calculatedMove + "@" + fenString[index + 1] + fenString[fenString.length-1] + "@n" + "\r\n");
  } else if (chess.in_checkmate()) {
    sendData("a@a@c" + "\r\n");
  } else if (chess.in_stalemate()) {
    sendData("a@a@s" + "\r\n");
  } else if (mode == "b" || mode == "n") {
    sendData("a@a@n" + "\r\n");
  } 

  let gameOutput;
  if (chess.in_checkmate()) {
    gameOutput = "Checkmate!";
  } else if (chess.in_stalemate()) {
    gameOutput = "Stalemate!"
  } else if (gameState == "d") {
    gameOutput = "Draw!"
  } else if (gameState == "w") {
    gameOutput = "White Resigns!"
  } else if (gameState == "b") {
    gameOutput = "Black Resigns!"
  } else {
    gameOutput = "";
  }
  
  if (!intervalSet) {
    const receivingInterval = setInterval(() => getData(setData), 500)
    const refreshInterval = setInterval(() => window.location.reload(false), 10000)
    setIntervalSet(true)
  }

  // console.log(getTopThree(fenString))
  // const [fen, setFen] = useState(fenString)

  return (
    <div className="App" style={container}>
      <Chessboard position={fenString}/> 
      <h3>{gameOutput}</h3>
    </div>
  );
}

export default App;
