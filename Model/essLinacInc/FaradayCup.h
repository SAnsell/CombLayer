/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/FaradayCup.h
 *
 * Copyright (c) 2004-2017 by Konstantin Batkov
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
#ifndef essSystem_FaradayCup_h
#define essSystem_FaradayCup_h

class Simulation;

namespace essSystem
{

/*!
  \class FaradayCup
  \version 1.0
  \author Konstantin Batkov
  \date 14 Mar 2017
  \brief Faraday Cup - based on SNS design
*/

class FaradayCup : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int active; ///< On/Off switch
  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double outerRadius;           ///< Outer radius (d2/2)
  double innerRadius;           ///< Inner radius (d1/2)

  double faceLength;             ///< Collimator length (face plate)
  double faceRadius; ///< Collimator inner radius

  double absLength; ///< Absorber length
  int absMat; ///< Absorber material
  double baseLength; ///< Base length (e1)

  double colLength;             ///< Collector length
  int colMat;                   ///< collector material

  int wallMat;                   ///< wall material
  int airMat;                    ///< air material

  double shieldRadius;          ///< shield radius
  double shieldInnerRadius;     ///< shielding inner radius
  double shieldLength;          ///< shield length
  double shieldInnerLength;     ///< shielding inner length
  int shieldMat;                ///< shielding material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  FaradayCup(const std::string&);
  FaradayCup(const FaradayCup&);
  FaradayCup& operator=(const FaradayCup&);
  virtual FaradayCup* clone() const;
  virtual ~FaradayCup();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


