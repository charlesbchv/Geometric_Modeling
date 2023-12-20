<p align="center"><a href="https:/laravel.com" target="_blanc"><img src="https://lh6.googleusercontent.com/SYu88FkCo5fn0hyPcy9mkUzpK92z4sWhJcFvB6FgE8MMqoKttgNhbqa_2KfW4JN82ljWbpmnBIoRiKLBSsnvKAor2_ctFBTvd0-DfMdYo7lDYdDyTLZgB0P3JXYCrqOLe2ilRexB" width="400"></a></p>
<p align="center"> 
</p>

# Geometric Modeling

Ce projet est une bibliothèque de manipulation de mesh 3D en C++, offrant des fonctionnalités telles que la subdivision Catmull-Clark, la simplification de mesh 3D, la triangulation, normals, silhouette.




Table des matières :

1. [Prérequis](#prerequis)
2. [Etape d'Installation](#etapeInstallation)
3. [Fonctionnalité](#fonctionnalite)
4. [Contribution](#contribution)



<div id='prerequis'/>  
  
## Installation

### Prérequis

- C++ Compiler (GCC recommandé)
- Visual Studio 2022
- Version SDK utilisée 10.0

*******

<div id='etapeInstallation'/>

### Étapes d'installation

1. **Cloner le dépôt Git**
   ```bash
   git clone https://github.com/charlesbchv/Geometrical_Modeling.git
   ```

2. **Compiler le projet**

* Une fois le projet ouvert via VS
* Ouvrir 'myproj.sln' ou cliquer 2 fois sur 'myproj.sln' qui se trouve dans l'Explorateur de solutions.
* Générer la solution, si cela ne fonctionne pas ou affiche des erreurs :
  - Clique droit sur Solution 'myproj' et ensuite Recibler la solution
  - Executer le Release avec Ctrl-F5

*******

<div id='fonctionnalite'/>
  
## Fonctionnalités

- **Subdivision Catmull-Clark** : Cette caractéristique implique l'utilisation de l'algorithme de Catmull-Clark
 pour affiner et lisser un maillage 3D. L'algorithme fonctionne en subdivisant chaque face du maillage en plusieurs
 faces plus petites, ce qui augmente le nombre de sommets et d'arêtes. Cette subdivision se fait de manière répétée,
 où chaque itération lisse davantage la surface du maillage, produisant une forme plus ronde et plus douce.
 Cette technique est particulièrement utile pour obtenir des modèles 3D de haute qualité avec des surfaces lisses,
 souvent utilisée dans l'animation 3D et les effets spéciaux pour créer des objets et des personnages aux apparences
 réalistes et détaillées.

- **Simplification de Maillage**: Cette fonctionnalité permet de réduire la complexité d'un maillage 3D en diminuant
  le nombre de ses arêtes. Le processus, appelé « effondrement d'arêtes », consiste à fusionner des paires de
  sommets voisins en un seul sommet. Cette opération entraîne la suppression des arêtes et des faces adjacentes,
  simplifiant ainsi le maillage tout en essayant de préserver sa forme globale. Cette technique est utile pour
  alléger les modèles 3D, notamment pour des applications en temps réel comme les jeux vidéo ou la visualisation
  3D, où une performance élevée est requise.
  
- **Triangulation** : Cette fonctionnalité transforme toutes les faces d'un maillage 3D en triangles. Chaque face, quel
  que soit son nombre de côtés (polygone), est découpée en plusieurs triangles. Cette opération est cruciale car les
  triangles constituent la forme de base la plus simple pour le rendu 3D dans la plupart des moteurs graphiques et
  des applications de modélisation. La triangulation est essentielle pour assurer la compatibilité et l'efficacité du
  maillage dans divers contextes d'usage, comme les jeux vidéo, la réalité augmentée, la simulation et l'impression 3D.
  Elle permet une représentation uniforme et souvent simplifie le calcul des normales de surface et l'application de textures.
(Autres fonctionnalités...)


*******

<div id='contribution'/>  

## Contribution

Les contributions sont les bienvenues. Veuillez suivre les étapes suivantes :

- Forkez le projet
- Créez votre branche de fonctionnalité (git checkout -b feature/AmazingFeature)
- Committez vos changements (git commit -m 'Ajout d'une fonctionnalité incroyable')
- Poussez vers la branche (git push origin feature/AmazingFeature)
- Ouvrez une Pull Request


© Réalisé par Charles Batchaev
