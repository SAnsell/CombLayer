/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   essInc/LayerGenerator.h
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
#ifndef setVariable_LayerGenerator_h
#define setVariable_LayerGenerator_h

class FuncDataBase;

namespace setVariable
{

/*!
  \struct layerItem
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief Indiviual layer info
*/

struct layerItem
{
  double thick;       ///< thickness [total]
  size_t nDiv;        ///< number of divisions
  std::string mat;    ///< Material

  layerItem(const double,const size_t,const std::string&);
  layerItem(const layerItem&);
  layerItem& operator=(const layerItem&);
  ~layerItem() {}  ///< destructor

};
  
  
/*!
  \class LayerGenerator
  \version 1.0
  \author S. Ansell
  \date May 2016
  \brief LayerGenerator for variables
*/

class LayerGenerator
{
 private:

  size_t nWall;                 ///< number of wall layers
  size_t nRoof;                 ///< number of roof layers
  size_t nFloor;                ///< number of floor layers  

  double wallThick;             ///< wall thickness [if set]
  double roofThick;             ///< roof thickness [if set]
  double floorThick;            ///< floor thickness [if set]
  

  std::vector<layerItem> wall;            ///< wall lengths
  std::vector<layerItem> roof;            ///< roof lenghts
  std::vector<layerItem> floor;           ///< floor lengths

  void setLayers(std::vector<layerItem>&,size_t&,double&,
		 const double,const std::vector<double>&,
		 const std::vector<size_t>&,
		 const std::vector<std::string>&);

  static void processLUnit(FuncDataBase&,const std::string&,
			  const std::vector<layerItem>&);

  void processLayers(FuncDataBase&,const std::string&) const;
  
 public:

  LayerGenerator();
  LayerGenerator(const LayerGenerator&);
  LayerGenerator& operator=(const LayerGenerator&);
  ~LayerGenerator();

  void setWall(const double,
	       const std::vector<double>&,
	       const std::vector<size_t>&, 
	       const std::vector<std::string>&);
  
  void setRoof(const double,
	       const std::vector<double>&,
	       const std::vector<size_t>&, 
	       const std::vector<std::string>&);

  void setFloor(const double,
	       const std::vector<double>&,
	       const std::vector<size_t>&, 
	       const std::vector<std::string>&);

  
  void generateLayer(FuncDataBase&,const std::string&,
		     const double,const size_t)  const;
};

}

#endif
 
