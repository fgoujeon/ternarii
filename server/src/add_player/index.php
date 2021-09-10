<?php
require '../common.php';

#input
$name = $_GET["name"];
$password = $_GET["password"];

#connect to DB
$mysqli = ternarii_db_connect();

#hash password
$password_hash = password_hash("$password", PASSWORD_BCRYPT);

#insert new player
$query = sprintf("INSERT INTO `player` (`id`, `name`, `password_hash`, `creation_time`) VALUES (NULL, '%s', '%s', CURRENT_TIMESTAMP);",
    $mysqli->real_escape_string($name),
    $mysqli->real_escape_string($password_hash));
$mysqli->query("$query");

print("Success");
?>
