#ifndef DBTRANSFORM_H
#define DBTRANSFORM_H

#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<set>
#include<vector>
using namespace std;

struct itemInfo
{
	int itemId;
	int support;
};

void db_transform(int sup, int k, string input_path, map<string, int>& itemMap, map<int, string>& itemMap_rev,
				  map<set<int>, int>& transformed_db, vector<itemInfo>& item_and_count);
//transform string items to int items, maintain a map to record the change;
//stored the transfromed database in a vector<vector<int>> structure
//remove transactions that contains items less than k

#endif