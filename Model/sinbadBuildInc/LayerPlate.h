#ifndef sinbadSystem_LayerPlate_h
#define sinbadSystem_LayerPlate_h

class Simulation;

namespace sinbadSystem
{
/*!
  \class LayerPlate
  \version 1.0
  \author A. Milocco
  \date Novenber 2013
  \brief Builds a layered stack object
\ build of shield array
*/

class LayerPlate  : public attachSystem::ContainedComp,
      public attachSystem::FixedOffset
{
 protected:
  
  const int slabIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index

  bool frontShared;           ///< Front surface shared

  double width;               ///< Width of slab
  double height;              ///< Height of block

  size_t nSlab;                ///< number of slabs

  std::vector<double> thick;       ///< slab thickness
  std::vector<int> mat;            ///< Material type
  std::vector<double> matTemp;     ///< Material temperature

  /// Cut out size
  double radiusWindow;

  std::string getFrontSurface(const size_t,const attachSystem::FixedComp&,
			      const long int) const;
  std::string getBackSurface(const size_t,const attachSystem::FixedComp&,
			     const long int) const;
  int getCellIndex(const size_t) const;

  virtual void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,const long int);
  virtual void createSurfaces();
  virtual void createObjects(Simulation&,const attachSystem::FixedComp&,
			     const long int);
  virtual void createLinks();


 public:

  LayerPlate(const std::string&);
  LayerPlate(const LayerPlate&);
  LayerPlate& operator=(const LayerPlate&);
  virtual LayerPlate* clone() const;
  virtual ~LayerPlate();

  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);
};

}

#endif
 
