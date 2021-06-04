/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/LBeamStopGenerator.h
 *
 * Copyright (c) 2004-2021 by Konstantin Batkov
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
#ifndef setVariable_LBeamStopGenerator_h
#define setVariable_LBeamStopGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class LBeamStopGenerator
  \version 1.0
  \author Stuart Ansell
  \date November 2020
  \brief LBeamStopGenerator for variables
*/

class LBeamStopGenerator
{
 private:

  double length;                ///< Total length including void

  double innerVoidLen;          ///< inner void lenth
  double innerLength;	        ///< inner length of material
  double innerRadius;	        ///< inner radius

  double midVoidLen;            ///< mid void lenth
  double midLength;             ///< mid length of material
  double midRadius;             ///< mid radius
  int midNLayers;               ///< Number of layers in the mid region

  double outerRadius;           ///< outer radius
  int outerNLayers;             ///< Number of layers in the outer region

  std::string voidMat;                  ///< Void material
  std::string innerMat;                 ///< Inner material
  std::string midMat;                   ///< Mid material
  std::string outerMat;                 ///< Outer material

 public:

  LBeamStopGenerator();
  LBeamStopGenerator(const std::string&);
  LBeamStopGenerator(const LBeamStopGenerator&);
  LBeamStopGenerator& operator=(const LBeamStopGenerator&);
  virtual ~LBeamStopGenerator();

  virtual void generateBStop(FuncDataBase&,const std::string&) const;

};

}

#endif
