B2-sfsu-spring2013
==================

intensity based 3D rigid medical image registration using mutual information maximization algorithm.
It is developed as a Term Project for the course CSC621-821 in Spring 2013 at San Francisco State University.
it make use of ITK image registration modules. Basic Framework used in this project includes: <br>
1. TranslationTransform as Transformation algorithm <br>
2. LinearInterpolation as Interpolation function <br>
3. RegularStepGradientDescentOptimizer as Optimizer <br>
4. MattesMutaulInformationImageToImageMetric as a metric <br>
5. MultiResolutionPyramidImageFilter used to speed up the convergence of the optimizer <br>
