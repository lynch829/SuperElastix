/*=========================================================================
 *
 *  Copyright Leiden University Medical Center, Erasmus University Medical 
 *  Center and contributors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "selxSuperElastixFilter.h"
//#include "Overlord.h"

#include "selxItkSmoothingRecursiveGaussianImageFilterComponent.h"
#include "selxDisplacementFieldItkImageFilterSink.h"
#include "selxItkImageSource.h"

#include "selxElastixComponent.h"
#include "selxItkImageSink.h"

#include "selxItkImageRegistrationMethodv4Component.h"
#include "selxItkANTSNeighborhoodCorrelationImageToImageMetricv4.h"
#include "selxItkMeanSquaresImageToImageMetricv4.h"
#include "selxItkImageSourceFixed.h"
#include "selxItkImageSourceMoving.h"

#include "selxDataManager.h"
#include "gtest/gtest.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace selx {

/** Temporary helper function to handle elastix deformation field output */
  template < int Dimensionality, typename PixelType> void CopyselxDeformationField(const std::string filename)
  {
    typedef itk::ImageFileReader<itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality>> ReaderType;
    typedef itk::ImageFileWriter<itk::Image<itk::Vector<PixelType, Dimensionality>, Dimensionality>> WriterType;

    typename ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName("deformationField.nii");
    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(filename);
    writer->SetInput(reader->GetOutput());
    writer->Update();
  };

/** A demo for our WBIR paper written as a Unit Test in the Google Test Framework */
class WBIRDemoTest : public ::testing::Test {
 
public:
  typedef Overlord::Pointer                 OverlordPointerType;
  typedef SuperElastixFilter<bool>          SuperElastixFilterType;
  typedef Blueprint::Pointer                BlueprintPointerType;
  typedef Blueprint::ConstPointer           BlueprintConstPointerType;
  typedef Blueprint::ParameterMapType       ParameterMapType;
  typedef Blueprint::ParameterValueType     ParameterValueType;
  typedef DataManager DataManagerType;

  /** Fill SUPERelastix' component data base by registering various components */
  virtual void SetUp() {
    
    
    ComponentFactory<DisplacementFieldItkImageFilterSinkComponent<2, float>>::RegisterOneFactory(); 
    ComponentFactory<ItkImageSourceFixedComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSourceMovingComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkSmoothingRecursiveGaussianImageFilterComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageRegistrationMethodv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkMeanSquaresImageToImageMetricv4Component<2, float>>::RegisterOneFactory();
    ComponentFactory<ElastixComponent<2, float>>::RegisterOneFactory();
    ComponentFactory<ItkImageSinkComponent<2, float>>::RegisterOneFactory();

  }

  virtual void TearDown() {
    itk::ObjectFactoryBase::UnRegisterAllFactories();
  }

  BlueprintPointerType blueprint;
  //Overlord::Pointer overlord;
  SuperElastixFilterType::Pointer superElastixFilter;
};

/** Experiment 2a: ITKv4 framework, stationary velocity field transform, ANTs neighborhood correlation metric */
TEST_F(WBIRDemoTest, itkv4_SVF_ANTSCC)
{
  /** make example blueprint configuration */
  blueprint = Blueprint::New();

  ParameterMapType component0Parameters;
  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
  blueprint->AddComponent("RegistrationMethod", component0Parameters);

  ParameterMapType component1Parameters;
  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
  blueprint->AddComponent("FixedImageSource", component1Parameters);

  ParameterMapType component2Parameters;
  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
  blueprint->AddComponent("MovingImageSource", component2Parameters);

  ParameterMapType component3Parameters;
  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
  blueprint->AddComponent("ResultImageSink", component3Parameters);

  ParameterMapType component4Parameters;
  component4Parameters["NameOfClass"] = { "DisplacementFieldItkImageFilterSinkComponent" };
  blueprint->AddComponent("ResultDisplacementFieldSink", component4Parameters);

  ParameterMapType component5Parameters;
  component5Parameters["NameOfClass"] = { "ItkANTSNeighborhoodCorrelationImageToImageMetricv4Component" };
  blueprint->AddComponent("Metric", component5Parameters);


  ParameterMapType connection1Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);

  ParameterMapType connection2Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);

  ParameterMapType connection3Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);

  ParameterMapType connection4Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
  blueprint->AddConnection("RegistrationMethod", "ResultDisplacementFieldSink", connection4Parameters);

  ParameterMapType connection5Parameters;
  //optionally, tie properties to connection to avoid ambiguities
  //connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
  blueprint->AddConnection("Metric", "RegistrationMethod", connection5Parameters);

  blueprint->WriteBlueprint("itkv4_SVF_ANTSCC.dot");

  EXPECT_NO_THROW(superElastixFilter = SuperElastixFilterType::New());

  typedef itk::Image<float, 2> Image2DType;
  typedef itk::ImageFileReader<Image2DType> ImageReader2DType;
  typedef itk::ImageFileWriter<Image2DType> ImageWriter2DType;
  
  typedef itk::Image<itk::Vector<float,2>, 2> VectorImage2DType;
  typedef itk::ImageFileWriter<VectorImage2DType> VectorImageWriter2DType;

  DataManagerType::Pointer dataManager = DataManagerType::New();

  ImageReader2DType::Pointer fixedImageReader = ImageReader2DType::New();
  fixedImageReader->SetFileName(dataManager->GetInputFile("coneA2d64.mhd"));

  ImageReader2DType::Pointer movingImageReader = ImageReader2DType::New();
  movingImageReader->SetFileName(dataManager->GetInputFile("coneB2d64.mhd"));
  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
  
  ImageWriter2DType::Pointer resultImageWriter = ImageWriter2DType::New();
  resultImageWriter->SetFileName(dataManager->GetOutputFile("itkv4_SVF_ANTSCC_Image.mhd"));

  VectorImageWriter2DType::Pointer vectorImageWriter = VectorImageWriter2DType::New();
  vectorImageWriter->SetFileName(dataManager->GetOutputFile("itkv4_SVF_ANTSCC_Displacement.mhd"));
  
  superElastixFilter->SetInput("FixedImageSource", fixedImageReader->GetOutput());
  superElastixFilter->SetInput("MovingImageSource", movingImageReader->GetOutput());

  resultImageWriter->SetInput(superElastixFilter->GetOutput<Image2DType>("ResultImageSink"));
  vectorImageWriter->SetInput(superElastixFilter->GetOutput<VectorImage2DType>("ResultDisplacementFieldSink"));

  EXPECT_NO_THROW(superElastixFilter->SetBlueprint(blueprint));

  //TODO remove Update call
  //superElastixFilter->Update();

  EXPECT_NO_THROW(resultImageWriter->Update());
  EXPECT_NO_THROW(vectorImageWriter->Update());

}

///** Experiment 2b: ITKv4 framework, stationary velocity field transform, mean squared differences metric */
//TEST_F(WBIRDemoTest, itkv4_SVF_MSD)
//{
//  /** make example blueprint configuration */
//  blueprint = Blueprint::New();
//
//  ParameterMapType component0Parameters;
//  component0Parameters["NameOfClass"] = { "ItkImageRegistrationMethodv4Component" };
//  blueprint->AddComponent("RegistrationMethod", component0Parameters);
//
//  ParameterMapType component1Parameters;
//  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
//  blueprint->AddComponent("FixedImageSource", component1Parameters);
//
//  ParameterMapType component2Parameters;
//  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
//  blueprint->AddComponent("MovingImageSource", component2Parameters);
//
//  ParameterMapType component3Parameters;
//  component3Parameters["NameOfClass"] = { "ItkImageFilterSinkComponent" };
//  blueprint->AddComponent("ResultImageSink", component3Parameters);
//
//  ParameterMapType component4Parameters;
//  component4Parameters["NameOfClass"] = { "DisplacementFieldItkImageFilterSinkComponent" };
//  blueprint->AddComponent("ResultDisplacementFieldSink", component4Parameters);
//
//  ParameterMapType component5Parameters;
//  component5Parameters["NameOfClass"] = { "ItkMeanSquaresImageToImageMetricv4Component" };
//  blueprint->AddComponent("Metric", component5Parameters);
//
//
//  ParameterMapType connection1Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
//  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);
//
//  ParameterMapType connection2Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
//  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);
//
//  ParameterMapType connection3Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection3Parameters["NameOfInterface"] = { "itkImageSourceInterface" };
//  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);
//
//  ParameterMapType connection4Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection4Parameters["NameOfInterface"] = { "DisplacementFieldItkImageSourceInterface" };
//  blueprint->AddConnection("RegistrationMethod", "ResultDisplacementFieldSink", connection4Parameters);
//
//  ParameterMapType connection5Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection5Parameters["NameOfInterface"] = { "itkMetricv4Interface" };
//  blueprint->AddConnection("Metric", "RegistrationMethod", connection5Parameters);
//
//  blueprint->WriteBlueprint("itkv4_SVF_MSD.dot");
//
//  EXPECT_NO_THROW(overlord = Overlord::New());
//
//  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
//  DataManagerType::Pointer dataManager = DataManagerType::New();
//  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
//  overlord->outputFileNames = { dataManager->GetOutputFile("itkv4_SVF_MSD_Image.mhd"), dataManager->GetOutputFile("itkv4_SVF_MSD_Displacement.mhd") };
//  
//  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
//  bool allUniqueComponents;
//  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
//  EXPECT_TRUE(allUniqueComponents);
//  EXPECT_NO_THROW(overlord->Execute());
//}
//
///** Experiment 1a: elastix framework, B-spline transform, normalized correlation metric */
//TEST_F(WBIRDemoTest, elastix_BS_NCC)
//{
//  /** make example blueprint configuration */
//  blueprint = Blueprint::New();
//
//  ParameterMapType component0Parameters;
//  component0Parameters["NameOfClass"] = { "ElastixComponent" };
//  //component0Parameters["RegistrationPreset"] = { "rigid" };
//  component0Parameters["Transform"] = { "BSplineTransform" }; 
//  component0Parameters["Metric"] = { "AdvancedNormalizedCorrelation" };
//  blueprint->AddComponent("RegistrationMethod", component0Parameters);
//
//  ParameterMapType component1Parameters;
//  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
//  blueprint->AddComponent("FixedImageSource", component1Parameters);
//
//  ParameterMapType component2Parameters;
//  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
//  blueprint->AddComponent("MovingImageSource", component2Parameters);
//
//  ParameterMapType component3Parameters;
//  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
//  blueprint->AddComponent("ResultImageSink", component3Parameters);
//
//  ParameterMapType connection1Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
//  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);
//
//  ParameterMapType connection2Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
//  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);
//
//  ParameterMapType connection3Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
//  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);
//
//  blueprint->WriteBlueprint("elastix_BS_NCC.dot");
//
//  EXPECT_NO_THROW(overlord = Overlord::New());
//  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
//
//  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
//  DataManagerType::Pointer dataManager = DataManagerType::New();
//  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
//  overlord->outputFileNames = { dataManager->GetOutputFile("elastix_BS_NCC_Image.mhd"), dataManager->GetOutputFile("elastix_BS_NCC_Displacement.mhd") };
//
//  bool allUniqueComponents;
//  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
//  EXPECT_TRUE(allUniqueComponents);
//  EXPECT_NO_THROW(overlord->Execute());
//
//  CopyselxDeformationField<2, float>(dataManager->GetOutputFile("elastix_BS_NCC_Displacement.mhd"));
//}
//
///** Experiment 1b: elastix framework, B-spline transform, mean squared differences metric */
//TEST_F(WBIRDemoTest, elastix_BS_MSD)
//{
//  /** make example blueprint configuration */
//  blueprint = Blueprint::New();
//
//  ParameterMapType component0Parameters;
//  component0Parameters["NameOfClass"] = { "ElastixComponent" };
//  //component0Parameters["RegistrationPreset"] = { "rigid" };
//  component0Parameters["Transform"] = { "BSplineTransform" };
//  component0Parameters["Metric"] = { "AdvancedMeanSquares" };
//  blueprint->AddComponent("RegistrationMethod", component0Parameters);
//
//  ParameterMapType component1Parameters;
//  component1Parameters["NameOfClass"] = { "ItkImageSourceFixedComponent" };
//  blueprint->AddComponent("FixedImageSource", component1Parameters);
//
//  ParameterMapType component2Parameters;
//  component2Parameters["NameOfClass"] = { "ItkImageSourceMovingComponent" };
//  blueprint->AddComponent("MovingImageSource", component2Parameters);
//
//  ParameterMapType component3Parameters;
//  component3Parameters["NameOfClass"] = { "ItkImageSinkComponent" };
//  blueprint->AddComponent("ResultImageSink", component3Parameters);
//
//  ParameterMapType connection1Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection1Parameters["NameOfInterface"] = { "itkImageFixedInterface" };
//  blueprint->AddConnection("FixedImageSource", "RegistrationMethod", connection1Parameters);
//
//  ParameterMapType connection2Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection2Parameters["NameOfInterface"] = { "itkImageMovingInterface" };
//  blueprint->AddConnection("MovingImageSource", "RegistrationMethod", connection2Parameters);
//
//  ParameterMapType connection3Parameters;
//  //optionally, tie properties to connection to avoid ambiguities
//  //connection3Parameters["NameOfInterface"] = { "GetItkImageInterface" };
//  blueprint->AddConnection("RegistrationMethod", "ResultImageSink", connection3Parameters);
//
//  blueprint->WriteBlueprint("elastix_BS_MSD.dot");
//
//  EXPECT_NO_THROW(overlord = Overlord::New());
//  EXPECT_NO_THROW(overlord->SetBlueprint(blueprint));
//
//  //The Overlord is not yet an itkfilter with inputs and outputs, therefore it reads and writes the files temporarily.
//  DataManagerType::Pointer dataManager = DataManagerType::New();
//  overlord->inputFileNames = { dataManager->GetInputFile("coneA2d64.mhd"), dataManager->GetInputFile("coneB2d64.mhd") };
//  overlord->outputFileNames = { dataManager->GetOutputFile("elastix_BS_MSD_Image.mhd"), dataManager->GetOutputFile("elastix_BS_MSD_Displacement.mhd") };
//
//  bool allUniqueComponents;
//  EXPECT_NO_THROW(allUniqueComponents = overlord->Configure());
//  EXPECT_TRUE(allUniqueComponents);
//  EXPECT_NO_THROW(overlord->Execute());
//
//  CopyselxDeformationField<2, float>(dataManager->GetOutputFile("elastix_BS_MSD_Displacement.mhd"));
//}
//
} // namespace selx

