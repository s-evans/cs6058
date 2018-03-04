# COMPILATION #

The following commands are used to build the aes and test tools.

$ cd build
$ cmake -G 'Unix Makefiles' ..
$ make -j

# EXECUTTION #

The following examples are provided for running the aes tool.

$ ./aes enc <key_file_path> <plaintext_file_path> <ciphertext_file_path>
$ ./aes dec <key_file_path> <ciphertext_file_path> <plaintext_file_path>
$ ./aes keygen <key_size> <key_file_path>

# TESTING #

The following commands are used to run the keygen tests.

$ ./test_running_time
