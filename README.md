# FileRenamer

FileRenamer creates renamed copies of numbered files in a destination folder. Your originals stay in the source folder, and existing files in the destination folder are never overwritten.

## Download for Windows

Download `FileRenamer-windows.zip` from [GitHub Releases](https://github.com/Tibxla/FileRenamer/releases). Extract the archive and run `FileRenamer.exe` from the extracted folder.

## Rename files

1. Choose the source and destination folders. Drag a folder onto the **Dossier source** or **Dossier d'arrivée** drop zone, or click anywhere on a zone to pick a folder in the standard Windows dialog. Dropping a file uses its parent folder. Each zone then shows the folder name and its full path. FileRenamer creates the destination folder if needed.
2. Enter a naming template in **Nomenclature**. Use `{n}` wherever the number should appear. For example, `PARIS_P{n}` turns `paris_004.pdf` into `PARIS_P004.pdf`. A template without `{n}`, such as `paris`, is treated as `paris_{n}`.
3. Check the live preview below **Nomenclature**. It shows the first planned copy as the source name and the new name, then counts the files to copy and skip, including conflicts with files already in the destination folder and numbered files outside the selected series (see Series detection below). Problems such as an invalid template or identical folders appear in red in the same place, and **Renommer** stays disabled until both folders and a valid template are set.
4. Click **Renommer**. A progress bar replaces the preview, and **Annuler** or the Escape key stops the batch after the current copy. The result then appears in the window: the number of copies created, the skipped files with the reason for each one (click the count to expand the list), and **Ouvrir le dossier d'arrivée** to open the destination folder. Changing a folder or the template brings the preview back.

FileRenamer uses the last group of digits before the final extension and always writes it on at least three digits: `paris_1.pdf` yields `001`, `paris_0004.pdf` yields `004`, and `paris_1234.pdf` keeps `1234`. `rapport.v2_004.pdf` yields `004`, and `clip.mp4` is skipped because its name before the extension has no number. Copies keep the source file's final extension, if any. The app processes visible files directly inside the source folder in case-insensitive name order.

## Series detection

A source folder often holds a few unrelated numbered files next to the scans, such as `IMG_4521.jpg`, `facture 2025.pdf` or `Scan 23-11.pdf`. FileRenamer groups the numbered files into series: files whose names are identical once the number is removed, ignoring letter case and the extension (`paris_004.pdf` and `PARIS_005.jpg` belong to the same series). Only the largest series is copied.

The preview names the series it takes and its size, for example `Série : paris_… · 148 fichiers`. When other series are present, an amber line below lists them (hover it to see all of them with their sizes), and their files are skipped with the reason "Le fichier ne fait pas partie de la série retenue." If two or more series share the largest size, nothing is copied: a red message lists them, and **Renommer** stays disabled until you move them into separate folders or impose a name start. A folder without any numbered file shows a message instead of a preview.

To choose the files yourself, click **Modifier** next to the series, or **Imposer un début de nom** in the red message, and type the beginning of the names in **Ne prendre que les fichiers dont le nom commence par**. The comparison ignores letter case. Only files whose names start with that text are copied, and the preview line becomes `Début de nom imposé : « IMG » · 1 fichier`. Click **Automatique** to return to automatic detection. The imposed name start is not saved: it is cleared whenever you choose another source folder.

Use **Enregistrer** to save a template to the list and **Supprimer** to remove it. The app restores the last folders and the last template used for a batch. The interface is in French and follows the Windows light or dark setting.

## Build and test

Install CMake 3.16 or later, Ninja, a C++17 compiler, and Qt 6 with the Widgets, Svg and Test modules. On Debian and Ubuntu, the Qt packages are `qt6-base-dev` and `qt6-svg-dev`. Run these commands from the repository root.

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

If CMake cannot find Qt, pass `-DCMAKE_PREFIX_PATH=/path/to/your/Qt/kit` when configuring. CTest sets `QT_QPA_PLATFORM=offscreen` for both test executables: `filerenamer_tests` covers the renaming logic and the drop zone, and `filerenamer_ui_tests` drives the main window with settings stored in a temporary folder.

Install the Qt translation catalogs for French dialog buttons and context menus when using a system Qt installation. On Debian and Ubuntu, the package is `qt6-translations-l10n`.

Launch `build/FileRenamer` on Linux, `build/FileRenamer.exe` on Windows, or `build/FileRenamer.app` on macOS.
