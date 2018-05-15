var webSocketUrl = "wss://api.artik.cloud/v1.1/websocket?ack=true";
var device_id = "8c6126a8e7484fd18c684eaa0632f27d";
var device_token = "f4d646a7f655433f881dd5f208a84e01";

var isWebSocketReady = false;
var ws = null;

var serialport = require("serialport")
var SerialPort = serialport.SerialPort;
var sp = new serialport("/dev/ttyACM0", {
    BaudRate: 9600,
    parser: new serialport.parsers.Readline("\n")
});

var WebSocket = require('ws');

/**
 * Gets the current time in millis
 */
function getTimeMillis(){
    return parseInt(Date.now().toString());
}

/**
 * Create a /websocket bi-directional connection 
 */
function start() {
    //Create the websocket connection
    isWebSocketReady = false;
    ws = new WebSocket(webSocketUrl);
    ws.on('open', function() {
         console.log("Websocket connection is open ....");
         register();
    });
    ws.on('message', function(data, flags) {
         console.log("Received message: " + data + '\n');
    });
    ws.on('close', function() {
         console.log("Websocket connection is closed ....");
    });	   
}

/**
 * Sends a register message to the websocket and starts the message flooder
 */
function register(){
    console.log("Registering device on the websocket connection");
    try{
        var registerMessage = '{"type":"register", "sdid":"'+device_id+'", "Authorization":"bearer '+device_token+'", "cid":"'+getTimeMillis()+'"}';
        console.log('Sending register message ' + registerMessage + '\n');
        ws.send(registerMessage, {mask: true});
        isWebSocketReady = true;
    }
    catch (e) {
        console.error('Failed to register messages. Error in registering message: ' + e.toString());
    }
}

/**
 * Send one message to ARTIK Cloud
 */
function sendData(parking){
    try{
        ts = ', "ts": '+getTimeMillis();
        var data = {
                    "parkingOneFree": parking[0],
		    "parkingTwoFree": parking[1]
                   };
        var payload = '{"sdid":"'+device_id+'"'+ts+', "data": '+JSON.stringify(data)+', "cid":"'+getTimeMillis()+'"}';
        console.log('Sending payload ' + payload);
        ws.send(payload, {mask: true});
    } catch (e) {
        console.error('Error in sending a message: ' + e.toString());
    }
}

/**
 * All start here
 */


start(); // create websocket connection

sp.on("open", function () {
    sp.on('data', function(data) {
            if (!isWebSocketReady){
                console.log("Websocket is not ready. Skip sending data to ARTIK Cloud (data:" + data +")");
                return;
            }
            console.log("Serial port received data:" + data);
	    var park=[];
	    if (String(data).split('\t').length>=2){
	    park[0]=parseInt(String(data).split('\t')[0]);
	    park[1]=parseInt(String(data).split('\t')[1]);
           // var parking = parseInt(data.split('\t')[0]);
            //sendData(parking);
	    //var parking2 = parseInt(data.split('\t')[1]);
            sendData(park);
	    }
    });
});

