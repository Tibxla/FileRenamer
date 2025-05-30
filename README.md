# FileRenamer

## Description

**FileRenamer** est une application de bureau développée en C++ avec le framework Qt, conçue pour simplifier le renommage de fichiers en masse et la création de fichiers PDF avec des noms personnalisés. Elle propose une interface utilisateur intuitive et des options de personnalisation telles que le choix de thèmes (clair ou sombre) et la configuration de motifs de numérotation. Une fonctionnalité ludique de simulation de virus (désactivable) est également incluse pour des démonstrations.

## Fonctionnalités

- **Renommage de fichiers** :
  - Sélection d’un dossier source et d’un dossier de destination.
  - Définition d’un nom de base pour les fichiers renommés.
  - Personnalisation des motifs de numérotation (nombre de chiffres, ajout de lettres avant ou après, regex personnalisé).
- **Création de fichiers PDF** :
  - Génération de fichiers PDF vides avec un nom de base et une numérotation séquentielle.
  - Choix du dossier de destination et du nombre de fichiers (de 1 à 10 000).
- **Paramètres personnalisables** :
  - Sélection du thème (clair ou sombre).
  - Configuration du nombre de chiffres pour la numérotation.
  - Options de motif pour le renommage (avec ou sans lettres, regex personnalisé).
- **Simulation de virus (optionnelle)** :
  - Après 5 minutes, ouverture de multiples pop-ups simulant une alerte virale (désactivable dans les paramètres).
- Interface moderne avec un splash screen au démarrage.

## Version exécutable
- Le projet inclut une version compilée et exécutable dans le dossier `bin`. Vous pouvez télécharger ce dossier pour utiliser l’application sans avoir à la compiler vous-même.
- Sous Windows, lancez `FileRenamer.exe` pour démarrer l’application.
- Assurez-vous que toutes les dépendances (DLL Qt, etc.) sont présentes dans le dossier `bin` pour que l’application fonctionne correctement.

## Installation

### Prérequis

Avant de commencer, assurez-vous d’avoir installé :
- **CMake** (version 3.16 ou supérieure) pour générer le projet.
- **Qt** (version 5 ou 6) avec le composant **Widgets**.
- Un compilateur C++ compatible (par exemple, MSVC sous Windows, GCC sous Linux/macOS).
- Un environnement de développement (optionnel, comme Qt Creator ou Visual Studio).

### Étapes d’installation

#### Téléchargement et préparation
1. Téléchargez le ZIP du projet depuis le dépôt GitHub et extrayez-le dans un dossier (par exemple, `FileRenamer-master`).
2. Renommez le dossier en `FileRenamer` pour simplifier les chemins (facultatif).

#### Configuration et compilation
1. Ouvrez un terminal dans le dossier `FileRenamer`.
2. Créez un dossier de construction :
   ```
   mkdir build
   cd build
   ```
3. Configurez le projet avec CMake (adaptez le chemin de Qt selon votre installation) :
   ```
   cmake .. -DCMAKE_PREFIX_PATH="chemin/vers/Qt/5.x.x/compiler/lib/cmake"
   ```
   Exemple sous Windows avec Qt 5.15.2 et MSVC :
   ```
   cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64/lib/cmake"
   ```
4. Compilez le projet :
   ```
   cmake --build . --config Release
   ```
5. L’exécutable `FileRenamer` (ou `FileRenamer.exe` sous Windows) sera généré dans `build/Release`.

#### Lancement de l’application
1. Accédez au dossier `build/Release`.
2. Lancez l’application :
   - Sous Windows : double-cliquez sur `FileRenamer.exe`.
   - Sous Linux/macOS : exécutez `./FileRenamer` dans le terminal.
3. Un splash screen s’affichera pendant 1,3 seconde, suivi de la fenêtre principale.

**Note :** Recompilez le projet si vous modifiez le code ou les ressources (par exemple, `resources.qrc`).

## Utilisation

1. **Renommer des fichiers** :
   - Cliquez sur "Parcourir" pour sélectionner le dossier source et le dossier de destination.
   - Saisissez un nom de base pour les fichiers.
   - Cliquez sur "Renommer" pour lancer le processus.
2. **Créer des fichiers PDF** :
   - Entrez un nom de base pour les fichiers PDF.
   - Sélectionnez le dossier de destination.
   - Indiquez le nombre de fichiers à créer (1 à 10 000).
   - Cliquez sur "Créer Fichiers PDF".
3. **Paramètres** :
   - Ajustez le nombre de chiffres pour la numérotation.
   - Choisissez un motif de renommage (lettres avant/après, regex personnalisé).
   - Sélectionnez un thème (clair ou sombre).
   - Activez/désactivez la simulation de virus (activée par défaut).

## Structure du projet

Le projet est organisé ainsi :
- **`resources.qrc`** : Contient les ressources comme :
  - `icon.png` : Icône de l’application.
  - `style.qss` : Style pour le thème sombre.
  - `light.qss` : Style pour le thème clair.
  - `animation.gif` : Animation (non utilisée actuellement).
- **`mainwindow.ui`** : Interface conçue avec Qt Designer (onglets "Renommer", "Créer PDF", "Paramètres").
- **`CMakeLists.txt`** : Script CMake pour la configuration et la compilation.
- **`app.rc`** : Ressources Windows pour l’icône (Windows uniquement).
- **`mainwindow.h` et `mainwindow.cpp`** : Logique principale (événements, paramètres, simulation de virus).
- **`main.cpp`** : Point d’entrée, initialise l’application et le splash screen.
- **`splashwidget.h` et `splashwidget.cpp`** : Gestion du splash screen.

## Résolution des problèmes courants

- **Erreur CMake lors de la compilation** :
  - Vérifiez le chemin de Qt dans `CMAKE_PREFIX_PATH`.
  - Assurez-vous que le composant **Widgets** est installé.
- **L’application ne démarre pas ou les thèmes ne s’appliquent pas** :
  - Vérifiez que `style.qss` et `light.qss` sont inclus dans `resources.qrc`.
- **La simulation de virus ne fonctionne pas** :
  - Confirmez que l’option est activée dans "Paramètres" et attendez 5 minutes.
- **Les boutons "Parcourir" ne répondent pas** :
  - Vérifiez les connexions des slots dans `mainwindow.cpp`.


