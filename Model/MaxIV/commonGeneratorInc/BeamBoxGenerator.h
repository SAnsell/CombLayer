/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/LinacInc/BeamBoxGenerator.h
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
#ifndef setVariable_BeamBoxGenerator_h
#define setVariable_BeamBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class BeamBoxGenerator
  \version 1.0
  \author Stuart Ansell
  \date June 2020
  \brief BeamBoxGenerator for variables
*/

class BeamBoxGenerator
{
 private:
      
  double width;             ///< Full Width
  double height;            ///< Full Height
  double length;            ///< Full Depth
  double fullCut;           ///< Full outer cut
  double innerCut;          ///< Inner cut thickness
  double backThick;         ///< back thickness
  double b4cThick;          ///< extra b4c layer thickness
  double backExtension;     ///< back extention
  double wallThick;         ///< wall thickness (roof/wall/base)

  std::string innerMat;     ///< Material in innerVoid [def: void]
  std::string backMat;      ///< Material for back wall
  std::string mainMat;      ///< Material for walls/roof
  std::string b4cMat;      ///< Material for b4c liner

 public:

  BeamBoxGenerator();
  BeamBoxGenerator(const BeamBoxGenerator&);
  BeamBoxGenerator& operator=(const BeamBoxGenerator&);
  virtual ~BeamBoxGenerator();

  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double) const;

};

}

#endif
