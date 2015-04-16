#ifndef essSystem_BeamMonitor_h
#define essSystem_BeamMonitor_h

class Simulation;

namespace essSystem
{

/*!
  \class BeamMonitor
  \author A. Milocco
  \version 1.0
  \date February 2013
  \brief Reflector object 
*/

class BeamMonitor : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int monIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  size_t nSec;                   ///< Number of main segments
  std::vector<double> radius;    ///< Radius of each secion
  std::vector<double> thick;     ///< Thickness
  std::vector<int> mat;          ///< Material     
  double halfThick;              ///< Mid point thickness

  double sideW;
  double frameSide;
  double frameHeightA;
  double frameHeightB;
  double frameHeightC;
  double frameHeightD;
  double frameWidthA;
  double frameWidthB;
  double frameWidthC;
  double frameThickA;  
  double frameThickB;
  double frameThickC;
  int tubeN;
  double tubeRadius;
  double tubeThick;
  int tubeHe;
  int tubeAl;
  int extTubeHe;
  int mainPBeamACell; 

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,const long int);
  void createSurfaces();
  void createObjects(Simulation&,const attachSystem::ContainedGroup&,
		     const std::string&);
  void createLinks();
  
  std::string calcExclude(const size_t,
			  const attachSystem::ContainedGroup&,
			  const std::string&) const;


 public:

  BeamMonitor(const std::string&);
  BeamMonitor(const BeamMonitor&);
  BeamMonitor& operator=(const BeamMonitor&);
  virtual ~BeamMonitor();
   

  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int,const attachSystem::ContainedGroup&,
		 const std::string&);
 
};

}

#endif
 
