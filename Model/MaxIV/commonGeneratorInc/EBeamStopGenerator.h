/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/EBeamStopGenerator.h
 *
 * Copyright (c) 2004-2020 by Stuart Ansell
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
#ifndef setVariable_EBeamStopGenerator_h
#define setVariable_EBeamStopGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class EBeamStopGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief EBeamStopGenerator for variables
*/

class EBeamStopGenerator
{
 private:

  double width;                 ///< void radius
  double length;                ///< void length [total]
  double height;                ///< void height
  double depth;                 ///< void depth

  double wallThick;             /// main wall thickness
  double baseThick;             /// Base thickness

  double baseFlangeExtra;       ///< base outstep
  double baseFlangeThick;       ///< base vertical rise

  double portLength;            ///< Port length 
  double portRadius;            ///< Port radius
  double portWallThick;         ///< Port wall thickness
  double portFlangeRadius;      ///< Port flange radius
  double portFlangeLength;      ///< Port flange length
  
  double stopRadius;            ///< Main stop radius
  double stopLength;            ///< Main stop length
  double stopZLift;             ///< Lift of support if open

  std::vector<double> stopLen;   ///< Stop material lengths

  double supportConeLen;        ///< Support close cone
  double supportConeRadius;     ///< Support radius at base
  double supportHoleRadius;     ///< Hole in support radius
  double supportRadius;         ///< Radius of support tube 

  double stopPortYStep;          ///< Support centre YStep  
  double stopPortRadius;         ///< stop port radius
  double stopPortLength;         ///< stop port total length
  double stopPortWallThick;      ///< stop port wall radius
  double stopPortDepth;         ///< stop port depth
  double stopPortFlangeRadius;   /// flange radius
  double stopPortFlangeLength;   /// flange length
  double stopPortPlateThick;     /// coverplate thick

  double ionPortYStep;          ///< Y step
  double ionPortRadius;         ///< ion pump port radius
  double ionPortLength;         ///< ion pump total length
  double ionPortWallThick;      ///< ion pump port wall thick
  double ionPortFlangeRadius;   /// flange radius
  double ionPortFlangeLength;   /// flange length
  double ionPortPlateThick;     /// coverplate thick
  
  std::vector<int> stopMat;     ///< Stop materials 

  std::string voidMat;                  ///< void material
  std::string wallMat;                  ///< wallmaterial
  std::string flangeMat;                ///< flange material
  std::string supportMat;               ///< support material
  std::string plateMat;                 ///< end plate material
  std::string outerMat;                 ///< pipe material


 public:

  EBeamStopGenerator();
  EBeamStopGenerator(const EBeamStopGenerator&);
  EBeamStopGenerator& operator=(const EBeamStopGenerator&);
  virtual ~EBeamStopGenerator();

  virtual void generateEBeamStop(FuncDataBase&,const std::string&,
				 const bool) const;

};

}

#endif
