<?php
echo "<html><body> 
<p>sending message</p>";
if (isset($_GET["regId"]) && isset($_GET["message"])) {
	echo "<p>sending message</p>";
    $regId = $_GET["regId"];
    $message = $_GET["message"];
     
    include_once './GCM.php';
     
    $gcm = new GCM();
 
    $registatoin_ids = array($regId);
    $message = array("price" => $message);
   
	echo "message created";
    $result = $gcm->send_notification($registatoin_ids, $message);
    echo $result;
	echo "sent message";
}
echo "</body></html>";
?>