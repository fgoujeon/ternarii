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
        throw new Exception("No player found");
    }

    #verify player password
    if (!password_verify($player_password, $player_password_hash)) {
        throw new Exception("Incorrect password");
    }
}

function ternarii_random_int64() {
    #We need cryptographically secure random, because we want the RNG to be
    #unpredictable to the player.
    return random_int(
        -0x8000000000000000, #-2^63
        0x7FFFFFFFFFFFFFFF #2^63 - 1
    );
}

?>
