This minilogin is Win32 only, Linux binary may be released at some point (..not yet)
=========================================================================

PREMISE
=======
Setting up the mini login requires basic experience with MySQL and running queries through it.

The minilogin server is limited to a total of five players (Account ID 1-5) and one world server,
(World ID 1).  Unlike the EQEmu minilogin, you can set your username and password to reflect the 
account, passwords for accounts and world servers are both md5'ed in the database.

The following is a step by step process to setup your login server, the accounts and world server.
=======

STEPS
=====
1)Use an existing world server database or create a new database for a login server.

2)source in the eq2logindb.sql (if you are using mysql.exe, source eq2logindb.sql with it in the 
same directory eg. c:\mysql\bin)

3) Modify login_db.ini to reflect the mysql database

4) Modify your eq2_default.ini such as you did to setup for eq2emulator.net

5)You can have up to 5 accounts using id's 1-5, you add an account either through another tool or in mysql.exe:
	insert into account set name='thename',passwd=md5('thepasswd');

	Verify their ID's by doing the command:
	select * from account;

	If you need to change an ID to a different number the following command should be used:
	update account set id=# where name='thename';

6) To add a world server do:
	insert into login_worldservers set name='The 	World',account='loginacct',password=md5('thepass'),chat_shortname='shortname';

	make sure the server id is set to 1, or it will not function.
	The command below will list all the world servers:
	select * from login_worldservers;

	If the ID is not 1 do the following command:
	update login_worldservers set id=1 where name='thename';
=====