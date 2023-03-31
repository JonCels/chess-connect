var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var cors = require('cors');
var logger = require('morgan');
const { Server } = require("socket.io");
const http = require("http")

var indexRouter = require('./routes/index');
var usersRouter = require('./routes/users');

var app = express();
const server = http.createServer(app)
const io = new Server(server, {
  cors: {
    origin: 'http://localhost:8005',
    methods: ['GET', 'POST']
  }
})

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');
app.use(cors());
app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);
app.use('/users', usersRouter);

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

let previousData = {board: "", deviceName: ""};
io.on("connection", (socket) => {
  console.log("a user connected")
  socket.emit("data_to_frontend", JSON.stringify({board: previousData.board.length == 0 ? "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@n" : previousData.board, deviceName: previousData.deviceName.length ? previousData.deviceName : ""}))
  socket.on("new_data", (data) => {
    previousData = JSON.parse(data)
    socket.broadcast.emit("data_to_frontend", data)
    console.log(data)
  })
})

server.listen(8005, () => {
  console.log("Server is online")
})

module.exports = app;
