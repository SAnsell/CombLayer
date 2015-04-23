#ifndef sinbadSystem_Cave_h
#define sinbadSystem_Cave_h

class Simulation;

namespace sinbadSystem
{
/*!
  \class Cave
  \version 1.0
  \author A. Milocco
  \date Novenber 2013
  \brief No idea
*/

class Cave  : public attachSystem::ContainedComp,
      public attachSystem::FixedComp
{
 private:
  
  const int caveIndex;          ///< Index of surface offset
  int cellIndex;                ///< Cell index


  double xStep;          ///< X step
  double yStep;          ///< Y Step
  double zStep;          ///< Z step

  double height;          ///< Height
  double length;          ///< Length
  double width;           ///< Width
  double wallThick;       ///< wall thickness
  int mat;                ///< Material for walls

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  Cave(const std::string&);
  Cave(const Cave&);
  Cave& operator=(const Cave&);
  virtual ~Cave();

  virtual void createAll(Simulation&,
                         const attachSystem::FixedComp&);
};

}

#endif
 
