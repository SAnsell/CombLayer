/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   maxpeemInc/GrateMonoBoxGenerator.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef setVariable_GrateMonoBoxGenerator_h
#define setVariable_GrateMonoBoxGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class GrateMonoBoxGenerator
  \version 1.0
  \author S. Ansell
  \date August 2018
  \brief GrateMonoBoxGenerator for variables
*/

class GrateMonoBoxGenerator : public VacBoxGenerator
{
 private:

 
  double overHangExtent;      ///< Edge overhang
  double overHangDepth;       ///< Roof/base overhang
  double roofThick;           ///< Roof thickness [half]
  double baseRadius;          ///< base radius
  double baseThick;           ///< base thick
   
 public:

  GrateMonoBoxGenerator();
  GrateMonoBoxGenerator(const GrateMonoBoxGenerator&);
  GrateMonoBoxGenerator& operator=(const GrateMonoBoxGenerator&);
  virtual ~GrateMonoBoxGenerator();

  void setLid(const double,const double,const double);
  
  virtual void generateBox(FuncDataBase&,const std::string&,
			   const double,const double,const double,
			   const double,const double) const;

};

}

#endif
 
