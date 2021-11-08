/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:  commonGeneratorInc/EPCombineGenerator.h
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
#ifndef setVariable_EPContinueGenerator_h
#define setVariable_EPContinueGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class EPContinueGenerator
  \version 1.0
  \author S. Ansell
  \date November 2021
  \brief EPContinueGenerator for variables
*/

class EPContinueGenerator 
{
 private:

  double length;                  ///< frame length

  double electronRadius;          ///< Electron radius
  
  double photonAGap;              ///< Photon radius at start [5mm]
  double photonBGap;              ///< Photon radius at end [14mm]
  double photonStep;              ///< Step at exit of photon path [10mm]
  
  double photonWidth;             ///< Extension from photon side
  double height;                  ///< Main height

  double outerRadius;             ///< outer
  
  std::string voidMat;                    ///< void material
  std::string wallMat;                    ///< wall material

 public:

  EPContinueGenerator();
  EPContinueGenerator(const EPContinueGenerator&);
  EPContinueGenerator& operator=(const EPContinueGenerator&);
  virtual ~EPContinueGenerator();
  
  virtual void generatePipe(FuncDataBase&,const std::string&) const;

};

}

#endif
 
