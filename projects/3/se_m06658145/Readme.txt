# COMPILATION #

The following commands are used to build the aes and test tools.

$ cd build
$ cmake -G 'Unix Makefiles' ..
$ make -j

# EXECUTTION #

The following examples are provided for running the aes tool.

$ ./se keygen <prf_key_file_path> <aes_key_file_path>
$ ./se enc <prf_key_file_path> <aes_key_file_path> <index_file_path> <plaintext_dir_path> <ciphertext_dir_path>
$ ./se token <keyword> <prf_key_file_path> <token_file_path>
$ ./se search <index_file_path> <token_file_path> <ciphertext_dir_path> <aes_key_file_path>

# TESTING #

The following command is used to run the running time tests.

$ ./test_running_time
