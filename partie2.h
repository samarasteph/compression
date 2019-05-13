
#ifndef __PARTIE2_H__
#define __PARTIE2_H__

#include <list>
#include "partie1.h"

class utilitaire2 : public utilitaire {
	//fonction de calcul de frequence pour iteration par bloc

public:

	std::pair<char*,int> readfile(const std::string& file_path, unsigned decalage_bloc) const;
	std::pair<char,int>* frequence(std::list< std::pair< char, int > >& items) const;
	void frequence_bloc(const char input[], int size, std::list< std::pair< char, int > >& items) const;

	void compresser(const std::string& src, const std::string& dest);
	void decompresser(const std::string& src, const std::string& dest);

};

class arbre2 : public arbre {

friend class utilitaire2;

public:

	arbre2(const std::string& file_path);
	std::string char_list;

};

#endif