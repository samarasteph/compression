#include <deque>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <bitset>

#include "partie1.h"
#include "partie2.h"

//------------------------------------------------
//			TESTS (à enlever... ou pas)
//------------------------------------------------

void reset_file(const std::string& path)
{
	std::ofstream f(path,std::ios_base::out | std::ios_base::trunc);
	if(f.is_open())
	{
		f.close();
	}
}

void log_code(const std::string& path, int code[])
{
	std::ofstream f(path, std::ios_base::out | std::ios_base::app );
	if (f.is_open())
	{
		for(unsigned i=1; i <= code[0]; i+=1)
		{
			f << code[i];
		}
		f.close();
	}
}

void log_bits(const std::string& path, const char bits)
{
	std::ofstream f(path, std::ios_base::out | std::ios_base::app );
	if (f.is_open())
	{
		std::bitset<8> b(bits);
		f << b;
		f.close();
	}
}

void log_file(const std::string& path, const std::string& str)
{
	std::ofstream f(path, std::ios_base::out | std::ios_base::app );
	if (f.is_open())
	{
		f.write(str.c_str(),str.size());
		f.close();
	}
}

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