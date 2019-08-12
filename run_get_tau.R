rm(list = ls()) #clean up from before
library(Rcpp) #sourceCpp()
setwd(FolderWhereYouStoreYourData)
load(file = "YourData.RData") #say you have called your matrix object "X"
setwd("/taustatisticspeedup")
#'Install and restart'

r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ptm <- proc.time()
for (i in 1:10) {
  taustatisticspeedup::getTau2(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
}
timing = (proc.time() - ptm)
timing/10 #2.0578s for commit 5dceac9 get_tau.cpp
#ans = 3.236489 2.596965 2.479944 2.332052 2.179765 2.097509 2.005453 1.940027 1.815320 1.732865
r.max = seq(50,500,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ptm <- proc.time()
ans = taustatisticspeedup::getTau3(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
timing = (proc.time() - ptm)
timing/10 #12702s or 211mins
ans # 15.714699  8.741592  7.314651  6.395508  5.551391  5.100317  4.596279  4.209525  3.607323  3.277187
r.max = seq(550,1000,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ans2 = taustatisticspeedup::getTau3(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
#2.896395 2.646779 2.466667 2.299608 2.179156 2.097263 2.006802 1.973677 1.934798 1.901334

ptm <- proc.time()
r.max = seq(50,1000,50) #upper bound of each bin, in metres
r.min = rep.int(0,length(r.max)) #lower bound of each bin
ans2 = taustatisticspeedup::getTau23(X.NW[,"ORIG_IDenum"],X.NW[,"x"],X.NW[,"y"],X.NW[,"KA"],r.max,r.min,as.integer(-1)) #note that as we want the point estimate of tau we set the last argument to -1. You could adapt this function for bootstrapping by parsing in a vector of indices for the last argument.
timing = (proc.time() - ptm) #15hrs
ans2 #[1] 24.121248 12.063201  9.397764  7.753535  6.479244  5.782907  5.110688  4.609856  3.918063  3.529968
# [11]  3.101090  2.818041  2.615020  2.428179  2.292652  2.197046  2.095277  2.053082  2.005737  1.965266

setwd(dir = "/home/tim/tau-statistic-speedup")
load(file = "X.NWtau23.RData")
load(file = "ans3.RData")
r.max = seq(50,1000,50)
pdf(file = "taucomparison.pdf")
plot(r.max,ans23,type = "l",ylim = range(c(1,max(ans23))),ylab = "Tau variant",xlab = "d_2 (metres)",col = "red",main = "Comparison of different tau metrics \n as 50m distance bands on X.NW data")
lines(r.max,ans3,col = "red",lty = 2)
lines(r.max,tau2)
legend(700, 20, legend=c("Tau_23","Tau_3","Tau_2","Tau_2 = 1"), col=c("red","red","black","black"), lty=c(1,2,1,2), cex=0.8)
abline(h = 1,lty = 2)
dev.off()

