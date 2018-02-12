# OS_charaldp_project_2018
Final Project for Operating Systems course on 7th semester of EE.AUTH. A shell with two execution modes, interactive and batch file mode.
First to compile and extract the executable from charaldpShellProject.c use [make] or [make all]
There are two usage modes for the executable:

1) Execute using [./charaldpShelProject] to run in interactive shell mode. Commands can be typed in the shell and responses will appear like in default OS bash. To exit the shell use [quit]

2) Execute with 1 argument using [./charaldpShelProject A_Batch_File] to run in batch mode shell. Commands written in the "A_Batch_File" file will be scanned by the executable and run as in interactive mode. Scanning will persist until [quit] is encountered in the file or EOF has been reached. 
