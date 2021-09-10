<?php
try {
    require '../common.php';

    #input
    $player_id = $_GET["player_id"];
    $player_password = $_GET["player_password"];
    $game_id = $_GET["game_id"];

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

    #set game as over
    $query = sprintf(
        "UPDATE `unverified_game` SET `is_over` = '1' WHERE `unverified_game`.`id` = '%s';",
        $mysqli->real_escape_string($game_id));
    $mysqli->query("$query");

    $output = array('success' => array());
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
