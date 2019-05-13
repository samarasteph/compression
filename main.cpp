#include <iostream>

#include "partie1.h"
#include "partie2.h"

//Quetion12: faire un programme qui compresse ou decompresse
int main(int argc, char* argv[]){

	std::string src,dest;
	std::cout << "Entrer le fichier source" << std::endl;
	std::cin >> src;
	std::cout << "Entrer le fichier destination" << std::endl;
	std::cin >> dest;
	int choix;
	
	do{
		std::cout << "Compresser (1) ou Decompresser(2)?" << std::endl;
		std::cin >> choix;
		if(choix!=1 && choix!=2)
			std::cout << "Choix incorrect. Retaper (1) ou (2)." << std::endl;
	} while(choix!=1 && choix!=2);

	utilitaire u;
	if (choix==1)
	{
		u.compresser(src,dest);
		std::cout << "Compression terminee" << std::endl;
	}
	else
	{
		u.decompresser(src,dest);
		std::cout << "Decompression terminée" << std::endl;

	}
	return 0;
}