#ifndef essSystem_ProtonTube_h
#define essSystem_ProtonTube_h

class Simulation;

namespace essSystem
{

/*!
  \class ProtonTube
  \author Konstantin Batkov / Stuart Ansell
  \version 2.0
  \date Apr 2018
  \brief proton beam tube
*/
  class PBW;
  class TelescopicPipe;

class ProtonTube :  public attachSystem::CopiedComp,
  public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset,
  public attachSystem::FrontBackCut
{
 private:
  int engActive; ///< True if engineering details (like PBW) should be built
  std::shared_ptr<TelescopicPipe> tube; ///< proton beam tube
  std::shared_ptr<PBW> pbw; ///< Proton beam window

  void populate(const FuncDataBase&);

 public:

  ProtonTube(const std::string&);
  ProtonTube(const ProtonTube&);
  ProtonTube& operator=(const ProtonTube&);
  virtual ~ProtonTube();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);

};
 
}

#endif

