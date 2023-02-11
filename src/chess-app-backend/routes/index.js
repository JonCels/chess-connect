var express = require('express');
var router = express.Router();
// var { storeData } = require("../utils/bluetooth")

/* GET home page. */
router.get('/', function(req, res, next) {
  let positions = [
    "rnbqkb1r/1ppppppp/5n2/p7/P3P3/8/1PPP1PPP/RNBQKBNR w KQkq - 0 3",
    "rnbqkb1r/1ppppppp/5n2/pB6/P3P3/8/1PPP1PPP/RNBQK1NR b KQkq - 1 3"
  ]
  // storeData()
  res.send({ test: positions[Math.floor(Math.random() * 2)] });
});

module.exports = router;
