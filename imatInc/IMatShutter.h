/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   imatInc/IMatShutter.h
 *
 * Copyright (c) 2004-2014 by Stuart Ansell
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
#ifndef IMatShutter_h
#define IMatShutter_h

class Simulation;

/*!
  \namespace IMatShutter Systemx
  \brief Add hutches to the model
  \author S. Ansell
  \version 1.0
  \date May 2012
*/

namespace shutterSystem
{
  class shutterInfo;


/*!
  \class IMatShutter
  \author S. Ansell
  \version 1.2
  \date July 2012
  \brief Hold the shutter for Ima
*/

class IMatShutter : public GeneralShutter
{
 private:

  std::string imatKey;              ///< Key Name
  int insIndex;                     ///< Insert number
  int cellIndex;                    ///< Cell index [masks generalshutter]
  
  double xStep;                     ///< Displacement of centre line
  double zStep;                     ///< Displacement of centre line
  double xyAngle;                   ///< XY-Axis rotation angle
  double zAngle;                    ///< Z-Axis rotation angle

  double width;                     ///< front width
  double height;                    ///< back height

  double innerThick;                ///< Inner wall thickness
  int innerMat;                     ///< Inner glass material

  double supportThick;              ///< Inner Wall support
  int supportMat;                   ///< Steel support

  double gapThick;                  ///< gap layer

  double maskHeight;                ///< Hole in mask
  double maskWidth;                 ///< Hole in mask
  double maskThick;                 ///< Front mask thickness
  int maskMat;                      ///< Front mask material

  //--------------
  // FUNCTIONS:
  //--------------
  void populate(const Simulation&);
  void createUnitVector();
  void createInsert(Simulation&);  
  void createObjects(Simulation&);  
  void createSurfaces();  
  void createLinks();

  double processShutterDrop() const;

 public:

  IMatShutter(const size_t,const std::string&,const std::string&);
  IMatShutter(const IMatShutter&);
  IMatShutter& operator=(const IMatShutter&);
  virtual ~IMatShutter();

  Geometry::Vec3D getExitTrack() const;
  Geometry::Vec3D getExitPoint() const;
  virtual void createAll(Simulation&,const double,
			 const attachSystem::FixedComp*);

};

}

#endif
 
