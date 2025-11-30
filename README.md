
# Projet : Simulation, Capture et Analyse de Trafic IoT (ns-3)

Ce projet vise à simuler un réseau domestique intelligent (Smart Home) réaliste en utilisant le simulateur **ns-3**, à capturer le trafic généré, et à le transformer en un jeu de données (Dataset) exploitable pour des algorithmes de Machine Learning.

## 📂 Structure du Projet

Voici l'arborescence du projet et le rôle précis de chaque dossier et fichier :

```text
Projet/
├── analysis/               # Cerveau de l'analyse de données (Python)
├── data/                   # Stockage des données (Brutes et Traitées)
├── images/                 # Résultats visuels (Graphiques générés)
├── simulation/             # Cœur de la simulation réseau (Code C++ ns-3)
├── venv/                   # Environnement virtuel Python (Dépendances)
├── .gitignore              # Configuration Git
└── README.md               # Documentation du projet
```

---

## 🔍 Détail des Composants

### 1. 🚀 `simulation/` (Génération des Données)
C'est ici que tout commence. Ce dossier contient le code source pour le simulateur ns-3.
*   **`iot-home.cc`** : C'est le script principal en C++.
    *   Il définit la topologie réseau (WiFi 802.11ac + Filaire CSMA).
    *   Il configure les appareils (Caméras, Thermomètres, Laptops, TV, etc.).
    *   Il génère le trafic réseau (UDP pour les capteurs, TCP pour le multimédia).
    *   **Sortie :** L'exécution de ce script produit les fichiers `.pcap` dans le dossier `data/raw/`.

### 2. 💾 `data/` (Stockage)
Ce dossier centralise toutes les données, de la capture brute au fichier final prêt pour l'IA.
*   **`raw/` (Données Brutes)** :
    *   **`iot-wired-42-1.pcap`** : C'est le fichier de capture réseau généré par ns-3. Il contient tous les paquets (octets, en-têtes IP/TCP/UDP) qui ont transité par le switch central durant la simulation.
*   **`processed/` (Données Traitées)** :
    *   **`iot_dataset_final.csv`** : C'est le résultat final du traitement. Ce fichier CSV est un tableau structuré où chaque ligne représente une fenêtre de trafic, avec des caractéristiques calculées (débit, taille moyenne, IAT) et une étiquette (Label).

### 3. 🐍 `analysis/` (Traitement Python)
Ce dossier contient les scripts qui transforment les données brutes en informations utiles.
*   **`create_dataset.py`** : Le script d'extraction (Feature Engineering).
    *   **Entrée :** Lit le fichier `.pcap` dans `data/raw/`.
    *   **Action :** Reconstitue les flux, calcule les statistiques, et labellise les données selon les ports.
    *   **Sortie :** Génère le fichier `.csv` dans `data/processed/`.
*   **`visualise_dataset.py`** : Le script de visualisation.
    *   **Entrée :** Lit le fichier `.csv` dans `data/processed/`.
    *   **Action :** Utilise `matplotlib` et `seaborn` pour créer des graphiques statistiques.
    *   **Sortie :** Sauvegarde les images dans le dossier `images/`.

### 4. 📊 `images/` (Résultats)
Contient les graphiques générés automatiquement par le script de visualisation, prouvant la qualité du dataset.
*   **`distribution.png`** : Montre l'équilibre des classes (nombre d'exemples par type d'appareil).
*   **`signature.png`** (ou `bitrate_size.png`) : Montre la distinction entre les appareils selon le débit et la taille des paquets.
*   **`interval.png`** : Analyse des temps entre les paquets (IAT).
*   **`pairplot.png`** : Matrice de corrélation globale.

### 5. ⚙️ Fichiers de Configuration
*   **`venv/`** : Contient les bibliothèques Python installées (pandas, scapy, seaborn, etc.) isolées du système pour éviter les conflits.
*   **`.gitignore`** : Indique à Git de ne pas sauvegarder les fichiers lourds ou temporaires (comme le dossier `venv/` ou les gros fichiers `.pcap`), mais de garder le code source.

---

## 🔄 Flux de Travail (Workflow)

Pour reproduire les résultats, les étapes s'enchaînent logiquement d'un dossier à l'autre :

1.  **Simulation :** Exécuter `simulation/iot-home.cc` via ns-3 $\rightarrow$ Crée le `.pcap` dans `data/raw/`.
2.  **Extraction :** Lancer `analysis/create_dataset.py` $\rightarrow$ Lit le `.pcap`, crée le `.csv` dans `data/processed/`.
3.  **Visualisation :** Lancer `analysis/visualise_dataset.py` $\rightarrow$ Lit le `.csv`, crée les PNG dans `images/`.

---

*Auteur : KOUASSI DE YOBO GILBERT BRYAN*
