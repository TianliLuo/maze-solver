
#include "stats.h"
#define _USE_MATH_DEFINES

stats::stats(PNG & im){
  sumHueX.resize(im.width(), (vector<double>) im.height());
  sumHueY.resize(im.width(), (vector<double>) im.height());
  sumSat.resize(im.width(), (vector<double>) im.height());
  sumLum.resize(im.width(), (vector<double>) im.height());
  hist.resize(im.width(), vector< vector<int> > (im.height(), vector<int>(36, 0)));
  for(int i = 0; i < (int)im.height(); i++){
    for(int j = 0; j < (int)im.width(); j++){
       HSLAPixel* node = im.getPixel(j,i);
      if(i == 0 && j == 0){
        sumSat[j][i] = node->s;
        sumLum[j][i] = node->l;
        sumHueX[j][i] = cos((PI * node->h)/180);
        sumHueY[j][i] = sin((PI * node->h)/180);
      }else if(j == 0){
        sumSat[j][i] = sumSat[j][i-1] + node->s;
        sumLum[j][i] = sumLum[j][i-1] + node->l;
        sumHueX[j][i] = sumHueX[j][i-1] + cos((PI * node->h)/180);
        sumHueY[j][i] = sumHueY[j][i-1] + sin((PI * node->h)/180);
        newhist(j,i);
      }else if(i == 0){
        sumSat[j][i] = sumSat[j-1][i] + node->s;
        sumLum[j][i] = sumLum[j-1][i] + node->l;
        sumHueX[j][i] = sumHueX[j-1][i] + cos((PI * node->h)/180);
        sumHueY[j][i] = sumHueY[j-1][i] + sin((PI * node->h)/180);
        newhist(j,i);
      }else{
        sumSat[j][i] = sumSat[j-1][i] + sumSat[j][i-1] - sumSat[j-1][i-1] + node->s;
        sumLum[j][i] = sumLum[j-1][i] + sumLum[j][i-1] - sumLum[j-1][i-1] + node->l;
        sumHueX[j][i] = sumHueX[j-1][i] + sumHueX[j][i-1] - sumHueX[j-1][i-1] + cos((PI * node->h)/180);
        sumHueY[j][i] = sumHueY[j-1][i] + sumHueY[j][i-1] - sumHueY[j-1][i-1] + sin((PI * node->h)/180);
        newhist(j,i);
      }
      if(node->h/10 == 36){
          hist[j][i][0]++;
        }else{
          hist[j][i][node->h/10]++;
        }
      }
  }
}

void stats::newhist(int x, int y){
    for (int i = 0; i < 36; i++){
        if (x==0){
            hist[x][y][i] = hist[x][y-1][i];
        } else if (y==0){
             hist[x][y][i] = hist[x-1][y][i];
        } else {
            hist[x][y][i] = hist[x-1][y][i] + hist[x][y-1][i] - hist[x-1][y-1][i];
        }
    }
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){
    return (1+lr.second-ul.second) * (1+lr.first-ul.first);
}
HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
  HSLAPixel* ret = new HSLAPixel;  
  double alpha = 1.0;
  double sumX = sumHueX[lr.first][lr.second];
  double sumY = sumHueY[lr.first][lr.second];
  double sumS = sumSat[lr.first][lr.second];
  double sumL = sumLum[lr.first][lr.second];
  long area = rectArea(ul, lr);
	if(ul.first - 1 >= 0){
    sumX -= sumHueX[ul.first-1][lr.second];
    sumY -= sumHueY[ul.first-1][lr.second];
    sumS -= sumSat[ul.first-1][lr.second];
    sumL -= sumLum[ul.first-1][lr.second];
  }
	if(ul.first - 1 >= 0 && ul.second - 1 >= 0){
    sumX += sumHueX[ul.first-1][ul.second-1];
    sumY += sumHueY[ul.first-1][ul.second-1];
    sumS += sumSat[ul.first-1][ul.second-1];
    sumL += sumLum[ul.first-1][ul.second-1];
  }
  if(ul.second - 1 >= 0){
    sumX -= sumHueX[lr.first][ul.second-1];
    sumY -= sumHueY[lr.first][ul.second-1];
    sumS -= sumSat[lr.first][ul.second-1];
    sumL -= sumLum[lr.first][ul.second-1];
  }
  double hue = (atan2(sumY/area, sumX/area) * 180/PI);
	if (hue < 0) {
    hue += 360;
  }
	ret->h = hue;
	ret->s = sumS / area;
	ret->l = sumL / area;
	ret->a = alpha;
  HSLAPixel re = *ret;
  delete(ret);
  return re;
}
double stats::entropy(pair<int,int> ul, pair<int,int> lr){

    vector<int> distn(hist[lr.first][lr.second]);;

    /* using private member hist, assemble the distribution over the
    *  given rectangle defined by points ul, and lr into variable distn.
    *  You will use distn to compute the entropy over the rectangle.
    *  if any bin in the distn has frequency 0, then do not add that 
    *  term to the entropy total. see .h file for more details.
    */

    /* my code includes the following lines:
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    */
  vector<int> vector;
  if(ul.first - 1 >= 0){
    vector = hist[ul.first-1][lr.second];
    for(int i = 0; i < 36; i++){
      distn[i] -= vector[i];
    }
  }
  if(ul.second - 1 >= 0){
    vector = hist[lr.first][ul.second-1];
    for(int i = 0; i < 36; i++){
     distn[i] -= vector[i];
    }
  }
  if(ul.first - 1 >= 0 && ul.second - 1 >= 0){
    vector = hist[ul.first-1][ul.second-1];
    for(int i = 0; i < 36; i++){
      distn[i] += vector[i];
    }
  }
    double entropy = 0;
    int area = rectArea(ul, lr);
    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 )
            entropy += ((double) distn[i]/(double) area)* log2((double) distn[i]/(double) area);
    }
    return (-1)*entropy;
}