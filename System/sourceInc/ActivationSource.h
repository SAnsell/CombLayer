/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   sourceInc/ActivationSource.h
*
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef SDef_ActivationSource_h
#define SDef_ActivationSource_h

namespace Geometry
{
  class Plane;
}

namespace SDef
{
  class Source;
  
/*!
  \class ActivationSource
  \version 1.0
  \author S. Ansell
  \date September 2016
  \brief Creates an active projection source
*/

class ActivationSource :
  public SourceBase
{
 private:

  size_t timeStep;                ///< Time step from cinder
  size_t nPoints;                 ///< Number of points
  size_t nTotal;                  ///< Total points
  
  Geometry::Vec3D ABoxPt;         ///< Bounding box corner
  Geometry::Vec3D BBoxPt;         ///< Bounding box corner

  std::map<int,double> volCorrection;   ///< cell / volume
  std::map<int,activeUnit> cellFlux;    ///< cell[active] : flux data
  std::vector<activeFluxPt> fluxPt;     ///< Flux emmision points [nps values]

  Geometry::Plane* PPtr;          ///< Plane point
  Geometry::Vec3D PlanePt;        ///< Plane point
  Geometry::Vec3D PlaneAxis;      ///< Plane axis
  double r2Power;                 ///< R power value

  Geometry::Vec3D weightPt;       ///< Centre weight intensity
  double weightDist;              ///< Centre weight scalar
  double externalScale;           ///< intensity scale [external]

  void createVolumeCount();
  void createFluxVolumes(const Simulation&);
  void readFluxes(const std::string&);
  void processFluxFiles(const std::vector<std::string>&,
			const std::vector<int>&);

  double calcWeight(const Geometry::Vec3D&) const;
  void normalizeScale();
  void writePoints(const std::string&) const;
  
 public:

  ActivationSource();
  ActivationSource(const ActivationSource&);
  ActivationSource& operator=(const ActivationSource&);
  ActivationSource* clone() const;
  virtual ~ActivationSource();

  /// Set number of output points
  void setNPoints(const size_t N) { nPoints=N; }
  /// set time segment number 
  void setTimeSegment(const size_t T) { timeStep=T+1; }
  void setBox(const Geometry::Vec3D&,const Geometry::Vec3D&);
  /// set scalar
  void setScale(const double S) { externalScale=S; }  
  void setWeightPoint(const Geometry::Vec3D&,const double);
  
  void setPlane(const Geometry::Vec3D&,const Geometry::Vec3D&,
		const double);
  void createOutput(const std::string&);

  void createAll(const Simulation&,const std::string&,
		 const std::string&);
  
  virtual void rotate(const localRotate&);
  virtual void createSource(SDef::Source&) const;
  virtual void writePHITS(std::ostream&) const;
  virtual void write(std::ostream&) const;

};

}

#endif
 
