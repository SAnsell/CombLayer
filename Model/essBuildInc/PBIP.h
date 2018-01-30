/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/PBIP.h
 *
 * Copyright (c) 2017 by Konstantin Batkov
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
#ifndef essSystem_PBIP_h
#define essSystem_PBIP_h

class Simulation;

namespace essSystem
{

/*!
  \class PBIP
  \version 1.0
  \author Konstantin Batkov
  \date 7 Feb 2017
  \brief Proton beam instrumentation plug
*/

class PBIP : public attachSystem::ContainedGroup,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  double length;                ///< Total length including void
  double width;                 ///< Width
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  double pipeBeforeHeight; ///< proton beam pipe height before PBIP
  double pipeBeforeWidthLeft; ///< left width of proton pipe before BPIP
  double pipeBeforeAngleLeft; ///< tilting angle of the left plane of the proton pipe before PBIP
  double pipeBeforeWidthRight; ///< right width of proton pipe before BPIP
  double pipeAfterHeight; ///< proton beam pipe height after PBIP
  double pipeAfterWidthLeft; ///< left width of proton pipe after BPIP
  double pipeAfterWidthRight; ///< right width of proton pipe after BPIP
  double pipeAfterAngleRight; ///< tilting angle of the right plane of the proton pipe after PBIP
  double foilOffset; ///< foil Y-offset
  double foilThick; ///< foil thickness
  int foilMat; ///< foil material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createLinks();
  void createObjects(Simulation&,
		     const attachSystem::FixedComp&,const long int,
		     const attachSystem::FixedComp&,const long int);


 public:

  PBIP(const std::string&);
  PBIP(const PBIP&);
  PBIP& operator=(const PBIP&);
  virtual PBIP* clone() const;
  virtual ~PBIP();

  void createAll(Simulation&,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int,
		 const attachSystem::FixedComp&,const long int);

};

}

#endif


