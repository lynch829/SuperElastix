/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/
//#include "itkComponentFactoryRegisterManager.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkDisplacementFieldTransform.h"
#include "itkComposeDisplacementFieldsImageFilter.h"
#include "itkCompositeTransform.h"
#include "itkVector.h"
#include "itkPoint.h"

#include "itkTransformFileReader.h"
#include "itkTransformFactoryBase.h"
#include "itkTransformFactory.h"
#include "itkMatrixOffsetTransformBase.h"

//Floris: The module factory is based on the ImageIO factory. 
// We manually register 2 dummy modules: itkTransformComponent1 and itkMetricComponent1. 
// in ITK CMake\UseITK.cmake sets up the IOFactory_Register_Manager for transforms and images
// Elastix (by Denis) uses (simpleITK's) Typelists to register all GPU filters 
//#include "itkComponentFactoryBase.h"
//#include "itkComponentFactory.h"

#include "itkComponentBase.h"
//#include "itkComponentFactory.h"

#include <map>
#include <string>

#include "itkTransformComponent1.h"
#include "itkTransformComponent1Factory.h"

#include "itkMetricComponent1.h"
#include "itkMetricComponent1Factory.h"

int main(int argc, char *argv[])
{
  typedef float ScalarType;

  typedef std::list< itk::LightObject::Pointer > RegisteredObjectsContainerType;
  RegisteredObjectsContainerType registeredComponents =
    itk::ObjectFactoryBase::CreateAllInstance("itkComponentBase");
  std::cout << "When CMake is not used to register the Component classes, there are\n"
    << registeredComponents.size()
    << " Component objects available to the Overlord.\n" << std::endl;

  itk::TransformComponent1Factory::RegisterOneFactory();
  itk::MetricComponent1Factory::RegisterOneFactory();
  std::cout << "After registering the TransformComponent1 and MetricComponent1object, ";
  std::cout << "there are\n";
  registeredComponents = itk::ObjectFactoryBase::CreateAllInstance("itkComponentBase");
  std::cout << registeredComponents.size()
    << " Component objects available to the Overlord.\n" << std::endl;

  typedef itk::ComponentBase       ComponentType;

  typedef std::map<std::string, std::string> CriteriaType;
  typedef std::pair<std::string, std::string> CriteriumType;

  CriteriaType criteria1;
  //criteria1.insert(CriteriumType("ComponentOutput","Metric")); 
  criteria1["ComponentOutput"] = "Transform";
  CriteriaType criteria2;
  criteria2["ComponentInput"] = "Transform";
  //criteria1.insert(CriteriumType("ComponentInput", "Metric"));

  CriteriaType emptyCriteria;

  typedef itk::ComponentFactory::Pointer NodePointer;

  NodePointer Node1 = itk::ComponentFactory::New();
  Node1->SetCriteria(emptyCriteria);
  ComponentType::Pointer Node1Component = Node1->GetComponent();
  if (Node1Component.IsNull())
  {
    std::cout << "Too few criteria means no Component could be selected." << std::endl;
  }
  else
  {
    std::cout << "FAILED" << std::endl;
  }

  // Narrow down the selection criteria
  Node1->AddCriteria(criteria1);
  Node1Component = Node1->GetComponent();
  if (Node1Component.IsNull())
  {
    std::cout << "FAILED" << std::endl;
  }
  else
  {
    std::cout << "Based on criteria, Node1 selected: " << Node1Component->GetNameOfClass() << std::endl;  
  }
 
  NodePointer Node2 = itk::ComponentFactory::New(); 
  Node2->SetCriteria(criteria2);
  ComponentType::Pointer Node2Component = Node2->GetComponent();
  if (Node2Component.IsNull())
  {
    std::cout << "FAILED" << std::endl;
  }
  else
  {
    std::cout << "Based on criteria, Node2 selected: " << Node2Component->GetNameOfClass() << std::endl;
  }



  


  return EXIT_SUCCESS;
}