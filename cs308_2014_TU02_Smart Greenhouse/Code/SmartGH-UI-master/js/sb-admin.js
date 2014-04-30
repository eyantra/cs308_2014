const MIN_CAMERA_ID = 0
const MAX_CAMERA_ID = 1
const VIDEO_FEED_DELAY = 4250;
const VIDEO_FEED_SRC_URL = "get_current_view.php?camera="

const SENSOR_READINGS_COUNT = 7;
const SENSOR_READINGS_DELAY = 2000;

var current_camera = 0;
var video_feed_handler;
var sensor_readings_handler;

$(function() {
    $("#pause-sensors-menu-item")
        .data("state", "paused")
        .click(function() {
            if($(this).data("state") == "play") {
                $(this).data("state", "paused").children("i").removeClass("fa-pause").addClass("fa-play");
                $("[id$=-progress]").removeClass("progress-striped active");
                window.clearInterval(sensor_readings_handler);
            } else {
                $(this).data("state", "play").children("i").removeClass("fa-play").addClass("fa-pause");
                $("[id$=-progress]").addClass("progress-striped active");
                update_sensor_readings();
                sensor_readings_handler = window.setInterval(update_sensor_readings, SENSOR_READINGS_DELAY);
            }
        })
        .trigger("click");

    $("#pause-video-menu-item")
        .data("state", "paused")
        .click(function() {
            if($(this).data("state") == "play") {
                $(this).data("state", "paused").children("i").removeClass("fa-pause").addClass("fa-play");
                window.clearInterval(video_feed_handler);
            } else {
                $(this).data("state", "play").children("i").removeClass("fa-play").addClass("fa-pause");
                update_video_feed();
                video_feed_handler = window.setInterval(update_video_feed, VIDEO_FEED_DELAY);
            }
        })
        .trigger("click");

    $("#capture-video-menu-item").click(function() {window.location.href = VIDEO_FEED_SRC_URL + current_camera;});

    $("#tempSlider").slider({
        range: "max",
        min: 10, max: 50,
        change: function(event, ui) {
            $("#temperature").val(ui.value);
        },
        slide: function(event, ui) {
            $("#reset-thresholds-button").addClass("btn-primary");
        }
    });

    $( "#lighSlider" ).slider({
        range: "min",
        min: 16, max: 100,
        change: function( event, ui ) {
            $("#light").val(ui.value);
        },
        slide: function(event, ui) {
            $("#reset-thresholds-button").addClass("btn-primary");
        }
    });

    $( "#humiSlider" ).slider({
        range: "min",
        min: 20, max: 80,
        change: function(event, ui) {
	    $("#humidity").val(ui.value);
        },
        slide: function(event, ui) {
            $("#reset-thresholds-button").addClass("btn-primary");
        }
    });

    $("#reset-thresholds-button").click(function() {
        $("#reset-thresholds-button").removeClass("btn-primary");
    });

    $("#submit-thresholds-button").click(function() {
        $.post("submit_thresholds.php",
               {   T: $("#tempSlider").slider("value"),
                   L: $("#lighSlider").slider("value"),
                   RH: $("#humiSlider").slider("value") },
               function (data) {create_thresholds_alert(data);$("#reset-thresholds-button").removeClass("btn-primary");},
               "json")
         .fail(function (data) {create_thresholds_alert({"res": false, "msg": "No connection."});});
    });
});

function create_thresholds_alert(data) {
    $("#thresholds-update-alerts").empty().append(
        '<div id="thresholds-alert" class="alert alert-' + (data.res ? 'success' : 'danger') + '">' +
        '<button type="button" class="close" data-dismiss="alert" aria-hidden="true"><i class="fa fa-times"></i></button>' +
        '<strong>' + (data.res ? 'Success : ' : 'Error : ') + '</strong> ' + data.msg + '</div>'
    );
    setTimeout(function() {$("#thresholds-update-alerts").empty();}, 3000);
}

function update_video_feed() {
    $("#video-feed-area").attr("src", VIDEO_FEED_SRC_URL + current_camera);
}

function update_sensor_readings() {
    $.getJSON("get_last_readings.php?limit=" + SENSOR_READINGS_COUNT, function(data) {
       if (!$("#reset-thresholds-button").hasClass("btn-primary")) {
            $("#tempSlider").slider("value", data.tT);
            $("#lighSlider").slider("value", data.tL);
            $("#humiSlider").slider("value", data.tRH);
        }

        $("#notification-panel").empty();
        $.map(data.evts, function(evt) {
            $("#notification-panel").prepend(
                "<a href='#' class='list-group-item "
                + (evt.stat == 0 ? "" : "list-group-item-warning") + "'><i class='fa "
                + (evt.stat == 0 ? "fa-check" : "fa-warning") + " fa-fw'></i> &nbsp;"
                + evt.msg + "<span class='pull-right text-muted small'><em>"
                + evt.T + "</em></span></a>"
        );});

        $("#sensor-readings-table > tbody").empty();
        $.map(data.vals, function(reading) {$("#sensor-readings-table > tbody").prepend(
            "<tr><td>" + reading.T
            + "</td><td>" + reading.t0 + "</td><td>" + reading.l0 + "</td><td>" + reading.h0 
            + "</td><td>" + reading.t1 + "</td><td>" + reading.l1 + "</td><td>" + reading.h1
            + "</td></tr>"
        );});
 
        var last = data.vals.pop();
        update_progress_bar($("#current-t0-progress .progress-bar"), last.t0, " \xB0C");
        update_progress_bar($("#current-l0-progress .progress-bar"), last.l0, " %");
        update_progress_bar($("#current-h0-progress .progress-bar"), last.h0, " %");
        update_progress_bar($("#current-t1-progress .progress-bar"), last.t1, " \xB0C");
        update_progress_bar($("#current-l1-progress .progress-bar"), last.l1, " %");
        update_progress_bar($("#current-h1-progress .progress-bar"), last.h1, " %");

        if ((last.t0 + last.t1)/2 > $("#tempSlider").slider("value")) {
            $("#current-t0-progress .progress-bar").addClass("progress-bar-warning");
            $("#current-t1-progress .progress-bar").addClass("progress-bar-warning");
        } else {
            $("#current-t0-progress .progress-bar").removeClass("progress-bar-warning");
            $("#current-t1-progress .progress-bar").removeClass("progress-bar-warning");
        }

        if ((last.l0 + last.l1)/2 < $("#lighSlider").slider("value")) {
            $("#current-l0-progress .progress-bar").addClass("progress-bar-warning");
            $("#current-l1-progress .progress-bar").addClass("progress-bar-warning");
        } else {
            $("#current-l0-progress .progress-bar").removeClass("progress-bar-warning");
            $("#current-l1-progress .progress-bar").removeClass("progress-bar-warning");
        }

        if ((last.h0 + last.h1)/2 < $("#humiSlider").slider("value")) {
            $("#current-h0-progress .progress-bar").addClass("progress-bar-warning");
            $("#current-h1-progress .progress-bar").addClass("progress-bar-warning");
        } else {
            $("#current-h0-progress .progress-bar").removeClass("progress-bar-warning");
            $("#current-h1-progress .progress-bar").removeClass("progress-bar-warning");
        }
    });
}

function update_progress_bar(pgb, val, suffix) {
    $(pgb).css("width", ((100.0 * (val - $(pgb).attr("aria-valuemin"))) / ($(pgb).attr("aria-valuemax") - $(pgb).attr("aria-valuemin"))) + "%")
          .attr("aria-valuenow", val)
          .children("span").text(val + suffix);
}
