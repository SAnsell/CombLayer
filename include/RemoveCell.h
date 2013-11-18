/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/RemoveCell.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
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
#ifndef RemoveCell_h
#define RemoveCell_h

/*!
  \class RemoveCell
  \brief Specific class to remove cells for a given geometry
  \author S. Ansell
  \version 1.0
  \date May 2006

  This stores the zero cell instances 
*/

class RemoveCell
{

 protected:

  /// Zero cell storage
  typedef std::multimap<int,std::string> ZCCType;
 
  ZCCType  ZeroCellComp;                        ///< Zero Cell variables 
  FuncDataBase* ConVar;                         ///< The reference data base 

  void substituteSurf(MonteCarlo::Qhull&,const int,const int) const;

  // private:

  /// Strings that if are zero eliminate the cell
  virtual void populateZero() =0; 

 public:

  RemoveCell();
  RemoveCell(FuncDataBase&);
  RemoveCell(const RemoveCell&);
  RemoveCell& operator=(const RemoveCell&);
  virtual ~RemoveCell() {}  ///< Destructor

  ///\cond ABSTRACT
  virtual RemoveCell* clone() const =0;
  virtual std::string className() const =0;
  virtual bool toBeRemoved(const int,MonteCarlo::Qhull&) =0; 
  ///\endcond ABSTRACT
  void reconfigure(FuncDataBase&);

};


#endif
