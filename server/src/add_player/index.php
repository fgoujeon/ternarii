<?php
try
{
    require '../common.php';

    #input
    $name = $_GET["name"];
    $password = $_GET["password"];

    #connect to DB
    $mysqli = ternarii_db_connect();

    #hash password
    $password_hash = password_hash("$password", PASSWORD_BCRYPT);

    #insert new player
    $query = sprintf(
        "INSERT INTO `player` (`id`, `name`, `password_hash`, `creation_time`) " .
        "VALUES (NULL, '%s', '%s', CURRENT_TIMESTAMP);",
        $mysqli->real_escape_string($name),
        $mysqli->real_escape_string($password_hash));
    $mysqli->query("$query");

    $output = array('success' => array('player_id' => $mysqli->insert_id));
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
