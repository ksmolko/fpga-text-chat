# Instructions

## Generating Vitis Project
To generate the Vitis platform and application projects, open an xsct shell, navigate to the "scripts" directory, and run the following:
```
source genproj.tcl
```
Once the project has finished generating, close the shell and open Vitis. Set the newly created "proj" directory as your Vitis workspace. Navigate to the design view in Vitis, and from here you can build and debug the application.

## Workflow
Everything under the "proj" folder is NOT tracked by git. When making changes to source files, copy the changes back out to the "src" folder before committing. When pulling the latest update, simply delete your "proj" folder, run git pull, and then run genproj.tcl again as outlined above.


## About this side submission
This is my current progress on trying to send audio data over TCP/IP. When I was developing the system, I thought that it would be easier if I tried to implement the code on core 0 first to see if it works before moving the code to core 1. But because I failed to transmit the data, I couldn't start the next phase which is implementing the second core. - Minh