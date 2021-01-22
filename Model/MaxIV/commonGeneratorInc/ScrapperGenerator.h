/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/ScrapperGenerator.h
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
#ifndef setVariable_ScrapperGenerator_h
#define setVariable_ScrapperGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class ScrapperGenerator
  \version 1.0
  \author S. Ansell
  \date April 2020
  \brief ScrapperGenerator for variables
*/

class ScrapperGenerator
{
 private:

  double radius;                ///< void radius
  double length;                ///< void length [total]
  double wallThick;             ///< pipe thickness

  double flangeRadius;          ///< Joining Flange radius
  double flangeLength;          ///< Joining Flange length

  double tubeAYStep;            ///< Tube Y Step (A)
  double tubeBYStep;            ///< Tube Y Step (B)
  
  double tubeRadius;            ///< Radius of tube
  double tubeLength;            ///< Length/height of tube
  double tubeThick;             ///< Tube thickness
  double tubeFlangeRadius;      ///< Top flange of tube
  double tubeFlangeLength;      ///< length of tube
  
  double scrapperRadius;         ///< Radius of tube
  double scrapperHeight;         ///< Radius of tube

  double driveRadius;           ///< Radius of drive
  double driveFlangeRadius;     ///< Radius of drive flange
  double driveFlangeLength;     ///< Length of drive flange
  double supportRadius;         ///< Radius of drive support
  double supportThick;          ///< Thickness of support wall
  double supportHeight;         ///< Height of support

  double topBoxWidth;           ///< top box width
  double topBoxHeight;          ///< top box height
  
  std::string voidMat;                  ///< void material
  std::string tubeMat;                  ///< main tube material
  std::string flangeMat;                ///< flange material
  std::string scrapperMat;               ///< scrapper material
  std::string driveMat;                 ///< drive pipe material
  std::string topMat;                   ///< top control material

 public:

  ScrapperGenerator();
  ScrapperGenerator(const ScrapperGenerator&);
  ScrapperGenerator& operator=(const ScrapperGenerator&);
  virtual ~ScrapperGenerator();

  template<typename T> void setCF();

  void generateScrapper(FuncDataBase&,const std::string&,
			const double) const;

};

}

#endif
