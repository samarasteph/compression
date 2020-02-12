# compression

Partie I
--------

### question 1: 
on lit le fichier; pour chaque caractere on l'insere dans une liste pour ne pas à allouer des 
tailles successivement. Une fois le fichier lu, on ecrit ensuite dans un tableau chq caratere

### question 2:
on itere sur chaq caractere dans le tableau en input. si il nest pas dans la liste, on l'insere
avec une occurence de 1, sinon on incremente l'occurence. on retourne un tableau de paires (caractere,occurences)

### question 3:
on initialize chque donnée membre de la classe

### question 4:
on prend en parametre la liste des caracteres avec leur occurence. On cree un tableau de la taille du nombre
de cracteres. On insere dans chaq case du tableau une instance de la classe Noeud initialisée avec le caratere et
son occurence en les classant les occurences par ordre croissant. On cree ensuite l'arbre a partir de ce tableau.
Pour cela, on cree un noeud intermediaire (non feuille). si le bras gauche est vide on accroche le noeud a gauche,
sinon on accroche le noeud a droite. On incremente le nombre d'occurences avec celle du nouveau noeud. Si tous les 
bras gaouche et droite sont occupes, on cree un nouveau noeud intermediaire on met le noeud intermediare precedent 
a gauche de ce noeud et le noeud avec le caractere a droite.

### question 5:
dans le construecteur d'arbre, on utilise les fonction precedentes pour construire l'arbre comme décrit dans l'énoncé

### question 6:
la fonction écrire de la classe arbre utilise une fonction ecrire de la classe Noeud pour ecrire l'arbre de facon 
recursive. chaque Noeud souvagarde s'il est un noeud intermediare 0 (non feuille) puis son nombre d'occurence. s'il est 
un noeud final (feuille) 1 (noeud feuille) puis son nombre d'occurences et le caractere. remarque cette function retourne
le nombre d'octets dans le fichiers

### question 7:
meme metode recursive de lecture à partir du noeud racine. chaque noeud va lire en premier l'entier si = 0 c'est un noeud 
intermediaire, si 1 c'est un noeud feuille. Si c'est un noeud intermediaire on cree un noeud gauche, appeler sa fonction 
lire pour lire la suite du fichier, puis un noeud droit et appeler sa fonction lire. 

### question 8:
on ajoute dans la classe arbre, un tableau de tableau d'entiers de 256 cases (1 pour chaque caractere ASCII).

 ### question 9:
 on prend en parametre la liste des caracteres qu'on veut coder. Pour chaque caractere on va essayer de le trouver dans l'
arbre. On utilise un pile qui va empiler les chemins parcourus dans l'arbre. Pour commencer on se place dans le noeud 
feuille le plus en bas à gauche en mettant chaque noeud precedant dans la pile, on regarde si c'est le caractere recherche,
 puis on remonte en verifiant si le noeud a droite etc... des que le noeud avec le caractere est trouve on depile. si le noeud
 se trouve a gauche du noeud parent, le code est 0 sinon 1. on obtient ainsi une suite de 0 et de 1 suivant le chemin dans l'arbre.
 Ces suites sont codées par des entiers. Chaque suite est inseree dans un tableau avec comme indice le code ASCII du caractere.

 ### question 10:
 pour generer le code on prend la suite de caractere du fichier. On fait 2 passes. La premiere va servir a determiner la taille
 du tbleau de code a allouer. Dans la deuxieme passe on ecrit les codes sequentiellement dans le tableau.

 ### question 11:
pour la conversion en bits, on alloue un tableau en divisant la taille par 8. On va ensuite lire chaque serie de 8 entiers du tableau
d'entiers passé en parametres et initialiser dans un premier cas chaque case d'un bitset, ou alors deplacer avec l'operateur <<, 
chaque entier suivant la place qu'il occupe dans l'octet (seconde methode).

### question 12:
on utilise les fonctions precedentes pour effectuer soit la compression soit la decompression. Pour la compression on lit le ficher, on 
ecrit l'arbre dans le fichier de destination, on construit la table de codage, on lit le fichier pour former un tableau de caractere qu'on
passe a la fonction qui retourne un tableau d'entier qu'on transforme ensuite en tableau de bits. Pour la decompression on est oblige de
creer une fonction arbre::getchar qui va trouver le caractere en suivant le chemin dans l'arbre suivant la suite de 0 et 1. 0 on va vers
la gauche, 1 vers  la droite jusqu'a tomber sur une caractere. 

Partie 2:
---------

On utilise les methode precedentes mais en lisant le fichier par blocs (ici 1024 octets). on est oblige de creer des utilisations iteratives des fonctions
pour constiuer la frequence de chaque caractere, transformer les caracteres en codes et les ecrire dans le fichier (en faisant attention à l'offset de bit
d'une iteration à l'autre). Pareil pour la decompression. La le probleme est qu'une lecture en bloc peut tronquer un code de caractere et aboutir à une erreur
de decodage. il faudra alors revenir en arriere juste apres le dernier caractere correctement decode et reprendre la lecture par bloc et ainsi de suite 
jusqu'a la fin du fichier. 
