
#include "MonteCarlo.hpp"
#include "Vanillas.h"

using namespace SiriusFM;
using namespace std;

int main(const int argc, const char* argv[]) {
    CcyE c1 = CcyE::USD;
    CcyE c2 = CcyE::CHF;
    
    if(argc < 10) {
        cerr << "not enough params\n";
        return 1;
    }
    
    IRProvider<IRMode::Const> irp = IRProvider<IRMode::Const>(argv[1]);
    printf("USD:%lf\n", irp.r(c1,0));
    printf("CHF:%lf\n", irp.r(c2,0));
    

    double mu = atof(argv[2]);
    double sigma = atof(argv[3]);
    double s0 = atof(argv[4]);
    char const* optionType = argv[5];
    double K = atof(argv[6]);
    long T_days = atol(argv[7]);
    int tau_min = atoi(argv[8]);
    long P = atol(argv[9]);
    
    
    Option const* opt;
    Diffusion_GBM diff = Diffusion_GBM(mu, sigma);
    if(strcmp(optionType, "Call") == 0) opt = new EurCallOption(K, T_days);
    else opt = new EurPutOption(K, T_days);
    
    
    time_t t0 = time(nullptr);
    time_t T = t0 + T_days*86400;
    double Ty = double(T_days)/365.25;
    
    
    MCEngine<decltype(diff), decltype(irp), decltype(irp), decltype(c1), decltype(c2)> mce(20'000, 20'000);
    mce.Simulate<false>(t0, T, tau_min, s0, P, &diff, &irp, &irp, c1, c2);
    mce.printPaths();
    auto res  = mce.GetPaths();
    long L1 = get<0>(res);
    long P1 = get<1>(res);
    double const* paths = get<2>(res);
    double est = 0., est2 = 0.;
    int nvp = 0;
    double priceOp = 0.;
    for(int p = 0; p < P1; ++p) {
        double const* path = paths + p*L1;
        double st = path[L1-1];
        if(st <= 0.) continue;
        ++nvp;
        double rt = log(st/s0);
        est += rt; est2 += rt*rt;
        priceOp += opt -> payoff(L1, nullptr, path);
       
    }
    assert(nvp > 1);
    est /= double(nvp);
    priceOp /= double(nvp);
    priceOp *= exp((-1)*irp.r(c1,0)*Ty);
    double VarST = (est2 - (double(nvp))*est*est)/double(nvp-1);
    double sigma2E = VarST/Ty;
    double muE = (est+VarST/2.0)/Ty;
    
    cout << "mu = " << mu << ", muE = " << muE << endl;
    cout << "sigma2 = " << sigma*sigma << ", sigma2E = " << sigma2E << endl;
    
    if(strcmp(optionType, "Call") == 0) cout << "price(Call) = " << priceOp << endl;
    else cout << "price(Put) = " << priceOp << endl;   
    
    
    
    return 0;
}
