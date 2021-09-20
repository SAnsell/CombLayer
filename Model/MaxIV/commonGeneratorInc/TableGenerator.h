/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   commonBeamInc/TableGenerator.h
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
#ifndef setVariable_TableGenerator_h
#define setVariable_TableGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \class TableGenerator
  \version 1.0
  \author S. Ansell
  \date September 2021
  \brief TableGenerator for variables
*/

class TableGenerator
{
 private:


  double thick;            ///< Thickness of table
  double width;            ///< Width of table [full]
  double legSize;          ///< Size of square leg

  double clearance;        ///< Clearance


  std::string plateMat;         ///< plate Material
  std::string legMat;           ///< leg Material
  std::string voidMat;          ///< void Material
    
 public:

  TableGenerator();
  TableGenerator(const TableGenerator&);
  TableGenerator& operator=(const TableGenerator&);
  ~TableGenerator();

  void generateTable(FuncDataBase&,const std::string&,
		     const double,const double) const;

};

}

#endif
 
