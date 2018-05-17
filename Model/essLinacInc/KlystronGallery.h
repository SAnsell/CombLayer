/*********************************************************************
  CombLayer : MCNP(X) Input builder

 * File:   essBuildInc/KlystronGallery.h
 *
 * Copyright (c) 2018 by Konstantin Batkov
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
#ifndef essSystem_KlystronGallery_h
#define essSystem_KlystronGallery_h

class Simulation;

namespace essSystem
{

/*!
  \class KlystronGallery
  \version 1.0
  \author Konstantin Batkov
  \date 17 May 2018
  \brief Klystron gallery
*/

class KlystronGallery : public attachSystem::ContainedComp,
  public attachSystem::FixedOffset
{
 private:

  const int surfIndex;             ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int engActive;                ///< Engineering active flag

  double lengthBack;            ///< length backward the proton beam direction
  double lengthFront;           ///< length forward to the proton beam direction
  double widthLeft;             ///< Inner width towards x+
  double widthRight;            ///< Inner width towards x-
  double height;                ///< height

  double wallThick;             ///< Thickness of wall

  int mainMat;                   ///< main material
  int wallMat;                   ///< wall material

  void populate(const FuncDataBase&);
  void createUnitVector(const attachSystem::FixedComp&,
			const long int);

  void createSurfaces();
  void createObjects(Simulation&);
  void createLinks();

 public:

  KlystronGallery(const std::string&);
  KlystronGallery(const KlystronGallery&);
  KlystronGallery& operator=(const KlystronGallery&);
  virtual KlystronGallery* clone() const;
  virtual ~KlystronGallery();

  void createAll(Simulation&,const attachSystem::FixedComp&,const long int);

};

}

#endif


