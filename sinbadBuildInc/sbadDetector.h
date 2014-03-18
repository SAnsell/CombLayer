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
  public attachSystem::FixedComp
{
 private:
  
  const std::string baseName;     ///< Base name
  const size_t detID;             ///< Index of Detector
  const int detIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index

  int active;                     ///< active flag
  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  double radius;                  ///< Radis of rod 
  double length;                  ///< Length of rod
  int mat;                        ///< Material 

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);  
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  sbadDetector(const std::string&,const size_t);
  sbadDetector(const sbadDetector&);
  sbadDetector& operator=(const sbadDetector&);
  virtual sbadDetector* clone() const; 
  virtual ~sbadDetector();

  virtual void createAll(Simulation&,
  			 const attachSystem::FixedComp&);
  
  int isActive() const { return active; }
  
};

}

#endif
 
