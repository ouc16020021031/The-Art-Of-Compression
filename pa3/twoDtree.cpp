/**
*
* twoDtree (pa3)
* slight modification of a Kd tree of dimension 2.
* twoDtree.cpp
* This file will be used for grading.
*
*/

#include "twoDtree.h"

using namespace std;
using namespace cs221util;

/* given */
twoDtree::Node::Node(pair<int, int> ul, pair<int, int> lr, RGBAPixel a)
	:upLeft(ul), lowRight(lr), avg(a), left(NULL), right(NULL)
{}

/* given */
twoDtree::~twoDtree() {
	clear();
}

/* given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}

/* given */
twoDtree & twoDtree::operator=(const twoDtree & rhs) {
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

twoDtree::twoDtree(PNG & imIn) {
	stats s = stats(imIn);
	width = imIn.width();
	height = imIn.height();
	pair<int, int> ul = make_pair(0, 0);
	pair<int, int> lr = make_pair(width - 1, height - 1);
	root = buildTree(s, ul, lr);
}

twoDtree::Node * twoDtree::buildTree(stats & s, pair<int, int> ul, pair<int, int> lr) {
	RGBAPixel a = s.getAvg(ul, lr);
	Node* node = new Node(ul, lr, a);

	int x1 = ul.first;
	int x2 = lr.first;
	int y1 = ul.second;
	int y2 = lr.second;

	if (x1 == x2 && y1 == y2)return node;

	long smallest;
	pair<int, int> smsub1lr;
	pair<int, int> smsub2ul;

	for (int i = x1; i < x2; i++) {
		pair<int, int> sub1_lr = make_pair(i, y2);
		pair<int, int> sub2_ul = make_pair(i + 1, y1);

		long double sub1 = s.getScore(ul, sub1_lr);
		long double sub2 = s.getScore(sub2_ul, lr);

		if (i == x1 || sub1 + sub2 < smallest) {
			smallest = sub1 + sub2;
			smsub1lr = sub1_lr;
			smsub2ul = sub2_ul;
		}
	}

	for (int i = y1; i < y2; i++) {
		pair<int, int> sub1_lr = make_pair(x2, i);
		pair<int, int> sub2_ul = make_pair(x1, i + 1);

		long double sub1 = s.getScore(ul, sub1_lr);
		long double sub2 = s.getScore(sub2_ul, lr);

		if (i == y1 && x1 ==x2 || sub1 + sub2 < smallest) {
			smallest = sub1 + sub2;
			smsub1lr = sub1_lr;
			smsub2ul = sub2_ul;
		}
	}

	node->left = buildTree(s, ul, smsub1lr);
	node->right = buildTree(s, smsub2ul, lr);

	return node;
}

PNG twoDtree::render() {
	PNG rendPNG = PNG(width, height);
	render(rendPNG, root);

	return rendPNG;
}

void twoDtree::render(PNG & rendPNG, Node* subRoot) {//渲染修剪过的子树，将像素替换为计算所得的平均值
	if (subRoot == NULL)return;

	int x1 = subRoot->upLeft.first;
	int y1 = subRoot->upLeft.second;
	int x2 = subRoot->lowRight.first;
	int y2 = subRoot->lowRight.second;

	for (int i = x1; i <= x2; i++) {
		for (int j = y1; j <= y2; j++) {
			*rendPNG.getPixel(i, j) = subRoot->avg;
		}
	}
	render(rendPNG, subRoot->left);
	render(rendPNG, subRoot->right);
}

void twoDtree::prune(double pct, int tol) {
	prune(pct, tol, root);
}

void twoDtree::prune(double pct, int tol, Node* subRoot) {
	if (subRoot == NULL || subRoot->left == NULL && subRoot->right == NULL)return;

	long double tolleaf_number = getTolleafNumber(tol, subRoot, subRoot->avg);
	long double leaf_number = getLeafNumber(subRoot);

	double ratio = (double)tolleaf_number / (double)leaf_number;

	if (ratio >= pct) {//如果比值大于pct则删除子树
		clear(subRoot->left);
		subRoot->left = NULL;
		clear(subRoot->right);
		subRoot->right = NULL;
	}
	else {//否则继续判断下一级子树
		prune(pct, tol, subRoot->left);
		prune(pct, tol, subRoot->right);
	}
}

long twoDtree::getTolleafNumber(int tol, Node* subRoot, RGBAPixel subAvg) {
	int r = subRoot->avg.r;
	int g = subRoot->avg.g;
	int b = subRoot->avg.b;

	if (subRoot == NULL) return 0;

	if (subRoot->left == NULL && subRoot->right == NULL) {
		long long diff = (r - subAvg.r)*(r - subAvg.r) + (g - subAvg.g)*(g - subAvg.g) + (b - subAvg.b)*(b - subAvg.b);

		if (diff <= tol) return 1;
		else return 0;
	}

	return getTolleafNumber(tol, subRoot->left, subAvg) + getTolleafNumber(tol, subRoot->right, subAvg);
}

long twoDtree::getLeafNumber(Node* subRoot) {
	if (subRoot == NULL)return 0;

	if (subRoot->left == NULL && subRoot->right == NULL)return 1;

	return getLeafNumber(subRoot->left) + getLeafNumber(subRoot->right);
}

void twoDtree::clear() {
	clear(root);
	root = NULL;
	height = 0;
	width = 0;
}

void twoDtree::clear(Node* subRoot)
{

	if (subRoot == NULL) return;

	clear(subRoot->left);
	clear(subRoot->right);

	subRoot = NULL;
	delete subRoot;
}

void twoDtree::copy(const twoDtree & orig) {
	root = copyNode(orig.root);
	height = orig.height;
	width = orig.width;
}

twoDtree::Node * twoDtree::copyNode(Node* subRoot)
{
	if (subRoot == NULL)return NULL;

	Node* newNode = new Node(subRoot->upLeft, subRoot->lowRight, subRoot->avg);
	newNode->left = copyNode(subRoot->left);
	newNode->right = copyNode(subRoot->right);

	return newNode;
}