import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os

# Configuration du style
sns.set_theme(style="whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)

def visualize_data(csv_file):
    if not os.path.exists(csv_file):
        print(f"❌ Erreur : Le fichier {csv_file} n'existe pas.")
        return

    print(f"Chargement de {csv_file}...")
    df = pd.read_csv(csv_file)
    
    print("Aperçu des données :")
    print(df.head())
    print("\nStatistiques par Label :")
    print(df.groupby('label')[['packet_count', 'bitrate', 'size_mean']].mean())

    # --- FIGURE 1 : Répartition des Classes ---
    plt.figure(figsize=(10, 6))
    ax = sns.countplot(x='label', data=df, palette='viridis', order=df['label'].value_counts().index)
    plt.title('Nombre d\'échantillons par Type d\'Appareil', fontsize=16)
    plt.xlabel('Appareil')
    plt.ylabel('Nombre de fenêtres capturées')
    for p in ax.patches:
        ax.annotate(f'{p.get_height()}', (p.get_x() + p.get_width() / 2., p.get_height()), 
                    ha = 'center', va = 'center', xytext = (0, 10), textcoords = 'offset points')
    plt.savefig('distribution.png')

    # --- FIGURE 2 : Débit vs Taille Moyenne des Paquets ---
    # C'est souvent ce qui distingue le mieux la Vidéo (Camera/TV) du reste
    plt.figure(figsize=(12, 8))
    sns.scatterplot(data=df, x='size_mean', y='bitrate', hue='label', style='protocol', s=100, alpha=0.7)
    plt.title('Signature du Trafic : Débit vs Taille des Paquets', fontsize=16)
    plt.xlabel('Taille Moyenne des Paquets (octets)')
    plt.ylabel('Débit (bits/sec)')
    plt.yscale('log') # Échelle logarithmique car les débits varient énormément
    plt.grid(True, which="both", ls="--")
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.tight_layout()
    plt.savefig('signature.png')

    # --- FIGURE 3 : Boxplot de l'Inter-Arrival Time (IAT) ---
    # Permet de voir la régularité du trafic (Streaming = IAT faible et stable, Capteur = IAT grand)
    plt.figure(figsize=(12, 8))
    # On filtre les valeurs nulles pour éviter les erreurs log
    df_iat = df[df['iat_mean'] > 0]
    sns.boxplot(x='label', y='iat_mean', data=df_iat, palette='Set2')
    plt.title('Intervalle Moyen entre Paquets (IAT) par Appareil', fontsize=16)
    plt.xlabel('Appareil')
    plt.ylabel('IAT Moyen (secondes) - Échelle Log')
    plt.yscale('log')
    plt.savefig('interval.png')

    # --- FIGURE 4 : Pairplot (Vue d'ensemble) ---
    # Montre les relations entre les 3 features principales
    subset = df[['bitrate', 'packet_count', 'size_mean', 'label']]
    sns.pairplot(subset, hue='label', height=3, plot_kws={'alpha': 0.6})
    plt.savefig('pairplot.png')

if __name__ == "__main__":
    visualize_data("iot_dataset_final.csv")