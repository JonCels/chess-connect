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
    origin: 'http://localhost:8002',
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

// let positions = [
//   "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@e\r", 
//   "r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 2 3@s@n\r", 
// ]

let previousData = "";

io.on("connection", (socket) => {
  console.log("Client connected")
  socket.emit("new_data", data.length == 0 ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@e" : data[data.length - 1])
  if (!intervalStarted) {
    intervalStarted = true
    setInterval(() => {
      // let randomPos = Math.floor(Math.random() * 5)
      // console.log("previousData: " + previousData[data.length - 1])
      // console.log("newData: " + data[data.length - 1])
      // console.log(data.length)
      if (data.length != 0 && previousData != data[data.length - 1]) {
        // console.log("Sending... " + data[data.length - 1])
        // console.log(data.length)
        previousData = data[data.length - 1];
        socket.emit("new_data", data[data.length - 1])
        socket.broadcast.emit("new_data", data[data.length - 1])
      }
    }, 1000)
  }
})

server.listen(8002, () => {
  console.log("Server is online")
})

module.exports = app;
