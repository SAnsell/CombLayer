#ifndef sinbadSystem_FissionPlate_h
#define sinbadSystem_FissionPlate_h

class Simulation;

namespace sinbadSystem
{

/*!
  \class FissionPlate
  \version 2.0
  \author S. Ansell 
  \date February 2014
  \brief Divided slab object specialized to 

  Based on object from A. Milocco
*/

class FissionPlate  : public LayerPlate
{
 private:

  std::vector<size_t> DIndex;      ///< Objects that are divided

  size_t nDivide;                  ///< Number of divided layers
  size_t nXSpace;                  ///< Number of X Spaces 
  size_t nZSpace;                  ///< Number of Z Spaces 

  std::vector<double> XPts;        ///< X divide boundary  [NXSpace-1]
  std::vector<double> ZPts;        ///< Z divide boundary [NZSpace-1]
  
  /// material array for system [default populate]
  boost::multi_array<int,3> matIndex;
  /// temperature array [default populated]
  boost::multi_array<double,3> matTemp;

  template<typename T> T getXZ(const FuncDataBase&,const std::string&,
			       const long int,const long int,
			       const long int) const;
  std::string getXSurf(const long int) const;
  std::string getZSurf(const long int) const;
  
  virtual void populate(const FuncDataBase&);

  virtual void createSurfaces();
  virtual void createObjects(Simulation&,const attachSystem::FixedComp&,
			     const long int);

 public:

  FissionPlate(const std::string&);
  FissionPlate(const FissionPlate&);
  FissionPlate& operator=(const FissionPlate&);
  FissionPlate* clone() const;
  virtual ~FissionPlate();

  virtual void createAll(Simulation&,const attachSystem::FixedComp&,
			     const long int);


};

}

#endif
 
