//Credit: Inspired and amended from https://github.com/HopkinsIDD/IDSpatialStats/commit/2782d6dcc9ee4be9855b5e468ce789425b81d49a by @gilesjohnr @jlessler
//Author: @t-pollington
//Date: 29 Jan 2019
#include <cstdlib>
#include <iostream>
#include <Rcpp.h>
using namespace std;
using namespace Rcpp;
// [[Rcpp::export]]

NumericVector getTau(NumericVector ORIG_ID, NumericVector x, NumericVector y, NumericVector onset, NumericVector r, NumericVector r_low, SEXP index){ //see how multiple vector arguments are parsed in here rather than the single posmat matrix
  int i,j,k;
  double dist2 = 0;
  double r2 = 0;
  double r2_low = 0;
  long long num_cnt, denom_cnt; /*counters for those filling conditions*/
  int f_ans = 3; /*used to hold the result of the function*/
  int serialintvl = 7; //mean serial interval of the disease
  int r_size = r.size();
  int N = ORIG_ID.size();
  int *inds = INTEGER(index);
  NumericVector tau(r_size, NULL);
  double piInf = 0;
  int nocheck = 0;

if(*inds==-1){ //if index was set to -1 then it means we can turn off bootstrapping checks 
  nocheck = 1; 
}

num_cnt = 0;
denom_cnt = 0;

//calc piInf
for (i=0;i<N;i++) {
  for (j=0; j<i;j++) {
    
    /*do not compare someone with themself if bootstrapping*/
    if (nocheck==0){
      if (inds[i] == inds[j]) continue;   
    }

    //Rfun function coded in C to choose between cases {1,2,3} for the numerator or denominator counts
    if (ORIG_ID[i]==ORIG_ID[j]) continue; // ie f_ans=3
    if((onset[i]==-999) || (onset[j]==-999)){ //prioritise the if-else order for NC-NC pair or NC-C/C-NC pair which are the largest % of the dataset. 
      f_ans = 2;
    } else if(abs(onset[i]-onset[j])>serialintvl){ //case pair not temporally-related
      f_ans = 2;
    //could add temporal restrictions here that the pair be within the start and end dates of the study
    } else {
      f_ans = 1;
    }

    if (f_ans==2) { /* this case is more common so placed at the top of this if-else chain*/
      denom_cnt++;
    } else if (f_ans==1) {
      num_cnt++;
      denom_cnt++;
    }
  }
}
piInf = (double)num_cnt/denom_cnt;

//calc pi(r2_low,r2)
for (k=0;k<r_size;k++) {
  num_cnt = 0;
  denom_cnt = 0;
  r2  = pow(r[k],2); //transformation to squared distances to make sqrt() redundant in this line and the one below
  r2_low = pow(r_low[k],2);

  for (i=0;i<N;i++) {
    for (j=0; j<i;j++) { //lower triangular access only as undirected pairs assumed
      
      /*do not compare someone with themself if bootstrapping*/
      if (nocheck==0){
        if (inds[i] == inds[j]) continue;  
      }

      /*calculate the distance*/ 
      dist2 = pow(x[i] - x[j],2) + pow(y[i] - y[j],2);    

      if ((dist2>=r2) || (dist2<r2_low)) continue;
  
      if (ORIG_ID[i]==ORIG_ID[j]) continue; //ie f_ans=3
      if((onset[i]==-999) || (onset[j]==-999)){ 
        f_ans = 2;
      } else if(abs(onset[i]-onset[j])>serialintvl){
        f_ans = 2;
      //could add temporal restrictions here that the pair be within the start and end dates of the study
      } else {
        f_ans = 1;
      }

      /*update the counts appropriately*/
      if (f_ans==2) { 
        denom_cnt++;
      } else if (f_ans==1) {
        num_cnt++;
        denom_cnt++;
      }
    }
  }
  tau[k] = (double)num_cnt/denom_cnt; // pi(r.min,r.max)
  tau[k] = (double)tau[k]/piInf;
}
return(tau);
}