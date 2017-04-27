#ifndef essSystem_OnionCooling_h
#define essSystem_OnionCooling_h

class Simulation;

namespace essSystem
{

/*!
  \class OnionCooling
  \date March 2014
  \brief Onion cooling pipes
*/

class OnionCooling : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:

  const int onionIndex;             ///< Index of surface offset
  int cellIndex;                  ///< Cell index. Every object has a space of about 10000 cells unless you request more.

  double xStep;                   ///< X step
  double yStep;                   ///< Y step
  double zStep;                   ///< Z step
  double xyAngle;                 ///< XY Angle
  double zAngle;                  ///< Z Angle

  //  double height;                  ///< Height
  double wallThick;               ///< Wall thickness
  int wallMat;                        ///< material
  double wallTemp;  ///< wall temperature

  size_t nRings;
  std::vector<double> radius;                  ///< Radius of the rings
  std::vector<double> gateWidth;                  ///< full width of spacing in the corresponding ring
  std::vector<double> gateLength;                  ///< length of 'the door' in the corresponding ring

  // Functions:

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createSurfaces();
  void createObjects(Simulation&, const attachSystem::FixedComp&);
  void createLinks();

 public:

  OnionCooling(const std::string&);
  OnionCooling(const OnionCooling&);
  OnionCooling& operator=(const OnionCooling&);
  virtual OnionCooling* clone() const;
  virtual ~OnionCooling();

  void setBottomSurface(const attachSystem::FixedComp& FC, const long int link);
  void setUpperSurface(const attachSystem::FixedComp& FC, const long int link);

  int getMainCell() const { return onionIndex+1; }
  virtual void addToInsertChain(attachSystem::ContainedComp&) const; // has to be there [2:1040]
  void createAll(Simulation&,const attachSystem::FixedComp&); // [2:1070]
  
};

}

#endif
 
