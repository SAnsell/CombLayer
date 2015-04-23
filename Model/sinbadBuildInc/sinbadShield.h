#ifndef sinbadSystem_sinbadShield_h
#define sinbadSystem_sinbadShield_h

class Simulation;

namespace sinbadSystem
{
  /*!
    \class sinbadShield
    \version 1.0
    \author A. Milocco
    \date Novenber 2013
    \brief 
  */

class sinbadShield  : public attachSystem::ContainedComp,
      public attachSystem::FixedComp
{
 private:
  
  const int slabIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index

  double offSetX;               ///< Master offset distance 
  double offSetY;               ///< Master offset distance 
  double offSetZ;               ///< Master offset distance 
  double lengthSlab;
  double heightSlab;
  double temperatureSlab;

  size_t nSlab;                ///< number of slabs
  std::vector<double> width;    ///< slab thickness
 std::vector<double> length;    ///< slab thickness
 std::vector<double> height;    ///< slab thickness  
 std::vector<int> mat;      ///< Material type
  int shieldCell;
  int detN;
  std::vector<double> PY;    // det pos
  double LM;
  double HM;

  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);
/* ,const attachSystem::ContainedComp& CC); */
  void createLinks();

 public:

  sinbadShield(const std::string&);
  sinbadShield(const sinbadShield&);
  sinbadShield& operator=(const sinbadShield&);
  virtual ~sinbadShield();

  std::string getComposite(const std::string&) const;
  int getCell() const { return shieldCell; }


  void createAll(Simulation&,const attachSystem::FixedComp&);
};

}

#endif
 
