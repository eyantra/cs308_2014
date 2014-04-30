var sockets; 
var db_con; 
var serialcom;
exports.setConn = function(sock,db,serial){
	sockets = sock;
	db_con = db;
	serialcom = serial;
}

exports.switchCommand = function(roomid,switchid,state){
	var command = new Buffer(5);
	command[0] = 255;
	command[1] = roomid;
	command[2] = 1;
	command[3] = switchid;
	command[4] = state;
	serialcom.sendCommand(command);		
};

exports.runCommand = function(buf){		
	if(typeof db_con !== 'undefined') {		
		if(buf[1] == 1){			
			var query = db_con.query(
			'UPDATE switches set switches.state = ? WHERE switches.id = ? and switches.room_id = ?;',[(buf[3]>0)?1:0,buf[2],buf[0]],
			    function(err, result) {  
			    	if(typeof err === 'undefined')
			    		console.log(err);
			    	else{			    		
			    		if(typeof sockets !== 'undefined')
							sockets.emit('server-switch-state',{switchId: buf[2],roomId: buf[0],value:(buf[3]>0)?1:0});										    		
			  			else
							console.log('socket is not defined')    			
				  	}
			  	}
			);
		} else if(buf[1] == 2) {
			//console.log('sensor no: '+buf[2]+' has value : '+buf[3]);
			var query = db_con.query(
				'UPDATE sensors set sensors.value = ? WHERE sensors.id = ? and sensors.room_id = ?;', [buf[3],buf[2],buf[0]],
			    function(err, result) {  
			    	if(typeof err === 'undefined')
			    		console.log(err);
			    	else{
			    		if(typeof sockets !== 'undefined'){
							sockets.emit('sensor-value',{sensorId: buf[2], roomId: buf[0], value: buf[3]});
			    		}
			  			else
							console.log('socket is not defined')    			
				  	}
			  	}
			);
		}
		else if(buf[1] == 10){			
			db_con.query('Select * from rooms where id=?',[buf[0]],function(err,result){
				if(typeof err === 'undefined'){
		    		console.log(err);
		    		return;
		    	}		    	
		    	if(result.length == 0){
		    		db_con.query('Insert into rooms (name,id) values (?,?);',['Room #'+buf[0],buf[0]],function(err,result){
		    			for(var i = 1; i <= buf[2];i++){
		    				db_con.query('Insert into switches (name,state,room_id,id) values(?,?,?,?);',['Switch #'+i,0,buf[0],i]);
		    			}
		    			for(var i = 1; i <= buf[3];i++){
		    				db_con.query('Insert into sensors (name,value,room_id,id) values(?,?,?,?);',['Sensor #'+i,0,buf[0],i]);
		    			}
		    		});
		    	}
		    	else{
		    		for(var i = 1; i <= buf[2];i++){
		    			(function(j){
			    			db_con.query('Select * from switches where id=? and room_id=?',[j,buf[0]],function(err,result){
			    				if(typeof err === 'undefined'){
			    					console.log(err);
			    					return;
			    				}			    
			    				if(result.length == 0)			
			    					db_con.query('Insert into switches (name,state,room_id,id) values(?,?,?,?);',['Switch #'+j,0,buf[0],j]);
			    			});
		    			})(i)	    				
	    			}
	    			for(var i = 1; i <= buf[3];i++){
	    				(function(j){
		    				db_con.query('Select * from sensors where id=? and room_id=?',[j,buf[0]],function(err,result){
		    					if(typeof err === 'undefined'){
			    					console.log(err);
			    					return;
			    				}
			    				if(result.length == 0)			
		    						db_con.query('Insert into sensors (name,value,room_id,id) values(?,?,?,?);',['Sensor #'+j,0,buf[0],j]);
		    				});
	    				})(i)
	    			}
		    	}

			    });
		}
	} else {
		console.log('db_conn is not defined')
	}	
}

var is_discovering = false;
exports.discoverCommand = function(socket){	
	if(!is_discovering){
		is_discovering = true;
		socket.emit('discover',{msg:"Wait for 5 sec"});
		var command = new Buffer(5);
		command[0] = 255;
		command[1] = 0;
		command[2] = 10;
		command[3] = 0;
		command[4] = 0;
		serialcom.sendCommand(command);		
		setTimeout(function() {
  			is_discovering = false;
  			sockets.emit('refresh');
		}, 5000);
	}
	else{
		socket.emit('discover',{msg:"Already discovering"});
	}
}



