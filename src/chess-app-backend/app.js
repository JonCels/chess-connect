var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var cors = require('cors');
const { Server } = require('socket.io');
const http = require("http")
var { data, deviceName } = require("./utils/bluetooth")

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
let previousData = {board: "", deviceName: ""};

io.on("connection", (socket) => {
  console.log("Client connected")
  socket.emit("new_data", JSON.stringify({board: data.length == 0 ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@n" : data[data.length - 1], deviceName: deviceName.length ? deviceName[0] : ""}))
  if (!intervalStarted) {
    intervalStarted = true
    setInterval(() => {
      console.log(deviceName)
      if ((data.length != 0 && previousData.board != data[data.length - 1]) || (deviceName.length != 0 && previousData.deviceName != deviceName[0])) {
        previousData = {board: data[data.length - 1], deviceName: deviceName[0]};
        socket.emit("new_data", JSON.stringify({board: data[data.length - 1], deviceName: deviceName[0]}))
        socket.broadcast.emit("new_data", JSON.stringify({board: data[data.length - 1], deviceName: deviceName[0]}))
      }
    }, 1000)
  }
})

server.listen(8002, () => {
  console.log("Server is online")
})

module.exports = app;
