# XOR File Encryptor 🔐

A lightweight, multi-threaded XOR encryption and decryption tool written in C. 

## 🚀 Features
- Multi-threaded for faster encryption/decryption
- Simple XOR cipher (symmetric encryption)
- Supports text and binary files
- Easy to build and use

## 🛠 Build
```bash
make
📖 Usage
bash
Copy code
./xor_encrypt <-e|-d> <input_file> <output_file> <key>
-e : Encrypt

-d : Decrypt

Example
bash
Copy code
# Encrypt
./xor_encrypt -e sample.txt encrypted.bin mysecretkey

# Decrypt
./xor_encrypt -d encrypted.bin decrypted.txt mysecretkey
📂 Project Structure
bash
Copy code
src/            # Source code
build/          # Compiled binaries (optional)
Makefile        # Build system
README.md       # Documentation
📜 License
This project is licensed under the MIT License.

yaml
Copy code

---

### ✅ 5. `LICENSE`
*(MIT License)*
```text
MIT License

Copyright (c) 2025 YOUR_NAME

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...