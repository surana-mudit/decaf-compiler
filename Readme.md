## Decaf Compiler

### Structure

* **Decaff_Compiler** : Directory containing the source code for the compiler.
    * **parser.yy** : The parser for the compiler.
    * **scanner.l** : The scanner for the compiler.
    * **ast.hh** : The declaration of various classes defining the structure of the AST.
    * **codegen.h** : Basic declarations for the code generation module.
    * **codegen.cpp** : Code for the code generation module.
    * **phase_1** : Directory containing the phase 1 submission.
    * **phase_2** : Directory containing the phase 2 submission.
    * **Makefile** : Used to run the code.

* **test-programs** : Contains the various test programs created for testing the compiler.
    *   arraysum.dcf
    *   maxmin.dcf
    *   nextmax.dcf
    *   prime.dcf
    *   sumn.dcf


### Run

* For running the code to create an executable named compiler use command ==make==
* This generates an executable file named **compiler**.
* Files can be run using the executable in the following ways.
    * ./compiler AST < filepath (For getting the phase-2 output)
    * ./compiler codegen < filepath (For getting the phase-3 output)

### Descriptiton
The file **ast.hh** defines the AST structure for the compiler. Any input which is given is first of parsed using the file parser.yy where we go on building the ast  structure on the go. Various syntactic checks are included to ensure correctness of the code.
The file **codegen.cpp** describes how code generation should be performed on various classes.
    

