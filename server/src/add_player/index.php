<?php
#input
$name = $_GET["name"];
$password = $_GET["password"];

#get DB credentials
$config = include($_SERVER["DOCUMENT_ROOT"] . "/../config.php");
$db_hostname = $config["db_hostname"];
$db_name = $config["db_name"];
$db_username = $config["db_user_username"];
$db_password = $config["db_user_password"];

#hash password
$password_hash = password_hash("$password", PASSWORD_BCRYPT);

#connect to DB
mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);
$mysqli = new mysqli($db_hostname, $db_username, $db_password, $db_name);
$mysqli->set_charset('utf8mb4');

#insert new player
$query = sprintf("INSERT INTO `player` (`id`, `name`, `password_hash`, `creation_time`) VALUES (NULL, '%s', '%s', CURRENT_TIMESTAMP);",
    $mysqli->real_escape_string($name),
    $mysqli->real_escape_string($password_hash));
$mysqli->query("$query");

print("Success");
?>
