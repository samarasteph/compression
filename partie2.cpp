#include "partie2.h"
#include <iostream>

#define TAILLE_BLOC 1024

std::pair<char*,int> utilitaire2::readfile(const std::string& file_path, unsigned decalage_bloc) const
{
	std::ifstream f(file_path, std::ios_base::in | std::ios_base::binary);
	
	std::pair<char*,int> pr;
	pr.first = NULL;
	pr.second = 0;

	//on lit un bloc a partir d'un decalage par rapport au debut du fichier
	if (f.is_open())
	{
		f.seekg(0, f.end);
		unsigned end = f.tellg();

		if(decalage_bloc<end)
		{
			f.seekg(decalage_bloc, f.beg);

			char* buffer = new char[TAILLE_BLOC];
			f.read(buffer,TAILLE_BLOC);

			pr.first = buffer;

			if(f)
				pr.second = TAILLE_BLOC;
			else
				pr.second = f.gcount();

		}
		f.close();
	}

	return pr;
}

void utilitaire2::frequence_bloc(const char input[], int size, std::list< std::pair< char, int > >& items) const
{
	std::list< std::pair< char, int> >::iterator it;
	int i;
	for(i=0; i < size; i+=1)
	{
		for(it = items.begin(); it != items.end() && it->first != input[i] ; ++it)
			;
		if(it==items.end())
		{
			std::pair<char, int> p;
			p.first = input[i];
			p.second = 1;
			items.push_back(p);
		}
		else
		{
			it->second += 1;
		}
	}
}

std::pair<char,int>* utilitaire2::frequence(std::list< std::pair< char, int > >& items) const
{
	std::pair<char,int> * freq = new std::pair<char,int>[items.size()+1];
	std::list< std::pair< char, int> >::iterator it;
	int i = 0;
	for (it = items.begin(); it != items.end(); ++it){
		freq[i].first = it->first;
		freq[i].second = it->second;
		i += 1;
	}
	//le dernier element mis a nul pour marquer la fin du tableau
	freq[i].first = freq[i].second = 0;

	return freq;
}


void utilitaire2::compresser(const std::string& src, const std::string& dest)
{

	arbre2 a(src);
	a.ecrire(dest);

	unsigned decalage = 0;
	char buffer[TAILLE_BLOC];
	unsigned ecrits = 0;
	bool continuer = true;
	//on lit par bloc et on compresse chaque bloc qu'on rajoute en fin de fichier destination
	do {

		std::ifstream ifs(src);
		std::string data;
		if(ifs.is_open())
		{
			ifs.seekg(decalage,ifs.beg);
			ifs.read(buffer,TAILLE_BLOC);
			if (ifs)
			{
				data.append(buffer,TAILLE_BLOC);
				decalage += TAILLE_BLOC;
			}
			else
			{
				data.append(buffer,ifs.gcount());
				continuer = false;
			}
			ifs.close();
		}
		else{
			std::cerr << "Impossible d'ouvrir fichier source " << src << ". Compression interrompue" << std::endl;
			return;
		}
		int * code = convertir_vers_code(data,a.table_codage);
		char* bits = convertir_en_bit1(code+1,code[0]);


		std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary | std::ios_base::app);
		if (ofs.is_open())
		{
			int n = utilitaire::nb_octets(code[0]);
			ofs.write(bits,n);
			ofs.close();
			ecrits += n;
		 }
		else
		{
			std::cerr << "cannot open destination file " << dest << ". Compression areetée." << std::endl;
			continuer = false;
		}

		delete [] code;
		delete [] bits;
	}while(continuer);

	std::cout << ecrits << " octets écrits" << std::endl;
	
}

void utilitaire2::decompresser(const std::string& src, const std::string& dest)
{
	arbre2 a("");
	unsigned lu = a.lire(src);

	if(lu)
	{
		unsigned size, end ;

		//calcul de la taille de fichier source
		std::ifstream ifs(src, std::ios_base::in | std::ios_base::binary);
		if(ifs.is_open())
		{
			ifs.seekg(0, ifs.end);
			end = ifs.tellg();
			
			ifs.close();
		}
		else
		{
			std::cerr << "Impossible ouvrir le fichier source " << src << std::endl;
			return;
		}
		//on retire la taille de l'arbre
		size = end - lu;

		char buffer[TAILLE_BLOC];
		bool continuer = true;
		unsigned decalage = lu;
		int offset = 0;
		lu = 0;
		do{
			std::ifstream ifs(src, std::ios_base::in | std::ios_base::binary);

			if(ifs.is_open())
			{
				ifs.seekg(decalage,ifs.beg);

				ifs.read(buffer,TAILLE_BLOC);

				if(ifs)
					decalage += TAILLE_BLOC;
				else
				{
					decalage += ifs.gcount();
					continuer = false;
				}
				ifs.close();
			}


			std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary);
			if(ofs.is_open())
			{
				char c;

				while( offset < decalage*8 )
				{
					c='\0';
					int fait = a.getchar(buffer,size,offset,c);
					if(c!='\0')
					{
						//std::cout << "'" << c << "' ";
						ofs.put(c);
						offset = fait;
					}
					else
					{
						if (offset< size*8) 
						{
							// le décodage a echoué il faut relire un bloc a partir du dernier caractère lu correctement 
							// on doit revenir en arrière en arrondissant à l'octet
							int nb_octets = utilitaire::nb_octets(fait-offset);
							decalage -=  nb_octets;
						}
					}
				}
				ofs.close();
			}
			else
			{
				std::cerr << "Imposible d'ouvrir le fichier de destination " << dest << std::endl;
				continuer = false;
			}
		}while(continuer);
		
	}
	else
		std::cerr << "l'arbre d'en tete n'a pas ete lu" << std::endl;

}

// on court circuite le constructeur de la class mere avec un nom de fichier non valide
arbre2::arbre2(const std::string& file_path) : arbre("") 
{

	utilitaire2 u;
	int decalage_bloc = 0;
	std::list< std::pair< char, int > > items;

	//lecture du fichier par bloc
	std::pair<char*,int> chars = u.readfile(file_path ,decalage_bloc);

	while (chars.second==TAILLE_BLOC) {

		//frequence des elements calculee par iteration
		u.frequence_bloc(chars.first, chars.second, items);
		
		decalage_bloc += TAILLE_BLOC;
		
		delete [] chars.first;
		chars = u.readfile(file_path, decalage_bloc);
	}

	u.frequence_bloc(chars.first, chars.second, items);
	delete [] chars.first;

	//on construit la paire pour la conruction de l'arbre
	std::pair<char,int>* freq = u.frequence(items);

	racine = u.construire_arbre(freq);

	delete freq;

	int i = 0;
	while(freq[i].first!='\0')
	{
		char_list.append(1,freq[i++].first);
	}
	creer_table_codage(char_list);


}

