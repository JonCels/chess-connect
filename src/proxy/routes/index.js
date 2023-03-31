var express = require('express');
var router = express.Router();
var axios = require("axios")

router.post('/', function(req, res, next) {
  axios.post("http://localhost:8006", { engineMove: req.body.engineMove }).then(response => {}).catch(error => {
    console.log("Server not running yet")
  })
  res.send("Done")
});

module.exports = router;
