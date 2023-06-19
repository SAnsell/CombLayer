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

class OnionCooling :
    public attachSystem::FixedRotate,
    public attachSystem::ContainedComp,
    public attachSystem::ExternalCut,
    public attachSystem::CellMap
{
 private:

  double wallThick;                   ///< Wall thickness
  int wallMat;                        ///< material
  double wallTemp;                    ///< wall temperature

  size_t nRings;                      ///< number of rings
  std::vector<double> radius;         ///< Radius of the rings
  std::vector<double> gateWidth;      ///< full width of spacing in ring
  std::vector<double> gateLength;     ///< length of "the door" in  ring

  // Functions:

  void populate(const FuncDataBase&) override;
  void createSurfaces();
  void createObjects(Simulation&, const attachSystem::FixedComp&);
  void createLinks();

  void addToInsertChain(attachSystem::ContainedComp&) const;
 public:

  OnionCooling(const std::string&);
  OnionCooling(const OnionCooling&);
  OnionCooling& operator=(const OnionCooling&);
  virtual OnionCooling* clone() const;
  ~OnionCooling() override;

  void setBottomSurface(const attachSystem::FixedComp& FC, const long int link);
  void setUpperSurface(const attachSystem::FixedComp& FC, const long int link);

  int getMainCell() const { return buildIndex+1; }


  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int) override; 
  
};

}

#endif
 
