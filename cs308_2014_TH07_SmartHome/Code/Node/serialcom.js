var SerialPort = require("serialport").SerialPort

var serialport;
var callback_function = function(){};

exports.open = function(path,rate,callback){ // callback is command callback
	serialPort = new SerialPort(path, {
		baudrate: rate
	});
	callback_function = callback;
	serialPort.on("open", function () {	  	
		var buf;
		var is_reading = 0;
		var length = 0;
		serialPort.on('data', function(data) {						
			for(var i = 0; i < data.length; i++){
				if(data[i] == 255){
					buf = new Buffer(4);
					is_reading = 1;
					command_length = 0;
				}
				else if(is_reading){
					buf[command_length] = data[i];
					command_length++;
					if(command_length >= 4){
						is_reading = 0;						
						if(typeof callback_function !== 'undefined')
							callback_function(buf);
						else
							console.log('callback function is not defined');
					}
				}
			}
		});	  
	});
};

exports.sendCommand = function(command){	
	serialPort.write(command, function(err, results) {	    
		if(typeof err !== 'undefined')		
			console.log('err ' + err);		
	});	
};

exports.setCallback = function(callback){
	callback_function = callback;	
};
