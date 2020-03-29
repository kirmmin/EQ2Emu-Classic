# HeidiSQL Dump 
#
# --------------------------------------------------------
# Host:                 69.60.121.221
# Database:             eq2_login
# Server version:       5.0.27
# Server OS:            redhat-linux-gnu
# Target-Compatibility: Standard ANSI SQL
# HeidiSQL version:     3.2 Revision: 1129
# --------------------------------------------------------

/*!40100 SET CHARACTER SET latin1;*/
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='ANSI';*/
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;*/


DROP TABLE IF EXISTS "account";

#
# Table structure for table 'account'
#

CREATE TABLE "account" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "name" varchar(64) NOT NULL default '',
  "passwd" varchar(64) NOT NULL default '',
  "ip_address" varchar(50) NOT NULL default '0',
  "email_address" varchar(50) NOT NULL default 'Unknown',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "Name" ("name")
) AUTO_INCREMENT=3633 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "appearances";

#
# Table structure for table 'appearances'
#

CREATE TABLE "appearances" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "appearance_id" int(10) unsigned NOT NULL,
  "name" varchar(250) default NULL,
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "AppID" ("appearance_id")
) AUTO_INCREMENT=15933 /*!40100 DEFAULT CHARSET=latin1*/;

DROP TABLE IF EXISTS "bugs";

#
# Table structure for table 'bugs'
#

CREATE TABLE "bugs" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "Status" enum('New','Invalid','Fixed') NOT NULL default 'New',
  "world_id" int(10) unsigned NOT NULL default '0',
  "account_id" int(10) unsigned NOT NULL default '0',
  "player" varchar(64) NOT NULL default ' ',
  "category" varchar(64) NOT NULL default ' ',
  "subcategory" varchar(64) NOT NULL default ' ',
  "causes_crash" varchar(64) NOT NULL default ' ',
  "reproducible" varchar(64) NOT NULL default ' ',
  "summary" varchar(128) NOT NULL default ' ',
  "description" text NOT NULL,
  "version" varchar(32) NOT NULL default ' ',
  "spawn_name" varchar(64) NOT NULL default 'N/A',
  "spawn_id" int(10) unsigned NOT NULL default '0',
  "bug_datetime" timestamp NOT NULL default CURRENT_TIMESTAMP,
  "zone_id" int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=38 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "command_list";

#
# Table structure for table 'command_list'
#

CREATE TABLE "command_list" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "type" enum('NPC','OBJECT','PLAYER') NOT NULL default 'NPC',
  "sub_type" enum('NORMAL','DEFAULT','DEFAULT_ATTACKABLE') NOT NULL default 'NORMAL',
  "spawn_id" int(10) unsigned NOT NULL default '0',
  "distance" float NOT NULL default '0',
  "command_text" varchar(32) NOT NULL default '',
  "command" varchar(32) NOT NULL default '',
  "comment" varchar(255) NOT NULL default '',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=6 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "commands";

#
# Table structure for table 'commands'
#

CREATE TABLE "commands" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "name" varchar(64) NOT NULL default '',
  "subcommand" varchar(64) NOT NULL,
  "handler" int(10) unsigned NOT NULL default '0',
  "required_status" smallint(5) NOT NULL default '0',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "NewIndex" ("name","subcommand")
) AUTO_INCREMENT=217 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "download_limits";

#
# Table structure for table 'download_limits'
#

CREATE TABLE "download_limits" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "username" varchar(64) default NULL,
  "ip_address" varchar(20) default NULL,
  "table_name" varchar(64) default NULL,
  "table_version" int(10) unsigned NOT NULL default '0',
  "data_version" int(10) unsigned NOT NULL default '0',
  "num_data_downloads" tinyint(3) unsigned NOT NULL default '0',
  "num_table_downloads" tinyint(3) unsigned NOT NULL default '0',
  "update_time" timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  ("id"),
  UNIQUE KEY "id" ("id"),
  KEY "id_2" ("id")
) /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "download_tables";

#
# Table structure for table 'download_tables'
#

CREATE TABLE "download_tables" (
  "id" int(3) unsigned NOT NULL auto_increment,
  "table_name" varchar(64) NOT NULL default '',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=32 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "factions";

#
# Table structure for table 'factions'
#

CREATE TABLE "factions" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "name" varchar(64) NOT NULL default '',
  "type" varchar(32) NOT NULL default '',
  "description" text NOT NULL,
  "default_level" mediumint(9) NOT NULL default '0',
  "negative_change" smallint(5) unsigned NOT NULL default '1',
  "positive_change" smallint(5) unsigned NOT NULL default '75',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=154 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "log_messages";

#
# Table structure for table 'log_messages'
#

CREATE TABLE "log_messages" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "time" timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  "type" varchar(64) NOT NULL,
  "message" text NOT NULL,
  "name" varchar(64) NOT NULL default '',
  "version" int(10) unsigned NOT NULL default '0',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=20293 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "login_char_colors";

#
# Table structure for table 'login_char_colors'
#

CREATE TABLE "login_char_colors" (
  "id" bigint(20) unsigned NOT NULL auto_increment,
  "char_id" int(10) unsigned NOT NULL,
  "signed_value" tinyint(4) NOT NULL default '0',
  "type" varchar(32) NOT NULL,
  "red" smallint(6) NOT NULL default '0',
  "green" smallint(6) NOT NULL default '0',
  "blue" smallint(6) NOT NULL default '0',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "id" ("id"),
  KEY "id_2" ("id"),
  KEY "CharID" ("char_id")
) AUTO_INCREMENT=320162 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "login_characters";

#
# Table structure for table 'login_characters'
#

CREATE TABLE "login_characters" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "account_id" int(10) unsigned NOT NULL default '0',
  "server_id" int(10) unsigned NOT NULL default '0',
  "name" varchar(64) NOT NULL default '',
  "race" tinyint(3) unsigned NOT NULL default '0',
  "class" tinyint(3) unsigned NOT NULL default '0',
  "gender" tinyint(3) unsigned NOT NULL default '0',
  "diety" tinyint(3) unsigned NOT NULL default '0',
  "body_size" float NOT NULL default '0',
  "body_age" float NOT NULL default '0',
  "current_zone" varchar(64) NOT NULL default 'antonica',
  "level" int(10) unsigned NOT NULL default '1',
  "tradeskill_level" int(10) unsigned NOT NULL default '1',
  "soga_wing_type" mediumint(8) unsigned NOT NULL,
  "soga_chest_type" mediumint(8) unsigned NOT NULL,
  "soga_legs_type" mediumint(8) unsigned NOT NULL,
  "soga_hair_type" mediumint(8) unsigned NOT NULL,
  "soga_race_type" mediumint(8) unsigned NOT NULL,
  "legs_type" mediumint(8) unsigned NOT NULL,
  "chest_type" mediumint(8) unsigned NOT NULL,
  "wing_type" mediumint(8) unsigned NOT NULL,
  "hair_type" mediumint(8) unsigned NOT NULL,
  "race_type" mediumint(8) unsigned NOT NULL,
  "deleted" tinyint(3) unsigned NOT NULL default '0',
  "unix_timestamp" int(10) NOT NULL default '0',
  "created_date" timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  "last_played" datetime NOT NULL default '0000-00-00 00:00:00',
  "char_id" int(11) NOT NULL default '0',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=6880 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "login_equipment";

#
# Table structure for table 'login_equipment'
#

CREATE TABLE "login_equipment" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "char_id" int(10) unsigned NOT NULL default '0',
  "item_type" smallint(5) unsigned NOT NULL default '0',
  "color1" tinyint(3) unsigned NOT NULL default '255',
  "color2" tinyint(3) unsigned NOT NULL default '255',
  "color3" tinyint(3) unsigned NOT NULL default '255',
  "highlight1" tinyint(3) unsigned NOT NULL default '255',
  "highlight2" tinyint(3) unsigned NOT NULL default '255',
  "highlight3" tinyint(3) unsigned NOT NULL default '255',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=26 /*!40100 DEFAULT CHARSET=latin1*/;


DROP TABLE IF EXISTS "login_versions";

#
# Table structure for table 'login_versions'
#

CREATE TABLE "login_versions" (
  "id" smallint(5) unsigned NOT NULL auto_increment,
  "version" varchar(30) NOT NULL default '',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "NewIndex" ("version")
) AUTO_INCREMENT=7 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "login_worldservers";

#
# Table structure for table 'login_worldservers'
#

CREATE TABLE "login_worldservers" (
  "id" int(11) unsigned NOT NULL auto_increment,
  "name" varchar(250) NOT NULL default '',
  "disabled" tinyint(3) unsigned NOT NULL default '0',
  "account" varchar(30) NOT NULL default '',
  "chat_shortname" varchar(20) NOT NULL default '',
  "description" text NOT NULL,
  "server_type" varchar(15) NOT NULL default '0',
  "password" varchar(32) NOT NULL,
  "serverop" varchar(64) NOT NULL default '',
  "lastseen" int(10) unsigned NOT NULL default '0',
  "admin_id" int(11) unsigned NOT NULL default '0',
  "greenname" tinyint(1) unsigned NOT NULL default '0',
  "showdown" tinyint(4) NOT NULL default '0',
  "chat" varchar(20) NOT NULL default '0',
  "note" tinytext NOT NULL,
  "ip_address" varchar(50) NOT NULL default '0',
  "reset_needed" tinyint(3) unsigned NOT NULL default '0',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "account" ("account")
) AUTO_INCREMENT=1425 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "login_worldstats";

#
# Table structure for table 'login_worldstats'
#

CREATE TABLE "login_worldstats" (
  "id" int(11) unsigned NOT NULL auto_increment,
  "world_id" int(11) unsigned NOT NULL default '0',
  "world_status" int(11) NOT NULL default '-1',
  "current_players" int(11) unsigned NOT NULL default '0',
  "current_zones" int(11) unsigned NOT NULL default '0',
  "connected_time" timestamp NOT NULL default CURRENT_TIMESTAMP,
  "last_update" timestamp NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "NewIndex" ("world_id")
) AUTO_INCREMENT=6037 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "opcodes";

#
# Table structure for table 'opcodes'
#

CREATE TABLE "opcodes" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "version_range1" smallint(5) unsigned NOT NULL default '0',
  "version_range2" smallint(5) unsigned NOT NULL default '0',
  "name" varchar(255) NOT NULL default '',
  "opcode" smallint(5) unsigned NOT NULL default '0',
  "table_data_version" smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "newindex" ("version_range1","name","version_range2")
) AUTO_INCREMENT=6168 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "variables";

#
# Table structure for table 'variables'
#

CREATE TABLE "variables" (
  "variable_name" varchar(64) NOT NULL default '',
  "variable_value" varchar(256) NOT NULL default '',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("variable_name")
) /*!40100 DEFAULT CHARSET=latin1*/;


DROP TABLE IF EXISTS "visual_states";

#
# Table structure for table 'visual_states'
#

CREATE TABLE "visual_states" (
  "id" int(10) unsigned NOT NULL auto_increment,
  "name" varchar(64) NOT NULL default 'None',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id")
) AUTO_INCREMENT=48398 /*!40100 DEFAULT CHARSET=latin1*/;



DROP TABLE IF EXISTS "zones";

#
# Table structure for table 'zones'
#

CREATE TABLE "zones" (
  "id" int(11) unsigned NOT NULL auto_increment,
  "name" varchar(64) NOT NULL default '',
  "file" varchar(64) NOT NULL default '',
  "description" varchar(255) NOT NULL default 'Describe me in the zones table! :)',
  "safe_x" float NOT NULL default '0',
  "safe_y" float NOT NULL default '0',
  "safe_z" float NOT NULL default '0',
  "underworld" float NOT NULL default '-1e+06',
  "table_data_version" smallint(5) unsigned NOT NULL default '1',
  PRIMARY KEY  ("id"),
  UNIQUE KEY "NewIndex" ("name")
) AUTO_INCREMENT=276 /*!40100 DEFAULT CHARSET=latin1*/;


/*!40101 SET SQL_MODE=@OLD_SQL_MODE;*/
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;*/
