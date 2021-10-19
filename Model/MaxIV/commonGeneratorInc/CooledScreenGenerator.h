/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/commonGeneratorInc/CooledScreenGenerator.h
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
#ifndef setVariable_CooledScreenGenerator_h
#define setVariable_CooledScreenGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class CooledScreenGenerator
  \version 1.0
  \author Konstantin Batkov
  \date May 2020
  \brief CooledScreenGenerator for variables
*/

class CooledScreenGenerator :
    public ScreenGenerator
{
 private:

  // volume containing Yag screen + mirror


  double screenAngle;           ///< screen inclination angle [deg]
  double copperWidth;           ///< in-beam copper block width
  double copperHeight;          ///< in-beam copper block height 
  double copperThick;           ///< Thickness of in-beam cooper

  double innerRadius;           ///< circle radius of beam
  double screenThick;           ///< screen thickness

  std::string copperMat;             ///< mirror  material
  std::string screenMat;             ///< mirror holder material

 public:

  CooledScreenGenerator();
  CooledScreenGenerator(const CooledScreenGenerator&);
  CooledScreenGenerator& operator=(const CooledScreenGenerator&);
  virtual ~CooledScreenGenerator();


  virtual void generateScreen(FuncDataBase&,const std::string&,
			      const bool) const;
  
};

}

#endif
