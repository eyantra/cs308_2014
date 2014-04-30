<?php
    session_start();
    if(isset($_SESSION['login']) && $_SESSION['login']=="true") {
        if(!(isset($_POST) && isset($_POST["T"]) && isset($_POST["L"]) && isset($_POST["RH"])))
            die(json_encode(array("res" => false, "msg" => "Invalid request.")));

        $THRESHOLDS_PATH = "/var/opt/serialmon/thresholds";

        $file = fopen("$THRESHOLDS_PATH/T", "w") or die(json_encode(array("res" => false, "msg" => "Write failed.")));
        fwrite($file,$_POST["T"]);
        fclose($file);
	
        $file = fopen("$THRESHOLDS_PATH/L", "w") or die(json_encode(array("res" => false, "msg" => "Write failed.")));
        fwrite($file,$_POST["L"]);
        fclose($file);
	
        $file = fopen("$THRESHOLDS_PATH/RH", "w") or die(json_encode(array("res" => false, "msg" => "Write failed.")));
        fwrite($file,$_POST["RH"]);
        fclose($file);
	
        die(json_encode(array("res" => true, "msg" => "Thresholds updated.")));
    } else
        die(json_encode(array("res" => false, "msg" => "Login Required.")));
?>
