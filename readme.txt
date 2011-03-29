==================================================================
This Homebrew is an entry for the PSP Genesis competition 2011,
sponsored by npt, psp-hacks.com, pspgen.com, pspslimhacks.com,
exophase.com, pspcustomfirmware.com, daxhordes.org, gamegaz.jp, 
and wololo.net
==================================================================

C4TurD4Y presents:
Selective Plugin Loader: SEPULKA v0.8 [EXPERIMENTAL] PGC Edition

BEFORE YOU START USING SEPULKA, MAKE SURE THAT ISO DRIVER IS M33 OR NP9660 AND CPU SPEED IS SET TO DEFAULT

Installing:
 1. Copy "SEPULKA.PRX" to ms0:/seplugins/ directory.
 2. Add "ms0:/seplugins/sepulka.prx 1" to game.txt 
 3. Create "SEPULKA.TXT" in ms0:/seplugins/ directory.
 4. Create "MACROS.TXT" in ms0:/seplugins/ directory. Your macros will be stored here.
Notes:
 - If you are using PSPgo you can place all stuff on ef0: (internal PSPgo memory)
 - You can also create list of modules to load (one per line) for each program (isoname_plugins.txt for iso/cso files and plugins.txt for eboot.pbp (in the same directory as eboot)).
 - To load plugin by Sepulka just add line with path to plugin and parameters (in main file).

Syntax:
 - Path to plugin (no spaces allowed in path) followed by space and parameters (game titles, game id (in  example: UCES-00666, ucus12345 ucjs-10041)) or mode (iso - all backups; umd - all umd games; pbp - all eboot.pbp files from MS (homebrew and PSN))).
 - Indeed path to plugin, you can choose ISO driver (normal - normal mode, m33driver - M33 Driver, np9660 - Sony NP9660).
 - Or cpu speed, just type "cpu" (w/o quotes) immediatly followed by three numbers (no spaces allowed).
 - If you add "(mode)" before game title/id, it will works for that game only on specified mode.
 - Parameters are case-insensitive and all signs different from letters, digits and Sepulka's special signs (| and ~) are ignored, so "God of War: Chains of Olympus" and "godofwarchainsofolympus" will mean the same.
 - Blacklisting is supported. Just add ~ before parameter (ex. ~motorstorm arctic edge).
 - Parameters have to be separated by | sign.

Comments:
 - Just add # sign at line start
 - Supported at main file (ms0:/seplugins/sepulka.txt), macros file and custom plugin lists

See more in example sepulka.txt

Boot screen:
 - It displays modules loaded by Sepulka and other various options like game id and title.
 - If no plugins were loaded, it wouldn't start.

Custom macros:
 - Sepulka searches for macros in file "ms0:/seplugins/macros.txt"
 - Syntax:
    - Structure of Sepulka's macro is very simple, first is game title (w/o spaces,
      in this example i replaced it with '_' sign for better readabillity), followed
      by a space and macro string. End of macro is new line sign (it can be LF, CRLF or CR).
    - Sepulka can handle up to 256 macros, next ones will be ignored.
    - Macros are case-insensitive, so u can write it as you wish.

See more in example macros.txt

To do:
 - Add support for non-latin title strings
 - Some cleanup in code

Note to other devs:
 I added systemctrl_se header completed by me (added FULL SEConfig struct, enumeration of MS Speed Up modes and complete fake regions enum)