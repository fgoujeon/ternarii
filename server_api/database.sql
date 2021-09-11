SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Database: `ternarii_game`
--

-- --------------------------------------------------------

--
-- Table structure for table `player`
--

CREATE TABLE `player` (
  `id` int(11) UNSIGNED NOT NULL,
  `name` char(20) COLLATE ascii_bin NOT NULL,
  `password_hash` tinytext COLLATE ascii_bin NOT NULL,
  `creation_time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin;

--
-- Dumping data for table `player`
--

-- --------------------------------------------------------

--
-- Table structure for table `unverified_game`
--

CREATE TABLE `unverified_game` (
  `id` int(10) UNSIGNED NOT NULL,
  `player_id` int(10) UNSIGNED NOT NULL,
  `stage_id` int(10) UNSIGNED NOT NULL,
  `first_input_random_number` bigint(20) NOT NULL,
  `first_next_input_random_number` bigint(20) NOT NULL,
  `is_over` tinyint(1) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin;

--
-- Dumping data for table `unverified_game`
--

-- --------------------------------------------------------

--
-- Table structure for table `unverified_game_move`
--

CREATE TABLE `unverified_game_move` (
  `game_id` int(10) UNSIGNED NOT NULL,
  `idx` smallint(5) UNSIGNED NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `column_offset` bit(3) NOT NULL,
  `rotation` bit(2) NOT NULL,
  `next_input_random_number` bigint(20) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=ascii COLLATE=ascii_bin;

--
-- Dumping data for table `unverified_game_move`
--

--
-- Indexes for dumped tables
--

--
-- Indexes for table `player`
--
ALTER TABLE `player`
  ADD PRIMARY KEY (`id`),
  ADD UNIQUE KEY `name` (`name`);

--
-- Indexes for table `unverified_game`
--
ALTER TABLE `unverified_game`
  ADD PRIMARY KEY (`id`);

--
-- Indexes for table `unverified_game_move`
--
ALTER TABLE `unverified_game_move`
  ADD UNIQUE KEY `id` (`game_id`,`idx`) USING BTREE;

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `player`
--
ALTER TABLE `player`
  MODIFY `id` int(11) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;

--
-- AUTO_INCREMENT for table `unverified_game`
--
ALTER TABLE `unverified_game`
  MODIFY `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
COMMIT;
