###Parameter file format
the parameter file is a text file with a list of parameters to be used by the registration algorithm

The parameters in the parameter file are to be specified in the following order:

    fixedimage 3D volume file path 
    moving 3D volume file path 
    output 3D volume file path 
    checkerboard 2D image before registartion file name (png format) 
    checkerboard 2D image after registration file name  (png format)
    difference 3D volume before registration file name  (.mha format)
    difference 3D volume after registration file name  (.mha format)
    number of histogram bins  
    number of iterations  
    relaxation or learning rate for the optimizer
    number of resolution or pyramid levels
