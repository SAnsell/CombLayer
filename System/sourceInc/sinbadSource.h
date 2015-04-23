#ifndef sinbadSource_h
#define sinbadSource_h

namespace SDef
{
  class Source;
}

namespace SDef
{

/*!
  \class sinbadSource
  \version 1.0
  \author S. Ansell
  \date August 2010
  \brief Adds a new source in the guide system
*/

class sinbadSource 
{
 private:
  
  double cutEnergy;             ///< Energy cut point
  double weight;                ///< neutron weight
  Geometry::Vec3D CentPoint;    ///< Origin Point [Start shutter]
  Geometry::Vec3D Direction;    ///< Direction of centre
  double angleSpread;           ///< Angle spread
  double radialSpread;          ///< Radial spread
  std::vector<double> Energy;   ///< Energies [MeV]
  std::vector<double> EWeight;  ///< Weights
   std::vector<double> EG;  

  void loadEnergy(const std::string&);
  void createSourceSinbad(SDef::Source&);

 public:

  sinbadSource();
  sinbadSource(const sinbadSource&);
  sinbadSource& operator=(const sinbadSource&);
  ~sinbadSource();

  /// Set cut energy
  void setCutEnergy(const double E) { cutEnergy=E; }

  /* void createAll(const std::string&, */
  /* 		 const Geometry::Vec3D&, */
  /* 		 const Geometry::Vec3D&,const double, */
  /* 		 const double,SDef::Source&); */

  /* void createAll(const std::string&, */
  /* 		 const attachSystem::LinearComp&, */
  /* 		 const double,const double,SDef::Source&); */

  // ALB
 void createAll(SDef::Source&);

  /* void createAll(const std::string&, */
  /* 		 const attachSystem::LinearComp&, */
  /* 		 const double,const double,SDef::Source&); */
 //ALB


  /// Access central point:
  const Geometry::Vec3D& getCentPoint() const { return CentPoint; }
  /// Access central axis:
  const Geometry::Vec3D& getDirection() const { return Direction; }
  
};

}

#endif
 
