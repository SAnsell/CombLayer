#ifndef essSystem_ProtonTube_h
#define essSystem_ProtonTube_h

class Simulation;

namespace essSystem
{

/*!
  \class ProtonTube
  \author Konstantin Batkov
  \version 1.0
  \date Jan 2017
  \brief proton beam tube
*/
  class PBW;

class ProtonTube :  public TelescopicPipe
{
 private:
  int engActive; ///< True if engineering details (like PBW) should be built
  std::shared_ptr<PBW> pbw; ///< Proton beam window

  void populate(const FuncDataBase&);

 public:

  ProtonTube(const std::string&);
  ProtonTube(const ProtonTube&);
  ProtonTube& operator=(const ProtonTube&);
  virtual ~ProtonTube();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&);

};

}

#endif

