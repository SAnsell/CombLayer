/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   buildInc/TS1remove.h
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
#ifndef TS1remove_h
#define TS1remove_h

/*!
  \class TS1remove
  \brief Specific class to remove cells for a given geometry
  \author S. Ansell
  \version 1.0
  \date May 2006
*/

class TS1remove : public RemoveCell
{
 private:

  virtual void populateZero(); 

 public:

  TS1remove();
  TS1remove(FuncDataBase&);
  TS1remove(const TS1remove&);
  TS1remove& operator=(const TS1remove&);
  virtual ~TS1remove() {}  ///< Destructor
  /// Clone constructor
  virtual TS1remove* clone() const { return new TS1remove(*this); }

  /// Accessor to class name
  virtual std::string className() const { return "TS1remove"; }

  bool toBeRemoved(const int,MonteCarlo::Qhull&);

};


#endif
