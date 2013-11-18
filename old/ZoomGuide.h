/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/ZoomGuide.h
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
#ifndef ZoomGuide_h
#define ZoomGuide_h

class Simulation;

namespace shutterSytem
{
  class shutterInfo;
  class GeneralShutter;
}

/*!
  \namespace zoomSystem
  \brief Adds Zoom componente
  \author S. Ansell
  \version 1.0
  \date March 2010
*/


namespace zoomSystem
{
/*!
  \class ZoomGuide
  \version 1.0
  \author S. Ansell
  \date January 2010
  \brief Adds the ZoomGuide [collimator]
*/

class ZoomGuide : public attachSystem::LinearComp
{
 private:
  
  const int guideIndex;         ///< Index of surface offset
  const std::string keyName;    ///< Key name
  int cellIndex;                ///< Cell index
  int populated;                ///< populated or not

  double guideLength;            ///< Box Length

  double AUp;               ///< Top roof height
  double ADown;             ///< Base (floor)
  double ALeft;             ///< Left wall
  double ARight;            ///< Right wall

  double BUp;               ///< Top roof height
  double BDown;             ///< Base (floor)
  double BLeft;             ///< Left wall
  double BRight;            ///< Right wall

  double midUp;               ///< Top roof height
  double midDown;             ///< Base (floor)
  double midLeft;             ///< Left wall
  double midRight;            ///< Right wall

  double roofTotal;         ///< Roof thickness
  double floorTotal;        ///< Floor Thickness
  double leftTotal;         ///< Left thickness [pre-straight]
  double rightTotal;        ///< Right thickness [pre-straight]
  
  double cutterLen;         ///< Cutter length
  double focusLen;          ///< focus Length

  int innerCollMat;        ///< Material inner stuff
  int shieldMat;           ///< Material for roof

  // Inner split
  int nInsert;                   ///< number of inserts
  int midBoxCell;                ///< Mid box to cut
  std::vector<double> innerFrac; ///< inner Layer thicknesss (fractions)
  std::vector<int> innerMat;     ///< inner Layer materials

  // Outer SHIELDING
  int nLayers;                 ///< number of layers
  int outerBoxCell;            ///< Outer box to cut
  std::vector<double> outerFrac; ///< guide Layer thicknesss (fractions)
  std::vector<int> outerMat; ///< guide Layer materials

  std::vector<int> Vlist;   ///< Virtual List for exclusion
  
  void populateInsert(const FuncDataBase& Control);
  void populate(const Simulation&);
  void createUnitVector(const attachSystem::LinearComp&);
  
  void createSurfaces();
  void createObjects(Simulation&);
  void addOuterVoid(Simulation&);
  void layerProcess(Simulation&);
    
 public:

  ZoomGuide(const std::string&);
  ZoomGuide(const ZoomGuide&);
  ZoomGuide& operator=(const ZoomGuide&);
  ~ZoomGuide();

  int exitWindow(const double,std::vector<int>&,
		 Geometry::Vec3D&) const;
  void createAll(Simulation&,const attachSystem::LinearComp&);

};

}

#endif
 
