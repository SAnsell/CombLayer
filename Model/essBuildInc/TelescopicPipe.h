#ifndef essSystem_TelescopicPipe_h
#define essSystem_TelescopicPipe_h

class Simulation;

namespace essSystem
{

/*!
  \class TelescopicPipe
  \author A. Milocco
  \version 1.0
  \date February 2013
  \brief Reflector object
*/
class TelescopicPipe : public attachSystem::ContainedGroup,
    public attachSystem::FixedComp
{
 private:

  const int ptIndex;              ///< Index of surface offset
  int cellIndex;                  ///< Cell index
  int engActive; ///< True if engineering details should be built

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  size_t nSec;                    ///< Number of tube sections
  std::vector<double> radius;     ///< Radius of inner void
  std::vector<double> length;     ///< cummulative length
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

  TelescopicPipe(const std::string&);
  TelescopicPipe(const TelescopicPipe&);
  TelescopicPipe& operator=(const TelescopicPipe&);
  virtual TelescopicPipe* clone() const;
  virtual ~TelescopicPipe();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&);

};

}

#endif

