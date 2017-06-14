#include"DBtransform.h"
#include<sstream>


void db_transform(int sup, int k, string input_path, map<string, int>& itemMap, map<int, string>& itemMap_rev,
				  map<set<int>, int>& transformed_db, vector<itemInfo>& item_and_count)
{
	string item;
	string line;
	int itemId = 0;
	set<int> tempTransaction;
	map<string, int>::iterator it_itemMap;
	map<set<int>, int>::iterator it_db;
	ifstream readfile;
	
	readfile.open(input_path);
	if(readfile.is_open())
	{
		while(!readfile.eof())
		{
			getline(readfile, line);
			stringstream ss(line);
			while(ss>>item)
			{
				it_itemMap = itemMap.find(item);
				if(it_itemMap == itemMap.end())
				{
					itemId ++;				
					itemMap.insert(make_pair(item, itemId));
					itemMap_rev.insert(make_pair(itemId, item));
					tempTransaction.insert(itemId);

					itemInfo tempItemInfo;
					tempItemInfo.itemId = itemId;
					tempItemInfo.support = 1;
					item_and_count.push_back(tempItemInfo);
				}
				else
				{
					tempTransaction.insert(it_itemMap->second);
					item_and_count[(it_itemMap->second) -1].support++;
				}
			}
			if((int)tempTransaction.size()>=k)
			{
				it_db = transformed_db.find(tempTransaction);
				if(it_db == transformed_db.end())
					transformed_db.insert(make_pair(tempTransaction, 1));
				else
					it_db->second++;
			}
			tempTransaction.clear();
		}
		readfile.close();
		cout<<"\nThere are "<<itemMap.size()<<" unique customer IDs.";
		cout<<"\nThere are "<<transformed_db.size()<<" unique transactions"
			<<"\n(identical transactions are grouped together, copy number is recorded.)";
		
	}
	else
		cout<<"\nCannot open file!";
}
