const BTSP = require('bluetooth-serial-port');
const btSerial = new BTSP.BluetoothSerialPort();
var data = [];
console.log("Connecting...");

const errFunction = (err) => {
    if(err) {
        console.log(err);
    }
}

function storeData(str) {
    // console.log(str);
    data.push(str);
    console.log(data);
    btSerial.write(Buffer.from("From node\r"), errFunction);
}

function writeArduino(str) {
    btSerial.write(Buffer.from(str), errFunction);
}

btSerial.on("found", function(address, name) {
    if(name.toLowerCase().includes('hc-05')) {
        btSerial.findSerialPortChannel(address, function(channel) {
            btSerial.connect(address, channel, function() {
                console.log('Connected to: ', name);
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

module.exports = { data: data, writeArduino: writeArduino }