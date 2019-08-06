#include <stdio.h> //printf
#include <iostream> //cout
#include <fstream> //streampos
#include <vector>
#include <iterator>
using namespace std;
int main(){
  ifstream fileorig_idenum("ORIG_IDenum.bin", ios::in|ios::binary|ios::ate);
  double* orig_idenum;
  int idx=16221;
  orig_idenum = (double*)malloc((idx+1) * sizeof(orig_idenum[0]));
  if(orig_idenum == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (fileorig_idenum.is_open())
  {
    streampos size = fileorig_idenum.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    fileorig_idenum.seekg (0, ios::beg);
    fileorig_idenum.read (memblock, size);
    fileorig_idenum.close();
    cout << "orig_idenum is in memory \n";
    orig_idenum = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  printf("orig_idenum[0] = %f\n", orig_idenum[0]);
  printf("orig_idenum[1] = %f\n", orig_idenum[1]);
  printf("orig_idenum[2] = %f\n", orig_idenum[2]);
  printf("orig_idenum[16220] = %f\n", orig_idenum[16220]);
  vector<short int> orig_idenumv;
  for(int i = 0; i<idx; i++){
    orig_idenumv.push_back(orig_idenum[i]);
  }
  printf("orig_idenumv[0] = %d\n", orig_idenumv[0]);
  printf("orig_idenumv[1] = %d\n", orig_idenumv[1]);
  printf("orig_idenumv[2] = %d\n", orig_idenumv[2]);
  printf("orig_idenumv[10] = %d\n", orig_idenumv[10]);
  printf("orig_idenumv[50] = %d\n", orig_idenumv[50]);
  printf("orig_idenumv[16220] = %d\n", orig_idenumv[16220]);
  
  ifstream filex("x.bin", ios::in|ios::binary|ios::ate);
  double* x;
  idx=16221;
  x = (double*)malloc((idx+1) * sizeof(x[0]));
  if(x == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (filex.is_open())
  {
    streampos size = filex.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    filex.seekg (0, ios::beg);
    filex.read (memblock, size);
    filex.close();
    cout << "x is in memory \n";
    x = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  vector<double> xv;
  for(int i = 0; i<idx; i++){
    xv.push_back(x[i]);
  }
  printf("xv[0] = %f\n", xv[0]);
  printf("xv[1] = %f\n", xv[1]);
  printf("xv[2] = %f\n", xv[2]);
  printf("xv[16220] = %f\n", xv[16220]);
  
  ifstream filey("y.bin", ios::in|ios::binary|ios::ate);
  double* y;
  idx=16221;
  y = (double*)malloc((idx+1) * sizeof(y[0]));
  if(y == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (filey.is_open())
  {
    streampos size = filey.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    filey.seekg (0, ios::beg);
    filey.read (memblock, size);
    filey.close();
    cout << "y is in memory \n";
    y = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  vector<double> yv;
  for(int i = 0; i<idx; i++){
    yv.push_back(y[i]);
  }
  printf("yv[0] = %f\n", yv[0]);
  printf("yv[1] = %f\n", yv[1]);
  printf("yv[2] = %f\n", yv[2]);
  printf("yv[16220] = %f\n", yv[16220]);
  
  ifstream fileKA("KA.bin", ios::in|ios::binary|ios::ate);
  double* KA;
  idx=16221;
  KA = (double*)malloc((idx+1) * sizeof(KA[0]));
  if(KA == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (fileKA.is_open())
  {
    streampos size = fileKA.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    fileKA.seekg (0, ios::beg);
    fileKA.read (memblock, size);
    fileKA.close();
    cout << "KA is in memory \n";
    KA = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  printf("KA[0] = %f\n", KA[0]);
  printf("KA[1] = %f\n", KA[1]);
  printf("KA[2] = %f\n", KA[2]);
  printf("KA[120] = %f\n", KA[120]);
  printf("KA[121] = %f\n", KA[121]);
  printf("KA[16220] = %f\n", KA[16220]);
  vector<short int> KAv;
  for(int i = 0; i<idx; i++){
    KAv.push_back(KA[i]);
  }
  printf("KAv[0] = %d\n", KAv[0]);
  printf("KAv[1] = %d\n", KAv[1]);
  printf("KAv[2] = %d\n", KAv[2]);
  printf("KAv[13] = %d\n", KAv[13]);
  printf("KAv[14] = %d\n", KAv[14]);
  printf("KAv[16220] = %d\n", KAv[16220]);
  
  ifstream filermax("r.max.bin", ios::in|ios::binary|ios::ate);
  double* rmax;
  idx=10;
  rmax = (double*)malloc((idx+1) * sizeof(rmax[0]));
  if(rmax == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (filermax.is_open())
  {
    streampos size = filermax.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    filermax.seekg (0, ios::beg);
    filermax.read (memblock, size);
    filermax.close();
    cout << "r.max is in memory \n";
    rmax = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  vector<unsigned short int> rmaxv(rmax, rmax + sizeof rmax / 2*sizeof rmax[0]);
  printf("rmaxv[0] = %d\n", rmaxv[0]);
  printf("rmaxv[1] = %d\n", rmaxv[1]);
  printf("rmaxv[2] = %d\n", rmaxv[2]);
  printf("rmaxv[9] = %d\n", rmaxv[9]);
  
  ifstream filermin("r.min.bin", ios::in|ios::binary|ios::ate);
  double* rmin;
  idx=10;
  rmin = (double*)malloc((idx+1) * sizeof(rmin[0]));
  if(rmin == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (filermin.is_open())
  {
    streampos size = filermin.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    filermin.seekg (0, ios::beg);
    filermin.read (memblock, size);
    filermin.close();
    cout << "r.min is in memory \n";
    rmin = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  vector<unsigned short int> rminv;
  for(int i = 0; i<idx; i++){
    rminv.push_back(rmin[i]);
  }
  printf("rminv[0] = %d\n", rminv[0]);
  printf("rminv[1] = %d\n", rminv[1]);
  printf("rminv[2] = %d\n", rminv[2]);
  printf("rminv[9] = %d\n", rminv[9]);
  
  ifstream fileindx("indx.bin", ios::in|ios::binary|ios::ate);
  double* indx;
  idx=16221;
  indx = (double*)malloc((idx+1) * sizeof(indx[0]));
  if(indx == NULL)
  {
    printf("not enough memory");
    return 1;
  }
  if (fileindx.is_open())
  {
    streampos size = fileindx.tellg();
    cout << "size=" << size << " bytes\n";
    char * memblock = new char [size];
    fileindx.seekg (0, ios::beg);
    fileindx.read (memblock, size);
    fileindx.close();
    cout << "indx is in memory \n";
    indx = (double*)memblock; //reinterpret
  }
  else
  {
    cout << "Unable to open file";
  }
  printf("indx[0] = %f\n", indx[0]);
  printf("indx[16220] = %f\n", indx[16220]);
  vector<short int> indxv;
  for(int i = 0; i<idx; i++){
    indxv.push_back(indx[i]);
  }
  printf("indxv[0] = %d\n", indxv[0]);
  printf("indxv[16220] = %d\n", indxv[16220]);
return(0);
}