// SImpleDB.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <fstream>

using namespace std;

#define MAXWAY 256
#define NAMELENGHT 20



struct product
{
	char naming[NAMELENGHT] = "";
	long code = 0;
	long price = 0;
	long category = 0; // takes value from another table by key
	bool correct = 0;
	bool deleted = 0;
};

struct types
{
	long categoryID = 0;
	char type[NAMELENGHT] = "";
	bool correct = 0;
};

class binStream
{
public:
	char fway[MAXWAY]; // way to db file
	FILE *streamer = NULL;
	
	binStream(char way[MAXWAY])
	{
		strcpy(fway, way);
		
		this->streamer = fopen(fway, "r+b");
		if (streamer == NULL)
		{
			cerr << "No such file found\n";
		}

	}
	void closing()
	{
		fclose(streamer);
	}
	void clear()//clear file and open them in r+b 
	{
		fclose(streamer);
		streamer = fopen(fway, "w");
		fclose(streamer);
		streamer = fopen(fway, "r+b");
	}
};

bool checkKeyValue(binStream &stream, long id)
{
	product newprod;
	fseek(stream.streamer, 0, SEEK_END);
	int offset = (sizeof(newprod.code) + sizeof(newprod.naming) + sizeof(newprod.category) + sizeof(newprod.price) + sizeof(newprod.deleted));
	int filesize = ftell(stream.streamer) / offset;
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&newprod.code,     sizeof(newprod.code),     1, stream.streamer);
		fread(&newprod.naming,   sizeof(newprod.naming),   1, stream.streamer);
		fread(&newprod.category, sizeof(newprod.category), 1, stream.streamer);
		fread(&newprod.price,    sizeof(newprod.price),    1, stream.streamer);
		fread(&newprod.deleted,  sizeof(newprod.deleted),  1, stream.streamer);
		if (!newprod.deleted && newprod.code == id)
			return 0;
		
	}
	return 1;
}

void addProd(binStream &stream, product prod)
{
	if (!checkKeyValue(stream, prod.code)) // get out if not key
	{
		cout << "\nNot a key value\n";
		return;
	}
	fseek(stream.streamer, 0, SEEK_END);
	fwrite(&prod.code,     sizeof(prod.code),     1, stream.streamer);
	fwrite(&prod.naming,   sizeof(prod.naming),   1, stream.streamer);
	fwrite(&prod.category, sizeof(prod.category), 1, stream.streamer);
	fwrite(&prod.price,    sizeof(prod.price),    1, stream.streamer);
	fwrite(&prod.deleted,  sizeof(prod.deleted),  1, stream.streamer);
}

void addType(binStream &stream, types type)
{
	fseek(stream.streamer, 0, SEEK_END);
	fwrite(&type.categoryID, sizeof(type.categoryID), 1, stream.streamer);
	fwrite(&type.type,       sizeof(type.type),       1, stream.streamer);
}

types searchType(binStream &stream, long code)
{
	types tmpres;
	fseek(stream.streamer, 0, SEEK_END);
	int filesize = ftell(stream.streamer)/(sizeof(tmpres.categoryID) + sizeof(tmpres.type));
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&tmpres.categoryID, sizeof(tmpres.categoryID), 1, stream.streamer);
		fread(&tmpres.type,       sizeof(tmpres.type),       1, stream.streamer);
		if (tmpres.categoryID == code)
		{
			tmpres.correct = 1;
			return tmpres;
		}
	}

	return tmpres;
}


void changeProd(binStream &stream,binStream &typ ,int i, product newprod) // change on i position in file
{
	if (!searchType(typ, newprod.category).correct)
	{
		cout << "\nThis type doesn\'t exist\n";
		return;
	}
	int offset = i*(sizeof(newprod.code) + sizeof(newprod.naming) + sizeof(newprod.category) + sizeof(newprod.price) + sizeof(newprod.deleted));
	fseek(stream.streamer, offset, SEEK_SET);
	fwrite(&newprod.code,     sizeof(newprod.code),     1, stream.streamer);
	fwrite(&newprod.naming,   sizeof(newprod.naming),   1, stream.streamer);
	fwrite(&newprod.category, sizeof(newprod.category), 1, stream.streamer);
	fwrite(&newprod.price,    sizeof(newprod.price),    1, stream.streamer);
	fwrite(&newprod.deleted,  sizeof(newprod.deleted),  1, stream.streamer);
}



int getIndexByCode(binStream &stream, long code)
{
	product tmpres;
	int result = -1;
	fseek(stream.streamer, 0, SEEK_END);
	int offset = (sizeof(tmpres.price) + sizeof(tmpres.naming) + sizeof(tmpres.category) + sizeof(tmpres.code)+sizeof(tmpres.deleted));
	int filesize = ftell(stream.streamer)/offset;
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&tmpres.code, sizeof(tmpres.code), 1, stream.streamer);
		fseek(stream.streamer, offset - sizeof(tmpres.code) - sizeof(tmpres.deleted), SEEK_CUR); //skip not used elems
		fread(&tmpres.deleted, sizeof(tmpres.deleted), 1, stream.streamer);
		if (tmpres.code == code && !tmpres.deleted)
		{
			result = i;
			return result;
		}
		
	}
	return result;
}

product readProd(binStream &stream, int i)
{
	product tmpprod;
	int offset = (sizeof(tmpprod.price) + sizeof(tmpprod.naming) + sizeof(tmpprod.category) +sizeof(tmpprod.code)+sizeof(tmpprod.deleted))*i;

	fseek(stream.streamer, 0, SEEK_END);
	int filelenght = ftell(stream.streamer);
	if (offset >= filelenght)
	{
		cout << "\nNo elem with this ID\n";
		tmpprod.correct = 0;

		return tmpprod;
	}

	fseek(stream.streamer, offset, SEEK_SET);
	fread(&tmpprod.code,     sizeof(tmpprod.code),     1, stream.streamer);
	fread(&tmpprod.naming,   sizeof(tmpprod.naming),   1, stream.streamer);
	fread(&tmpprod.category, sizeof(tmpprod.category), 1, stream.streamer);
	fread(&tmpprod.price,    sizeof(tmpprod.price),    1, stream.streamer);
	fread(&tmpprod.deleted,  sizeof(tmpprod.deleted),  1, stream.streamer);
	tmpprod.correct = 1;

	return tmpprod;
}

void readAllT(binStream stream, binStream typ)
{
	product tmpprod;
	types tmptype;
	fseek(stream.streamer, 0, SEEK_END);
	int offset = (sizeof(tmpprod.price) + sizeof(tmpprod.naming) + sizeof(tmpprod.category) + sizeof(tmpprod.code) + sizeof(tmpprod.deleted));
	int filelenght = ftell(stream.streamer) / offset;
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filelenght; i++)
	{
		tmpprod = readProd(stream, i);
		tmptype = searchType(typ, tmpprod.category);
		if (!tmpprod.deleted)
		{
			cout << '\n' << tmpprod.code << " " << tmpprod.naming << " " << tmptype.type << " " << tmpprod.price << "$";
		}
	}
	cout << "\n";
}

int numProd(binStream &stream)
{
	product tmpprod;
	int offset = (sizeof(tmpprod.price) + sizeof(tmpprod.naming) + sizeof(tmpprod.category) + sizeof(tmpprod.code)+sizeof(tmpprod.deleted));
	fseek(stream.streamer, 0, SEEK_END);
	int res = (ftell(stream.streamer) / offset);
	return res;
}

void readAll(binStream stream)
{
	product tmpprod;
	int filesize = numProd(stream);
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&tmpprod.code,     sizeof(tmpprod.code),     1, stream.streamer);
		fread(&tmpprod.naming,   sizeof(tmpprod.naming),   1, stream.streamer);
		fread(&tmpprod.category, sizeof(tmpprod.category), 1, stream.streamer);
		fread(&tmpprod.price,    sizeof(tmpprod.price),    1, stream.streamer);
		fread(&tmpprod.deleted,  sizeof(tmpprod.deleted),  1, stream.streamer);
		if(!tmpprod.deleted)
			cout << "\n" << tmpprod.code << " " << tmpprod.naming << " " << tmpprod.category << " " << tmpprod.price << "$";
	}
	cout << '\n';
}

void markDeleted(binStream stream, long id)
{
	product tmpprod;
	tmpprod.deleted = 1;
	int index = getIndexByCode(stream, id);
	if (index == -1)
	{
		cout << "\nNo such elem id\n";
		return;
	}
	int offset = sizeof(tmpprod.price) + sizeof(tmpprod.naming) + sizeof(tmpprod.category) + sizeof(tmpprod.code) + sizeof(tmpprod.deleted);
	offset *= index;
	offset += sizeof(tmpprod.price) + sizeof(tmpprod.naming) + sizeof(tmpprod.category) + sizeof(tmpprod.code);
	fseek(stream.streamer, offset, SEEK_SET);
	fwrite(&tmpprod.deleted, sizeof(tmpprod.deleted), 1, stream.streamer);
}

void archivate(binStream stream)
{
	product tmpprod;
	int filesize = numProd(stream);

	ofstream ofs("productstmp.bin");
	ofs.close();

	binStream tmpstr("productstmp.bin");
	fseek(tmpstr.streamer, 0, SEEK_SET);
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&tmpprod.code,     sizeof(tmpprod.code),     1, stream.streamer);
		fread(&tmpprod.naming,   sizeof(tmpprod.naming),   1, stream.streamer);
		fread(&tmpprod.category, sizeof(tmpprod.category), 1, stream.streamer);
		fread(&tmpprod.price,    sizeof(tmpprod.price),    1, stream.streamer);
		fread(&tmpprod.deleted,  sizeof(tmpprod.deleted),  1, stream.streamer);

		if (!tmpprod.deleted)
		{
			fwrite(&tmpprod.code,     sizeof(tmpprod.code),     1, tmpstr.streamer);
			fwrite(&tmpprod.naming,   sizeof(tmpprod.naming),   1, tmpstr.streamer);
			fwrite(&tmpprod.category, sizeof(tmpprod.category), 1, tmpstr.streamer);
			fwrite(&tmpprod.price,    sizeof(tmpprod.price),    1, tmpstr.streamer);
			fwrite(&tmpprod.deleted,  sizeof(tmpprod.deleted),  1, tmpstr.streamer);
		}
	}
	stream.closing();
	tmpstr.closing();
	remove(stream.fway);
	rename("productstmp.bin", stream.fway);
}
void readTypes(binStream stream)
{
	types tmptype;
	fseek(stream.streamer, 0, SEEK_END);
	int offset = sizeof(tmptype.categoryID) + sizeof(tmptype.type);
	int fsize = ftell(stream.streamer) / offset;
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < fsize; i++)
	{
		fread(&tmptype.categoryID, sizeof(tmptype.categoryID), 1, stream.streamer);
		fread(&tmptype.type,       sizeof(tmptype.type),       1, stream.streamer);
		cout << "\n" << tmptype.categoryID << " " << tmptype.type;
	}
	cout << "\n";
}

void readSelectedType(binStream stream, long key)
{
	product tmpprod;
	int filesize = numProd(stream);
	fseek(stream.streamer, 0, SEEK_SET);
	for (int i = 0; i < filesize; i++)
	{
		fread(&tmpprod.code, sizeof(tmpprod.code), 1, stream.streamer);
		fread(&tmpprod.naming, sizeof(tmpprod.naming), 1, stream.streamer);
		fread(&tmpprod.category, sizeof(tmpprod.category), 1, stream.streamer);
		fread(&tmpprod.price, sizeof(tmpprod.price), 1, stream.streamer);
		fread(&tmpprod.deleted, sizeof(tmpprod.deleted), 1, stream.streamer);
		if (!tmpprod.deleted && tmpprod.category == key)
			cout << "\n" << tmpprod.code << " " << tmpprod.naming << " "  << tmpprod.price;
	}
	cout << '\n';
}

void work(binStream &fprod, binStream &ftypes)
{
	string cmnd;
	product tmpprod;
	types tmptype;
	while (cmnd != "END")
	{
		cin >> cmnd;
		if (cmnd == "RSELT")
		{
			cout << "\nType ID\n";
			long tmpid;
			cin >> tmpid;
			readSelectedType(fprod, tmpid);
		}
		if (cmnd == "RTYPES")
		{
			readTypes(ftypes);
			continue;
		}
		if (cmnd == "ADDP")
		{
			cout << "\nAdd new product (code name category price)\n";
			cin >> tmpprod.code >> tmpprod.naming >> tmpprod.category >> tmpprod.price;
			if (searchType(ftypes, tmpprod.category).correct)
				addProd(fprod, tmpprod);
			else
			{
				cout << "\nNo such category\n";
			}
			continue;
		}
		if (cmnd == "CPROD")
		{
			cout << "\nCode to change\n";
			long it;
			int ind;
			product newprod;
			cin >> it;
			ind = getIndexByCode(fprod, it);
			if (ind != -1)
			{
				cout << "\nReplace with (name category price)\n";
				cin >> newprod.naming >> newprod.category >> newprod.price;
				newprod.code = it;
				changeProd(fprod, ftypes, ind, newprod);
			}
			else
			{
				cout << "\nNo items with this code\n";
			}
			continue;
		}

		if (cmnd == "ADDT")
		{
			cin >> tmptype.categoryID >> tmptype.type;
			addType(ftypes, tmptype);
			continue;
		}
		if (cmnd == "READ")
		{
			int tmpnum;
			cin >> tmpnum;
			tmpprod = readProd(fprod, tmpnum - 1);
			tmptype = searchType(ftypes, tmpprod.category);
			if (tmpprod.correct)
				cout << '\n' << tmpprod.code << " " << tmpprod.naming << " " << tmptype.type << " " << tmpprod.price << "\n";
			continue;
		}
		if (cmnd == "READALL")
		{
			readAll(fprod);
			continue;
		}
		if (cmnd == "READALLT")
		{
			readAllT(fprod, ftypes);
			continue;
		}
		if (cmnd == "DROPDB")
		{
			fprod.clear();
			ftypes.clear();
			continue;
		}
		if (cmnd == "DEL")
		{
			cout << "\nEnter ID\n";
			long tmpid;
			cin >> tmpid;
			markDeleted(fprod, tmpid);
			continue;
		}

	}
	archivate(fprod); // rewrite file without elems with del mark
	fprod.closing();
	ftypes.closing();
}

int main()
{
	binStream fprod("D:/work/products.bin");
	binStream ftypes("D:/work/types.bin");
	
	work(fprod, ftypes);
	system("pause");
    
	return 0;
}

