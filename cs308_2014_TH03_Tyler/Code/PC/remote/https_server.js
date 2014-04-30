var express = require('express')
  , app = express()
  , httpapp = express()
  , fs = require('fs')
  , options = {
    	key: fs.readFileSync('privatekey.pem'),
    	cert: fs.readFileSync('certificate.pem'),
    	requestCert: true
	}
  , http = require('http').createServer(httpapp)
  , server = require('https').createServer(options, app)
  , io = require('socket.io').listen(server)
  , io2 = require('socket.io').listen(http);

httpapp.get('*',function(req,res){
    res.redirect('https://127.0.0.1:8080'+req.url)
});

app.get('/test', function(req, res){
	res.sendfile('public/index.html');
});

app.get('/', function(req, res){
	res.sendfile('dashboard.html');
});

app.get('/op/:op', function(req, res){
	res.sendfile('operate.html');
});

app.get('/client/:op', function(req, res){
    res.sendfile('client.html');
});

app.use('/static', express.static(__dirname + '/public'));

server.listen(8080);
http.listen(8081);

// var io = require('socket.io').listen(server);
io.sockets.on('connection', function (socket){
	function log(){
		var array = [">>> "];
	    for (var i = 0; i < arguments.length; i++) {
	  	    array.push(arguments[i]);
	    }
	    socket.emit('log', array);
	}

	socket.on('message', function (data) {
		log('Got message: ', data.data);
		socket.broadcast.to(data.room).emit('message', data); // should be room only
	});

    socket.on('dir', function (data) {
        log('Got message: ', data);
        socket.broadcast.emit('dir', data); // should be room only
    });

	socket.on('create or join', function (room) {
		var numClients = io.sockets.clients(room).length;

		log('Room ' + room + ' has ' + numClients + ' client(s)');
		log('Request to create or join room', room);

		if (numClients == 0){
			socket.join(room);
			socket.emit('created', room);
		} else if (numClients == 1) {
			io.sockets.in(room).emit('join', room);
			socket.join(room);
			socket.emit('joined', room);
		} else { // max two clients
			socket.emit('full', room);
		}
		socket.emit('emit(): client ' + socket.id + ' joined room ' + room);
		socket.broadcast.emit('broadcast(): client ' + socket.id + ' joined room ' + room);
	});
});

// io2.sockets.on('connection', function(socket){
//     function log(){
//         var array = [">>> "];
//         for (var i = 0; i < arguments.length; i++) {
//             array.push(arguments[i]);
//         }
//         socket.emit('log', array);
//     }

//     socket.on('create or join', function (room) {
//         log('Request to create or join room', room);
//     });
// });