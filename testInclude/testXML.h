/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   testInclude/testXML.h
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
#ifndef testXML_h
#define testXML_h 

/*!
  \class testXML 
  \brief test of XML write out control
  \version 1.0
  \date January 2006
  \author S.Ansell
  
  Test the simple XML formating

*/

class testXML 
{
private:

  XML::XMLcollect XOut;              ///< Collection for build/retrieval


  void buildXML();
  const std::string& buildXMLstring() const;   

  void populateString(std::string&);
  void populateString2(std::string&);
  void populateString3(std::string&);
  //Tests 
  int testCutString();
  int testNumber();
  int testGetObj();
  int testGroupContent();
  int testBinSearch();
  int testLoadSystem();
  int testSplitGroup();
  int testReadObject();
  int testParent();
  int testFileName();
  int testNextGrp();
  int testXMLiterator();
  int testPartLoad();
  int testVectorWrite();
  int testLoadInclude();
  int testCombineGrid();
  int testCombineDeepGrid();
  int testDeleteObj();
  int testDataBlock();
  int testProcString();
  
public:

  testXML();
  ~testXML();

  int applyTest(int const =0);     
};

#endif
