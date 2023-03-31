const BTSP = require('bluetooth-serial-port');
const btSerial = new BTSP.BluetoothSerialPort();
var data = ["rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1@s@n"];
var deviceName = [];
console.log("Connecting...");

const errFunction = (err) => {
    if(err) {
        console.log(err);
    }
}

function storeData(str) {
    data.push(str);
}

function writeArduino(str) {
    btSerial.write(Buffer.from(str), errFunction);
}

btSerial.on("found", function(address, name) {
    if(name.toLowerCase().includes('hc-05')) {
        btSerial.findSerialPortChannel(address, function(channel) {
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

btSerial.inquire();
console.log(deviceName)

module.exports = { data: data, deviceName: deviceName, writeArduino: writeArduino }