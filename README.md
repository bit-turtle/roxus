# roxus
A UEFI Operating System
## Installing
- Using Roxus CI Build
  1. Download `Roxus Build.zip` from the most recent successful Roxus CI build
  2. Create an EFI partition on a flash drive using FAT `mkfs.fat`
  3. Copy the contents of `Roxus Build.zip` into the partition
- Using Local Roxus Build: `write.sh \dev\your_drive`
## Building
- Install [tup](https://github.com/gittup/tup), Clang, and LLD
- `tup`
### Test (Emulation)
- Install QEMU x86_64
- `test.sh` (builds and runs un qemu)

### I/O Features
| I/O Feature     | Current Implementation       | Details                          | Future Implementation                                           |
|-----------------|------------------------------|----------------------------------|-----------------------------------------------------------------|
| Console Output  | Simple Text Output Protocol  | With ANSI Escape Code Processing | Custom Text Rendering with Simple Text Output Protocol fallback |
| Keyboard Input  | Simple Text Input Protocol   |                                  |                                                                 |
| Graphics Output | Graphics Output Protocol BLT | Custom Image Format              | Graphics Output Protocol BLT with Console fallback              |
| Mouse Input     | *Not Implemented*            |                                  | Simple Pointer Protocol                                         |
| Sound Output    | *Not Implemented*            | UEFI Has no Sound Protocol       | Custom Universal Sound Driver                                   |
