var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var cors = require('cors');
const { Server } = require('socket.io');
const http = require("http")
var { data, writeArduino } = require("./utils/bluetooth")

var indexRouter = require('./routes/index');

var app = express();
const server = http.createServer(app)
const io = new Server(server, {
  cors: {
    origin: 'http://localhost:8001',
    methods: ['GET', 'POST']
  }
})

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(cors());
app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

let intervalStarted = false;

// "rnbqkbnr/pp2pppp/2p5/3pP3/3P4/8/PPP2PPP/RNBQKBNR b KQkq - 0 3", 
// "rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3", 
// "rnbqkbnr/pp1p1ppp/2p5/4p3/2P5/6P1/PP1PPP1P/RNBQKBNR w KQkq - 0 3"

// const positions = [
//   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 
//   "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3", 
// ]

io.on("connection", (socket) => {
  console.log("Client connected")

  let previousData = data;
  socket.broadcast.emit("new_data", previousData)
  console.log(previousData)
  if (!intervalStarted) {
    intervalStarted = true
    setInterval(() => {
      // let randomPos = Math.floor(Math.random() * 2)
      let index = data[data.length - 1].indexOf("\r")
      let strippedString = data[data.length - 1].substring(0,index)
      let splitString = strippedString.split("@")
      if (previousData != splitString) {
        previousData = splitString;
        socket.broadcast.emit("new_data", previousData)
      }
    }, 1000)
  }
})

server.listen(8001, () => {
  console.log("Server is online")
})

module.exports = app;
