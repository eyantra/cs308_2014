<?php

$lim = $_GET['limit'] ? $_GET['limit'] : 8;

$data = shell_exec("tail -n $lim /var/log/serialmon/serialmon.dat");
$data_arr = explode("\n", $data);
array_pop($data_arr);

$data_arr = array_map(
    function($line) {
        $num_arr = preg_split("/\s+/", $line);
        return array("T" => date("d-m H:i:s", $num_arr[0]),

                     "t0" => intval($num_arr[1]),
                     "l0" => intval($num_arr[2]),
                     "h0" => intval($num_arr[3]),

                     "t1" => intval($num_arr[4]),
                     "l1" => intval($num_arr[5]),
                     "h1" => intval($num_arr[6]));
    },
    $data_arr);

$evt_lim = $_GET['events'] ? $_GET['events'] : 4;

$events = shell_exec("tail -n $evt_lim /var/log/serialmon/events.dat");
$events_arr = explode("\n", $events);
array_pop($events_arr);

$events_arr = array_map(
    function($line) {
        $arr = preg_split("/\t/", $line);
        return array("T"    => date("d-m H:i:s", $arr[0]),
                     "stat" => $arr[1],
		     "msg"  => $arr[2]);
    },
    $events_arr);


$THRESHOLDS_PATH = "/var/opt/serialmon/thresholds";
echo json_encode(array(
    "vals" => $data_arr,
    "evts" => $events_arr,
    "tT"   => intval(trim(file_get_contents("$THRESHOLDS_PATH/T"))),
    "tL"   => intval(trim(file_get_contents("$THRESHOLDS_PATH/L"))),
    "tRH"  => intval(trim(file_get_contents("$THRESHOLDS_PATH/RH")))
));

?>
