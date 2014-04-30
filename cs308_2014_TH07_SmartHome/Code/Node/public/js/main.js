var socket;
function disable_switches(){
    $(".on-toggle-io").bootstrapSwitch('disabled',true);
    $('.switch-edit').attr('disabled','disabled');
    $('#sensors-edit').attr('disabled','disabled');
    $('#discover-devices').attr('disabled','disabled');
}

function enable_switches(){
    $(".on-toggle-io").bootstrapSwitch('disabled',false);
    $('.switch-edit').removeAttr('disabled');
    $('#sensors-edit').removeAttr('disabled');
    $('#discover-devices').removeAttr('disabled');
}

function connection_change(elem){
        var modal = $('#attach-sensor-modal');
        var switch_id = modal.find("#switch-name-td").data("switch-id");
        var room_id = modal.find("#switch-room-td").data("room-id");
        var connection_state = elem.checked?1:0;        
        socket.emit("onoff-connection",{switchId: switch_id, roomId: room_id,connectionState: connection_state });
        if(connection_state == 1){
            modal.find("#switch-sensor-table").show();
            modal.find("#save-sensor-map-btn").show();
        }else{
            modal.find("#switch-sensor-table").hide();
            modal.find("#save-sensor-map-btn").hide();
        }    
}

$(document).ready(function(){    
    socket = io.connect(document.location.origin);

    socket.on('server-switch-state',function (data){                                
        $("#switch-"+data.roomId+"-"+data.switchId).bootstrapSwitch('state', (data.value > 0)?true:false, true);        
    });    
    socket.on('sensor-value',function(data){
        $("tr[data-room-id="+data.roomId+"] div[data-sensor-id="+data.sensorId+"]").width(String(parseFloat(data.value)/2.55) + '%').html(data.value);
    });

    socket.on('refresh',function(data){
        location.reload();
    });

    socket.on('error',function (){        
        disable_switches();
        $('#socket-error').html('Error occured, please refresh');
        $('#socket-error').show();
    });
    socket.on('reconnect_failed', function () {        
        $('#socket-error').html('Reconnecting failed');
        $('#socket-error').show();
        disable_switches();
    });
    socket.on('connect_failed', function () {        
        $('#socket-error').html('Connection to server failed, please refresh');
        $('#socket-error').show();
        disable_switches();
    });
    socket.on('reconnecting', function () {        
        $('#socket-error').html('Trying to connect');
        $('#socket-error').show();
        disable_switches();
    });
    socket.on('disconnect', function () {        
        $('#socket-error').html('Not able to connect');
        $('#socket-error').show();
        disable_switches();
    });
    socket.on('reconnect', function () {       
       enable_switches();
       $('#socket-error').html();
       $('#socket-error').hide();
    });
    socket.on('set-sensor-switch-map',function (data){
        var modal = $('#attach-sensor-modal');
        if(data.result.length == 1){
            modal.find("#auto-onoff-input").bootstrapSwitch('state', (data.result[0]['connection_state'] > 0)?true:false, true);            
            modal.find("#sensor-room-select").val(data.result[0]['sensor_room_id']).change();
            modal.find("#sensors-select-byroom").val(data.result[0]['sensor_id']);
            modal.find("#sensor-bottom-input").val(data.result[0]['range_bottom']);
            modal.find("#sensor-top-input").val(data.result[0]['range_top']);
            modal.find("#switch-in-limit-value").bootstrapSwitch('state', (data.result[0]['range_state'] > 0)?true:false, true);
            if(data.result[0]['connection_state'] == 1){
                modal.find("#switch-sensor-table").show();
                modal.find("#save-sensor-map-btn").show();
            }else{
                modal.find("#switch-sensor-table").hide();
                modal.find("#save-sensor-map-btn").hide();
            }            
        }
        else{
            modal.find("#auto-onoff-input").bootstrapSwitch('state', false, true);
            modal.find("#switch-sensor-table").hide();
            modal.find("#save-sensor-map-btn").hide();
        }

    });

    $("input[data-toggle=switch]").bootstrapSwitch();
    $(".on-toggle-io").on('switchChange.bootstrapSwitch',function(){                    
            socket.emit('switch-state',{switchId: $(this).data('switch-id'),roomId: $(this).closest('.tab-pane').data('room-id'),value:($(this).is(':checked'))?1:0});            
    });

    $('.switch-edit').click(function(){
        if($(this).data('current-state') =='edit'){
            var div = $(this).closest('div');
            var room_id = div.data('room-id');
            div.find(".switch-name").each(function (){
                var switch_name = $(this).html();
                var switch_id = $(this).data('switch-id');
                $(this).html('<input data-switch-id="'+switch_id+'" class="switch-input-'+room_id+' form-control"'+ switch_name+'" type="text" data-old-value="'+switch_name+'" value="'+switch_name+'"></input>');
            });
            var room_li_a = $("#li-room-"+room_id+" a");
            var room_name = room_li_a.html();
            room_li_a.html("<input id='room-name-input-"+room_id+"' value='"+room_name+"' class='form-control' type='text' data-old-value='"+room_name+"'></input>");
            $(this).data('current-state','save');
            $(this).html("<span class='glyphicon glyphicon-floppy-disk'> Save</span>");
        }
        else{
            var div = $(this).closest('div');
            var room_id = div.data('room-id');
            var switch_names = new Array();
            var switch_ids = new Array();
            var i=0;
            $(".switch-input-"+room_id).each(function () {
                if($(this).data('old-value') != this.value){
                    switch_names[i] = this.value;
                    switch_ids[i] = $(this).data('switch-id');
                    i++;
                }   
            });
            var room_input = $('#room-name-input-'+room_id);
            var new_room_name = room_input.val();
            var room_name_changed = false;
            if(room_input.data('old-value')!=new_room_name){
                room_name_changed = true;
            }
            socket.emit('switch-names',{switchIds: switch_ids,switchNewNames : switch_names, roomId: room_id, roomName: new_room_name, roomNameChanged : room_name_changed});
            
            div.find(".switch-name").each(function (){
                $(this).html($(this).find('input').val());                
            });

            var room_li_a = $("#li-room-"+room_id+" a");            
            room_li_a.html(room_li_a.find('input').val());

            if(room_name_changed){
                $(".room-name-marker-"+room_id).each(function () {
                    this.innerHTML = room_name;
                });
            }
            $(this).data('current-state','edit');
            this.innerHTML = "<span class='glyphicon glyphicon-edit'> Edit</span>";                    
        }
    });

    $('#sensors-edit').click(function(){
        if($(this).data('current-state') == "edit"){
            $(".sensor-name").each(function () {
                var sensor_name = this.innerHTML;                
                this.innerHTML = "<input value='"+sensor_name+"' class='form-control sensor-inputs' data-old-value='"+sensor_name+"' data-sensor-room-id = "+$(this).data('sensor-room-id')+" data-sensor-id="+$(this).data('sensor-id')+"></input>";
            });
            $(this).html("<span class='glyphicon glyphicon-floppy-disk'></span>");
            $(this).data('current-state','save');
        }
        else{
            var sensor_ids =  new Array();
            var sensor_names = new Array();
            var sensor_rooms = new Array();
            var i =0;
            $(".sensor-inputs").each(function () {                
                if(this.value != $(this).data('old-value')){                
                    sensor_ids[i] = $(this).data('sensor-id');
                    sensor_names[i] = this.value;
                    sensor_rooms[i] = $(this).data('sensor-room-id');
                    i++;
                }
            });
            if(i>0){
                socket.emit('sensor-names',{sensorIds: sensor_ids, sensorNames: sensor_names, sensorRooms: sensor_rooms});
            }
            $(".sensor-name").each(function () {
                var sensor_input = this.getElementsByTagName('input')[0];                
                this.innerHTML = sensor_input.value;
            });
            $(this).html("<span class='glyphicon glyphicon-edit'></span>");            
            $(this).data('current-state','edit');
        }
    });

    $("#discover-devices").click(function(){
        $(this).attr('disabled','disabled');
        socket.emit('discover');
        socket.on('discover',function(data){
            $("#discover-devices").html(data.msg);
        });            
    });

    $('.attach-sensors-btn').click(function(){
        var modal = $('#attach-sensor-modal');
        var switch_id = $(this).data("switch-id");
        var room_id = $(this).data("room-id");
        var switch_name = $(this).data("switch-name");
        var room_name = $(this).data("room-name");
        modal.find('#switch-name-td').html(switch_name).data("switch-id",switch_id);
        modal.find('#switch-room-td').html(room_name).data("room-id",room_id);        
        socket.emit('get-sensor-switch-map',{switchId: switch_id,roomId: room_id});
        modal.modal("show");
    });

    $("#sensor-room-select").change( function () {
        var room_sensors_ids = new Array();
        var room_sensors_names = new Array();
        var i = 0;
        var selected_room_id = $( "#sensor-room-select option:selected").val();
        $("td[data-sensor-room-id="+selected_room_id
            +"]").each(function (){
                room_sensors_ids[i] = $(this).data("sensor-id");
                room_sensors_names[i] = this.innerHTML;
                i++;
        });
        var sensors_byroom_td = document.getElementById("sensor-name-td");
        sensors_byroom_td.innerHTML ="<select id='sensors-select-byroom'></select>";
        var sensor_select = document.getElementById("sensors-select-byroom");
        for (var j = 0; j < i; j++ ){
            sensor_select.innerHTML = sensor_select.innerHTML + "<option value="+room_sensors_ids[j]+">"+room_sensors_names[j]+"</option>";
        }
        if(j > 0){
            $("#save-sensor-map-btn").show();
        }
    });
    
    $("#save-sensor-map-btn").click(function (){        
        var switch_id = $("#switch-name-td").data('switch-id');
        var switch_room_id = $("#switch-room-td").data('room-id');
        var sensor_room_id = $("#sensor-room-select option:selected").val();
        var sensor_id = $("#sensors-select-byroom option:selected").val();
        var sensor_btm = $("#sensor-bottom-input").val();
        var sensor_top = $("#sensor-top-input").val();
        var switch_state_in_limit = ($("#switch-in-limit-value").is(':checked'))?1:0;        
        socket.emit('add-sensor-map',{switchId: switch_id,switchRoomId: switch_room_id,sensorRoomId: sensor_room_id, sensorId: sensor_id, sensorBtm: sensor_btm, sensorTop: sensor_top,switchStateInLimit: switch_state_in_limit});
    });

    $("#auto-onoff-input").on('switchChange.bootstrapSwitch',function (){
        connection_change(this);
    });
})
