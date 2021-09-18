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
    $player_name = $_GET["player_name"];

    #connect to DB
    $mysqli = ternarii_db_connect();

    #get player id
    $query = sprintf(
        "SELECT `id` FROM `player` WHERE `name` = '%s' LIMIT 1",
        $mysqli->real_escape_string($player_name));
    $result = $mysqli->query("$query");
    if ($row = $result->fetch_assoc()) {
        $player_id = (int)$row["id"];
    } else {
        throw new Exception("Unknown player name.");
    }

    $output = array('success' => array('id' => $player_id));
    echo json_encode($output);
} catch (Exception $ex) {
    $output = array('failure' => array('message' => $ex->getMessage()));
    echo json_encode($output);
}
?>
