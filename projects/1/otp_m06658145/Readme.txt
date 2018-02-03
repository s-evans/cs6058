# COMPILATION #

cd build
cmake -G 'Unix Makefiles' ..
make

# EXECUTTION #

./otp enc <key_file_path> <plaintext_file_path> <ciphertext_file_path>
./otp dec <key_file_path> <ciphertext_file_path> <plaintext_file_path>
./otp keygen <key_size> <key_file_path>

