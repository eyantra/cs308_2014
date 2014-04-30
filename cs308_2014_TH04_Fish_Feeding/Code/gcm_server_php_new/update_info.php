<?php

include_once 'db_functions.php';
$db = new DB_Functions();

$db->updateInterval($_GET["regId"],$_GET["feed_interval"],$_GET["dose"]);
$name = $_GET["name"];
header( 'Location: ./index.php?name='.$name);

?>