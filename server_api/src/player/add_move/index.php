<?php
try {
    require '../common.php';

    #input
    $player_id = $_GET["player_id"];
    $player_password = $_GET["player_password"];
    $game_id = $_GET["game_id"];
    $move_idx = $_GET["move_idx"];
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
            throw new Exception("Given game doesn't belong to given player");
        }
    } else {
        throw new Exception("No game found");
    }

    #get expected move ID
    $query = sprintf(
        "SELECT MAX(`idx`) as `max_idx` FROM `unverified_game_move` WHERE `game_id` = '%s'",
        $mysqli->real_escape_string($game_id),
    );
    $result = $mysqli->query("$query");
    if ($row = $result->fetch_assoc()) {
        $expected_move_idx = (int)$row["max_idx"] + 1;
    } else {
        $expected_move_idx = 0;
    }

    #check move ID
    if ($move_idx != $expected_move_idx)
    {
        throw new Exception("Unexpected move index. Expected " . $expected_move_idx . ", got " . $move_idx);
    }

    #insert new move
    $next_input_random_number = ternarii_random_int64();
    $query = sprintf(
        "INSERT INTO `unverified_game_move` (`game_id`, `idx`, `time`, `column_offset`, `rotation`, `next_input_random_number`) " .
        "VALUES ('%s', '%s', CURRENT_TIMESTAMP, b'%s', b'%s', '%s');",
        $mysqli->real_escape_string($game_id),
        $mysqli->real_escape_string($move_idx),
        $mysqli->real_escape_string(decbin($column_offset)),
        $mysqli->real_escape_string(decbin($rotation)),
        $mysqli->real_escape_string($next_input_random_number));
    $mysqli->query("$query");

    $output = array(
        'success' => array(
            'next_input_random_number' => $next_input_random_number
        ));
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
