/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/Detector.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef Transport_Detector_h
#define Transport_Detector_h

namespace Transport
{

/*!  
  \class Detector
  \brief Defines a detector
  \version 1.0
  \author S. Ansell
  \date December 2009
*/

class Detector
{
 private: 

  long int nps;                ///< Number of detector units

  int nH;                      ///< Number of bins horrizonal
  int nV;                      ///< Number of bins vertical
  int nE;                      ///< Number of bins [energy]
  Geometry::Vec3D Cent;        ///< Cent Pos
  Geometry::Vec3D H;           ///< Horrizontal unitVector
  Geometry::Vec3D V;           ///< Vert extent
  double hSize;                ///< horizontal size 
  double vSize;                ///< Vertial size
  Geometry::Vec3D PlnNorm;     ///< Plane normal
  double PlnDist;              ///< Plane distance
  
  std::vector<double> EGrid;   ///< Energy Grid [eV]

  boost::multi_array<double,3> EData;  ///< Energy data set

 public:
  
  Detector();
  Detector(const int,const int,const int,
	   const Geometry::Vec3D&,const Geometry::Vec3D&,
	   const Geometry::Vec3D&,const double,const double);
  Detector(const Detector&);
  Detector& operator=(const Detector&);
  /// Clone constructor
  virtual Detector* clone() const { return new Detector(*this); }
  virtual ~Detector();

  /// Effective typeid
  virtual std::string className() const { return "Detector"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  /// Access bins
  std::pair<int,int> bins() const { return std::pair<int,int>(nH,nV); }
  std::pair<Geometry::Vec3D,Geometry::Vec3D> getAxis() const;
  /// Access Centre
  const Geometry::Vec3D& getCentre() const { return Cent; }

  Geometry::Vec3D getRandPos() const;
  void project(const MonteCarlo::neutron&,
	        MonteCarlo::neutron&) const;
  int calcCell(const MonteCarlo::neutron&,int&,int&) const;
  void addEvent(const MonteCarlo::neutron&);

  void clear();
  void setDataSize(const int,const int,const int);
  void setCentre(const Geometry::Vec3D&);
  void setEnergy(const double,const double);

  long int calcWavePoint(const double) const;
  long int calcEnergyPoint(const double) const;

  // Output stuff
  void write(const std::string&,const long int) const;
  void write(std::ostream&,const long int) const;
};

std::ostream&
operator<<(std::ostream&,const Detector&);

}  // NAMESPACE Transport

#endif
