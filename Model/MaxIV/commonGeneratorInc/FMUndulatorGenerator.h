/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   commonGeneratorInc/FMUndulatorGenerator.h
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
#ifndef setVariable_FMUndulatorGenerator_h
#define setVariable_FMUndulatorGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class FMUndulatorGenerator
  \version 1.0
  \author S. Ansell
  \date July 2020
  \brief FMUndulatorGenerator for variables
*/

class FMUndulatorGenerator 
{
 private:

  double vGap;                     ///< Vertical gap
  
  double length;                   ///< Main length
  double poleWidth;                ///< Block [close] width
  double poleDepth;                ///< Depth of unit

  double magnetWidth;              ///< Block [main] width
  double magnetDepth;              ///< Depth of unit

  double baseDepth;                ///< support thick [z axis]
  double baseExtraLen;             ///< support extra [y axis + length]

  double midWidth;                 ///< Width of mid support 
  double midDepth;                 ///< Depth of mid support
  
  double mainWidth;                ///< stand depth [x]
  double mainDepth;                ///< stand depth [z]

  std::string voidMat;             ///< Void material
  std::string magnetMat;           ///< Block material
  std::string supportMat;          ///< support material
  
  
  
 public:

  FMUndulatorGenerator();
  FMUndulatorGenerator(const FMUndulatorGenerator&);
  FMUndulatorGenerator& operator=(const FMUndulatorGenerator&);
  virtual ~FMUndulatorGenerator();


  virtual void generateUndulator(FuncDataBase&,const std::string&,
				 const double) const;

};

}

#endif
