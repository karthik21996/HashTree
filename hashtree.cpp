#include <bits/stdc++.h>
using namespace std;
#define pb push_back
#define ins insert
#define mp make_pair
#define ff first
#define ss second
// ordering of the items sets would be first yes then no
#define totalTrans 435
#define modVal 10
#define buckSize 10

typedef struct nodeTrans{
	vector <int> trans;
	int supCount;
	nodeTrans(){
		supCount = 0;
		trans.clear();
	}
}nodeTrans;

struct hashNode{
	int leaf; // whether the current node has been leaf or not if it is a leaf node or not
	struct hashNode *child[modVal];
	vector < nodeTrans > itemSet; // Itemset , count
	int level; // which index on which mod value should be taken
	hashNode(){
		for(int i = 0  ; i < modVal ; i++){
			child[i] = NULL;
		}
		level = 0;
		leaf = 1;
	}
};

float supportThreshold = 0.4;
float confiThreshold = 0.8;
int trans[500][35];
string line , temp;
string attr[] = {"handicapped-infants=y","handicapped-infants=n","water-project-cost-sharing=y","water-project-cost-sharing=n","adoption-of-the-budget-resolution=y","adoption-of-the-budget-resolution=n","physician-fee-freeze=y","physician-fee-freeze=n","el-salvador-aid=y","el-salvador-aid=n","religious-groups-in-schools=y","religious-groups-in-schools=n","anti-satellite-test-ban=y","anti-satellite-test-ban=n","aid-to-nicaraguan-contras=y","aid-to-nicaraguan-contras=n","mx-missile=y","mx-missile=n","immigration=y","immigration=n","synfuels-corporation-cutback=y","synfuels-corporation-cutback=n","education-spending=y","education-spending=n",
"superfund-right-to-sue=y","superfund-right-to-sue=n","crime=y","crime=n","duty-free-exports=y","duty-free-exports=n","export-administration-act-south-africa=y","export-administration-act-south-africa=n","Class=democrat","Class=republican"};
float sup1ItemSet[35]; // Initial support of 1 item sets
vector <int>  inpTrans[450]; // input transactions
vector <nodeTrans *>  freqItemSets[50] ; // freq item sets of size k , will contain the pointer to node->itemset

void insertHashTree(int k ,struct hashNode *root , struct nodeTrans Cfk){
	if(root->leaf==1){ // if it is a leaf node
		struct nodeTrans t;
		if(root->itemSet.size() < buckSize || root->level >= (k-1)){
			root->itemSet.pb(Cfk);
		}
		else{ // making node non-leaf and distributing amongst the children
			root->leaf = 0;
			int childLevel = root->level + 1;
			for(int i = 0 ; i < modVal ; i++){
				root->child[i] = new hashNode();
				root->child[i]->level = childLevel;
				root->child[i]->leaf = 1;
			}
			int whichChild;
			for(int i = 0 ; i < root->itemSet.size() ; i++){
				whichChild = (root->itemSet[i]).trans[root->level] % modVal ;  	
				insertHashTree(k,root->child[whichChild],root->itemSet[i]);	
			}
			whichChild = Cfk.trans[root->level] % modVal ;
			insertHashTree(k,root->child[whichChild],Cfk);
			root->itemSet.clear();

		}
	}
	else{ // if not a leaf node
		int whichChild = Cfk.trans[root->level] % modVal;
		insertHashTree(k,root->child[whichChild],Cfk);
	}
}
void print(int k ,struct hashNode *root)
{ if(root==NULL)
   return ;
  if(root->leaf==1)
  {		
  		for(int i = 0 ; i < root->itemSet.size() ; i++){
  			for(int j = 0 ; j < root->itemSet[i].trans.size() ; j++){
  				cout << root->itemSet[i].trans[j] << " ";
  			}
  		} 
  }
  else
  {
  	for(int i = 0 ; i < modVal ; i++){
  		print(k,root->child[i]);
  	}
  }
}
void parseTrans(int arr[] , int k ,  struct hashNode * root){
	
	if(root==NULL)return ;
	if(root->leaf==1){
		for(int i = 0 ; i < root->itemSet.size() ; i++){
			bool check = false;
			for(int j = 0 ; j < k ; j++){
				if((root->itemSet[i].trans)[j]!=arr[j]){
					check = true;
					break;
				}
			}
			if(!check){
				root->itemSet[i].supCount++;
				break;
			}
		}
		return ;
		
	}
	else{ // if not a leaf node
		if(root->level >= k)return ;
		int whichChild = arr[root->level] % modVal;
		parseTrans(arr,k,root->child[whichChild]);
		
	}
}
void getFreq(int k ,struct hashNode *root){ 

	if(root==NULL)
	   return ;
	if(root->leaf==1){		
	  	for(int i = 0 ; i < root->itemSet.size() ; i++){
	  		float sup = (root->itemSet[i].supCount * 1.0) / (totalTrans * 1.0 );
	  		if(sup >= supportThreshold)
	  			freqItemSets[k].pb(&(root->itemSet[i])) ;  // push the pointer of the frequent item set of size k
	  	} 
	}
	else{
	  	for(int i = 0 ; i < modVal ; i++){
	  		getFreq(k,root->child[i]);
	  	}
	}
}
void subsetUtil(int arr[] , int data[] , int start , int end , int index , int k , struct hashNode *root){ // generate all k-sized subsets
	if(index == k){
		parseTrans(data,k,root);
		return;

	}
	for(int i = start ; i <= end && end - i + 1 >= k - index ; i++){
		data[index] = arr[i];
		subsetUtil(arr,data,i+1,end,index+1,k,root);
	}
}
void subset(int arr[] , int n , int k , struct hashNode *root){
	int data[k+1];
	subsetUtil(arr,data,0,n-1,0,k,root);
}
void runAllTrans(struct hashNode *root , int k ){
	for(int i = 0 ; i < totalTrans ; i++){
		int arr[inpTrans[i].size()+2];
		for(int j = 0 ; j < inpTrans[i].size() ; j++)arr[j] = inpTrans[i][j];
		subset(arr,inpTrans[i].size(),k,root);
	}
	getFreq(k,root);
}
void generateCandidate( int k , vector < nodeTrans >  f_old ){ //  generate item set of size k 

	if(k>=17)return ;
	vector < nodeTrans > Cfk;
	struct nodeTrans t;
	for(int i = 0 ; i < f_old.size() ; i++){
		for(int j = 0 ; j < 34 ; j++){
			bool flag = false; // flag to check if  f1 > fk-1
			int lastEl = f_old[i].trans.size() - 1;
			if(j > f_old[i].trans[lastEl]){
				if((j-f_old[i].trans[lastEl])==1){ // cannot have contradicting attributes both yes and no
					if(j&1)continue;
				}
				t.trans = f_old[i].trans;
				t.trans.pb(j);
				t.supCount = 0;
				Cfk.pb(t);
			}
		}
	}
	struct hashNode *root = new hashNode();
	root->level = 0;
	root->leaf = 1;
	struct nodeTrans v;
	for(int i = 0 ; i < Cfk.size() ; i++){
		v = Cfk[i];
		insertHashTree(k,root,v);
	}
	runAllTrans(root,k);
	vector < nodeTrans > f_new;
	for(int i = 0 ; i < freqItemSets[k].size() ; i++){
		f_new.pb(*freqItemSets[k][i]);
	}
	generateCandidate(k+1,f_new);
}
int getSupCount(int k , vector <int> f){

	for(int i = 0 ; i < freqItemSets[k].size() ; i++){
		bool check =  true;
		for(int j = 0 ; j < freqItemSets[k][i]->trans.size() ; j++){
			if(f[j]!=freqItemSets[k][i]->trans[j]){
				check = false;
				break;
			}
		}
		if(check){
			return freqItemSets[k][i]->supCount;
		}
	}
	return 0;
}
vector < pair < pair < vector <int> ,vector <int> > , pair <int, pair <int,float> > > >  rules; // second pair ff - left sup , right whole sup , rightmost rule confidence
void genAllRules(int k , nodeTrans *f){

	nodeTrans t = *f;
	int lim = 1<<k;
	int supWhole = getSupCount(t.trans.size(),t.trans);
	for(int mask = 0 ; mask < lim ; mask++){
		vector <int> l;
		vector <int> r;
		for(int i = 0 ; i < k ; i++){
			if(mask & (1<<i)){
				r.pb(t.trans[i]);
			}
			else{
				l.pb(t.trans[i]);
			}
		}
		if(l.size()==0 || r.size()==0)continue;
		int supCountOfL = getSupCount(l.size(),l);
		float conf = ( supWhole * 1.0 ) / ( supCountOfL * 1.0 );
		if(conf >= confiThreshold){
			rules.pb(mp(mp(l,r) , mp(supCountOfL,mp(supWhole,conf))));
		}
	}
}
void generateRules(){
	for(int i = 2 ; i <= 17 ; i++){
		for(int j = 0 ; j < freqItemSets[i].size() ; j++){
			genAllRules(i,freqItemSets[i][j]);
		}
	}
}
void printRules(){
	int ruleCnt = 1;
	for(int i = 0 ; i < rules.size() ; i++){
		bool filter = false;
		if(rules[i].ff.ss.size()==1 && (rules[i].ff.ss[0]==32 || rules[i].ff.ss[0]==33))
			filter = true;
		if(true){
			cout << ruleCnt++ << "\t";
			for(int j = 0 ; j < rules[i].ff.ff.size() ; j++){
				cout << attr[rules[i].ff.ff[j]] << "\t" ; 
			}
			cout << rules[i].ss.ff << "\t" << "==>" << "\t" ;
			for(int j = 0 ; j < rules[i].ff.ss.size() ; j++){
				cout << attr[rules[i].ff.ss[j]] << "\t";
			}
			cout << rules[i].ss.ss.ff << "\t" << "conf:(" << rules[i].ss.ss.ss << ")" << endl;
		}
	}
}
int main(){

	{	ifstream cin("fin.txt");
		for(int i = 0 ; i < totalTrans ; i++){
			cin >> line;
			stringstream  lineStream(line);
			int itemcount = 0;
			while(getline(lineStream,temp,',')){
				if(temp=="\'y\'"){
					trans[i][itemcount] = 1;
				}
				else if(temp=="\'n\'"){
					trans[i][itemcount + 1] = 1;
				}
				else if(temp=="\'democrat\'"){
					trans[i][itemcount] = 1;
				}
				else if(temp=="\'republican\'")trans[i][itemcount+1] = 1;
				itemcount+=2;
			}
		}
	}
	for(int i = 0 ; i < 435 ; i++){ // calculating support of 1-itemsets
		for(int j = 0 ; j < 34 ; j++){
			if(trans[i][j]){
				sup1ItemSet[j]++;
				inpTrans[i].pb(j);
			}
		}
	}
	vector < nodeTrans > f;
	struct nodeTrans t[35];
	for(int i = 0 ; i < 34 ; i++){
		float sup  = (sup1ItemSet[i] * 1.0) / (totalTrans * 1.0 );
		if(sup >= supportThreshold){
			t[i].trans.pb(i);
			t[i].supCount = sup1ItemSet[i];
			freqItemSets[1].pb(&t[i]);
			f.pb(t[i]);
		}
	}
	generateCandidate(2,f);
	freopen("output.txt","w",stdout);
	generateRules();
	printRules();
	fclose(stdout);
	return 0;
}