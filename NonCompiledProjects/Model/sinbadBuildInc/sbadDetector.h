#ifndef sinbadSystem_sbadDetector_h
#define sinbadSystem_sbadDetector_h

class Simulation;

namespace sinbadSystem
{
/*!
  \class sbadDetector
  \version 1.0
  \author A. Milocco
  \date February 2014
  \brief Rod detector
*/

class sbadDetector : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:
  
  const std::string baseName;     ///< Base name
  const size_t detID;             ///< Index of Detector

  int active;                     ///< active flag

  double radius;                  ///< Radis of rod 
  double length;                  ///< Length of rod
  int mat;                        ///< Material 

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  sbadDetector(const std::string&,const size_t);
  sbadDetector(const sbadDetector&);
  sbadDetector& operator=(const sbadDetector&);
  virtual sbadDetector* clone() const; 
  virtual ~sbadDetector();

  
  int isActive() const { return active; }

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,
		 const long int);
  
};

}

#endif
 
