/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonGeneratorInc/MagnetU1Generator.h
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
#ifndef setVariable_MagnetU1Generator_h
#define setVariable_MagnetU1Generator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MagnetU1Generator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief MagnetU1Generator for variables
*/

class MagnetU1Generator 
{
 private:

  double yOffset;                ///< Offset from straight centre
  double blockYStep;             ///< Step forward
  double length;                 ///< frame length

  double outerVoid;              ///< Size of outer void gap
  double ringVoid;               ///< Size of outer void gap
  double topVoid;                ///< Size of outer void gap
  double baseVoid;               ///< Size of outer void gap

  double baseThick;              ///< base thickness
  double wallThick;              ///< side wall thickness

  std::string voidMat;           ///< void material
  std::string wallMat;           ///< wall material

 public:

  MagnetU1Generator();
  MagnetU1Generator(const MagnetU1Generator&);
  MagnetU1Generator& operator=(const MagnetU1Generator&);
  virtual ~MagnetU1Generator();
  
  void generateComponents(FuncDataBase&,const std::string&) const;
  
  virtual void generateBlock(FuncDataBase&,const std::string&) const;

};

}

#endif
 
