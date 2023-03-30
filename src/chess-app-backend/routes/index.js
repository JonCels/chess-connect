var express = require('express');
var router = express.Router();
var { writeArduino } = require("../utils/bluetooth")

router.post('/', function(req, res, next) {
  writeArduino(req.body.engineMove)
  res.send("Done")
});

module.exports = router;