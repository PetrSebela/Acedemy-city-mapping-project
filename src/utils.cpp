#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "utils.hpp"

std::vector<std::string> split(std::string sen, char delimiter) {
  
    std::stringstream ss(sen);
    std::string word;
    std::vector<std::string> words;

	while (getline(ss, word, delimiter)) 
	{        
		words.push_back(word);
    }
    
    return words;
}