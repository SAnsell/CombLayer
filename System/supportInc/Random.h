#ifndef Random_h
#define Random_h

namespace Random
{
  
  static std::mt19937 gen(17823);
  static std::uniform_real_distribution<double> RNG(0.0,1.0);
  static std::normal_distribution<double> RNN(0.0,1.0);

  inline double rand() { return RNG(gen); }
  inline double randNorm(const double mean,const double sigma)
    { return sigma*RNN(gen)-mean; }
}

#endif
