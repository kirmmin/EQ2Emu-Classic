# EverQuest2 Classic Emulator

## THIS IS A WORK IN PROGRESS. YOU CAN ONLY LOGIN TO THE CHARACTER SELECT AT THIS TIME.

## SETUP

Check out the eq2emulator.net site for guides on how to setup their software.

### Notes
- Ensure that the `opcodes.sql` file from the `docs` folder is sourced into both LoginServer and World Databases.
- Use the LoginServer included (`<root>/LS` directory). It's had a modification to the packet it uses to communicate with the WorldServer when passing a Player to zone connection due to overriding the `account_id` parameter was breaking the original packet structure.
- If you need to build the LoginServer (you shouldn't need to, but still). Make sure you adjust the project dependencies to point to the includes/lib folders appropriate for your system (check out `<root>/source/LoginServer/Login.vcxproj` to adjust the directory paths).