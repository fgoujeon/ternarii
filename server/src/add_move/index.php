<?php
require '../common.php';

#input
$player_id = $_GET["player_id"];
$player_password = $_GET["player_password"];
$game_id = $_GET["game_id"];
$move_id = $_GET["move_id"];
$column_offset = $_GET["column_offset"];
$rotation = $_GET["rotation"];

#connect to DB and check player password
$mysqli = ternarii_db_connect();
ternarii_verify_player_password($mysqli, $player_id, $player_password);

#check game belongs to given player
$query = sprintf(
    "SELECT `player_id` FROM `unverified_game` WHERE `id` = '%s' LIMIT 1",
    $mysqli->real_escape_string($game_id));
$result = $mysqli->query("$query");
if ($row = $result->fetch_assoc()) {
    if ($player_id !== $row["player_id"]) {
        exit("Given game doesn't belong to given player");
    }
} else {
    exit("No game found");
}

#get expected move ID
$query = sprintf(
    "SELECT MAX(`id`) as `max_id` FROM `unverified_game_move` WHERE `game_id` = '%s'",
    $mysqli->real_escape_string($game_id),
);
$result = $mysqli->query("$query");
if ($row = $result->fetch_assoc()) {
    $expected_move_id = (int)$row["max_id"] + 1;
} else {
    $expected_move_id = 0;
}

#check move ID
if ($move_id != $expected_move_id)
{
    exit("Unexpected move ID. Expected " . $expected_move_id . ", got " . $move_id);
}

#insert new move
$next_input_random_number = rand();
$query = sprintf(
    "INSERT INTO `unverified_game_move` (`game_id`, `id`, `time`, `column_offset`, `rotation`, `next_input_random_number`) " .
    "VALUES ('%s', '%s', CURRENT_TIMESTAMP, b'%s', b'%s', '%s');",
    $mysqli->real_escape_string($game_id),
    $mysqli->real_escape_string($move_id),
    $mysqli->real_escape_string(decbin($column_offset)),
    $mysqli->real_escape_string(decbin($rotation)),
    $mysqli->real_escape_string($next_input_random_number));
$mysqli->query("$query");

echo $next_input_random_number;
?>
