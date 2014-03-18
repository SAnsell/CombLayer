#ifndef sinbadSystem_makeSinbad_h
#define sinbadSystem_makeSinbad_h


/*!
  \namespace sinbadSystem
  \brief General Sinbad stuff
  \version 1.0
  \date November 2013
  \author A. Milocco
*/

namespace sinbadSystem
{
  class Cave;
  class sinbadShield;
  class LayerPlate;
  class FissionPlate;
  class sbadDetector;

/*!
  \class makeSinbad
  \version 1.0
  \author A. Milocco
  \date Novenber 2013
  \brief General building manifest for Sinbad
*/

class makeSinbad
{
 private:

  const std::string preName;          ///< Initializtion tag

  boost::shared_ptr<Cave> Surround;   

  boost::shared_ptr<LayerPlate> Primary;     ///< Initial beam control
  boost::shared_ptr<LayerPlate> Secondary;   ///< Stopping volume

  boost::shared_ptr<FissionPlate> fPlate;   ///< Fission Plate object

  std::vector<boost::shared_ptr<sbadDetector> > detArray;

  void buildDetectors(Simulation&);

 public:
  
  makeSinbad(const std::string&);
  makeSinbad(const makeSinbad&);
  makeSinbad& operator=(const makeSinbad&);
  ~makeSinbad();
  
  void build(Simulation*,const mainSystem::inputParam&);

};

}

#endif
