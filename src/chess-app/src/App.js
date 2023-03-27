import './App.css';
import React, {useEffect, useState} from "react"
import Chessboard from 'chessboardjsx';
import { Chess } from 'chess.js'
import { calculateBestMove, initGame } from "chess-ai";
import axios from "axios";
import { Grid } from '@mui/material';
import { io } from 'socket.io-client';
const socket = io("http://localhost:8001");

// const getData = (setData) => {
//   axios.get("http://localhost:3000/").then(data => {
//     console.log(data.data.test)
//     setData(data.data.test)
// })
// }

const sendData = (data) => {
  axios.post("http://localhost:3000/", { engineMove: data })
}

const container = {
  marginTop: "6rem",
  display: "flex",
  justifyContent: "center",
  alignItems: "center"
}

function App() {
  const [data, setData] = useState([])
  useEffect(() => {
    socket.on("new_data", (data) => {
      console.log("This is new data " + data)
      setData(data)
    })
  }, [socket])

  useEffect(() => {
    socket.connect();
  
    return () => {
      socket.disconnect();
    };
  }, []);

  // const [bestMove, setBestMove] = useState("")
  // const [currentFenString, setCurrentFenString] = useState("")
  // // getData(setData)
  // const [intervalSet, setIntervalSet] = useState(false)
  // const fenString = data[0]
  // const gameState = data[1]
  // const mode = data[2]
  // const chess = new Chess(fenString) 
  // const difficulty = 1
  // initGame(chess, difficulty)
  // const calculatedMove = calculateBestMove(chess, difficulty)
  // // console.log("currentFenString: ", currentFenString)
  // console.log("fenString: ", fenString)
  // console.log(calculatedMove != "Nf3" && !chess.in_checkmate() && !chess.in_stalemate())
  // if (calculatedMove != "Nf3" && !chess.in_checkmate() && !chess.in_stalemate() && mode == "e") {
  //   // setBestMove(calculatedMove);
  //   // setCurrentFenString(fenString)
  //   let index = fenString.indexOf(" ")
  //   sendData(calculatedMove + "@" + fenString[index + 1] + fenString[fenString.length-1] + "@n" + "\r\n");
  // } else if (chess.in_checkmate()) {
  //   sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@c" + "\r\n");
  // } else if (chess.in_stalemate()) {
  //   sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@s" + "\r\n");
  // } else if (mode == "b" || mode == "n") {
  //   sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@n" + "\r\n");
  // } 

  // let gameOutput;
  // if (chess.in_checkmate()) {
  //   gameOutput = "Checkmate!";
  // } else if (chess.in_stalemate()) {
  //   gameOutput = "Stalemate!"
  // } else if (gameState == "d") {
  //   gameOutput = "Draw!"
  // } else if (gameState == "w") {
  //   gameOutput = "White Resigns!"
  // } else if (gameState == "b") {
  //   gameOutput = "Black Resigns!"
  // } else if (gameState == "n") {
  //   gameOutput = "Empty Game State"
  // } else if (gameState == "s") {
  //   gameOutput = "Normal Position"
  // } else {
  //   gameOutput = "";
  // }

  // getData(setData)
  
  // if (!intervalSet) {
  //   const receivingInterval = setInterval(() => getData(setData), 500)
  //   const refreshInterval = setInterval(() => window.location.reload(false), 10000)
  //   setIntervalSet(true)
  // }

  return (
    <div className="App" style={container}>
      <Grid container spacing={2}>
        <Grid style={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }} item xs={3}>
          <h1 className="font-link">Chess Connect</h1>
        </Grid>
        <Grid style={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }} item xs={6}>
          <Chessboard position={data}/> 
        </Grid>
        {/* <Grid style={{ display: 'flex', alignItems: 'center' }} item xs={3}>
          <h3 className="font-link">{gameOutput}</h3>
        </Grid> */}
      </Grid>
    </div>
  );
}

export default App;
