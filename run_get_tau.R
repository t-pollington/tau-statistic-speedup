rm(list = ls()) #clean up
library(Rcpp) #sourceCpp()
setwd(FolderWhereYouStoreYourData)
load(file = "YourData.RData") #say you have called your matrix object "X"

sourceCpp("get_tau.cpp")

r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
getTau(X[,"ORIG_ID"],X[,"x"],X[,"y"],X[,"onset"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
