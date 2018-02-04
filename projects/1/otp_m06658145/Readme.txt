# COMPILATION #

The following commands are used to build the otp and test tools.

$ cd build
$ cmake -G 'Unix Makefiles' ..
$ make -j

# EXECUTTION #

The following examples are provided for running the otp tool.

$ ./otp enc <key_file_path> <plaintext_file_path> <ciphertext_file_path>
$ ./otp dec <key_file_path> <ciphertext_file_path> <plaintext_file_path>
$ ./otp keygen <key_size> <key_file_path>

# TESTING #

The following commands are used to run the keygen tests.

$ ./test_running_time
$ ./test_frequency

