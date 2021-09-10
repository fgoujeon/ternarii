<?php
require '../common.php';

#input
$player_id = $_GET["player_id"];
$player_password = $_GET["player_password"];
$stage_id = $_GET["stage_id"];

#connect to DB
$mysqli = ternarii_db_connect();

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

#get unfinished game, if any
$query = sprintf(
    "SELECT `id` FROM `unverified_game` WHERE `player_id` = '%s' AND `stage_id` = '%s' AND `is_over` = FALSE LIMIT 1",
    $mysqli->real_escape_string($player_id),
    $mysqli->real_escape_string($stage_id));
$result = $mysqli->query("$query");
if ($row = $result->fetch_assoc()) {
    exit($row["id"]);
}

#create game
$query = sprintf(
    "INSERT INTO `unverified_game` (`id`, `player_id`, `stage_id`, `first_input_random_number`, `first_next_input_random_number`, `is_over`) " .
    "VALUES (NULL, '%s', '%s', '%s', '%s', '0');",
    $mysqli->real_escape_string($player_id),
    $mysqli->real_escape_string($stage_id),
    rand(),
    rand());
$result = $mysqli->query("$query");

#return game ID
echo $mysqli->insert_id;
?>
