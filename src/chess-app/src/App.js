import './App.css';
import { TbChessKnightFilled, TbChessKnight, TbCircleFilled } from "react-icons/tb"
import React, {useEffect, useState, useLayoutEffect } from "react"
import Chessboard from 'chessboardjsx';
import { Chess } from 'chess.js'
import { calculateBestMove, initGame } from "chess-ai";
import axios from "axios";
import { Grid } from '@mui/material';
import { io } from 'socket.io-client';
const socket = io("http://localhost:8005");

const sendData = (data) => {
  axios.post("http://localhost:8005/", { engineMove: data }).then(response => {})
}

const container = {
  display: "flex",
  justifyContent: "center",
  alignItems: "center"
}

function App() {
  const [data, setData] = useState([])
  const [deviceName, setDeviceName] = useState("")
  useEffect(() => {
    socket.on("data_to_frontend", (data) => {
      console.log(JSON.parse(data))
      setData(JSON.parse(data).board)
      setDeviceName(JSON.parse(data).deviceName)
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
  } else if (chess.in_stalemate() && data.length != 0) {
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

  let currentPlayer = ""
  if (fenString[index + 1] == "w") {
    currentPlayer = "w"
  } else {
    currentPlayer = "b"
  }

  return (
    <div className="App" style={container}>
      <Grid container spacing={2} style={{marginBottom: '100px'}}>
        <Grid style={{ display: 'flex', justifyContent: 'center', alignItems: 'center' }} item xs={3}>
        </Grid>
        <Grid style={{ display: "grid", justifyContent: 'center', alignItems: 'center' }} item xs={6}>
          <h1 className="font-link" style={{ margin: "auto", marginBottom: "40px", marginTop: "50px" }}>Chess Connect</h1>
          <div style={{display: 'flex', justifyContent: 'space-between'}}>
            {!deviceName ? 
              <div style={{display: 'flex', paddingBottom: '0px', marginBottom: '0px', marginTop: '15px'}}>
              <TbCircleFilled style={{ color: "red", marginTop: '4px', marginRight: '7px', paddingBottom: '0px', marginBottom: '0px' }}/>
              <h3 className="font-link" style={{ color: "red", marginTop: '0px', paddingTop: '0px', paddingBottom: '0px', marginBottom: '0px'}}>Disconnected</h3> 
              </div>
              : 
              <div style={{display: 'flex', paddingBottom: '0px', marginBottom: '0px', padding: '0px', marginTop: '15px'}}>
              <TbCircleFilled style={{ color: "lightGreen", marginTop: '3px', marginRight: '7px', paddingBottom: '0px', marginBottom: '0px' }}/>
              <h3 className="font-link" style={{ color: "green", marginTop: '0px', paddingTop: '0px', paddingBottom: '0px', marginBottom: '0px'}}>Connected To: {deviceName}</h3> 
              </div>
            }
            {/* <h3 className="font-link">{currentPlayer} to {calculatedMove}</h3> */}
            {currentPlayer == "w" ? 
              <TbChessKnight style={{ fontSize: "50px" }}/> :
              <TbChessKnightFilled style={{ fontSize: "50px" }}/>
            }
          </div>
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
