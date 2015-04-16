#ifndef essSystem_ProtonTube_h
#define essSystem_ProtonTube_h

class Simulation;

namespace essSystem
{

/*!
  \class ProtonTube
  \author A. Milocco
  \version 1.0
  \date February 2013
  \brief Reflector object 
*/

class ProtonTube : public attachSystem::ContainedGroup,
    public attachSystem::FixedComp
{
 private:

  const int ptIndex;              ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  size_t nSec;                    ///< Number of tube sections
  std::vector<double> radius;     ///< Radius of inner void
  std::vector<double> length;     ///< cummolative length 
  std::vector<double> zCut;       ///< Z cut for section
  std::vector<double> thick;      ///< Wall thickness  
  std::vector<int> inMat;         ///< Inner material
  std::vector<int> wallMat;       ///< wall material

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);

  void createSurfaces();
  void createObjects(Simulation&,const std::string&,const std::string&);
  void createLinks();

 public:

  ProtonTube(const std::string&);
  ProtonTube(const ProtonTube&);
  ProtonTube& operator=(const ProtonTube&);
  virtual ~ProtonTube();
   
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const attachSystem::FixedComp&,const long int);
 
};

}

#endif
 
