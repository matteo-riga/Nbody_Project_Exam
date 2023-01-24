# Readme and compiling

We were unable to run the code with OpenGL. To try to compile we did:
* Install several OpenGL packages (installing `libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev` was not enough)
* Clone a new version of `glew` since the bundled one was not working
* Change the definition of colors in `Particle.h` since they gave issues with the linker 

Then when I tried to run it the code went in segmentation fault during the compiling of the shader.

Things are not so surprising since OpenGL is a bit delicate. Unfortunately we did not have the time to do further tests. We will see the OpneGL version during your presentation! Anyway, the integration with OpenGL is very good effort.

The version without OpenGL compiles and runs fine.


# Code
* You should not keep the build folder in the git repo. In the repository you have only the files files that are NOT produced by the compilation process: headers, source files, documentation files... No build directories, nor executables, object files, object libraries!
* Nice idea to keep constant and globals isolated in specific header files. 
A few suggestions however. Use namespaces, for instance 
`namespace globals`. This way, you reduce the risk of name clashes with local variables!. If a constant is a in fact a constant expression you can use `constexpr` instead for `const`. For instance `constexpr unsigned int DIM = 3`;
* Function `sumUpTo` is useless since you could just exploit Gauss formula (sum of first n integers is (n + 1) n / 2)!
* C-style cast in `_updateSpeed` and `_updatePos`. It is not an error, but in C++ it is better to use the C++-style cast because they are safer.
* Constants like `mass_constant_k` and `tol` of `Particle.h` should be in another class or be static. If a variable is meant to be shared by all object of the class make it static. 
* You re-build the tree with the particles at each iteration. Maybe it can  be more efficient to build it once and then update it at each iteration, since (if the time step is small) the tree won't change much.
* The tree is built and updated serially, a parallel efficient implementation is difficult but one can try try to use OMP task to see if there is any performance gain (as seen during lecture with fibbonacci and lab with kd-tree). It is true that normally you don't gain much, unless you have a very big problem.
* `root.updateForce(particle, root, THETA)` is ugly: `THETA` should be a parameter or at least constexpr and not defined by preprocessor directives.  Try to avoid preprocessor macro unless really necessary. Moreover, you can give the user a better interface, meaning you should hide this recursive version (you can still use it behind the scene).
* The class `Cell` is template but actually works just in `3D`
* You do not specify `const` for some local variables (e.g. `double r_norm = r.euNorm();`). It's not really an error, but if you get the habit of declaring `const` variables that are meant to be constant you are safer, and you will never be sorry for that.
* In `updateForce` you assume all particles have the same mass as `particle[0]` and that the center of mass of the cell is the same of the center of the cell, which in general is not true. 
* There is a potential memory leak: `c.children[i] = new Cell((center + offset), h, temp);`. It is not clear who is then deleting the resource created by `new`. You could have used smart pointers instead of ordinary pointers, this way you delegate to the smart pointer the memory handling (one responsibility principle). 
* I do not see where `updateTree` is being used
* It is better not have all member variables of a complex class to be public (e.g. the class `Cell`), in particular variables that are just used internally should be kept private or protected. By making them public you allow everybody modify them.
* In `JsonParser`, using `unordered_map` should be better in this context. Is not a great deal, but for the type of queries necessary here a hash table, like that implemented in `unordered_map` is more efficient than a binary tree structure like than in `map`.
* `Particle` and `Cell` constructors takes copies instead of references, moreover `bodies` is unused. Take the habit of using  `const &` instead of passing by value, particularly for object that may be big.
With the possible exception of int and double. But, in any case, you do not lose anything by using `const &`, but you may gain by sparing a copy.
* `pow(distance, 3)` Pow may be is inefficient when the exponent is a small integer, better `distance*distance*distance`.
* It is much better to use `nullptr` over `NULL`.
* In `constexpr friend Vector<dim> operator+(const Vector<dim> lhs, const Vector<dim>& rhs)` you pass lhs by copy but you do not exploit the fact that it is a copy and you copy it again inside the body of the function. By the way, why `constexpr`? It is true that nowadays constexpr is popping up everywhere, but here I do not see how the compiler can take advantage of it  since you cannot have a constexpr `std::vector` (at least it does not make sense to me). Anyway, it is not an error, if the function cannot be executed at compile time `constexpr` is equivalent to `inline`.


