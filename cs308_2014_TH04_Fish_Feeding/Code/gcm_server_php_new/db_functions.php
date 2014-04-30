<?php
 
class DB_Functions {
 
    private $db;
 
    //put your code here
    // constructor
    function __construct() {
        include_once './db_connect.php';
        // connecting to database
        $this->db = new DB_Connect();
        $this->db->connect();
    }
 
    // destructor
    function __destruct() {
         
    }
 
    /**
     * Storing new user
     * returns user details
     */
    public function storeUser($name, $email, $gcm_regid) {
        // insert user into database
        $result = mysql_query("INSERT INTO gcm_users(name, email, gcm_regid, created_at, last_feed, feed_interval) VALUES('$name', '$email', '$gcm_regid', NOW(), ".time().", 120)");
        // check for successful store
        if ($result) {
            // get user details
            $id = mysql_insert_id(); // last inserted id
            $result = mysql_query("SELECT * FROM gcm_users WHERE id = $id") or die(mysql_error());
            // return user details
            if (mysql_num_rows($result) > 0) {
                return mysql_fetch_array($result);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    public function shouldFeed($gcm_regid) {
        // insert user into database
        $result = mysql_query("SELECT * FROM gcm_users WHERE gcm_regid = '$gcm_regid'");
        // check for successful store
        if ($result) {
            if (mysql_num_rows($result) > 0) {
                return mysql_fetch_array($result);
            } else {
                return false;
            }
        } else {
            return false;
        }
    }


    public function updateFeedTime($gcm_regid) {
        // update user into database
        $result = mysql_query("UPDATE gcm_users SET last_feed = ".time()." WHERE gcm_regid = '$gcm_regid'");
    }
	
	public function updateInterval($gcm_regid,$interval,$dosage) {
        // update user into database
        $result = mysql_query("UPDATE gcm_users SET feed_interval = '$interval', dosage = '$dosage' WHERE gcm_regid = '$gcm_regid'");
    }


 
    /**
     * Getting all users
     */
    public function getAllUsers() {
        $result = mysql_query("select * FROM gcm_users");
        return $result;
    }
	
	public function getUsers($name) {
        $result = mysql_query("select * FROM gcm_users WHERE name = '$name'");
		return $result;
		
    }
 
}
 
?>