#include "partie2.h"
#include <iostream>
#include <deque>


#define TAILLE_BLOC_SMALL 1024
#define TAILLE_BLOC_BIG 71680
#define TAILLE_BLOC 	TAILLE_BLOC_SMALL



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

//on accumule les freq de caracteres dans input
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


#ifdef TAILLE_BLOC

//#undef TAILLE_BLOC
//#define TAILLE_BLOC TAILLE_BLOC_BIG

void utilitaire2::compresser(const std::string& src, const std::string& dest)
{

	std::cout << "commencement compression (taille de bloc " << TAILLE_BLOC << ")" << std::endl;
	arbre2 a(src);
	std::cout << "taille de l'arbre:" << a.ecrire(dest) << std::endl;

	unsigned decalage = 0;
	char buffer[TAILLE_BLOC];
	unsigned ecrits = 0;
	bool continuer = true;
	unsigned bit_offset = 0;
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
				std::cout << TAILLE_BLOC  << " octets lus / " ;
			}
			else
			{
				data.append(buffer,ifs.gcount());
				std::cout << ifs.gcount() << " octets lus / ";
				continuer = false;
			}
			ifs.close();
		}
		else{
			std::cerr << "Impossible d'ouvrir fichier source " << src << ". Compression interrompue" << std::endl;
			return;
		}
		int * code = convertir_vers_code(data,a.table_codage);
		char* bits = convertir_en_bit2(code+1,code[0]);

		//si il y a un nombre de bits non multiple de 8 il faut ecraser la fin de fichier 
		//en rajoutant la partie droite du premier octect code suivant
		unsigned char mask_bytes [] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x5f, 0x7f, 0xff };
		char c = '\0';
		if(bit_offset>0)
		{
			char pc[1];

			ifs.open(dest, std::ios_base::in | std::ios_base::binary);
			if( ifs.is_open() )
			{
				ifs.seekg(-1,ifs.end);
				ifs.read(pc,1);

				c = mask_bytes[8-bit_offset] & (bits[1] >> (8-bit_offset));
				c = *pc | c;
				ifs.close();
			}
			else
				std::cerr << "Impossible d'ouvrir le fichier de destination en lecture" << std::endl;
		}

		std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary | std::ios_base::app);
		if (ofs.is_open())
		{
			unsigned size = utilitaire::nb_octets(code[0]);

			std::cout << size << " octets compresses" << std::endl;

			//on écrase la fin du fichier avec le dernier caractère mixe avec le debut du code suivant
			if(bit_offset>0)
			{
				ofs.seekp(-1,ofs.end);
				ofs.write(&c,1);
				unsigned j;
				//on ecrit les code en tenant compte de l'offset de bit
				for ( j =  0; j < size - 1; j += 1 )
				{
					c = (bits[j] << bit_offset) | ( mask_bytes[8-bit_offset] & (bits[j+1] >> (8-bit_offset)) ) ;

					ofs.write(&c,1);
					//ecrits += 1;
				}
				c = bits[j] << bit_offset;
				ofs.write(&c,1);
				//ecrits += 1;

			}
			else
			{
				ofs.write(bits,size);
			}
			ecrits += size;

			bit_offset = (bit_offset + code[0])%8; // on réévalue le nouveau offset

			ofs.close();
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
#undef  TAILLE_BLOC
#define TAILLE_BLOC TAILLE_BLOC_SMALL

//#pragma message ("TAILLE_BLOC redefined to " ##TAILLE_BLOC)

#endif

#ifdef TAILLE_BLOC

//#undef TAILLE_BLOC
//#define TAILLE_BLOC TAILLE_BLOC_BIG

void utilitaire2::decompresser(const std::string& src, const std::string& dest)
{
	std::cout << "commencement decompression (taille de bloc " << TAILLE_BLOC << ")" << std::endl;

	arbre2 a("");
	//on lit l'arbre  en en-tete du fichier source
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
		unsigned decalage_bloc = lu;
		unsigned decalage_bit = 0;
		unsigned taille_bloc_lu = 0;
		lu = 0;
		{
			std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary);
			if (ofs.is_open())
			{
				ofs.close();
			}

		}
		do{
			std::ifstream ifs(src, std::ios_base::in | std::ios_base::binary);

			if(ifs.is_open())
			{
				ifs.seekg(decalage_bloc,ifs.beg);

				ifs.read(buffer,TAILLE_BLOC);

				if(ifs)
					taille_bloc_lu = TAILLE_BLOC;
				else // on est en fin de fichier, on fait la derniere passe et on arrete
				{
					taille_bloc_lu = ifs.gcount();
					continuer = false;
				}
				ifs.close();
				std::cout << "[" << decalage_bloc << "," <<  decalage_bloc+taille_bloc_lu << "] taille bloc: " << taille_bloc_lu << " ";

				decalage_bloc += taille_bloc_lu;

			}

			std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary | std::ios_base::app);
			if(ofs.is_open())
			{
				char c;
				unsigned offset = decalage_bit;
				//std::cout << "reading chars : ";
				while( offset < taille_bloc_lu*8 )
				{
					c='\0';
					//on decode chq caractere
					int fait = a.getchar(buffer,taille_bloc_lu,offset,c);
					if(c!='\0')
					{
						//std << cout << "'" << c << "' ";
						ofs.put(c);
						offset = fait;
						decalage_bit = 0;
					}
					else
					{
						if (size*8 - decalage_bloc*8 - offset >= 8) //
						{
							// le décodage a echoué il faut relire un bloc a partir du dernier caractère lu correctement 
							// on doit revenir en arrière en arrondissant à l'octet inferieur à offset (qui est en bits)
							
							decalage_bloc +=  ((offset/8) - taille_bloc_lu);
							std::cout << "error reading, restarting from " << decalage_bloc << " with bit offset " << decalage_bit ;
							decalage_bit = (offset%8);

							break;
						}
						else //il ne reste que quelques bits de bourrage en fin de fichier: on arrete
						{
							continuer = false;
						}
					}
				}
				std::cout << std::endl;
				//ofs << '@' << decalage_bit << '@';
				ofs.close();
			}
			else
			{
				std::cerr << "Imposible d'ouvrir le fichier de destination " << dest << std::endl;
				continuer = false;
			}
		}while(continuer);

		std::cout << decalage_bloc << " octets lus" << std::endl;
		
	}
	else
		std::cerr << "l'arbre d'en tete n'a pas ete lu" << std::endl;

}
#undef  TAILLE_BLOC
#define TAILLE_BLOC TAILLE_BLOC_SMALL
#endif
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

	int i = 0;
	//double total_occurences = 0;
	while(freq[i].first!='\0')
	{
		//total_occurences += (double)freq[i].second;

		char_list.append(1,freq[i++].first);
	}
	creer_table_codage(char_list);

	/*double esp = 0.0;
	for(i=0; i<256;++i)
	{
		if ( table_codage[i] )
		{
			int j;
			for( j = 0; freq[j].first != '\0' && freq[j].first != (char)i ; j+=1)
				;
			if(freq[j].first != '\0'){
				esp += (double)(freq[j].second * table_codage[i][0]);
			}
			else
				std::cerr << "Calcul esperance: le caractere " << (char)i << " n'est pas trouvé" << std::endl ;
		}
	}
	esp = esp / total_occurences;
	std::cout << "L'espérence de la taille en bit de l'encodage dun caractere est de " << esp << std::endl;*/
	delete freq;
}

