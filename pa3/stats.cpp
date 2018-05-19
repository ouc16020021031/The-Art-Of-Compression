#include "stats.h"

using namespace std;
using namespace cs221util;

stats::stats(PNG & im) {
	unsigned int width = im.width();
	unsigned int height = im.height();

	sumRed.resize(width, vector<long>(height, 0));
	sumGreen.resize(width, vector<long>(height, 0));
	sumBlue.resize(width, vector<long>(height, 0));
	sumsqRed.resize(width, vector<long>(height, 0));
	sumsqGreen.resize(width, vector<long>(height, 0));
	sumsqBlue.resize(width, vector<long>(height, 0));

	RGBAPixel* pxl = im.getPixel(0, 0);
	
	sumRed[0][0] = pxl->r;
	sumGreen[0][0] = pxl->g;
	sumBlue[0][0] = pxl->b;
	
	sumsqRed[0][0] = (pxl->r)*(pxl->r);
	sumsqGreen[0][0] = (pxl->g)*(pxl->g);
	sumsqBlue[0][0] = (pxl->b)*(pxl->b);

	for (unsigned int j = 1; j<height; j++) {
		RGBAPixel* pxl = im.getPixel(0, j);
		
		sumRed[0][j] = sumRed[0][j-1] + pxl->r;
		sumGreen[0][j] = sumGreen[0][j - 1] + pxl->g;
		sumBlue[0][j] = sumBlue[0][j - 1] + pxl->b;
		
		sumsqRed[0][j] = sumsqRed[0][j - 1] + (pxl->r)*(pxl->r);
		sumsqGreen[0][j] = sumsqGreen[0][j - 1] + (pxl->g)*(pxl->g);
		sumsqBlue[0][j] = sumsqBlue[0][j - 1] + (pxl->b)*(pxl->b);
	}

	for (unsigned int i = 1; i<width; i++) {
		RGBAPixel* pxl = im.getPixel(i, 0);
	
		sumRed[i][0] = sumRed[i - 1][0] + pxl->r;
		sumGreen[i][0] = sumGreen[i - 1][0] + pxl->g;
		sumBlue[i][0] = sumBlue[i - 1][0] + pxl->b;
		
		sumsqRed[i][0] = sumsqRed[i - 1][0] + (pxl->r)*(pxl->r);
		sumsqGreen[i][0] = sumsqGreen[i - 1][0] + (pxl->g)*(pxl->g);
		sumsqBlue[i][0] = sumsqBlue[i - 1][0] + (pxl->b)*(pxl->b);
	}

	for (unsigned int i = 1; i<width; i++) {//sumRed[i - 1][j] + sumRed[i][j - 1]有重合部分，故需减去sumRed[i - 1][j - 1]
		for (unsigned int j = 1; j<height; j++) {
			RGBAPixel* pxl = im.getPixel(i, j);

			sumRed[i][j] = sumRed[i - 1][j] + sumRed[i][j - 1] - sumRed[i - 1][j - 1] + pxl->r;
			sumGreen[i][j] = sumGreen[i - 1][j] + sumGreen[i][j - 1] - sumGreen[i - 1][j - 1] + pxl->g;
			sumBlue[i][j] = sumBlue[i - 1][j] + sumBlue[i][j - 1] - sumBlue[i - 1][j - 1] + pxl->b;

			sumsqRed[i][j] = sumsqRed[i - 1][j] + sumsqRed[i][j - 1] - sumsqRed[i - 1][j - 1] + (pxl->r)*(pxl->r);
			sumsqGreen[i][j] = sumsqGreen[i - 1][j] + sumsqGreen[i][j - 1] - sumsqGreen[i - 1][j - 1] + (pxl->g)*(pxl->g);
			sumsqBlue[i][j] = sumsqBlue[i - 1][j] + sumsqBlue[i][j - 1] - sumsqBlue[i - 1][j - 1] + (pxl->b)*(pxl->b);

		}
	}
}

long stats::getScore(pair<int, int> ul, pair<int, int> lr) {
	long double area = rectArea(ul, lr);
	long double diff;

	long double sumR = getSum('r', ul, lr);
	long double sumG = getSum('g', ul, lr);
	long double sumB = getSum('b', ul, lr);
	
	long double sumSqR = getSumSq('r', ul, lr);
	long double sumSqG = getSumSq('g', ul, lr);
	long double sumSqB = getSumSq('b', ul, lr);

	long double diffR = sumSqR - ((sumR*sumR) / (long double)area);
	long double diffG = sumSqG - ((sumG*sumG) / (long double)area);
	long double diffB = sumSqB - ((sumB*sumB) / (long double)area);

	diff = diffR + diffG + diffB;

	return diff;
};

RGBAPixel stats::getAvg(pair<int, int> ul, pair<int, int> lr) {
	long sumR = getSum('r', ul, lr);
	long sumG = getSum('g', ul, lr);
	long sumB = getSum('b', ul, lr);
	long total = rectArea(ul, lr);

	long double redAvg = (double)sumR / (double)total;
	long double greenAvg = (double)sumG / (double)total;
	long double blueAvg = (double)sumB / (double)total;

	return RGBAPixel(redAvg, greenAvg, blueAvg); 
}

long stats::rectArea(pair<int, int> ul, pair<int, int> lr) {
	return (lr.first - ul.first + 1)*(lr.second - ul.second + 1);
}

long stats::getSum(char channel, pair<int, int> ul, pair<int, int> lr) {
	long sum = 0;
	switch (channel) {
	case 'r':
		if (ul.first != 0 && ul.second != 0)
			sum = sumRed[lr.first][lr.second] + sumRed[ul.first - 1][ul.second - 1] - sumRed[lr.first][ul.second - 1] - sumRed[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumRed[lr.first][lr.second] - sumRed[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumRed[lr.first][lr.second] - sumRed[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumRed[lr.first][lr.second];
		break;
	case 'g':
		if (ul.first != 0 && ul.second != 0)
			sum = sumGreen[lr.first][lr.second] + sumGreen[ul.first - 1][ul.second - 1] - sumGreen[lr.first][ul.second - 1] - sumGreen[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumGreen[lr.first][lr.second] - sumGreen[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumGreen[lr.first][lr.second] - sumGreen[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumGreen[lr.first][lr.second];
		break;
	case 'b':
		if (ul.first != 0 && ul.second != 0)
			sum = sumBlue[lr.first][lr.second] + sumBlue[ul.first - 1][ul.second - 1] - sumBlue[lr.first][ul.second - 1] - sumBlue[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumBlue[lr.first][lr.second] - sumBlue[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumBlue[lr.first][lr.second] - sumBlue[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumBlue[lr.first][lr.second];
		break;
	}
	return sum;
}
long stats::getSumSq(char channel, pair<int, int> ul, pair<int, int> lr) {
	long sum = 0;
	switch (channel) {
	case 'r':
		if (ul.first != 0 && ul.second != 0)
			sum = sumsqRed[lr.first][lr.second] + sumsqRed[ul.first - 1][ul.second - 1] - sumsqRed[lr.first][ul.second - 1] - sumsqRed[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumsqRed[lr.first][lr.second] - sumsqRed[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumsqRed[lr.first][lr.second] - sumsqRed[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumsqRed[lr.first][lr.second];
		break;
	case 'g':
		if (ul.first != 0 && ul.second != 0)
			sum = sumsqGreen[lr.first][lr.second] + sumsqGreen[ul.first - 1][ul.second - 1] - sumsqGreen[lr.first][ul.second - 1] - sumsqGreen[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumsqGreen[lr.first][lr.second] - sumsqGreen[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumsqGreen[lr.first][lr.second] - sumsqGreen[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumsqGreen[lr.first][lr.second];
		break;
	case 'b':
		if (ul.first != 0 && ul.second != 0)
			sum = sumsqBlue[lr.first][lr.second] + sumsqBlue[ul.first - 1][ul.second - 1] - sumsqBlue[lr.first][ul.second - 1] - sumsqBlue[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second != 0)
			sum = sumsqBlue[lr.first][lr.second] - sumsqBlue[lr.first][ul.second - 1];
		else if (ul.first != 0 && ul.second == 0)
			sum = sumsqBlue[lr.first][lr.second] - sumsqBlue[ul.first - 1][lr.second];
		else if (ul.first == 0 && ul.second == 0)
			sum = sumsqBlue[lr.first][lr.second];
		break;
	}
	return sum;
}