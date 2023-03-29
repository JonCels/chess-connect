import './App.css';
import React, {useEffect, useState, useLayoutEffect } from "react"
import Chessboard from 'chessboardjsx';
import { Chess } from 'chess.js'
import { calculateBestMove, initGame } from "chess-ai";
import axios from "axios";
import { Grid } from '@mui/material';
import { io } from 'socket.io-client';
const socket = io("http://localhost:8002");

const sendData = (data) => {
  axios.post("http://localhost:8002/", { engineMove: data })
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
      setData(data)
    })
  }, [socket])

  useEffect(() => {
    socket.connect();
  
    return () => {
      socket.disconnect();
    };
  }, []);

  let fenString = ""
  let gameState = ""
  let mode = ""
  if (data.length) {
    let splitString = data.toString().split("@")
    fenString = splitString[0]
    gameState = splitString[1]
    mode = splitString[2]  
  }
  const chess = new Chess(fenString) 
  const difficulty = 1
  initGame(chess, difficulty)
  const calculatedMove = calculateBestMove(chess, difficulty)
  let index = fenString.indexOf(" ")
  if (!chess.in_checkmate() && !chess.in_stalemate() && mode[0] == "e") {
    sendData(calculatedMove + "@" + fenString[index + 1] + fenString[fenString.length-1] + "@n" + "\r\n");
  } else if (chess.in_checkmate()) {
    sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@c" + "\r\n");
  } else if (chess.in_stalemate()) {
    sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@s" + "\r\n");
  } else if (mode[0] == "b" || mode[0] == "n") {
    sendData("a@" + fenString[index + 1] + fenString[fenString.length-1] + "@n" + "\r\n");
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

  return (
    <div className="App" style={container}>
      <Grid container spacing={2}>
        <Grid style={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }} item xs={3}>
        </Grid>
        <Grid style={{ display: "grid", justifyContent: 'center', alignItems: 'center' }} item xs={6}>
          <h1 className="font-link">Chess Connect</h1>
          <Chessboard position={fenString}/> 
        </Grid>
        <Grid style={{ display: 'flex', alignItems: 'center' }} item xs={3}>
          <h3 className="font-link">{gameOutput}</h3>
        </Grid>
      </Grid>
    </div>
  );
}

export default App;
