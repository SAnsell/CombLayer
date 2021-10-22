/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/BeamScrapperGenerator.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef setVariable_BeamScrapperGenerator_h
#define setVariable_BeamScrapperGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BeamScrapperGenerator
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief BeamScrapperGenerator for variables
*/

class BeamScrapperGenerator 
{
 private:

  double tubeRadius;        ///< inner water radius
  double tubeOffset;        ///< +/- beam offset of each pipe
  double tubeWall;          ///< wall thickness of pip

  double plateOffset;       ///< Scren plate offset relative to beam
  double plateYStep;        ///< Screen plate relative ot pipe centre
  double plateAngle;        ///< Z rotation of screen (y is tube dir)
  double plateLength;       ///< Length along beam (angled)
  double plateHeight;       ///< Height of plate
  double plateThick;        ///< Thickness of plate

  double tubeWidth;         ///< horrizontal exten of final cooling loop
  double tubeHeight;        ///< vertical exten of final cooling loop

  double inletZOffset;      ///< Variable for Z displacement of pipe inlet
  
  std::string voidMat;      ///< void material
  std::string waterMat;     ///< coolant material
  std::string copperMat;    ///< pipe outer  material


 public:

  BeamScrapperGenerator();
  BeamScrapperGenerator(const BeamScrapperGenerator&);
  BeamScrapperGenerator& operator=(const BeamScrapperGenerator&);
  virtual ~BeamScrapperGenerator();


  virtual void generateScreen(FuncDataBase&,const std::string&) const;
  
};

}

#endif
