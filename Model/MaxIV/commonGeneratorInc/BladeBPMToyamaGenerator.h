/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonBeamInc/BladeBPMToyamaGenerator.h
 *
 * Copyright (c) 2004-2026 by Konstantin Batkov
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
#ifndef setVariable_BladeBPMToyamaGenerator_h
#define setVariable_BladeBPMToyamaGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BladeBPMToyamaGenerator
  \version 1.0
  \author Konstantin Batkov
  \date February 2026
  \brief BladeBPMToyamaGenerator for variables
*/

class BladeBPMToyamaGenerator
{
 private:

  double length;                ///< Total length including XBPM insert and vacuum chamber
  double chamberLength;         ///< Vacuum chamber length including flanges
  double chamberRadius;         ///< Vacuum chamber radius (inner)
  double chamberWallThick;      ///< Vacuum chamber wall thickness
  double chamberFlangeRadius;   ///< Vacuum chamber flange radius
  double chamberFlangeLength;   ///< Vacuum chamber flange length
  double insertFlangeRadius;    ///< Blades insert flange radius
  double insertFlangeLength;    ///< Blades insert flange length
  double insertInnerRadius;     ///< Blades insert inner radius
  double insertOuterRadius;     ///< Blades insert outer radius
  double insertPreOuterRadius;  ///< Outer radius of the insert part containing blades (in the vacuum chamber)
  double insertLength;          ///< Blades insert length
  double portLength;            ///< Feedthrough port length (distance between beam axis and flange upper plane, without its cap)
  double portWallThick;         ///< Feedthrough port wall thickness
  double portRadius;            ///< Feedthrough port inner radius
  double portCapLength;         ///< Feedthrough port cap length
  double portCapCentralLength;  ///< Feedthrough port cap central part length
  double portCapCentralRadius;  ///< Feedthrough port cap central part radius
  double portFlangeLength;      ///< Feedthrough port flange length
  double portFlangeRadius;      ///< Feedthrough port flange radius

  std::string chamberFlangeMat; ///< Vacuum chamber flange material
  std::string chamberWallMat;   ///< Vacuum chamber wall material
  std::string insertMat;        ///< Blades insert material
  std::string insertPreMat;     ///< Homogenised material of the insert part with blades
  std::string insertFlangeMat;  ///< Blades insert flange material
  std::string voidMat;          ///< Void material
  std::string airMat;           ///< Air material

 public:

  BladeBPMToyamaGenerator();
  BladeBPMToyamaGenerator(const BladeBPMToyamaGenerator&);
  BladeBPMToyamaGenerator& operator=(const BladeBPMToyamaGenerator&);
  virtual ~BladeBPMToyamaGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
