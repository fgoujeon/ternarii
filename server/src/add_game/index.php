<?php
#input
$player_id = $_GET["player_id"];
$player_password = $_GET["player_password"];
$stage_id = $_GET["stage_id"];

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

#create game
$query = sprintf(
    "INSERT INTO `unverified_game` (`id`, `player_id`, `stage_id`, `first_input_random_number`, `first_next_input_random_number`, `is_over`) " .
    "VALUES (NULL, '%s', '%s', '%s', '%s', '0');",
    $mysqli->real_escape_string($player_id),
    $mysqli->real_escape_string($stage_id),
    rand(),
    rand()
);
$result = $mysqli->query("$query");

#return game ID
echo $mysqli->insert_id;
?>
