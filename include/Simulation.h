/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   include/Simulation.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef Simulation_h
#define Simulation_h

class HeadRule;

namespace Geometry
{
  class Transform;
}

namespace tallySystem
{
  class Tally;
  class sswTally;
}

namespace Surface 
{
  class Surface;
}

namespace ModelSupport
{
  class ObjSurfMap;
}

namespace WeightSystem
{
  class WeightMesh;
  class WeightControl;
}

namespace MonteCarlo
{
  class Object;
  class Material;
  class Qhull;
}

/*!
  \class Simulation
  \version 1.3
  \date Dec 2005
  \author S.Ansell
  \brief Containes all the information on from the simulations.

  Contains the running simulation information 
  Mainly list of maps and process information
  Is expected to be mainly a singleton class.
*/

class Simulation
{
 protected:

  typedef std::map<int,Geometry::Transform> TransTYPE; ///< Transform type
 
 public:

  // UGLY
  typedef std::map<int,MonteCarlo::Qhull*> OTYPE;      ///< Object type

 protected:

  std::string inputFile;                ///< Input file
  std::string cmdLine;                  ///< Command line : historical recall 
  
  FuncDataBase DB;                      ///< DataBase of variables
  ModelSupport::ObjSurfMap* OSMPtr;     ///< Object surface map [if required]

  TransTYPE TList;                      ///< Transforms List (key=Transform)

  size_t cellDNF;                       ///< max size to convert into DNF
  size_t cellCNF;                       ///< max size to convert into CNF
  OTYPE OList;   ///< List of objects  (allow to become hulls)
  std::vector<int> cellOutOrder;        ///< List of cells [output order]
  std::set<int> voidCells;              ///< List of void cells

  std::string sourceName;               ///< Source name
  
  // METHODS:

  void deleteObjects();
  

  int checkInsert(const MonteCarlo::Qhull&);       ///< Inserts (and test) new hull into Olist map 
  int removeNullSurfaces();
  int removeComplement(MonteCarlo::Qhull&) const;
  void addObjSurfMap(MonteCarlo::Qhull*);

  std::map<int,int> calcCellRenumber(const std::vector<int>&,
				     const std::vector<int>&) const;

 public:

  Simulation();
  Simulation(const Simulation&);
  Simulation& operator=(const Simulation&);
  virtual ~Simulation();
  
  /// set the command line
  void setCmdLine(const std::string& S) { cmdLine=S; }
  virtual void resetAll();
  int applyTransforms();
  
  int isValidCell(const int,const Geometry::Vec3D&) const;

  /// set cell DNF
  void setCellDNF(const size_t C) { cellDNF=C; }
  /// set cell CNF
  void setCellCNF(const size_t C) { cellCNF=C; }
  MonteCarlo::Qhull* findQhull(const int);         
  const MonteCarlo::Qhull* findQhull(const int) const; 
  MonteCarlo::Object* findCell(const Geometry::Vec3D&,
			       MonteCarlo::Object*) const;
  std::pair<const MonteCarlo::Object*,const MonteCarlo::Object*>
    findCellPair(const Geometry::Vec3D&,const int) const;
  
  int findCellNumber(const Geometry::Vec3D&,const int) const;  

  int existCell(const int) const;              ///< check if cell exist
  int getCellMaterial(const int) const;        ///< return cell material
  int setMaterialDensity(OTYPE&);
  int setMaterialDensity(const int);

  /// Gets the data base
  FuncDataBase& getDataBase() { return DB; }  
  /// Gets the data base
  const FuncDataBase& getDataBase() const { return DB; }

  /// set Source name
  void setSourceName(const std::string&);

  const OTYPE& getCells() const { return OList; } ///< Get cells(const)
  OTYPE& getCells() { return OList; } ///< Get cells

  int removeComplements(); 

  int populateCells();  
  int populateCells(const std::vector<int>&);  

  int calcVertex(const int); 
  void calcAllVertex();
  
  void masterRotation();
  void masterSourceRotation();

  int getNextCell(int) const;
  // ADD Objects
  int addCell(const MonteCarlo::Qhull&);         
  int addCell(const int,const MonteCarlo::Qhull&);         
  int addCell(const int,const int,const std::string&);
  int addCell(const int,const int,const double,const std::string&);
  int addCell(const int,const int,const double,const HeadRule&);

  // LIST Stuff

  void reZeroFromVertex(const int,const unsigned int,const unsigned int,
			const unsigned int,const unsigned int,Geometry::Vec3D&,
			Geometry::Matrix<double>&);

  /// Get values

  std::vector<int> getCellVector() const;
  std::vector<int> getCellTempRange(const double,const double) const;
  std::vector<int> getCellInputVector() const;
  std::vector<int> getNonVoidCellVector() const;
  std::vector<int> getCellVectorRange(const int,const int) const;
  std::vector<int> getCellWithMaterial(const int) const;
  std::vector<int> getCellWithZaid(const size_t) const;

  std::vector<std::pair<int,int>> getCellImp() const;            

  int removeDeadSurfaces(const int); 
  virtual void removeCell(const int);
  int removeAllSurface(const int);

  void voidObject(const std::string&);
  void updateSurface(const int,const std::string&);

  void createObjSurfMap();
  void validateObjSurfMap();
  /// Access surface map
  const ModelSupport::ObjSurfMap* getOSM() const;

  // Tally processing

  virtual void setEnergy(const double);
  void setENDF7();
  
  void renumberAll();
  void renumberSurfaces(const std::vector<int>&,
			const std::vector<int>&);
  int splitObject(const int,const int);
  void minimizeObject(const int);
  void makeObjectsDNForCNF();
  virtual void prepareWrite();

  virtual void substituteAllSurface(const int,const int);
  virtual std::map<int,int> renumberCells(const std::vector<int>&,
					  const std::vector<int>&);
  /// no-op call
  virtual void writeCinder() const {}

  void writeVariables(std::ostream&,const char ='c') const;
  virtual void write(const std::string&) const =0;  
    
  // Debug stuff
  
  void printVertex(const int) const;

};

#endif
