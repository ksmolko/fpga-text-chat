# Instructions

## Generating Vitis Project
To generate the Vitis platform and application projects, open an xsct shell, navigate to the "scripts" directory, and run the following:
```
source genproj.tcl
```
Once the project has finished generating, close the shell and open Vitis. Set the newly created "proj" directory as your Vitis workspace. Navigate to the design view in Vitis, and from here you can build and debug the application.

## Workflow
Everything under the "proj" folder is NOT tracked by git. When making changes to source files, copy the changes back out to the "src" folder before committing. When pulling the latest update, simply delete your "proj" folder, run git pull, and then run genproj.tcl again as outlined above.
