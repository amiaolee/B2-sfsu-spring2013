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

    //
    // This example is the 3D version of MultiResImageRegistration
    //

    #include "itkMultiResolutionImageRegistrationMethod.h"
    #include "itkTranslationTransform.h"
    #include "itkMattesMutualInformationImageToImageMetric.h"
    #include "itkRegularStepGradientDescentOptimizer.h"
    #include "itkImage.h"
    #include "itkImageFileReader.h"
    #include "itkImageFileWriter.h"
    #include "itkResampleImageFilter.h"
    #include "itkCastImageFilter.h"
    #include "itkCheckerBoardImageFilter.h"
    #include "itkSubtractImageFilter.h"
    #include "itkRescaleIntensityImageFilter.h"
    #include "itkExtractImageFilter.h"

    #include "itkCommand.h"

    int parseParamFile(std::string);

    std::string fixedFileName ;
    std::string movingFileName ; 
    std::string outputFileName;
    std::string checkBoardBeforeFileName;
    std::string checkBoardAfterFileName; 
    std::string diffbeforeFileName; 
    std::string diffAfterFileName;
    unsigned int numBins;
    unsigned int numIters;
    unsigned int resLevels;
    float        relRate;

    template <typename TRegistration>
    class RegistrationInterfaceCommand : public itk::Command
    {
    public:
      typedef  RegistrationInterfaceCommand   Self;
      typedef  itk::Command                   Superclass;
      typedef  itk::SmartPointer<Self>        Pointer;
      itkNewMacro( Self );

    protected:
      RegistrationInterfaceCommand() {};

    public:
      typedef   TRegistration                              RegistrationType;
      typedef   RegistrationType *                         RegistrationPointer;
      typedef   itk::RegularStepGradientDescentOptimizer   OptimizerType;
      typedef   OptimizerType *                            OptimizerPointer;

      void Execute(itk::Object * object, const itk::EventObject & event)
        {
        if( !(itk::IterationEvent().CheckEvent( &event )) )
          {
          return;
          }
        RegistrationPointer registration =
          dynamic_cast<RegistrationPointer>( object );

        OptimizerPointer optimizer = dynamic_cast< OptimizerPointer >(
          registration->GetOptimizer() );

        std::cout << "-------------------------------------" << std::endl;
        std::cout << "MultiResolution Level : "
                  << registration->GetCurrentLevel()  << std::endl;
        std::cout << std::endl;

        if ( registration->GetCurrentLevel() == 0 )
          {
          optimizer->SetMaximumStepLength( 16.00 );
          optimizer->SetMinimumStepLength( 0.01 );
          }
        else
          {
           optimizer->SetMaximumStepLength( optimizer->GetMaximumStepLength() / 4.0 );
           optimizer->SetMinimumStepLength( optimizer->GetMinimumStepLength() / 10.0 );
          }
        }

      void Execute(const itk::Object * , const itk::EventObject & )
        { return; }
    };


    //  The following section of code implements an observer
    //  that will monitor the evolution of the registration process.
    //
    class CommandIterationUpdate : public itk::Command
    {
    public:
      typedef  CommandIterationUpdate   Self;
      typedef  itk::Command             Superclass;
      typedef  itk::SmartPointer<Self>  Pointer;
      itkNewMacro( Self );

    protected:
      CommandIterationUpdate() {};

    public:
      typedef   itk::RegularStepGradientDescentOptimizer OptimizerType;
      typedef   const OptimizerType *                    OptimizerPointer;

      void Execute(itk::Object *caller, const itk::EventObject & event)
        {
          Execute( (const itk::Object *)caller, event);
        }

      void Execute(const itk::Object * object, const itk::EventObject & event)
        {
          OptimizerPointer optimizer =
            dynamic_cast< OptimizerPointer >( object );
          if( !(itk::IterationEvent().CheckEvent( &event )) )
            {
            return;
            }
          std::cout << optimizer->GetCurrentIteration() << "   ";
          std::cout << optimizer->GetValue() << "   ";
          std::cout << optimizer->GetCurrentPosition() << std::endl;
        }
    };



    int main( int argc, char *argv[] )
    {
      if( argc > 2 )
        {
        std::cerr << "Usage: " << argv[0];
        std::cerr << " [parameterFile]" << std::endl;
        
        return EXIT_FAILURE;
        }

        std::string paramfileName("param.dat");
        if(argc > 1)
        {
          paramfileName = argv[1];
        }

      parseParamFile(paramfileName);

      const    unsigned int    Dimension = 3;
      typedef  unsigned short  PixelType;

      typedef itk::Image< PixelType, Dimension >  FixedImageType;
      typedef itk::Image< PixelType, Dimension >  MovingImageType;

      typedef   float                                    InternalPixelType;
      typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

      typedef itk::TranslationTransform< double, Dimension > TransformType;
      typedef itk::RegularStepGradientDescentOptimizer       OptimizerType;
      typedef itk::LinearInterpolateImageFunction<
                                        InternalImageType,
                                        double             > InterpolatorType;
      typedef itk::MattesMutualInformationImageToImageMetric<
                                        InternalImageType,
                                        InternalImageType >   MetricType;
      typedef itk::MultiResolutionImageRegistrationMethod<
                                        InternalImageType,
                                        InternalImageType >   RegistrationType;

      typedef itk::MultiResolutionPyramidImageFilter<
                                        InternalImageType,
                                        InternalImageType >   FixedImagePyramidType;
      typedef itk::MultiResolutionPyramidImageFilter<
                                        InternalImageType,
                                        InternalImageType >   MovingImagePyramidType;


      //  All the components are instantiated using their \code{New()} method
      //  and connected to the registration object as in previous example.
      //
      TransformType::Pointer      transform     = TransformType::New();
      OptimizerType::Pointer      optimizer     = OptimizerType::New();
      InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
      RegistrationType::Pointer   registration  = RegistrationType::New();
      MetricType::Pointer         metric        = MetricType::New();

      FixedImagePyramidType::Pointer fixedImagePyramid =
          FixedImagePyramidType::New();
      MovingImagePyramidType::Pointer movingImagePyramid =
          MovingImagePyramidType::New();

      registration->SetOptimizer(     optimizer     );
      registration->SetTransform(     transform     );
      registration->SetInterpolator(  interpolator  );
      registration->SetMetric( metric  );
      registration->SetFixedImagePyramid( fixedImagePyramid );
      registration->SetMovingImagePyramid( movingImagePyramid );


      typedef itk::ImageFileReader< FixedImageType  > FixedImageReaderType;
      typedef itk::ImageFileReader< MovingImageType > MovingImageReaderType;

      FixedImageReaderType::Pointer  fixedImageReader  = FixedImageReaderType::New();
      MovingImageReaderType::Pointer movingImageReader = MovingImageReaderType::New();

      fixedImageReader-> SetFileName(fixedFileName);
      movingImageReader->SetFileName(movingFileName);

      typedef itk::CastImageFilter<
                            FixedImageType, InternalImageType > FixedCastFilterType;
      typedef itk::CastImageFilter<
                            MovingImageType, InternalImageType > MovingCastFilterType;

      FixedCastFilterType::Pointer fixedCaster   = FixedCastFilterType::New();
      MovingCastFilterType::Pointer movingCaster = MovingCastFilterType::New();

      fixedCaster->SetInput(  fixedImageReader->GetOutput() );
      movingCaster->SetInput( movingImageReader->GetOutput() );

      registration->SetFixedImage(    fixedCaster->GetOutput()    );
      registration->SetMovingImage(   movingCaster->GetOutput()   );


      fixedCaster->Update();

      registration->SetFixedImageRegion(
      fixedCaster->GetOutput()->GetBufferedRegion() );


      typedef RegistrationType::ParametersType ParametersType;
      ParametersType initialParameters( transform->GetNumberOfParameters() );

      initialParameters[0] = 0.0;  // Initial offset in mm along X
      initialParameters[1] = 0.0;  // Initial offset in mm along Y
      initialParameters[2] = 0.0;  // Initial offset in mm along Z

      registration->SetInitialTransformParameters( initialParameters );

      
      metric->SetNumberOfSpatialSamples( 50000 );

      metric->SetNumberOfHistogramBins( numBins );

      
      metric->ReinitializeSeed( 76926294 );


      optimizer->SetNumberOfIterations( numIters );
      
      optimizer->SetRelaxationFactor( relRate);

      std::cout << "------------------------------"<<std::endl;

      std::cout << "Starting Registration Process" << std::endl<<std::endl;

      
      // Create the Command observer and register it with the optimizer.
      //
      CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
      optimizer->AddObserver( itk::IterationEvent(), observer );

      typedef RegistrationInterfaceCommand<RegistrationType> CommandType;
      CommandType::Pointer command = CommandType::New();
      registration->AddObserver( itk::IterationEvent(), command );


      registration->SetNumberOfLevels( resLevels);

      try
        {
        registration->Update();
        std::cout << "Optimizer stop condition: "
                  << registration->GetOptimizer()->GetStopConditionDescription()
                  << std::endl;
        }
      catch( itk::ExceptionObject & err )
        {
        std::cout << "ExceptionObject caught !" << std::endl;
        std::cout << err << std::endl;
        return EXIT_FAILURE;
        }

      ParametersType finalParameters = registration->GetLastTransformParameters();

      double TranslationAlongX = finalParameters[0];
      double TranslationAlongY = finalParameters[1];
      double TranslationAlongZ = finalParameters[2];

      unsigned int numberOfIterations = optimizer->GetCurrentIteration();

      double bestValue = optimizer->GetValue();


      // Print out results
      //
      std::cout << "Result = " << std::endl;
      std::cout << " Translation X = " << TranslationAlongX  << std::endl;
      std::cout << " Translation Y = " << TranslationAlongY  << std::endl;
      std::cout << " Translation Z = " << TranslationAlongZ  << std::endl;
      std::cout << " Iterations    = " << numberOfIterations << std::endl;
      std::cout << " Metric value  = " << bestValue          << std::endl;

      typedef itk::ResampleImageFilter<
                                MovingImageType,
                                FixedImageType >    ResampleFilterType;

      TransformType::Pointer finalTransform = TransformType::New();

      finalTransform->SetParameters( finalParameters );
      finalTransform->SetFixedParameters( transform->GetFixedParameters() );

      ResampleFilterType::Pointer resample = ResampleFilterType::New();

      resample->SetTransform( finalTransform );
      resample->SetInput( movingImageReader->GetOutput() );

      FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();

     
      resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize() );
      resample->SetOutputOrigin(  fixedImage->GetOrigin() );
      resample->SetOutputSpacing( fixedImage->GetSpacing() );
      resample->SetOutputDirection( fixedImage->GetDirection() );
      resample->SetDefaultPixelValue( 100 );


      typedef  unsigned char  OutputPixelType;

      typedef itk::Image< OutputPixelType, Dimension > OutputImageType;

      typedef itk::CastImageFilter<
                            FixedImageType,
                            OutputImageType > CastFilterType;

      typedef itk::ImageFileWriter< OutputImageType >  WriterType;


      WriterType::Pointer      writer =  WriterType::New();
      CastFilterType::Pointer  caster =  CastFilterType::New();

      writer->SetFileName(outputFileName);


      caster->SetInput( resample->GetOutput() );
      writer->SetInput( caster->GetOutput()   );
      writer->Update();

      //
      // Generate checkerboards before and after registration
      //
      typedef itk::CheckerBoardImageFilter< FixedImageType > CheckerBoardFilterType;

      CheckerBoardFilterType::Pointer checker = CheckerBoardFilterType::New();

      checker->SetInput1( fixedImage );
      checker->SetInput2( resample->GetOutput() );

      caster->SetInput( checker->GetOutput() );
      writer->SetInput( caster->GetOutput()   );

      resample->SetDefaultPixelValue( 0 );

      // Before registration
      TransformType::Pointer identityTransform = TransformType::New();
      identityTransform->SetIdentity();
      resample->SetTransform( identityTransform );

      writer->SetFileName(checkBoardBeforeFileName);
      writer->Update();


      // After registration
      resample->SetTransform( finalTransform );

      writer->SetFileName( checkBoardAfterFileName);
      writer->Update();
      
      typedef itk::SubtractImageFilter<
                                      FixedImageType,
                                      FixedImageType,
                                      FixedImageType > DifferenceFilterType;
      DifferenceFilterType::Pointer difference = DifferenceFilterType::New();

      typedef itk::RescaleIntensityImageFilter<
                                      FixedImageType,
                                      OutputImageType >   RescalerType;
      RescalerType::Pointer intensityRescaler = RescalerType::New();
      intensityRescaler->SetInput( difference->GetOutput() );
      intensityRescaler->SetOutputMinimum(   0 );
      intensityRescaler->SetOutputMaximum( 255 );
      difference->SetInput1( fixedImageReader->GetOutput() );
      difference->SetInput2( resample->GetOutput() );
      resample->SetDefaultPixelValue( 1 );
      WriterType::Pointer writer2 = WriterType::New();
      writer2->SetInput( intensityRescaler->GetOutput() );

      writer2->SetFileName( diffbeforeFileName);
      writer2->Update();
      
      resample->SetTransform( identityTransform );
      writer2->SetFileName( diffAfterFileName);
      writer2->Update();
       
      
      return EXIT_SUCCESS;
    }


    int parseParamFile(std::string paramfile)
    {

      // open up the parameter file
      FILE * paramFile;
      paramFile = fopen( paramfile.c_str(), "r" );
      if( !paramFile )
        {
        std::cerr << "Could not open parameter file." << std::endl;
        return EXIT_FAILURE;
        }
        
      // parse the file
      char currentLine[150];
      /************************************
       * Parse fixed image information
       ************************************/

       std::cout << "List of Parameters Read from " << paramfile << std::endl;
       std::cout << "--------------------------------------- " << std::endl;
      // get fixed image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr <<  "Could not find fixed image filename." << std::endl;
        }
      fixedFileName = currentLine;
      std::cout << "Fixed image filename: " << fixedFileName << std::endl;

      /************************************
       * Parse moving image information
       ************************************/
      // get moving image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr << "Could not find moving image filename." << std::endl;
        }
       movingFileName = currentLine;
       std::cout << "Moving image filename: " << movingFileName << std::endl;

      /************************************
       * Parse output image information
       ************************************/
      // get output image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr << "Could not find output image filename." << std::endl;
        }
        outputFileName = currentLine;
        std::cout << "output image filename: " << outputFileName << std::endl;


      /************************************
       * Parse checkboard image information
       ************************************/
      // get checkboard before image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr << "Could not find checkboard before image filename." << std::endl;
        }
        checkBoardBeforeFileName = currentLine;
        std::cout << "checkBoardBefore image filename: " << checkBoardBeforeFileName << std::endl;
      
      // get checkboard after image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr <<  "Could not find checkboard image filename." << std::endl;
        }
        checkBoardAfterFileName = currentLine;
        std::cout << "checkBoardAfter image filename: " << checkBoardAfterFileName << std::endl;
      /************************************
       * Parse difference image inforamtion
       ************************************/
      // get difference before image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr <<  "Could not find difference before image filename." << std::endl;
        }
        diffbeforeFileName = currentLine;
        std::cout << "differencebefore volume filename: " << diffbeforeFileName << std::endl;
      
      // get difference after image file name
      if( fscanf( paramFile, "%s", currentLine ) != 1 ) 
        {
          std::cerr << "Could not find difference after image filename." << std::endl;
        }
        diffAfterFileName = currentLine;
        std::cout << "differenceAfter volume filename: " << diffAfterFileName << std::endl;

      /************************************
       * Parse registration parameters
       ************************************/
      // get number of levels
      std::cout << "Number of Bins: ";
      if( fscanf( paramFile, "%d", &numBins) != 1 )
        {
          std::cerr << "Could not find the number of Bins." << std::endl;
        }
        std::cout << numBins << std::endl;

       // get number of Iterations
      std::cout << "Number of Iterations: ";
      if( fscanf( paramFile, "%d", &numIters) != 1 )
        {
          std::cerr << "Could not find the number of Iterations." << std::endl;
        }
       std::cout << numIters << std::endl;

     std::cout << "Relaxation Rate: ";
      if( fscanf( paramFile, "%f", &relRate) != 1 )
        {
          std::cerr << "Could not find the relaxation rate." << std::endl;
        }
       std::cout << relRate << std::endl;

     // get number of Iterations
      std::cout << "Number of Levels: ";
      if( fscanf( paramFile, "%d", &resLevels) != 1 )
        {
          std::cerr << "Could not find the number of Levels." << std::endl;
        }
       std::cout << resLevels << std::endl;

      
    return EXIT_SUCCESS;
    }