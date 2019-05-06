/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/MagnetM1Generator.h
 *
 * Copyright (c) 2004-2019 by Stuart Ansell
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
#ifndef setVariable_MagnetM1Generator_h
#define setVariable_MagnetM1Generator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class MagnetM1Generator
  \version 1.0
  \author S. Ansell
  \date May 2018
  \brief MagnetM1Generator for variables
*/

class MagnetM1Generator 
{
 private:

  double blockYStep;            ///< Step forward
  double length;                ///< frame length

  double outerVoid;             ///< Size of outer void gap
  double ringVoid;             ///< Size of outer void gap
  double topVoid;             ///< Size of outer void gap
  double baseVoid;             ///< Size of outer void gap

  double baseThick;              ///< base thickness
  double wallThick;              ///< side wall thickness


  std::string voidMat;                    ///< void material
  std::string wallMat;                    ///< wall material

 public:

  MagnetM1Generator();
  MagnetM1Generator(const MagnetM1Generator&);
  MagnetM1Generator& operator=(const MagnetM1Generator&);
  virtual ~MagnetM1Generator();
  
  
  virtual void generateBlock(FuncDataBase&,const std::string&) const;

};

}

#endif
 
