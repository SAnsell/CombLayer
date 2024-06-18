/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/GunTestFacilityInc/RFGunGenerator.h
 *
 * Copyright (c) 2004-2023 by Konstantin Batkov
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
#ifndef setVariable_RFGunGenerator_h
#define setVariable_RFGunGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class RFGunGenerator
  \version 1.0
  \author Konstantin Batkov
  \date October 2023
  \brief RFGunGenerator for variables
*/

class RFGunGenerator
{
 private:

  double length;                ///< Total length including void
  double frontTubeLength;        ///< Front tube length
  double frontTubeRadius;        ///< Front tube radius
  double frontTubeFlangeLength;  ///< Front tube flange length
  double frontTubeFlangeRadius;  ///< Front tube flange radius
  double frontTubePipeThick;     ///< Front tube thickness
  double backTubeRadius;       ///< Back tube radius
  double backTubeFlangeLength; ///< Back tube flange length
  double backTubeFlangeRadius; ///< Back tube flange radius
  double backTubePipeThick;    ///< Back tube thickness
  double cavityRadius;          ///< CavityRadius
  double cavityLength;          ///< Cavity length
  double cavityOffset;          ///< Main cavity offset
  double irisThick;             ///< Iris thickness
  double irisRadius;            ///< Cavity iris radius
  double irisStretch;           ///< Cavity iris stretching
  double wallThick;             ///< Wall thickness
  size_t nFrameFacets;          ///< Number of frame facets
  double frameWidth;            ///< Frame width
  double frontPreFlangeThick;   ///< Front pre flange outer radius (smaller)
  double frontPreFlangeRadius;  ///< Front pre flange outer radius (smaller)
  double frontFlangeThick;      ///< Front flange thickness
  double frontFlangeRadius;     ///< Front flange outer radius (bigger)
  double insertWidth;           ///< Insert width
  double insertLength;          ///< Insert length
  double insertDepth;           ///< Insert depth
  double insertWallThick;       ///< Insert wall thick
  double insertCut;             ///< Insert cut width

  std::string mainMat;          ///< Main material
  std::string wallMat;          ///< Wall material
  std::string frontTubePipeMat; ///< Front tube material
  std::string backTubePipeMat;  ///< Back tube material

 public:

  RFGunGenerator();
  RFGunGenerator(const RFGunGenerator&);
  RFGunGenerator& operator=(const RFGunGenerator&);
  virtual ~RFGunGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
