var express = require('express');
var router = express.Router();
var { writeArduino } = require("../utils/bluetooth")

// Define a POST route for the root path
router.post('/', function(req, res, next) {
  // Write the engine move received from the client to the Arduino
  writeArduino(req.body.engineMove)
  // Send a "Done" response back to the client
  res.send("Done")
});

// Export the router for use in other parts of the application
module.exports = router;