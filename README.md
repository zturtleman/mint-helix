**Spearmint Soldier of Fortune II: Double Helix** game code. It is very incomplete. It's slightly better than trying to run SoF2 using Quake 3 game code.

To use this you'll need the [Spearmint engine](https://github.com/zturtleman/spearmint).

  * On Windows, install [Cygwin and mingw-w64](https://github.com/zturtleman/spearmint/wiki/Compiling#windows).
  * Get the source for Spearmint and build it using `make`.
  * Get the source for this repo and build it using `make`.
  * Copy the pk3 files from SoF2's base into basesof2 in `mint-helix/build/release-mingw32-x86/`.
  * Copy the [spearmint-patch-data](https://github.com/zturtleman/spearmint-patch-data) for basesof2 there too.

If you put both projects in the same directory you can launch the game using;

    spearmint/build/release-mingw32-x86/spearmint_x86.exe +set fs_basepath "mint-helix/build/release-mingw32-x86/" +set fs_game "basesof2"

On Linux and OS X you'll need to put `./` before the command and substitute the correct platform and architecture (look in the build directory).

