In this project, **Rayleigh-Benard convection** is simulated in a thin layer of fluid (2D domain).

Rayleigh-Benard convection is a thermal gradient driven natural convection, occurring in a planar, horizontal layer of fluid heated from below. 
During this phenomena the fluid develops regular patterns of convection cells called Benard cells. 

* The **linear algebra** algorithms are implemented via [Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page).

* The **graphical interface** is implemented via [raylib](https://www.raylib.com/).

* The **network and I/O programming** are implemented with [Boost.Asio](https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio.html).

* The **numerical solution** of the Navier-Stockes equation and the set of default parameter values is tacken from: Suraj Shankar (2022). [Rayleigh Benard Convection](https://www.mathworks.com/matlabcentral/fileexchange/38093-rayleigh-benard-convection), MATLAB Central File Exchange. Retrieved May 29, 2022.


The simulation is run according to the following hypotesis:
* Bottom and top walls are rigid, impermeable and isothermal.
* Side walls are adiabatic.
* No-slip is enforced for velocity on all sides.
* Homogenous boundary conditions are employed for pressure.
* Boussinesq approximation is used to model the bouyancy driven flow. 


 


\image html logo.png "Natural convection simulation" width=1000 height=900 
