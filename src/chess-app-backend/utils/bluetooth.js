// Import the Bluetooth Serial Port module
const BTSP = require('bluetooth-serial-port');
// Create a new Bluetooth Serial Port instance
const btSerial = new BTSP.BluetoothSerialPort();

// Initialize an array to store the data
var data = ["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@n"];
var deviceName = [];
console.log("Connecting...");

// Define an error handling function for Bluetooth connections
const errFunction = (err) => {
    if(err) {
        console.log(err);
    }
}

// Function to store received data
function storeData(str) {
    data.push(str);
}

// Function to write data to Arduino
function writeArduino(str) {
    btSerial.write(Buffer.from(str), errFunction);
}

// Event listener for when a Bluetooth device is found
btSerial.on("found", function(address, name) {
    // Check if the device name contains 'hc-05'
    if(name.toLowerCase().includes('hc-05')) {
        // Find the serial port channel for the device
        btSerial.findSerialPortChannel(address, function(channel) {
            // Connect to the device on the specified channel
            btSerial.connect(address, channel, function() {
                console.log('Connected to: ', name);
                deviceName.push(name)
                var str = "";
                btSerial.on('data', function(bufferData) {
                    str += bufferData;
                    if(str.indexOf("\r") >= 0) {
                        storeData(str);
                        str = "";
                    }
                });
            }, errFunction);
        }, errFunction);
    }
    else {
        console.log('Not connecting to: ', name);
    }
});

// Start the Bluetooth device inquiry
btSerial.inquire();
console.log(deviceName)

// Export the data, deviceName, and writeArduino function
module.exports = { data: data, deviceName: deviceName, writeArduino: writeArduino }