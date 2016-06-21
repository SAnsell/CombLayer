/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonVarInc/FocusGenerator.h
 *
 * Copyright (c) 2004-2016 by Stuart Ansell
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
#ifndef essSystem_FocusGenerator_h
#define essSystem_FocusGenerator_h

class Simulation;

namespace setVariable
{

/*!
  \class FocusGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief FocusGenerator for variables

  Part of the simiplification process to produce a simpler variable 
  initialisation.
*/

class FocusGenerator
{
 private:

  
  double substrateThick;   ///< substrate thickness
  double voidThick;        ///< clearance thickness
  bool yStepActive;        ///< Y step active
  double yStep;            ///< Y step
  
  std::string guideMat;    ///< Main guide material

  
 public:

  FocusGenerator();
  FocusGenerator(const FocusGenerator&);
  FocusGenerator& operator=(const FocusGenerator&);
  ~FocusGenerator();

  /// set guide material
  void setGuideMat(const std::string& matName)
  { guideMat=matName; }
  /// set thickness
  void setThickness(const double S,const double V)
  { substrateThick=S;voidThick=V; }

  /// set the y step
  void setYOffset(const double D)
  { yStep=D; yStepActive=1; }
  /// clear the y step
  void clearYOffset()
  { yStepActive=0; }
  
  void generateRectangle(FuncDataBase&,
			 const std::string&,const double,
			 const double,const double) const;

  void generateTaper(FuncDataBase&,
		     const std::string&,const double,
		     const double,const double,
		     const double,const double) const;

  void generateBender(FuncDataBase&,
		      const std::string&,const double,
		      const double,const double,
		      const double,const double,
		      const double,const double) const;
  

  
};

}

#endif
 
