// https://github.com/LearnBoost/socket.io-client
// https://github.com/voodootikigod/node-serialport

var SerialPort = require("serialport").SerialPort;
var serialPort = new SerialPort("/dev/cu.usbserial-AH001BS6", /*{ baudrate: 57600 },*/ false); // this is the openImmediately flag [default is true]

var clientio = require('socket.io-client');         // this is the socket.io client
var socket = clientio.connect('192.168.0.109', {port: 8080});

var http = require('http');

var room = 'b0';
if (room !== '') {
  console.log('Create or join room', room);
  socket.emit('create or join', room);
}

serialPort.open(function () {
   serialPort.on('data', function(data) {
      if (data == '1') {
        socket.emit('message', {'type': 'ping', 'bot': 'b0'});
      } else if (data == '0') {

      } else {

      }
      console.log('data received: ' + data);
      sendStatus();
   });
   serialPort.write("ls\n", function(err, results) {
       console.log('err ' + err);
       console.log('results ' + results);
   });
});

socket.on('created', function (room){
  console.log('Created room ' + room);
});

socket.on('full', function (room){
  console.log('Room ' + room + ' is full');
});

socket.on('join', function (room){
  console.log('Another peer made a request to join room ' + room);
  console.log('This peer is the initiator of room ' + room + '!');
});

socket.on('joined', function (room){
  console.log('This peer has joined room ' + room);
  isChannelReady = true;
});

socket.on('log', function (array){
  console.log.apply(console, array);
});

socket.on('dir', function(data){
    if (data.direction == 'right') {
        serialPort.write('6');
    } else if (data.direction == 'left') {
        serialPort.write('4');
    } else if (data.direction == 'up') {
        serialPort.write('8');
    } else if (data.direction == 'down') {
        serialPort.write('5');
    } else {
        console.log('asd');
    }
});

////////////////////////////////////////////////

socket.on('message', function (message){
  // console.log('Received message:', message);
  // if (message === 'got user media') {
  //   maybeStart();
  // } else if (message.type === 'offer') {
  //   if (!isInitiator && !isStarted) {
  //     maybeStart();
  //   }
  //   pc.setRemoteDescription(new RTCSessionDescription(message));
  //   doAnswer();
  // } else if (message.type === 'answer' && isStarted) {
  //   pc.setRemoteDescription(new RTCSessionDescription(message));
  // } else if (message.type === 'candidate' && isStarted) {
  //   var candidate = new RTCIceCandidate({sdpMLineIndex:message.label,
  //     candidate:message.candidate});
  //   pc.addIceCandidate(candidate);
  // } else if (message === 'bye' && isStarted) {
  //   handleRemoteHangup();
  // }
  console.log(message);
});

var botStatus = false;
var wait_count = 5;

var previous = [0,0],
    ATHRESHOLD = 20,
    DTHRESHOLD = 400,
    first = true;

function threeD(num) {
  if (num < 10) {
    return '00' + num;
  } else if (num < 100) {
    return '0' + num;
  } else {
    return num;
  }
}

var centroidX = 212;
var centroidY = 315;
var stopped = 25,
    stoppedB = true;

function sendStatus(){
  var options = {
    host: 'localhost',
    port: 8001,
    path: '/'
  };

  http.get(options, function(resp){
    resp.setEncoding('utf8');
    resp.on('data', function(chunk){
      // console.log('DATA', chunk);
      parts = chunk.split(' ');
      var botX = parseInt(parts[0]);
      var botY = parseInt(parts[1]);

      if (stopped > 0) {
        stopped--;
        return;
      }

      if (stoppedB) {
        centroidX = parseInt(parts[3]);
        centroidY = parseInt(parts[2]);
        stoppedB = false;
      }

      if (wait_count > 0) {wait_count--;}

      console.log(centroidX)

      if (botX == 0 && botY == 0) {
        console.log('>>> ERROR !! Bot Missing');
      } else if (first) {
        console.log('First write: forward');
        serialPort.write('8');
        first = false;
        previous = [botX, botY];
      } else {
        if ((botY - centroidY)*(botY - centroidY) + (botX - centroidX)*(botX - centroidX) < DTHRESHOLD) {
          // Stop if close
          serialPort.write('5');
          stopped = 10;
          stoppedB = true;
        } else {
          if (wait_count > 0) {
            console.log('DB: ', botX, botY, centroidX, centroidY, previous[0], previous[1], 'forward');
            serialPort.write('8');
          } else if (centroidY - botY == 0) {
            console.log('DB: ', botX, botY, centroidX, centroidY, previous[0], previous[1], 'forward');
            serialPort.write('8');
          } else {
            // 114 143 240 49
            var botToPos = Math.atan(((botX - centroidX) * 1.0) / (botY - centroidY)) * 180.0/3.142;
            if (centroidX - botX < 0) {
              botToPos += 180;
            }

            botToPos = 360 - botToPos;

            if (botY - previous[1] == 0) {
              console.log('DB: ', botX, botY, centroidX, centroidY, previous[0], previous[1], botToPos, 'forward');
              serialPort.write('8');
            } else {

              var botRel = Math.atan(((botX - previous[0]) * 1.0) / (botY - previous[1])) * 180.0/3.142;
              if (botX - previous[0] < 0) {
                botRel += 180;
              }

              botRel = 360 - botRel;

              var angleDiff = parseInt(botToPos - botRel);
              while (angleDiff < 0) {
                angleDiff += 360;
              }
              angleDiff = angleDiff % 360;

              if (Math.abs(angleDiff) < ATHRESHOLD || Math.abs(360 - angleDiff) < ATHRESHOLD) {
                console.log('DB: ', botX, botY, centroidX, centroidY, previous[0], previous[1], botToPos, botRel, angleDiff, 'forward');
                serialPort.write('8');
              } else {
                var d3 = 'r' + threeD(angleDiff);
                console.log('DB: ', botX, botY, centroidX, centroidY, previous[0], previous[1], botToPos, botRel, angleDiff, d3);
                serialPort.write(d3[0]);
                serialPort.write(d3[1]);
                serialPort.write(d3[2]);
                serialPort.write(d3[3]);
                wait_count = 3;
              }
            }
          }
        }
        previous = [botX, botY];
      }
    });
  }).on("error", function(e){
    console.log("Got error: " + e.message);
  });
}

// socket.on('connect', function(){
// 	console.log('connected');



  //   socket.on('disconnect', function(){

  //   });
// });
