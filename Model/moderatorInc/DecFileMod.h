/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   moderatorInc/DecFileMod.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef moderatorSystem_DecFileMod_h
#define moderatorSystem_DecFileMod_h

class Simulation;

namespace moderatorSystem
{

/*!
  \class DecFileMod
  \version 1.0
  \author S. Ansell
  \date January 2011
  \brief DecFileMod [insert object]
*/

class DecFileMod : public Decoupled
{
 private:

  int incIndex;                     ///< Include index;
  int cellIndex;                    ///< Cell index;

  Geometry::Matrix<double> RBase;  ///< ReBase matrix
  
  void populate(const FuncDataBase&);
  virtual void createUnitVector(const attachSystem::FixedComp&,
				const long int);
  int readFile(Simulation&,const std::string&);
  void reMapSurf(ReadFunc::OTYPE&) const;

 public:

  DecFileMod(const std::string&);
  DecFileMod(const DecFileMod&);
  DecFileMod& operator=(const DecFileMod&);
  /// Clone function
  virtual DecFileMod* clone() const { return new DecFileMod(*this); }
  virtual ~DecFileMod();

  void createAllFromFile(Simulation&,const attachSystem::FixedComp&,
			 const long int,const std::string&);

};

}

#endif
 
