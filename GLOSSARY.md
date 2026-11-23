# FileRenamer

Outil de bureau qui produit, à partir d'un dossier de fichiers numérotés, des copies renommées selon un nom choisi par l'utilisateur.

## Langage

**Dossier source** :
Le dossier dont l'app lit les fichiers à copier ; seuls les fichiers visibles du premier niveau en font partie.
_À éviter_ : dossier d'entrée, dossier de départ

**Dossier d'arrivée** :
Le dossier où l'app dépose les copies renommées ; il doit être différent du dossier source.
_À éviter_ : nouveau dossier, destination

**Nomenclature** :
Modèle de nom de sortie enregistré par l'utilisateur, où `{n}` marque l'emplacement du numéro (par exemple `PARIS_P{n}`).
_À éviter_ : nom de base, nouveau nom, motif

**Numéro** :
Le dernier nombre présent dans le nom d'un fichier source, toujours écrit sur au moins trois chiffres dans la copie renommée (1 devient 001).
_À éviter_ : index, compteur

**Série** :
Les fichiers du dossier source dont le nom, numéro retiré, est identique sans tenir compte des majuscules ni de l'extension (`paris_004.pdf` et `PARIS_005.jpg`) ; seule la série la plus nombreuse est copiée.
_À éviter_ : famille, groupe, lot

**Début de nom imposé** :
Texte saisi par l'utilisatrice qui remplace la détection de la série : seuls les fichiers dont le nom commence par ce texte sont copiés.
_À éviter_ : filtre, préfixe, motif

**Copie renommée** :
Le fichier produit dans le dossier d'arrivée ; l'original n'est jamais modifié ni déplacé.
_À éviter_ : fichier renommé

**Fichier ignoré** :
Fichier source pour lequel aucune copie renommée n'a été produite, toujours accompagné de sa raison.
_À éviter_ : échec
