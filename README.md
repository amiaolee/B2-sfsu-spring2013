###Intensity Based 3D Image Registration Using Maximization of Mutual Information

intensity based 3D rigid medical image registration using mutual information maximization algorithm.
It is developed as a Term Project for the course CSC621-821 in Spring 2013 at San Francisco State University.
it made use of ITK image registration modules. Basic modules used in this project includes:

    1. TranslationTransform as Transformation algorithm 
    2. LinearInterpolation as Interpolation function
    3. RegularStepGradientDescentOptimizer as Optimizer
    4. MattesMutaulInformationImageToImageMetric as a metric
    5. MultiResolutionPyramidImageFilter used to speed up the convergence of the optimizer

###file Organization
    1. all the source files are in source folder
    2. example 3D volumes in mha format are provided in the Data folder 
    3. output folder is used to store registration output files
    4.build contains executables

###How to Compile
    1. follow all the necessary steps used to build ITK project using cmake provide source 
        folder as project source and build folder as project build folder
    2. if unix Makefile is selected as the generator for the project in cmake then go to 
        build type make you will have all the executable necessary for the project in that folder.
    3. if some other project generator are selected in cmake then  you should use the generator's 
        build tools to build the project. Here too you will get all the executables ready in the build folder

###How to run 
    1. if make is used to create the executables you can use the provided shell script to run the project
          ./register [parameter file] 
          
      where register is the shell script and parameter file is optional which contains the parameters to be used 
      by the registration tool. an example parameter file with name "param.dat" is provided. if not parameter 
      file is provided as argument to the script then the defualt parameter file will be used refer to README 
      file in the build directory to know what these parameters are and how they are written in the file
    2. if some other build tools are used to generate the executables or if system is windows then you 
      should run the executable manualy by passing the optional parameter file.

      if either the fixed or moving volumes are in dicom file formate then the script in case 1 will 
      automatically convert them to .mha to be used by the registration class. but in the case of 2. 
      they have to be converted manualy using the DicomRead executable which requirs dicom series directory 
      and output file name as parameter to generate .mha volume from the dicom series.

    3.once you run your registration system. you may use the "plot" script provided in the build folder if you 
      want to have a plot of the optimizer behaviour during the optimization process. the script will write 
      the output plot in postscript .eps format which can be easly read by any pdf reader. 

