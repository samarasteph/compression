#include <list>
#include <iostream>
#include <cstring>
#include <stack>
#include <deque>
#include <bitset>

#include "partie1.h"

std::pair<char*,int> utilitaire::readfile(const std::string& file_path) const {
	std::ifstream f(file_path, std::ios_base::in | std::ios_base::binary);
	
	std::pair<char*,int> pr;
	pr.first = NULL;
	pr.second = 0;

	if (f.is_open())
	{
		char c;
		std::list<char > items;
		std::list< char >::iterator it;
		while(!f.eof())
		{
			f.get(c);
			items.push_back(c);
		}

		pr.second = items.size();
		pr.first = new char [pr.second];
		int index = 0;
		for (it = items.begin(); it != items.end() ; ++it)
		{
			pr.first[index++] = *it;
		}
		f.close();
	}

	return pr;
}

std::pair<char,int>* utilitaire::frequence(const char input[], int size) const
{
	std::list< std::pair< char, int > > items;
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
	std::pair<char,int> * freq = new std::pair<char,int>[items.size()+1];
	i = 0;
	for (it = items.begin(); it != items.end(); ++it){
		freq[i].first = it->first;
		freq[i].second = it->second;
		i += 1;
	}
	//set extra last element to zero to size the array
	freq[i].first = freq[i].second = 0;

	return freq;
}

Noeud* utilitaire::construire_arbre(std::pair<char,int>* T) const
{
	int size = 0;
	
	while(T[size].first>0)
		size += 1;

	Noeud** L = new Noeud*[size];

	for ( int i = 0; i < size; i += 1 )
	{
		int j;

		//ici l'enonce indique d'utiliser les fonctions 'indice' 
		// et 'insert_tab_tri' non fournies. A comparer.
		Noeud* pn = new Noeud(T[i].first,T[i].second);
		for (j=0; j<i && pn->occ > L[j]->occ; j+=1)
			;
		for ( int k = i; j<k; k -= 1)
		{
			L[k] = L[k-1];
		}
		L[j] = pn;
	}

	int i = 0;
	Noeud * root = new Noeud();
	while(i<size)
	{
		if (root->fg==NULL)
		{
			root->fg = L[i];
			root->occ += root->fg->occ;
		}
		else if (root->fd == NULL)
		{
			root->fd = L[i];
			root->occ += root->fd->occ;
		}
		else
		{
			Noeud * new_node = new Noeud();
			new_node->fg = root;
			new_node->occ += root->occ + L[i]->occ;
			root = new_node;
			root->fd = L[i];
		}
		i += 1;
	};

	return root;
}

int* utilitaire::convertir_vers_code(const std::string& texte, int* table_codage[]) const
{
	int size = 0;
	//1ere passe: on calcule la taille totale
	for(std::string::const_iterator cit = texte.begin(); cit != texte.end(); ++cit)
	{
		size += table_codage[ (unsigned)*cit][0]; //longueur stockée à l'index 0
	}

	int *texte_encode = new int[size+1];

	texte_encode[0] = size; //prefixer avec longeur totale
	int offset = 1;

	//2eme passe: recopie des entiers dans le tableau
	for(std::string::const_iterator cit = texte.begin(); cit != texte.end(); ++cit)
	{
		size = table_codage[(unsigned)*cit][0]; //longueur stockée à l'index 0
		int *ptr = table_codage[(unsigned)*cit]+1;
		std::memcpy( texte_encode+offset , ptr, size*sizeof(int) );
		offset += size;
	}

	return texte_encode;

}
int utilitaire::nb_octets(int nb_bits)
{
	return (nb_bits / 8) + ((( nb_bits % 8 ) + 7) / 8) ; //arrondir à l'octect superieur
}

char* utilitaire::convertir_en_bit1(int* tab,int size)
{
	int nb_octets =  utilitaire::nb_octets(size);
	char* bits = new char[nb_octets];

	for(int i=0; i<nb_octets; i += 1)
	{
		std::bitset<8> bs(0);
		int offsetbit = i*8;
		for(int k=0; k+offsetbit < size && k<8; k+=1 )
		{
			bs[7-k] = tab[k+offsetbit];
		}
		char c = bs.to_ulong();
		std::memcpy(bits+i,&c,sizeof(c));
	}
	return bits;
}

char* utilitaire::convertir_en_bit2(int* tab,int size)
{
	int nb_octets =  utilitaire::nb_octets(size);
	char* bits = new char[nb_octets];

	for(int i=0; i<nb_octets; i += 1)
	{
		char c = 0;
		int offsetbit = i*8;
		for(int k=0; k+offsetbit < size && k<8; k+=1 )
		{
			char n = (char)tab[k+offsetbit];
			c = c | ( n << (7-k));
		}
		std::memcpy(bits+i,&c,sizeof(c));
	}
	return bits;
}

void utilitaire::compresser(const std::string& src, const std::string& dest)
{

	std::pair<char*,int> chars = readfile(src);
	arbre a(src);
	a.ecrire(dest);

	std::string data(chars.first,chars.second);
	a.creer_table_codage(data);

	std::ifstream ifs(src);
	data.clear();

	if(ifs.is_open())
	{
		char buffer[64];
		while(!ifs.eof())
		{
			ifs.read(buffer,64);
			if (ifs)
				data.append(buffer,64);
			else
				data.append(buffer,ifs.gcount());
		}
		ifs.close();
	}

	int * code = convertir_vers_code(data,a.table_codage);
	char* bits = convertir_en_bit1(code+1,code[0]);


	std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary | std::ios_base::app);
	if (ofs.is_open())
	{
		ofs.write(bits,utilitaire::nb_octets(code[0]));
		ofs.close();

		/*
		for (int i=0;i<utilitaire::nb_octets(code[0]);i++)
	 	{
	 		std::cout << std::bitset<8>(bits[i]) << " ";
	 	}
	 	*/
		std::cout << utilitaire::nb_octets(code[0]) << " octets écrits" << std::endl;
	}
	else
	{
		std::cerr << "cannot open destination file " << dest << std::endl;
	}
	delete [] code;
	delete [] bits;
}

void utilitaire::decompresser(const std::string& src, const std::string& dest)
{
	arbre a("");
	unsigned lu = a.lire(src);

	if(lu)
	{
		char* buffer;
		unsigned size ;

		std::ifstream ifs(src, std::ios_base::in | std::ios_base::binary);
		if(ifs.is_open())
		{
			ifs.seekg(0, ifs.end);
			unsigned end = ifs.tellg();
			size = end - lu;

			ifs.seekg(lu,ifs.beg);

			buffer = new char[size];

			ifs.read(buffer,size);
			std::cout << size << " octets lus" << std::endl;

			/*
			for (int i=0;i<size;i++)
		 	{
		 		std::cout << std::bitset<8>(buffer[i]) << " ";
		 	}
		 	*/

			ifs.close();
		}

		/*
		std::string chars;
		for(unsigned char c = 0; c < 256; c += 1)
		{
			chars.append( 1 , c );
		}
		a.creer_table_codage(chars);
		*/
		std::string output;

		std::ofstream ofs(dest,std::ios_base::out | std::ios_base::binary);
		if(ofs.is_open())
		{
			int offset = 0;
			char c;
			while( offset < size*8 )
			{
				c='\0';
				offset = a.getchar(buffer,size,offset,c);
				if(c!='\0')
				{
					//std::cout << "'" << c << "' ";
					ofs.put(c);
				}
				else
				{
					if (offset< size*8)
						std::cerr << "resultat de decodage inattendu: le caractere retourne est 0" << std::endl;
				}
			}
			ofs.close();
		}
		else
		{
			std::cerr << "Imposible d'ouvrir le fichier de destination " << dest << std::endl;
		}
	}
	else
		std::cerr << "l'arbre d'en tete n'a pas ete lu" << std::endl;

}


Noeud::Noeud(const char a_c /*='\0'*/, int a_occ/*=0*/, Noeud *a_fg/*=NULL*/, Noeud *a_fd/*=NULL*/) :
	c(a_c), occ(a_occ), fg(a_fg), fd(a_fd)
{}
Noeud::~Noeud()
{
	delete fg;
	delete fd;
}

void Noeud::ecrire(std::ostream& s) const
{
	int leaf = 0;

	if(c=='\0')
	{
		s.write(reinterpret_cast<char*>(&leaf),sizeof(leaf));//not leaf = 0
		s.write(reinterpret_cast<const char*>(&occ),sizeof(occ));

		if (fg)
		{
			fg->ecrire(s);
		}
		if (fd)
		{
			fd->ecrire(s);
		}
	}
	else
	{
		leaf = 1;
		s.write(reinterpret_cast<char*>(&leaf),sizeof(leaf));
		s.write(reinterpret_cast<const char*>(&occ),sizeof(occ));
		s.write(reinterpret_cast<const char*>(&c),sizeof(c));
	}

}

void Noeud::lire(std::istream& s)
{
	int leaf;
	s.read(reinterpret_cast<char*>(&leaf),sizeof(leaf));
	if(leaf==0)
	{
		s.read(reinterpret_cast<char*>(&occ),sizeof(occ));
		if(occ>0)
		{
			fg = new Noeud();
			fg->lire(s);
		}
		if(occ - fg->occ > 0)
		{
			fd = new Noeud();
			fd->lire(s);
		}
	}
	else if (leaf==1)
	{
		s.read(reinterpret_cast<char*>(&occ),sizeof(occ));
		s.read(reinterpret_cast<char*>(&c),sizeof(c));
	}
	else
	{

		std::cout << "unexpected leaf value: " << leaf << std::endl ;
	}
}

arbre::arbre(const std::string& s)
{
	utilitaire u;
	std::pair<char*,int> p = u.readfile(s);
	std::pair<char, int>* freq = u.frequence(p.first,p.second);
	racine = u.construire_arbre(freq);

	delete [] p.first;
	delete [] freq;
}

arbre::~arbre()
{
	detruire_arbre();
}

void arbre::detruire_arbre()
{
	delete racine;
}

unsigned arbre::ecrire(const std::string& s) const
{
	std::ofstream of(s, std::ios_base::out | std::ios_base::binary);
	unsigned res = 0;
	if (of.is_open())
	{
		racine->ecrire(of);
		res = of.tellp();
		of.close();
		return res;
	}
		return res;
}

unsigned arbre::lire (const std::string& s)
{
	std::ifstream ifs(s, std::ios_base::in | std::ios_base::binary);
	unsigned res = 0;
	if (ifs.is_open())
	{
		detruire_arbre();
		racine = new Noeud();
		racine->lire(ifs);
		res=ifs.tellg();
		ifs.close();
	}
	return res;
}

void arbre::creer_table_codage(const std::string& str)
{
	std::memset(table_codage,0,sizeof(int*)*256);

	std::string::const_iterator cit;


	for( cit = str.begin(); cit != str.end(); ++cit)
	{
		std::stack<Noeud*> pile;
		Noeud* pn = racine;

		while(pn) //empiler tous les elements à gauche
		{
			pile.push(pn);
			pn = pn->fg;
		}

		while(!pile.empty()){

			pn = pile.top();
			if( pn->c == '\0' ) //noeud non terminal
			{
				pile.push(pn->fd);
			}
			else
			{
				pile.pop();
				if (pn->c==*cit)
				{
					int taille_pile = pile.size();
					int *code = new int[taille_pile+1]; //on alloue un item en plus pour la longueur
					int index = taille_pile;

					while(!pile.empty())
					{
						Noeud* parent = pile.top();
						pile.pop();

						if (pn==parent->fd)
							code[index] = 1;
						else
							code[index] = 0;

						index -= 1;
						pn = parent;
					}
					code[0] = taille_pile; //longueur stockee en index 0

					table_codage[(unsigned)*cit] = code;
				}
				else if(pn == pile.top()->fd || !pile.top()->fd)
				{
					pile.pop();
				}
			}
			
		}
	}
}

unsigned arbre::getchar(char* data, unsigned longueur, unsigned offset, char& result) const
{
	unsigned lu = offset;

	int bit_offset = offset % 8;
	int octet_offset = offset / 8;

	static const char masks_list[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
	//on se positionne sur l'octet
	char* flux = data + octet_offset;
	//l'offset de bit est a decaler de droite a gauche

	Noeud* tete = racine;

	while(tete && tete->c=='\0') // on itere tant que le noeud est un noeud intermediaire
	{ 
		
		if(bit_offset >= 8 ) // si le mask est a zero on pass a l'octet suivant
		{
			bit_offset= 0;
			flux += 1;
			if (flux>=data+longueur) // on arrive en fin de donnee: on arrete
			{
				tete = NULL;
				break;
			}
		}

		int bit = *flux & masks_list[bit_offset++];
		if(bit==0)
			tete = tete->fg;
		else
			tete = tete->fd;
		lu +=1;
	}

	if(tete)
	{
		result = tete->c;
	}
	else
	{
		if(flux<data+longueur)
			std::cerr << "decodage non trouve dans l'arbre (offset=" << lu << ")" << std::endl;
	}
	return lu;
}


//------------------------------------------------
//			TESTS (à enlever)
//------------------------------------------------

void arbre::test(arbre& a)
{
	utilitaire u;
	std::deque<Noeud*> q;
	q.push_back(a.racine);
	int n = 1;
	while( !q.empty() )
	{
		Noeud* pn = q.front();
		if(pn->c=='\0')
		{
			std::cout << "[ node n°"<< n++ << " occ=" << pn->occ << "] "; 
			if (pn->fg)
				q.push_back(pn->fg);
			if(pn->fd)
				q.push_back(pn->fd);
		}
		else
		{
			std::cout << "[ leaf n°"<< n++ << " char='" << pn->c << "' occ=" << pn->occ << "] " << std::endl; 
		}
		q.pop_front();
	}
	std::cout << std::endl;
	std::cout << "Table de codage:" << std::endl;
	for ( int c = 0; c < 256; ++c)
	{
		if (a.table_codage[c]!=0)
		{
			std::cout << (char) c << " ";
			for( int i = 1; i <= a.table_codage[c][0]; i += 1 )
			{
				std::cout << a.table_codage[c][i];
			}
			std::cout << std::endl;
		}
	}
	std::string s("eNa");
	std::cout << "convertir en code '" << s << "'" << std::endl;
	int* code = u.convertir_vers_code(s,a.table_codage);
	for(int i=1; i<=code[0]; i+=1)
	{
		std::cout << code[i];
 	}
 	std::cout << std::endl;

 	n = utilitaire::nb_octets(code[0]);
 	std::cout << "nb octets de " << code[0] << " bits=" << n << std::endl;
 	char *bits1 = u.convertir_en_bit1(code+1,code[0]);
 	char *bits2 = u.convertir_en_bit1(code+1,code[0]);

 	std::cout << "Avec bitset: ";
 	for (int i=0;i<n;i++)
 	{
 		std::cout << std::bitset<8>(bits1[i]) << " ";
 	}
 	std::cout << std::endl;

 	std::cout << "Avec shift: ";
 	for (int i=0;i<n;i++)
 	{
 		std::cout << std::bitset<8>(bits2[i]) << " ";
 	}
 	std::cout << std::endl;

 	delete [] code;
 	delete [] bits1;
 	delete [] bits2;

}


void utilitaire::test(const char* file){
	utilitaire u;
	std::pair<char*,int> p = u.readfile(file);

	std::cout << p.second << " items in array" << std::endl;
	for(int i=0; i< p.second; i += 1)
	{
		std::cout << "\"" << p.first[i] << "\" ";
	}
	std::cout << std::endl;
	
	std::pair<char,int> *pfreq = u.frequence(p.first,p.second);
	for(int i=0; pfreq[i].first != 0; i += 1)
	{
		std::cout << "(\"" << pfreq[i].first << "\"," <<  pfreq[i].second << ") " ;
	}
	std::cout << std::endl;

	Noeud* root = u.construire_arbre( pfreq );

	std::deque<Noeud*> q;
	q.push_back(root);
	int n = 1;
	while( !q.empty() )
	{
		Noeud* pn = q.front();
		if(pn->c=='\0')
		{
			std::cout << "[ node n°"<< n++ << " occ=" << pn->occ << "] "; 
			if (pn->fg)
				q.push_back(pn->fg);
			if(pn->fd)
				q.push_back(pn->fd);
		}
		else
		{
			std::cout << "[ leaf n°"<< n++ << " char='" << pn->c << "' occ=" << pn->occ << "] " << std::endl; 
		}
		q.pop_front();
	}
	std::cout << std::endl;
	delete [] p.first;
	delete [] pfreq;
}