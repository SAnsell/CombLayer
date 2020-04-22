/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   Model/MaxIV/LinacInc/DipoleDIBMagGenerator.h
 *
 * Copyright (c) 2004-2020 by Konstantin Batkov
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
#ifndef setVariable_DipoleDIBMagGenerator_h
#define setVariable_DipoleDIBMagGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class DipoleDIBMagGenerator
  \version 1.0
  \author Konstantin Batkov
  \date Apr 2020
  \brief DipoleDIBMagGenerator for variables
*/

class DipoleDIBMagGenerator
{
 private:

  double magOffset;             ///< Magnet offset from pipe centre
  double magHeight;             ///< Magnet thickness
  double magWidth;              ///< Magnet full width
  double magLength;             ///< Magnet full length
  double magCorner;             ///< Magnet corner radius
  double magInnerWidth; ///< Magnet inner width
  double magInnerLength; ///< Magnet inner length
  double frameHeight; ///< Frame height

  std::string voidMat;                  ///< Void material
  std::string coilMat;                  ///< Coil material
  std::string frameMat; ///< Frame material

 public:

  DipoleDIBMagGenerator();
  DipoleDIBMagGenerator(const DipoleDIBMagGenerator&);
  DipoleDIBMagGenerator& operator=(const DipoleDIBMagGenerator&);
  virtual ~DipoleDIBMagGenerator();

  virtual void generate(FuncDataBase&,const std::string&) const;

};

}

#endif
