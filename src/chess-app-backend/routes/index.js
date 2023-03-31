var express = require('express');
var router = express.Router();
var { writeArduino } = require("../utils/bluetooth")

previousData = ""
router.post('/', function(req, res, next) {
  if (previousData != req.body.engineMove) {
    previousData = req.body.engineMove
    writeArduino(req.body.engineMove)
  }
  res.send("Done")
});

module.exports = router;