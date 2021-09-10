<?php
try
{
    require '../common.php';

    #input
    $player_id = $_GET["player_id"];
    $player_password = $_GET["player_password"];
    $stage_id = $_GET["stage_id"];

    #connect to DB and check player password
    $mysqli = ternarii_db_connect();
    ternarii_verify_player_password($mysqli, $player_id, $player_password);

    #get unfinished game, if any
    $query = sprintf(
        "SELECT `id` FROM `unverified_game` WHERE `player_id` = '%s' AND `stage_id` = '%s' AND `is_over` = FALSE LIMIT 1",
        $mysqli->real_escape_string($player_id),
        $mysqli->real_escape_string($stage_id));
    $result = $mysqli->query("$query");
    if ($row = $result->fetch_assoc()) {
        $output = array('success' => array('game_id' => $row["id"]));
        echo json_encode($output);
        exit();
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
    $output = array('success' => array('game_id' => $mysqli->insert_id));
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
