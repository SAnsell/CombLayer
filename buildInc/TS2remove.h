/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/TS2remove.h
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
#ifndef TS2remove_h
#define TS2remove_h

/*!
  \class TS2remove
  \brief Specific class to remove cells for a given geometry
  \author S. Ansell
  \version 1.0
  \date May 2006
*/

class TS2remove : public RemoveCell
{
 private:

  virtual void populateZero(); 
  int spaceReq(const int);

  void sliceReprocess(const int,MonteCarlo::Qhull&);

 public:

  TS2remove();
  TS2remove(FuncDataBase&);
  TS2remove(const TS2remove&);
  TS2remove& operator=(const TS2remove&);
  virtual ~TS2remove() {}   ///< Destructor
  /// Clone constructor
  virtual TS2remove* clone() const { return new TS2remove(*this); }
  /// Accessor to class name
  virtual std::string className() const { return "TS2remove"; }

  bool toBeRemoved(const int,MonteCarlo::Qhull&);

};


#endif
