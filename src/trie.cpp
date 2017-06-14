#include "trie.h"

node* create_node(int item, int level)
{
	node* newNode = new node;
	newNode->item = item;
	newNode->support_count = 0;
	newNode->node_level = level;
	return newNode;
}

//return the child node that contains the item
node* find_item_in_node(int item, node* target)
{
	list<node*>::iterator it;
	for(it = (target->child_ptr_list).begin(); it != (target->child_ptr_list).end(); it++)
		if((*it)->item == item)
			return (*it);
	return NULL;
}

//insert a child node to a parent
void insert_child(int item, node* parent, int level)
{
	if(find_item_in_node(item, parent) == NULL)
	{
		node* newNode = create_node(item, level);
		parent->child_ptr_list.push_back(newNode);
	}
}

void build_L1(node* root, list<itemInfo>& freq_1_itemset,vector<list<node*>>& k_level_list)
{
	list<node*> templist;
	for(list<itemInfo>::iterator it = freq_1_itemset.begin(); it != freq_1_itemset.end(); it++)
	{
		node* tempNode = create_node(it->itemId, 1);
		tempNode->support_count = it->support;
		
		root->child_ptr_list.push_back(tempNode);
		templist.push_back(tempNode);
	}
	k_level_list.push_back(templist);
	templist.clear();
}


void build_L2(node* root, vector<list<node*>>& k_level_list)
{
	//generate all possible candidates
	list<node*> templist;
	if(root->child_ptr_list.size()!=0)
	{
		list<node*>::iterator it1, it2;
		for(it1 = root->child_ptr_list.begin(); it1 != prev(root->child_ptr_list.end(), 1); ++it1)
		{
			for(it2 = next(it1, 1); it2 != root->child_ptr_list.end(); ++it2)
			{
				node*tempNode = new node;
				tempNode->item = (*it2)->item;
				tempNode->support_count = 0;
				tempNode->node_level = 2;
				(*it1)->child_ptr_list.push_back(tempNode);
				
			}
		}
	}
}


//run a single transaction through the trie to get count
void candidate_supp_count(node* currentNode, vector<int>& trans, int trans_copy_num, int pos, int level)
{
	if(currentNode->child_ptr_list.empty()&& currentNode->node_level == level)
	{
		currentNode->support_count += trans_copy_num;
	}
	else
		for(int i = pos; i < (int)trans.size(); i++)
		{
			node* child = find_item_in_node(trans[i], currentNode);
			if(child != NULL)
				candidate_supp_count(child, trans, trans_copy_num, i+1, level);
		}
}

//prune infrequent itemsets
void prune(node* root, int level, int min_sup)
{
	list<node*>::iterator it = root->child_ptr_list.begin();
	while(it != root->child_ptr_list.end())
	{
		node* childNode = *it;
		if(!childNode->child_ptr_list.empty())
			prune(childNode, level, min_sup);
		if(childNode->child_ptr_list.empty() && childNode->support_count < min_sup)
		{
			delete *it;
			it = root->child_ptr_list.erase(it);
		}
		else
		it++;
	}
}

//scan database and call "candidate_supp_count" to get the support of each candidate
void create_freq_set(node* root, map<set<int>, int>& database, int level, int min_sup)
{
	cout<<"\ncounting...";
	for(auto& trans:database)
	{
		//for each transaction, store the items in a temp vector
		vector<int> tempTrans;
		int trans_copy_num = trans.second;
		
		for(auto& item:trans.first)
			tempTrans.push_back(item);
		for(int i = 0 ; i < (int)tempTrans.size(); i++)
		{
			node* child = find_item_in_node(tempTrans[i], root);
			if(child != NULL)
				candidate_supp_count(child, tempTrans, trans_copy_num, i+1, level);
		}
		tempTrans.clear();
	}
	cout<<"\nstart pruning...";
	prune(root, level, min_sup);
}

void update_k_level_list(node* root, vector<list<node*>>& k_level_list, int end_level)
{
	if(root->child_ptr_list.empty() && root->node_level == end_level)
	{
		k_level_list[end_level].push_back(root);
	}
	else
	{
		for(auto& child: root->child_ptr_list)
			update_k_level_list(child, k_level_list, end_level);
	}
}
void print_freq_set(node* root, int level, int k, string prefix, ofstream& outFile, map<int, string>& itemMap_rev)
{
	if(root == NULL||level < k)
		return;
	if(root->item != 0)
		prefix = prefix + itemMap_rev.find(root->item)->second +" ";
	if(root->child_ptr_list.empty() && root->node_level == level)
	{
		outFile<<prefix<<"  ("<<root->support_count<<")"<<endl;
	}
	else
	{
		for(auto& child: root->child_ptr_list)
			print_freq_set(child, level, k, prefix, outFile, itemMap_rev);
	}
}