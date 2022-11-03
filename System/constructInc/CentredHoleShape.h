/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   constructInc/CentredHoleShape.h
 *
 * Copyright (c) 2004-2022 by Stuart Ansell
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
#ifndef constructSystem_CentredHoleShape_h
#define constructSystem_CentredHoleShape_h

class Simulation;

namespace constructSystem
{
  /*!
    \class CentredHoleShape
    \version 1.0
    \author S. Ansell
    \date November 2022
    \brief Variable detemine hole type for a rotation centred system
  */
  
class CentredHoleShape : 
  public HoleShape
{
 private:

  double masterAngle;           ///< Main master rotation
  double angleCentre;           ///< Mid point for alignment [for interigation]
  double angleOffset;           ///< Rotation around centre point
  double radialStep;            ///< Centre radial position
  
  Geometry::Vec3D rotCentre;       ///< Centre position
  double rotAngle;                 ///< Angle of whole system [true pos]

  virtual void createUnitVector(const attachSystem::FixedComp&,
			const long int);
  
 public:
  
  CentredHoleShape(const std::string&);
  CentredHoleShape(const CentredHoleShape&);
  CentredHoleShape& operator=(const CentredHoleShape&);
  virtual ~CentredHoleShape() {}

  void setMasterAngle(const double);
  /// accessor to central angle
  double getCentreAngle() const { return angleCentre; }

  virtual void populate(const FuncDataBase&);
  
};

}

#endif
 
