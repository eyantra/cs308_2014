<?php
include_once 'db_functions.php';
include_once './GCM.php';
$db = new DB_Functions();
$last = $db->shouldFeed($_GET["reg_id"]);
$lastfeed = time() - $last["last_feed"];
if($lastfeed >= $last["feed_interval"]){
	//header( 'Location: /send_message.php?regId='+$_GET["reg_id"]+'&message=a' );
	//http_redirect("/send_message.php?regId="+$_GET["reg_id"]+"&message=a", array(), true, HTTP_REDIRECT_PERM);
	
	
     
    $gcm = new GCM();
	$regId = $_GET["reg_id"];
	$message = $last["dosage"];
    $registatoin_ids = array($regId);
    $message = array("price" => $message);
   
	echo "message created";
    $result = $gcm->send_notification($registatoin_ids, $message);
    echo $result;
	echo "sent message";
	/*?>
	<html>
	<body>
	<form id="<?php echo $row["id"] ?>" name="hidform" method="GET" action="send_message.php">
        <input type="hidden" name="regId" value="<?php echo $_GET["reg_id"] ?>"/>
		<input type="hidden" name="message" value="a"/>
    </form>
	<script> document.hidform.submit(); </script>
	</body>
	</html>
	
	<?php*/
	$db->updateFeedTime($_GET["reg_id"]);
}
else{
echo $lastfeed."<br/>";
}
?>
