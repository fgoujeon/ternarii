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
        "SELECT `id`, `first_input_random_number`, `first_next_input_random_number` FROM `unverified_game` WHERE `player_id` = '%s' AND `stage_id` = '%s' AND `is_over` = FALSE LIMIT 1",
        $mysqli->real_escape_string($player_id),
        $mysqli->real_escape_string($stage_id));
    $result = $mysqli->query("$query");
    if ($row = $result->fetch_assoc()) {
        $has_unfinished_game = TRUE;
        $game_id = (int)$row["id"];
        $first_input_random_number = (int)$row["first_input_random_number"];
        $first_next_input_random_number = (int)$row["first_next_input_random_number"];
    } else {
        $has_unfinished_game = FALSE;
    }

    #create game if there's no unfinished game
    if(!$has_unfinished_game) {
        $first_input_random_number = ternarii_random_int64();
        $first_next_input_random_number = ternarii_random_int64();
        $query = sprintf(
            "INSERT INTO `unverified_game` (`id`, `player_id`, `stage_id`, `first_input_random_number`, `first_next_input_random_number`, `is_over`) " .
            "VALUES (NULL, '%s', '%s', '%s', '%s', '0');",
            $mysqli->real_escape_string($player_id),
            $mysqli->real_escape_string($stage_id),
            $first_input_random_number,
            $first_next_input_random_number);
        $result = $mysqli->query("$query");
        $game_id = $mysqli->insert_id;
    }

    #get move list (necessarily empty for a new game)
    $moves = array();
    if($has_unfinished_game) {
        $query = sprintf(
            "SELECT `idx`, `column_offset`, `rotation`, `next_input_random_number` ".
            "FROM `unverified_game_move` ".
            "WHERE `game_id` = '%s'",
            $mysqli->real_escape_string($game_id));
        $result = $mysqli->query("$query");
        while ($row = $result->fetch_assoc()) {
            $moves[$row["idx"]] = array(
                'column_offset' => (int)$row["column_offset"],
                'rotation' => (int)$row["rotation"],
                'next_input_random_number' => (int)$row["next_input_random_number"]);
        }
    }

    #return info
    $output = array(
        'success' => array(
            'game_id' => $game_id,
            'first_input_random_number' => $first_input_random_number,
            'first_next_input_random_number' => $first_next_input_random_number,
            'moves' => $moves));
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
