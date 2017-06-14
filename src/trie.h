#ifndef TRIE_H
#define TRIE_H
#include <vector>
#include <list>
#include "DBtransform.h"
using namespace std;

struct node
{
	int item;
	int support_count;
	list<node*> child_ptr_list;
	int node_level;
};

node* create_node(int item, int level);
node* find_item_in_node(int item, node* target);
void insert_child(int item, node* parent);
void build_L1(node* root, list<itemInfo>& freq_1_itemset, vector<list<node*>>& k_level_list);
void build_L2(node* root, vector<list<node*>>& k_level_list);
void candidate_supp_count(node* currentNode, vector<int>& trans, int trans_copy_num, int pos, int level);
void create_freq_set(node* root, map<set<int>, int>& database, int level, int min_sup);
void prune(node* root, int level, int min_sup);
void update_k_level_list(node* root, vector<list<node*>>& k_level_list, int end_level);
void print_freq_set(node* root, int level, int k, string prefix, ofstream& outFile, map<int, string>& itemMap_rev);

#endif
