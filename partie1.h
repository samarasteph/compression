#include <utility>
#include <string>
#include <fstream>

#ifndef __PARTIE1_H__
#define __PARTIE1_H__

class Noeud
{
public:
	friend class utilitaire;
	friend class arbre;

	int occ;
	char c;
	Noeud *fg;
	Noeud *fd;
public:
	//Question 3
	Noeud(const char a_c='\0', int a_occ=0, Noeud *a_fg=NULL, Noeud *a_fd=NULL);
	~Noeud();
	void ecrire(std::ostream& s) const;
	void lire(std::istream& s);
};

class arbre
{
	Noeud* racine;
	//Question 8
	int* table_codage[256]; 
	void detruire_arbre();
public:
	friend class utilitaire;

	~arbre();
	//Question 5
	arbre(const std::string& s);
	//Question 6
	unsigned ecrire(const std::string& s) const;
	//Question 7
	unsigned lire (const std::string& s);
	//Question 9
	void creer_table_codage(const std::string& str);

	//fonction pour decoder lors de decompression: cherche le caractere dans l'arbre
	//renvoie le nouveau decalage en bit par rapport au debut. 
	//data: flux compressés en bits, longueur: taille du flux, offset par rapport au debut du flux
	//result le caractere decode
	unsigned getchar(char* data,unsigned longueur,unsigned offset, char& result) const;
	static void test(arbre& a);
};

class utilitaire {

public:
	//Question 1
	std::pair<char*,int> readfile(const std::string& file_path) const;
	//Question 2
	std::pair<char,int>* frequence(const char input[], int size) const;
	//Question 4
	Noeud* construire_arbre(std::pair<char,int>* T) const;
	//Question 10
	int* convertir_vers_code(const std::string& texte, int* table_codage[]) const;
	//Question 11
	char* convertir_en_bit1(int* tab, int size);
	char* convertir_en_bit2(int* tab, int size);
	//Question 12
	void compresser(const std::string& src, const std::string& dest);
	void decompresser(const std::string& src, const std::string& dest);

	//donne le nombre d'octets pour un nb  de bits donné en arrondissant à l'octet supérieur
	static int nb_octets(int nb_bits);
	static void test(const char* file);

		
};


#endif