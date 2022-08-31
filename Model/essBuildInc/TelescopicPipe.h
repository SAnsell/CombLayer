#ifndef essSystem_TelescopicPipe_h
#define essSystem_TelescopicPipe_h

class Simulation;

namespace essSystem
{

/*!
  \class TelescopicPipe
  \author S. Ansell
  \version 1.0
  \date February 2013
  \brief Multi-component pipe
*/

class TelescopicPipe : public attachSystem::ContainedGroup,
  public attachSystem::FixedRotate,
  public attachSystem::FrontBackCut,
  public attachSystem::CellMap
{
 private:

  size_t nSec;                    ///< Number of tube sections
  std::vector<double> radius;     ///< Radius of inner void
  std::vector<double> length;     ///< cummolative length 
  std::vector<double> zCut;       ///< Z cut for section
  std::vector<double> thick;      ///< Wall thickness  
  std::vector<int> inMat;         ///< Inner material
  std::vector<int> wallMat;       ///< wall material

  // Functions:

  void populate(const FuncDataBase&);
  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  TelescopicPipe(const std::string&);
  TelescopicPipe(const TelescopicPipe&);
  TelescopicPipe& operator=(const TelescopicPipe&);
    virtual TelescopicPipe *clone() const;
  virtual ~TelescopicPipe();

  using FixedComp::createAll;
  void createAll(Simulation&,const attachSystem::FixedComp&,
		 const long int);
 
};

}

#endif
 
