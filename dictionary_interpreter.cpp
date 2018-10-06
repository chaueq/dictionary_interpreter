//chaueq
#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;

struct Flag
{
	string symbol;
	bool state = false;

	Flag()
	{

	}

	Flag(string x)
	{
		symbol = x;
	}
};

const uintmax_t FLAGS_NO = 3;
Flag* FLAGS = new Flag[FLAGS_NO];

struct Alphabet
{
	vector<string>* data;
	unsigned char size;
	
	Alphabet(char* name)
	{
		ifstream file(name);
		string temp;
		getline(file, temp);
		istringstream iss(temp);
		iss >> size;
		data = new vector<string>[size];
		for(uintmax_t i = 0; i < size; ++i)
		{
			getline(file, temp);
			istringstream ss(temp);
			while(ss >> temp)
			{
				reverse(temp.begin(), temp.end());
				data[i].push_back(temp);
			}
		}
		file.close();
	}
	
	uintmax_t find_symbol(string x)
	{
		for(uintmax_t i = 0; i < size; ++i)
		{
			uintmax_t row_size = data[i].size();
			for(uintmax_t j = 0; j < row_size; ++j)
			{
				if(strcmp(x.c_str(), data[i][j].c_str()) == 0)
				{
					return i;
				}
			}
		}
	}
};

struct Dnode
{
	unsigned char letter;
	Dnode* parent = NULL;
	vector<Dnode*> children;
	bool end = false;
	
	Dnode(unsigned char l, Dnode* p)
	{
		letter = l;
		parent = p;
	}
	
	Dnode* find_child(unsigned char x)
	{
		uintmax_t size = children.size();
		for(uintmax_t i = 0; i < size; ++i)
		{
			if(children[i] -> letter == x)
			{
				return children[i];
			}
		}
		
		return NULL;
	}
};

struct Word
{
	uintmax_t node = 0;
	uintmax_t mileStone = 1;
	uintmax_t iterator = 0;
	Alphabet* alpha;
	vector<Dnode*>* nodes;
	
	Word()
	{
		alpha = NULL;
		nodes = NULL;
	}
	
	Word(Alphabet* a, vector<Dnode*>* n)
	{
		alpha = a;
		nodes = n;
		setMile();
	}
	
	bool next()
	{
		
		if(iterator < (mileStone - 1))
		{
			++iterator;
			return true;
		}
		
		
		if(node < (nodes -> size() - 1))
		{
			do
			{
				++node;
			}
			while((nodes -> at(node) -> end == false) && (node < (nodes -> size() -1)));
			setMile();
			return (nodes -> at(node) -> end);
		}
		return false;
	}
	
	void setMile()
	{
		mileStone = 1;
		iterator = 0;
		Dnode* current = nodes -> at(node);
		do
		{
			mileStone *= alpha -> data[current -> letter].size();
			current = current -> parent;

			if(FLAGS[0].state && current != NULL)
			{
				if(current -> parent == NULL)
				{
					mileStone = 1;
				}
			}
		}
		while(current != NULL);
	}
	
	string generateString()
	{
		string output = "";
		Dnode* current = nodes -> at(node);
		uintmax_t dzielnik = 1;
		
		do
		{
			if(FLAGS[0].state && current != NULL)
			{
				if(current -> parent != NULL)
				{
					output += alpha -> data[current -> letter][0];
					current = current -> parent;
					continue;
				}
			}
			output += alpha -> data[current -> letter][(iterator / dzielnik) % (alpha -> data[current -> letter].size())];
			dzielnik *= alpha -> data[current -> letter].size();
			current = current -> parent;
		}
		while(current != NULL);
		
		reverse(output.begin(), output.end());
		return output;
	}
	
	void reset()
	{
		node = 0;
		setMile();
	}
};

int main(int argc, char* argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	FLAGS[0].symbol = "f";
	FLAGS[1].symbol = "a";
	FLAGS[2].symbol = "s";

	if(argc < (1 + 4))
	{
		cout << "dictionary_interpreter <alphabet file> <dictionary file> <min number of words> <max number of words in password (0 ↔ ∞)> [flags]" << endl;
		cout << "Flags:" << endl;
		cout << "f\tCombine only first letters" << endl;
		cout << "a\tUse letters from alphabet as words" << endl;
		cout << "s\tSimple mode (no letters replacement)" << endl;
		return 1;
	}

	for(uintmax_t i = 0; i < (argc - 5); ++i)
	{
		for(uintmax_t j = 0; j < FLAGS_NO; ++j)
		{
			if(strcmp(argv[i + 5], FLAGS[j].symbol.c_str()) == 0)
			{
				FLAGS[j].state = true;
			}
		}
	}

	uintmax_t Words = atoi(argv[4]);
	uintmax_t startWords = atoi(argv[3]);
	Alphabet alphabet(argv[1]);
	vector<Dnode*> nodes;
	
	for(uintmax_t i = 0; i < alphabet.size; ++i)
	{
		Dnode* TOC = new Dnode(i, NULL);
		if(FLAGS[1].state)
		{
			TOC -> end = true;
		}
		nodes.push_back(TOC);
	}
	
	ifstream dict(argv[2]);
	string word;
	
	clog << "Reading dictionary... " << flush;
	time_t now, last;
	time(&last);

	while(dict >> word)
	{
		uintmax_t word_size = word.size();
		uintmax_t* translated = new uintmax_t[word_size];
		
		for(uintmax_t i = 0; i < word_size; ++i)
		{
			translated[i] = alphabet.find_symbol(word.substr(i, 1));
		}
		
		Dnode* current = nodes[translated[0]];
		
		for(uintmax_t i = 1; i < word_size; ++i)
		{
			//cout << word.substr(i, 1) << '\t' << nodes.size() << '\t' << nodes.max_size() << '\t' << current << '\t' << current -> find_child(translated[i]) << endl;
			Dnode* tmp = current -> find_child(translated[i]);
			
			if(tmp == NULL)
			{
				tmp = new Dnode(translated[i], current);
				nodes.push_back(tmp);
				current -> children.push_back(tmp);
			}
			
			current = tmp;
		}
		
		current -> end = true;
		time(&now);
		if(difftime(now, last) != 0)
		{
			time(&last);
//			clog << "                                        \rReading dictionary... " << word << flush;
			clog << "                                                                               \r" << "Memory: " << sizeof(Dnode) * nodes.size() << " B  =  " << sizeof(Dnode) * nodes.size() / 1024 / 1024 << " MB  =  " << sizeof(Dnode) * nodes.size() / 1024 / 1024 / 1024 << " GB" << flush;
		}
	}
	
	for(uintmax_t words_no = startWords; (Words == 0) || (words_no <= Words); ++words_no)
	{
		Word* phrase = new Word[words_no];
		for(uintmax_t i = 0; i < words_no; ++i)
		{
			phrase[i].alpha = &alphabet;
			phrase[i].nodes = &nodes;
			phrase[i].setMile();
		}
		
		bool dodane;
		do
		{
			for(uintmax_t i = 0; i < words_no; ++i)
			{
				cout << phrase[i].generateString();
			}
			
			cout << endl << flush;
			dodane = false;
		
			for(uintmax_t i = 0; i < words_no; ++i)
			{
				if(phrase[i].next())
				{
					dodane = true;
					break;
				}
			
				else
				{
					phrase[i].reset();
				}
			}
		}
		while(dodane);
	}
	
	return 0;
}
