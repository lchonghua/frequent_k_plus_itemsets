#include<iostream>
#include<string>
#include<map>
#include<list>
#include"DBtransform.h"
#include"trie.h"
using namespace std;

int min_sup, k;
string inputPath, outputPath;
map<string, int> itemMap;
map<int, string> itemMap_rev;
map<set<int>, int> transformed_db;
vector<itemInfo> itemCountVector;//store each item's frequency
list<itemInfo> itemCountList;
vector<list<node*>> k_level_list;
ofstream output;
int level;
int main(int argc, char* argv[])
{
	if(argc == 5)
	{
		min_sup = stoi(argv[1]);
		k = stoi(argv[2]);
		inputPath = argv[3];
		outputPath = argv[4];
		cout<<"\nThe minimum support is: " << min_sup;
		cout<<"\nThe k is: " << k;
		cout<<"\nThe input path is: " << inputPath;
		cout<<"\nThe output path is: " << outputPath;
		
		db_transform(min_sup, k, inputPath, itemMap, itemMap_rev,transformed_db, itemCountVector);	
		
		//assign the item and support vector to an equivalent list for easy deletion
		vector<itemInfo>::iterator it_itemCountV = itemCountVector.begin();
		while(it_itemCountV != itemCountVector.end())
		{
			itemCountList.push_back(*it_itemCountV);
			it_itemCountV++;
		}
		itemCountVector.clear();

		//pruning the items less than min_sup
		list<itemInfo>::iterator it_itemCountL = itemCountList.begin();
		while(it_itemCountL != itemCountList.end())
		{
			if(it_itemCountL->support < min_sup)
				it_itemCountL = itemCountList.erase(it_itemCountL);
			else it_itemCountL++;
		}
		//The pruned 1-item list will be used at the 1st level for trie.

		output.open(outputPath, ios::app);
/*----------------------------------------------------------------------------
 *----------------------------------------------------------------------------
 */
//start building the trie
		node* root = create_node(0, 0);
		
		list<node*> templist;
		templist.push_back(root);
		k_level_list.push_back(templist);
		templist.clear();

		//build L1
		build_L1(root, itemCountList, k_level_list);
		cout<<"\nEnd of L1 building. Number of L1 nodes: "<<k_level_list[1].size();
		print_freq_set(root, 1, k, " ", output, itemMap_rev);

		//build L2
		build_L2(root, k_level_list);//just candidate
		cout<<"\nEnd of L2 candidates generation.";
		create_freq_set(root, transformed_db, 2, min_sup);//get count and prune
		cout<<"\nEnd of L2 pruning.";
		k_level_list.resize(3);
		update_k_level_list(root, k_level_list, 2);
		cout<<"\nNumer of L2 nodes: "<<k_level_list[2].size();
		print_freq_set(root, 2, k, " ", output, itemMap_rev);

		//build L3 and more
		if(k_level_list[2].size()>1)
		{
			level = 2;
			do
			{
				level++;
				list<node*>::iterator it1 = k_level_list[level-2].begin();
				while(it1!= k_level_list[level-2].end())
				{
					if(!(*it1)->child_ptr_list.empty())
					{
						list<node*>::iterator it2, it3;
						for(it2 = (*it1)->child_ptr_list.begin(); it2 != prev((*it1)->child_ptr_list.end(), 1); ++it2)
						{
							for(it3 = next(it2, 1); it3 != (*it1)->child_ptr_list.end(); ++it3)
							{
								node*tempNode = new node;
								tempNode->item = (*it3)->item;
								tempNode->support_count = 0;
								tempNode->node_level = level;
								(*it2)->child_ptr_list.push_back(tempNode);
							}
						}
					}
					it1++;
				}
				cout<<"\nFinish building candidates for level "<< level;

				create_freq_set(root, transformed_db, level, min_sup);//scan db, count and prune

				cout<<"\nupdateing k_level_list...";
				k_level_list.resize(level+1);
				update_k_level_list(root, k_level_list, level);

				cout<<"\nLevel "<<level<< "size = "<<k_level_list[level].size();
				cout<<"\nprinting....";
				if(k_level_list[level].size() !=0)
					print_freq_set(root, level, k, " ", output,  itemMap_rev);
				else break;
			}while(k_level_list[level].size() >1);
		}
		
		output.close();

	}
	else
		cout<<"\nThe number of arguments should be 5!";
	
	return 0;
}