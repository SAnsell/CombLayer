/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   transportInc/Detector.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef Transport_PointDetector_h
#define Transport_PointDetector_h

namespace Transport
{

/*!  
  \class PointDetector
  \brief Defines a detector
  \version 1.0
  \author S. Ansell
  \date  2014
*/

class PointDetector : public Detector
{
 private: 

  double angle;                ///< Detector angle
  Geometry::Vec3D Cent;        ///< Cent Pos
  std::map<int,double> cnt;    ///< Number of counts per cell/mat

 public:
  
  explicit PointDetector(const size_t);
  PointDetector(const size_t,const Geometry::Vec3D&);
  PointDetector(const PointDetector&);
  PointDetector& operator=(const PointDetector&);
  /// Clone constructor
  virtual PointDetector* clone() const { return new PointDetector(*this); }
  virtual ~PointDetector();

  /// Effective typeid
  virtual std::string className() const { return "PointDetector"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  virtual double project(const MonteCarlo::neutron&,
			 MonteCarlo::neutron&) const;

  virtual void addEvent(const MonteCarlo::neutron&);
  virtual void clear();
  virtual void normalize(const size_t);


  void setCentre(const Geometry::Vec3D&);
  void setAngle(const double A) { angle=A; }

  // Output stuff
  virtual void writeHeader(std::ostream&) const;
  virtual void write(std::ostream&) const;

};

}  // NAMESPACE Transport

#endif
