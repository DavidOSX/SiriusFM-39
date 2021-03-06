#pragma once

#include <iostream>
#include <tuple>
#include <time.h>
#include "Time.h"


namespace SiriusFM {

template <typename Diffusion, 
          typename AProvider, 
          typename BProvider, 
          typename AssetClassA, 
          typename AssetClassB,
          typename PathEvaluator> class MCEngine {
private:
    long const         m_MaxL;
    long const         m_MaxPM; //max number of stored paths
    double* const      m_paths; //stored paths
    double* const      m_ts; //timeline
    //long               m_L;//
    //long               m_P;//
   
    
public:
    MCEngine(long a_MaxL, long a_MaxPM):
    m_MaxL(a_MaxL),
    m_MaxPM(a_MaxPM),
    m_paths(new double[m_MaxL * m_MaxPM]),
    m_ts(new double[m_MaxL])
    //m_L(0),
    //m_P(0)
    { 
              if(m_MaxL <= 0 || m_MaxPM <= 0) throw std::invalid_argument("bad Max_L or MaxPM"); 
              for(long l = 0; l < m_MaxL; ++l) {
                  m_ts[l] = 0;
                  long lp = l * m_MaxPM;
                  for(long p = 0; p < m_MaxPM; ++p) m_paths[lp + p] = 0; 
              }
    }
    
    template<bool> void Simulate(time_t                       a_t0, 
                                 time_t                       a_T, 
                                 int                          a_tau_min,
                                 long                         a_P,
                                 bool                         a_useTimerSeed,
                                 Diffusion const*             a_diff, 
                                 AProvider const*             a_ap,
                                 BProvider const*             a_bp,
                                 AssetClassA                  a_A,
                                 AssetClassB                  a_B,
                                 PathEvaluator*               a_pathEval
                                );
                    
    void printPaths(double a_S0) { 
        FILE *f = fopen("PATHS.txt","w");
        int i = 0;
        //fprintf(f,"%ld %ld\n", m_L, m_P);
        while((m_paths[i + 1] != a_S0 && i < 5000)) {
            fprintf(f,"%lf\n", m_paths[i]);
            i++;
        }
        fclose(f);
    }
                    
    //std::tuple<long, long, double const*> GetPaths() const {
    //    return (m_L <= 0 || m_P < 0) ? std::make_tuple(0,0,nullptr) : std::make_tuple(m_L,m_P,m_paths);
    //}
                    
    ~MCEngine() { 
        delete[] m_paths; 
        delete[] m_ts;
        //m_paths = nullptr;
        //m_ts = nullptr;
        //const_cast<MCEngine>
    }
                    
    MCEngine(MCEngine&) = delete;
                    
    MCEngine& operator= (MCEngine const&) = delete; 
};
    
};    
        
