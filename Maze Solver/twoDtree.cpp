
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"
#include "cs221util/HSLAPixel.h"

/* node constructor given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, HSLAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),LT(NULL),RB(NULL)
	{}

/* destructor given */
twoDtree::~twoDtree(){
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


/* operator= given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

/* twoDtree constructor */
twoDtree::twoDtree(PNG & imIn){ 
	stats stat = stats(imIn);
	height = imIn.height();
	width = imIn.width();
	pair<int,int> ul = make_pair(0,0);
	pair<int,int> lr = make_pair(width-1,height-1);
	root = buildTree(stat,ul,lr,true);

}

/* buildTree helper for twoDtree constructor */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {

double areaTop;
double areaBottom;
double entropyTop;
double entropyBottom;
double entropySum;
double minEntropysf = 999999999;
// Constructor a new node
Node *curr = new Node (ul, lr, s.getAvg(ul,lr));
int areaTotal = s.rectArea(ul,lr);

if(lr.first == ul.first&& lr.second == ul.second){
	return curr;
}

	
	if(lr.first==ul.first){
		vert = false;
	}
	if(lr.second==ul.second){
vert = true;
}
	if (vert) {
		pair<int,int> lowerTop;
		pair<int, int> rightBottom;
		// int a = 0;
		for (int i = ul.first; i < lr.first; i++) {
			areaTop= s.rectArea(ul, make_pair(i, lr.second));
			areaBottom= s.rectArea(make_pair(i+1, ul.second), lr);
			entropyTop = (areaTop * s.entropy(ul, make_pair(i, lr.second))) / areaTotal;
			entropyBottom = (areaBottom * s.entropy(make_pair(i+1, ul.second), lr)) / areaTotal;
			entropySum = entropyTop + entropyBottom;
			if (entropySum <= minEntropysf) {
				minEntropysf = entropySum;
				lowerTop = make_pair(i, lr.second);
				rightBottom = make_pair(i+1, ul.second);
			}
		}
		if (ul.second == lowerTop.second) {
			curr -> LT = buildTree(s,ul,lowerTop, vert);
		}else{
			curr->LT = buildTree(s,ul,lowerTop, !vert);}


		if (rightBottom.second == lr.second) {
			curr->RB = buildTree(s, rightBottom, lr, vert);
		}
		else{
			curr->RB = buildTree(s, rightBottom, lr, !vert);
		}
	}
	else{
		pair<int,int> lowerTop;
		pair <int, int> rightBottom;
		for (int j = ul.second ; j < lr.second; j++) {
			double areaTop= s.rectArea(ul, make_pair(lr.first, j));
			double areaBottom= s.rectArea(make_pair(ul.first, j+1), lr);
			entropyTop = (areaTop * s.entropy(ul, make_pair(lr.first, j))) / areaTotal;
			entropyBottom = (areaBottom * s.entropy(make_pair(ul.first, j+1), lr)) / areaTotal;
			entropySum = entropyTop + entropyBottom;
			if (entropySum <= minEntropysf) {
				minEntropysf = entropySum;
				rightBottom = make_pair(ul.first , j+1);
				lowerTop = make_pair(lr.first, j);
			}
		}
		if (ul.first == lowerTop.first) {
			curr -> LT = buildTree(s,ul,lowerTop, false);
		}else{
			curr->LT = buildTree(s,ul,lowerTop, true);}
		if (rightBottom.first == lr.first) {
			curr->RB = buildTree(s, rightBottom, lr, false);
		}else{
			curr->RB = buildTree(s, rightBottom, lr, true);
		}
		
		}
/* your code here */
return curr;


}

PNG twoDtree::render(){
PNG picture = PNG(width, height);
 renderPicture(root, picture);
 return picture;
}
void twoDtree::renderPicture(Node* nd, PNG& pic){
	if (nd == NULL){
		return;
	}
	pair<int,int> UL = nd->upLeft;
	pair<int,int> LR = nd->lowRight;
	if(nd->LT == NULL && nd->RB == NULL){
		for(int i = UL.second; i <= LR.second; i++){
			for(int j = UL.first; j <= LR.first; j++){
				HSLAPixel* currPic = pic.getPixel(j,i);
				*currPic = nd->avg;
			}
		}
		return;
	}else{
		renderPicture(nd->LT, pic);
		renderPicture(nd->RB, pic);
	}
};
/* prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of 
 * the average pixel value contained in the root
 * of the subtree
 */
void twoDtree::prune(double tol){
	pruneHelper(root,tol);
}

void twoDtree::pruneHelper(Node* &nd, double tol){
	if(nd == NULL){
		return;
	}
	if(nd->RB == NULL && nd->LT == NULL){
		return;
	}
	if(ifPrune(nd->avg, nd,tol)){
		clearHelper(nd->LT);
		nd->LT = NULL;
		clearHelper(nd->RB);
		nd->RB = NULL;
	}else{
		pruneHelper(nd->LT,tol);
		pruneHelper(nd->RB,tol);
	}
}

bool twoDtree::ifPrune(HSLAPixel &avg, Node* node , double tol){
	if(node==NULL){
		return true;
	}
	if(node->LT == NULL && node->RB == NULL){
		return node->avg.dist(avg) <= tol;
	}else{
		return ifPrune(avg, node->LT, tol) && ifPrune(avg,node->RB,tol);
	}
}




/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
	clearHelper(root);
	root = NULL;
}

void twoDtree::clearHelper(Node* node){
	if(node == NULL) 
		return;
	clearHelper(node->LT);
	clearHelper(node->RB);
	delete node;
	node = NULL;
}


/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree orig){

height = orig.height;
width = orig.width;
root = copyHelper(orig.root);


}
twoDtree::Node* twoDtree::copyHelper(Node* node){
	if(node == NULL){
		return NULL;
	}
	Node *rNode = new Node(node->upLeft,node->lowRight, node->avg);
	rNode->LT = copyHelper(node->LT);
	rNode->RB = copyHelper(node->RB);
	return rNode;
}



