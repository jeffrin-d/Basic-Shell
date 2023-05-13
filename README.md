Some notes:
    If you wish to run this file, you might need to do the following on your Linux/Ubuntu command line:
        1. `sudo apt update`
            This checks for any updates in the existing list of packages
        
        2. `sudo apt install build-essential`
            This command installs packages like `gcc`, `g++`, and `make`
            
        3. (Optional) `sudo apt-get install manpages-dev`
            This is if you wish to download the manual pages for using GNU/Linux commands
            
        4. `gcc --version`
            This checks whether the GCC complier was properly installed and which version it was
            
        5. (IMPORTANT) `sudo apt-get install libreadline-dev`
            This installs the readline development libraries which is needed before running `make all`
