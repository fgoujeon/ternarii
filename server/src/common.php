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

function ternarii_verify_player_password($mysqli, $player_id, $player_password) {
    #get player password hash
    $query = sprintf("SELECT `password_hash` FROM `player` WHERE `id` = '%s' LIMIT 1",
        $mysqli->real_escape_string($player_id));
    $result = $mysqli->query("$query");
    if ($row = $result->fetch_assoc()) {
        $player_password_hash = $row["password_hash"];
    } else {
        exit("No player found");
    }

    #verify player password
    if (!password_verify($player_password, $player_password_hash)) {
        exit("Incorrect password");
    }
}

?>
