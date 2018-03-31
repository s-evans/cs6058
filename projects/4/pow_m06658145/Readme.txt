# COMPILATION #

The following commands are used to build the pow and test tools.

$ cd build
$ cmake -G 'Unix Makefiles' ..
$ make -j

# EXECUTTION #

The following examples are provided for running the pow tool.

$ ./pow target d <target_file_path>
$ ./pow sol <input_file_path> <target_file_path> <solution_file_path>
$ ./pow verify <input_File_path> <target_file_path> <solution_file_path>

# TESTING #

The following command is used to run the running time tests.

$ ./test_running_time
