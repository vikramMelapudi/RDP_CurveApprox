#include <stdio.h>
#include <stdlib.h>
#include <math.h>

class Point{
  public:
  double m_x, m_y;
  
  Point() {
    Point(0,0);
  }
  Point(double x, double y) {
    m_x = x;
    m_y = y;
  }
  
  double norm() {
    return sqrt(m_x*m_x + m_y*m_y);
  }
  
  void scale(double sc) {
    m_x *= sc;
    m_y *= sc;
  }
};

typedef Point Vector;


Vector getVec(Point p1, Point p2, bool norm=true) {
    Vector vec(p2.m_x - p1.m_x, p2.m_y - p1.m_y);
    if (norm) {
        double len = vec.norm();
        vec.scale(1.0/len);
    }
    return vec;
}

double ptLineDist(Point pt, Point lp1, Point lp2) {
    Vector lvec = getVec(lp1, lp2);
    Vector pvec = getVec(lp1, pt, false);
    // line normal
    double nx = -lvec.m_y;
    double ny = lvec.m_x;
    // proj (px,py) onto line normal
    double nProj = abs(nx*pvec.m_x + ny*pvec.m_y);
    
    return nProj;
}
    
void unitTestPtLineDist() {
    Point pt1(0, 0);
    Point pt2(1, 0);
    Point pt3(0, 0.5);
    Point pt4(0.5, 0);
    Point pt5(0, 1);
    Point pt6(1, 1);
    Point pt7(0.5, 0.5);
    
    double d;
    d = ptLineDist(pt3, pt1, pt2);
    printf("%5.2f\n", d);
    
    d = ptLineDist(pt7, pt1, pt6);
    printf("%5.2f\n", d);
    
    d = ptLineDist(pt5, pt1, pt6);
    printf("%5.2f\n", d);
}
class Line {
  public:
  bool m_isLine;
  Point m_p1, m_p2;
 
  Line() {
    m_isLine = false;
  }
  Line(Point &p1, Point &p2) {
    m_isLine = true;
    m_p1 = Point(p1.m_x, p1.m_y);
    m_p2 = Point(p2.m_x, p2.m_y);
  }
  void show(FILE *fp=stdout) {
    fprintf(fp, " Line: %7.2f, %7.2f  -- %7.2f, %7.2f\n",m_p1.m_x, m_p1.m_y, m_p2.m_x, m_p2.m_y);
  }
};


#include <vector>
#include <algorithm>

template <typename T, typename A>
int arg_max(std::vector<T, A> const& vec) {
  return static_cast<int>(std::distance(vec.begin(), std::max_element(vec.begin(), vec.end())));
}

std::vector<Line> RDPRecur(std::vector<Point> &ptsList, std::vector<Line> &linesList, double thresh=0.25, int n1=0, int n2=-1, int nRecur=0, bool dbg=false) {
  nRecur+=1;
  
  // if (nRecur>10) return linesList;
  if (n2<0) n2 = ptsList.size()-1;
  
  Line line = Line(ptsList[n1], ptsList[n2]);
  double dmax = 0;
  int imax = 0;
  if ((n2-n1)>1) {
    std::vector<double> ds;
    for(int n=n1+1; n<n2; n++) {
      ds.push_back(ptLineDist(ptsList[n], line.m_p1, line.m_p2));
    }
    imax = arg_max(ds);
    dmax = ds[imax];
    if (dbg) printf("imax=%d, dmax=%5.2f\n",imax,dmax);
  }
        
  if (dmax<thresh) {
    linesList.push_back(line);
    if (n2 < (ptsList.size()-1)) {
      if (dbg) printf("(%d, %d) --> (%d, %d) %d \n", n1,n2,n2,-1, linesList.size());
      return RDPRecur(ptsList, linesList, thresh, n2, -1, nRecur, dbg);
    } else {
      return linesList;
    }
  } else {
    int nmax = imax + n1 + 1;
    if (dbg) printf("(%d, %d) --> (%d, %d), %d, %5.2f \n", n1,n2,n1,nmax,linesList.size(),dmax);
    return RDPRecur(ptsList, linesList, thresh, n1, nmax, nRecur, dbg);
  }
}

int main(int argc, char *argv[]) {
  printf("usage: %s [input csv file] [threshold:float] [debug:0/1]\n\n", argv[0]);
  unitTestPtLineDist();
  char inpFile[40], outFile[40];
  sprintf(inpFile, "tmp.csv");
  sprintf(outFile, "tmp.out");
  
  std::vector<Point> ptsList;
  if (argc>1) {
    sprintf(inpFile, "%s", argv[1]);
    sprintf(outFile, "%s_out", argv[1]);
    
    printf("Reading from file: [%s]\n", inpFile);
    FILE *fp = fopen(argv[1], "r");
    while(!feof(fp)) {
      double a, b;
      fscanf(fp, "%lf,%lf\n",&a,&b);
      ptsList.push_back(Point(a,b));
    }
    fclose(fp);
  } else {  
    for(int n=0; n<10; n++) ptsList.push_back(Point(n,0));
    ptsList[2].m_y = 0.2;
    ptsList[6].m_y = -0.3;
  }
  
  double thresh = 0.2;
  if (argc>2) {
    sscanf(argv[2], "%lf", &thresh);
  }
  
  bool debug = false;
  if (argc>3) {
    int val = 0;
    sscanf(argv[3], "%d", &val);
    if(val) debug = true;
  }
  
  // for(int n=0; n<ptsList.size(); n++) printf("%d : %5.2f, %5.2f\n",n,ptsList[n].m_x, ptsList[n].m_y);
  printf("thresh=%5.2f, debug=%d\n",thresh, debug);
  
  std::vector<Line> linesList;
  linesList = RDPRecur(ptsList, linesList, thresh, 0, -1, 0, debug);
  printf("writing to [%s]....\n",outFile);
  FILE *fp = fopen(outFile, "w");
  for(int n=0; n<linesList.size(); n++) {
    linesList[n].show(fp);
    linesList[n].show();
  }
  fclose(fp);
  return 0;
}