var express = require('express');
var router = express.Router();
var { data, writeArduino } = require("../utils/bluetooth")

/* GET home page. */
router.get('/', function(req, res, next) {
  let index = data[data.length - 1].indexOf("\r")
  let strippedString = data[data.length - 1].substring(0,index)
  let splitString = strippedString.split("@")
  console.log(splitString)
  res.send({ test: splitString });
});

router.post('/', function(req, res, next) {
  console.log(req.body.engineMove)
  writeArduino(req.body.engineMove)
});


module.exports = router;
