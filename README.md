# FileRenamer

FileRenamer creates renamed copies of numbered files in a destination folder. Your originals stay in the source folder, and existing files in the destination folder are never overwritten.

## Download for Windows

Download `FileRenamer-windows.zip` from [GitHub Releases](https://github.com/Tibxla/FileRenamer/releases). Extract the archive and run `FileRenamer.exe` from the extracted folder.

## Rename files

1. Choose the source and destination folders with **Parcourir**, or drag a folder onto either field. Dropping a file fills the field with its parent folder. You can also type the paths. FileRenamer creates the destination folder if needed.
2. Enter a naming template in **Nomenclature**. Use `{n}` wherever the number should appear. For example, `PARIS_P{n}` turns `paris_004.pdf` into `PARIS_P004.pdf`. A template without `{n}`, such as `paris`, is treated as `paris_{n}`.
3. Check the live preview below **Nomenclature**. It shows the first planned copy and counts the files to copy and skip, including conflicts with files already in the destination folder.
4. Click **Renommer**. The progress dialog lets you cancel between copies. The result dialog lists the reason for each skipped file. If at least one copy was created, click **Ouvrir le dossier d'arrivée** to open the destination folder.

FileRenamer uses the last group of digits before the final extension and preserves leading zeroes. For example, `rapport.v2_004.pdf` yields `004`, and `clip.mp4` is skipped because its name before the extension has no number. Copies keep the source file's final extension, if any. The app processes visible files directly inside the source folder in case-insensitive name order.

Use **Enregistrer** to save a template to the list and **Supprimer** to remove it. The app restores the last template used for a batch. The interface is in French and follows the Windows light or dark setting.

## Build and test

Install CMake 3.16 or later, Ninja, a C++17 compiler, and Qt 6 with the Widgets and Test modules. Run these commands from the repository root.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

If CMake cannot find Qt, pass `-DCMAKE_PREFIX_PATH=/path/to/your/Qt/kit` when configuring. CTest sets `QT_QPA_PLATFORM=offscreen` for the test executable.

Install the Qt translation catalogs for French dialog buttons and context menus when using a system Qt installation. On Debian and Ubuntu, the package is `qt6-translations-l10n`.

Launch `build/FileRenamer` on Linux, `build/FileRenamer.exe` on Windows, or `build/FileRenamer.app` on macOS.
