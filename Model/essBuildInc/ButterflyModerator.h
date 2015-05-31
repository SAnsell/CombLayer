#ifndef essSystem_ButterflyModerator_h
#define essSystem_ButterflyModerator_h

class Simulation;

namespace essSystem
{
  class MidWaterDivider;
  class EdgeWater;

/*!
  \class ButterflyModerator
  \author Stuart Ansell 
  \version 1.0
  \date April 2015
  \brief Butterfly moderator object [composite for mutli-system]
     
  Implementation based on K. Bat concept
*/

class ButterflyModerator :
  public constructSystem::ModBase
{
 private:

  const int flyIndex;        ///< Index of surface offset
  int cellIndex;             ///< Cell index
  
  std::shared_ptr<H2Wing> LeftUnit;        ///< Left part of the moderator
  std::shared_ptr<H2Wing> RightUnit;       ///< Right part of the moderator
  std::shared_ptr<MidWaterDivider> MidWater;    ///< Water divider
  std::shared_ptr<EdgeWater> LeftWater;    ///< Water divider
  std::shared_ptr<EdgeWater> RightWater;    ///< Water divider

  double totalHeight;                     ///< Total height
  double outerRadius;                     ///< Main outer radius

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const attachSystem::FixedComp*,
			const long int);

  void createExternal();
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();
  
 public:
  
  ButterflyModerator(const std::string&);
  ButterflyModerator(const ButterflyModerator&);
  ButterflyModerator& operator=(const ButterflyModerator&);
  virtual ButterflyModerator* clone() const;
  virtual ~ButterflyModerator();
  
  virtual Geometry::Vec3D getSurfacePoint(const size_t,const size_t) const;
  virtual int getLayerSurf(const size_t,const size_t) const;
  virtual std::string getLayerString(const size_t,const size_t) const;
  virtual int getCommonSurf(const size_t) const;

  /// Accessor to radius
  void setRadiusX(const double R) { outerRadius=R; }
  
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const attachSystem::FixedComp*,
		 const long int);
};

}

#endif
 
