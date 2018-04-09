#include <bits/stdc++.h>
#define ull unsigned long long
#define ll long long
#define ff first
#define ss second
#define pb push_back
#define ins insert
#define mp make_pair
#define PI 3.141592653589793
#define all(x) x.begin(),x.end()
#define iOS ios::sync_with_stdio(false)
using namespace std;
vector<int> transactions;
typedef struct node{
    int item;
    int freq;
    vector<node*> next;
}node;
map<int,int> supcnt;
node* root;
int thresh_fq=200;
int no_items=18;
float confiThreshold = 0.5;
vector <int> freqItemSets[20];
vector <int> tp;
node* createNode(int item)
{
    node* nd=new node();
    nd->item=item;
    nd->freq=0;
    nd->next.clear();
    for(int i=0;i<no_items;i++)
        nd->next.pb(NULL);
    return nd;
}
int getFirstItem(int item)
{
    for(int i=0;i<no_items;i++)
        if((1<<i)&item)
            return i;
    return -1;
}
void addItem(node* cur_node,int item)
{
    //cout<<cur_node->item<<endl;
    if(cur_node->item==-1)
    {
        int id=getFirstItem(item);
        if(cur_node->next[id]==NULL)
            cur_node->next[id]=createNode(id);
        addItem(cur_node->next[id],item);
    }
    else
    {
        int id=getFirstItem(item);
        cur_node->freq++;
        item=item^(1<<id);
        if(item==0) return;
        id=getFirstItem(item);
        if(cur_node->next[id]==NULL)
            cur_node->next[id]=createNode(id);
        addItem(cur_node->next[id],item);
    }
}
pair<node*,int> getTempTree(node* cur_node,int lowid)
{
    node* nd=createNode(cur_node->item);
    if(nd->item>lowid) return mp(nd,0);
    if(nd->item==lowid)
    {
        nd->freq=cur_node->freq;
        return mp(nd,1);
    }
    int red=0;
    for(int i=0;i<no_items;i++)
        if(cur_node->next[i]!=NULL)
        {
            pair<node*,int> temp=getTempTree(cur_node->next[i],lowid);
            if(temp.ss==0) continue;
            red=1;
            node* tmp=temp.ff;
            if(nd->item!=-1)
                nd->freq+=tmp->freq;
            nd->next[i]=tmp;
        }
    return mp(nd,red);
}
void getCounts(node* cur_node,vector<int>& v)
{
    if(cur_node->item!=-1)
        v[cur_node->item]+=cur_node->freq;
    for(int i=0;i<no_items;i++)
        if(cur_node->next[i]!=NULL)
            getCounts(cur_node->next[i],v);
}
void printItemset(int msk,int fq)
{
    int cnt = __builtin_popcount(msk);
    freqItemSets[cnt].pb(msk);
    supcnt[msk]=fq;
    for(int i=0;i<no_items;i++)
        if((1<<i)&msk);//printf("%d ",i);
    //printf("%d \n",supcnt[msk]);
}
void FPGrowth(node* root,int msk,int lowid)
{
    node* tr=getTempTree(root,lowid).ff;
    if(lowid==1e8) tr=root;
    vector<int> cnts(no_items,0);
    getCounts(tr,cnts);
    int id=getFirstItem(msk);
    if(id==0) return;
    if(id==-1) id=no_items;
    for(int i=id-1;i>=0;i--)
        if(cnts[i]>=thresh_fq && ((1<<i)&msk)==0)
        {
            int tmsk=msk|(1<<i);
            printItemset(tmsk,cnts[i]);
            FPGrowth(tr,tmsk,i);
        }
}
void traverse(node* nd)
{
    printf("%d %d\n",nd->item,nd->freq);
    for(int i=0;i<no_items;i++)
        if(nd->next[i]!=NULL)
            traverse(nd->next[i]);
}
int getithpos(int j,int msk)
{
    int cnt=0;
    for(int i=0;i<no_items;i++)
        if(msk&(1<<i))
        {
            if(cnt==j) return i;
            cnt++;
        }
    return -1;
}
vector < pair < pair < vector <int> ,vector <int> > , pair <int, pair <int,float> > > >  rules; // second pair ff - left sup , right whole sup , rightmost rule confidence
void genAllRules(int k ,int msk){
    int lim = 1<<k;
    int supWhole = supcnt[msk];
    for(int mask = 0 ; mask < lim ; mask++){
        vector <int> l;
        vector <int> r;
        for(int i = 0 ; i < k ; i++){
            if(mask & (1<<i)){
                r.pb(i);
            }
            else{
                l.pb(i);
            }
        }
        if(l.size()==0 || r.size()==0)continue;
        int supCountOfL=0;
        for(int i=0;i<l.size();i++){
            supCountOfL+=(1<<getithpos(l[i],msk));
        }
        supCountOfL=supcnt[supCountOfL];
        float conf = ( supWhole * 1.0 ) / ( supCountOfL * 1.0 );
        if(conf >= confiThreshold){
            for(int i = 0 ; i < l.size() ; i++){
                printf("%d ",getithpos(l[i],msk));
            }
            printf(" ==> ");
            for(int i = 0 ; i < r.size() ; i++){
                printf("%d ",getithpos(r[i],msk));
            }
            printf(" Conf = %lf\n",conf);
        }
    }
}

void generateRules(){
    for(int i = 2 ; i <= no_items ; i++){
        for(int j = 0 ; j < freqItemSets[i].size() ; j++){
            genAllRules(i,freqItemSets[i][j]);
        }
    }
}
void bruteForceChecker()
{
    int tot=1<<no_items;
    for(int i=0;i<tot;i++)
    {
        int cnt=0;
        for(int j=0;j<transactions.size();j++)
        {
            if((i&transactions[j])==i) cnt++;
        }
        if(cnt>=thresh_fq) printItemset(i,cnt);
    }
}
int n,x,y;
int main()
{
    freopen("data.txt","r",stdin);
    freopen("output.txt","w",stdout);
    scanf("%d",&n);
    scanf("%d",&x);
    for(int i=0;i<n;i++)
    {
        int msk=0;
        for(int j=0;j<x;j++)
        {
            scanf("%d",&y);
            y--;
            msk+=(1<<y);
        }
        transactions.pb(msk);
    }
    root=createNode(-1);
    for(int i=0;i<transactions.size();i++){
        addItem(root,transactions[i]);
    }
    FPGrowth(root,0,1e8);
    generateRules(); 
    return 0;
}
