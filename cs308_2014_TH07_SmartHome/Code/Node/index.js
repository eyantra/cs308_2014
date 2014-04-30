var express = require("express");
var app = express();
var port = 3700;
var serialcom = require('./serialcom.js');
var commands = require('./commands.js');
var xbee_path = "/dev/ttyUSB0";
serialcom.open(xbee_path,9600,commands.runCommand);

app.set('views', __dirname + '/tpl');
app.set('view engine', "jade");
app.engine('jade', require('jade').__express);


var sqlInfo = {
    host     : 'localhost',
    user     : 'smarthome',
    password : 'smarthome',
  database: 'smarthome'
}

var mysql      = require('mysql');
var db_con = mysql.createConnection(sqlInfo);
db_con.connect(function(err) {
  // connected! (unless `err` is set)
    if(err != null)
      console.log("Database error : "+err);
});

app.get("/", function(req, res){    
  var query = db_con.query(
    'SELECT r.id as room_id, r.name as room_name, s.id as switch_id, s.name as switch_name, s.state as state FROM switches s JOIN rooms r on s.room_id=r.id ORDER BY room_id, switch_id', function(err, result) {      
      if(err != null){
        console.log("" + err);
        return;
      }
      var prev_room_id = -1;
      var rooms_list = new Array();
      var switches_by_room = new Array();
      var switch_i = 0;
      var room_i = -1;
      for (var i=0;i<result.length;i++){
        if(prev_room_id != result[i]['room_id']){
          room_i++;
          switches_by_room[room_i] = new Array();
          switches_by_room[room_i]['room_name'] = result[i]['room_name'];
          switches_by_room[room_i]['room_id'] = result[i]['room_id'];
          prev_room_id = result[i]['room_id'];
          switch_i=0;
        }
        switches_by_room[room_i][switch_i] = new Array();
        switches_by_room[room_i][switch_i]['name'] = result[i]['switch_name'];
        switches_by_room[room_i][switch_i]['id'] = result[i]['switch_id'];
        switches_by_room[room_i][switch_i]['state'] = result[i]['state'];
        switch_i++;
      }
      db_con.query('SELECT s.id as sensor_id, r.id as room_id, s.type as type, s.value as value, s.name as sensor_name, r.name as room_name  FROM sensors s JOIN rooms r on s.room_id=r.id ORDER BY s.room_id,s.id',function(err,result){        
        if(err != null){
          console.log(err);
          return;
        }
        res.render('page', {"switchlist" : switches_by_room, "sensorlist" : result});
      });      
  });    
});
 

app.use(express.static(__dirname + '/public'));

var io = require('socket.io').listen(app.listen(port));
commands.setConn(io.sockets,db_con,serialcom);
io.set('log level', 1);

io.sockets.on('connection', function (socket) {
    socket.on('switch-state', function (data) {
      var query = db_con.query('UPDATE switches set switches.state = ? WHERE switches.id = ? and switches.room_id = ?;',[(data.value > 0)?1:0, data.switchId,data.roomId], function(err, result) {          
          console.log(data.switchId+" "+data.roomId+" is "+data.value);
          io.sockets.emit('server-switch-state',data);
          commands.switchCommand(data.roomId,data.switchId,(data.value > 0)?1:0);
      }); 
    });

    socket.on('discover',function (data){
      commands.discoverCommand(socket);    
    });   

    socket.on('switch-names',function (data) {      
      if(data.roomNameChanged){
        var query = db_con.query('UPDATE rooms set rooms.name = ? WHERE rooms.id = ?;',[data.roomName,data.roomId], function(err,result){
          if(err != null){
            throw err;
          }          
        });

      }
      for(var i=0; i< data.switchNewNames.length; i++){
        var query = db_con.query('UPDATE switches set switches.name = ? WHERE switches.id = ? and switches.room_id = ?',[data.switchNewNames[i],data.switchIds[i],data.roomId],function (err,result){
          if(err != null){
            throw err;
          }
        });
      }
    });
    socket.on('sensor-names',function (data){
      console.log(data.sensorIds);
      for(var i=0; i<data.sensorIds.length; i++){
        var query = db_con.query('UPDATE sensors set sensors.name= ? WHERE sensors.id = ? and sensors.room_id = ?;',[data.sensorNames[i],data.sensorIds[i],data.sensorRooms[i]],function(err,result){
          if(err != null){
            throw err;
          }
        });
      }
    });
    socket.on('get-sensor-switch-map',function (data){
      var query = db_con.query('Select * from sensor_switch_maps where switch_id = ? and switch_room_id = ?',[data.switchId,data.roomId],function (err,result){
        if(err != null){
          throw err;
        }
        socket.emit('set-sensor-switch-map',{result: result});
      });
    });
    socket.on('add-sensor-map',function (data){
      console.log("here "+data.sensorRoomId+" "+data.switchRoomId);
      var query = db_con.query('Select * from sensor_switch_maps where switch_id = ? and switch_room_id = ?;',[data.switchId,data.switchRoomId],function (err,result){
        if(err != null){
          throw err;
        }
        //console.log("here2");
        if(result.length < 1){
          var new_query = db_con.query("INSERT INTO sensor_switch_maps (sensor_id, switch_id, sensor_room_id, switch_room_id, connection_state,range_bottom, range_top, range_state) VALUES (?, ?, ?, ?, 1, ?, ?, ?)",[data.sensorId,data.switchId,data.sensorRoomId,data.switchRoomId,data.sensorBtm,data.sensorTop,data.switchStateInLimit],function (err,result){
            if(err != null){
              throw err;
            }
            //console.log("here3");
          });
        }
        else{
          var new_query = db_con.query("UPDATE sensor_switch_maps set sensor_id = ?, sensor_room_id = ?, connection_state = 1,range_bottom = ?, range_top = ?, range_state = ? WHERE switch_id = ? and switch_room_id = ?;",[data.sensorId,data.sensorRoomId,data.sensorBtm,data.sensorTop,data.switchStateInLimit,data.switchId,data.switchRoomId],function (err,result){
            if(err != null){
              throw err;
            }
            //console.log("updated for "+data.switchId);
          });
        }
      });
    });
    socket.on('onoff-connection',function (data){
      //console.log("wassaaa here too");
      var new_query = db_con.query("UPDATE sensor_switch_maps set connection_state = ? where switch_id = ? and switch_room_id = ?;",[data.connectionState,data.switchId,data.roomId],function (err,result){
        if(err != null){
              throw err;
            }
            //console.log("updated for "+data.switchId);
      });
    });
});

console.log("Listening on port " + port);

setInterval(function() {  
  db_con.query('SELECT s.switch_id, s.switch_room_id, s1.value, s.range_bottom, s.range_top,s.range_state FROM sensor_switch_maps s JOIN sensors s1 on s1.id = s.sensor_id and s1.room_id = sensor_room_id where s.connection_state = 1',function(err, result){      
      if(err != null){
        throw err;
      }
      for(var i =0; i < result.length; i++){
        var state;
        if(result[i]['value'] >= result[i]['range_bottom'] && result[i]['value'] <= result[i]['range_top']){
          state = result[i]['range_state'];          
        }else{
          state = (result[i]['range_state'] + 1)%2;          
        }
        (function(state,i,result){
          db_con.query('UPDATE switches set switches.state = ? WHERE switches.id = ? and switches.room_id = ?;',[state, result[i]['switch_id'],result[i]['switch_room_id']], function(err, result1) {                    
            io.sockets.emit('server-switch-state',{switchId: result[i]['switch_id'], roomId: result[i]['switch_room_id'], value:state});
            commands.switchCommand(result[i]['switch_room_id'],result[i]['switch_id'],state);
          });          
        })(state,i,result)
      }
  });    
}, 2000);
