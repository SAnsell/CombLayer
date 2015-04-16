#ifndef sinbadSystem_Nestor_h
#define sinbadSystem_Nestor_h

class Simulation;

namespace sinbadSystem
{
/*!
  \class Nestor
  \version 1.0
  \author A. Milocco
  \date Novenber 2013
  \brief Builds a layered stack object
\ build of shield array
*/

class Nestor  : public attachSystem::ContainedComp,
      public attachSystem::FixedComp
{
 protected:
  
  const int slabIndex;        ///< Index of surface offset

  int cellIndex;                ///< Cell index

  double xStep;               ///< X Step distance 
  double yStep;               ///< Y Step distance
  double zStep;               ///< Z Step distance 

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

  Nestor(const std::string&);
  Nestor(const Nestor&);
  Nestor& operator=(const Nestor&);
  virtual Nestor* clone() const;
  virtual ~Nestor();

  virtual void createAll(Simulation&,
			 const attachSystem::FixedComp&,
			 const long int);
};

}

#endif
 
