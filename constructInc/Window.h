/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   constructInc/Window.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef constructSystem_Window_h
#define constructSystem_Window_h

class Simulation;


namespace constructSystem
{

/*!
  \class Window
  \version 1.0
  \author S. Ansell
  \date May 2012
  \brief TS1 t1Reflector [insert object]
*/

class Window : public attachSystem::ContainedComp,
    public attachSystem::FixedComp
{
 private:
 
  const int winIndex;           ///< Index of surface offset
  int cellIndex;                ///< Cell index

  int baseCell;                 ///< Cell that it resides

  Geometry::Vec3D Centre;       ///< Centre point of view
  Geometry::Vec3D WAxis;        ///< Window axis
  int fSign;                    ///< Surface sign
  int bSign;                    ///< Surface sign
  const Geometry::Surface* FSurf;     ///< Front surface
  const Geometry::Surface* BSurf;     ///< Back surface
  int divideFlag;               ///< Dividing plane required

  double width;                 ///< full width
  double height;                ///< full height
  size_t nLayers;               ///< number of extra layers

  std::vector<double> layerThick;   ///< Layer thickness [nlayer-1]
  std::vector<int> layerMat;        ///< Material number [nlayer]
  std::vector<int> layerSurf;       ///< Surface layres
  
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::FixedComp&);

  void createCentre(Simulation&);
  void createSurfaces();
  void createObjects(Simulation&);


 public:

  Window(const std::string&);
  Window(const Window&);
  Window& operator=(const Window&);
  ~Window();

  std::string getComposite(const std::string&) const;

  void setSize(const double,const double);
  void setCentre(const Geometry::Vec3D&,const Geometry::Vec3D&);
  void setBaseCell(const int);
  
  void createAll(Simulation&,
		 const attachSystem::FixedComp&);
};

}

#endif
 
