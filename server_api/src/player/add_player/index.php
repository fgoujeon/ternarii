<?php
/*
Copyright 2018 - 2021 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

try {
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
