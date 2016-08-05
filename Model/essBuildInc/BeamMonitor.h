#ifndef essSystem_BeamMonitor_h
#define essSystem_BeamMonitor_h

class Simulation;

namespace essSystem
{

/*!
  \class BeamMonitor
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief BeamMonitor
*/

class BeamMonitor : public attachSystem::ContainedComp,
    public attachSystem::FixedOffset
{
 private:

  const int monIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  size_t nSec;                   ///< Number of main segments
  std::vector<double> radius;    ///< Radius of each secion
  std::vector<double> thick;     ///< Thickness
  std::vector<int> mat;          ///< Material     
  double halfThick;              ///< Mid point thickness

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
 
