<?php

function ternarii_db_connect() {
    #get DB credentials
    $config = include($_SERVER["DOCUMENT_ROOT"] . "/../config.php");
    $db_hostname = $config["db_hostname"];
    $db_name = $config["db_name"];
    $db_username = $config["db_user_username"];
    $db_password = $config["db_user_password"];

    #connect to DB
    mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
    $mysqli = new mysqli($db_hostname, $db_username, $db_password, $db_name);
    $mysqli->set_charset('utf8mb4');

    return $mysqli;
}

?>
