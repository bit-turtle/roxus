# roxus
A UEFI Operating System
## Installing
- Using Roxus CI Build
  1. Download `Roxus Build.zip` from the most recent successful Roxus CI build
  2. Create an EFI partition on a flash drive using the FAT file system `mkfs.fat -F32 /dev/drivename`
  3. Copy the contents of `Roxus Build.zip` into the partition
- Using Local Roxus Build
  - Create an EFI partition on a flash drive using the FAT file system `mkfs.fat -F32 /dev/drivename`
  - Copy the files from the `.\root` directory to the EFI partition `.\write.sh \dev\drivename`
## Building
- Install [tup](https://gittup.org/gittup/), [clang](https://clang.llvm.org/), and [lld](https://lld.llvm.org/)
- `tup` (Compiles quite quickly)
- Builds for x86_64 and aarch64 by default (32-bit UEFI seems uncommon)
### Test (Emulation)
- Install QEMU x86_64
- `test.sh` (builds and runs on qemu)

### I/O Features
| I/O Feature     | Current Implementation       | Details                          | Future Implementation                                           |
|-----------------|------------------------------|----------------------------------|-----------------------------------------------------------------|
| Console Output  | Simple Text Output Protocol  | With ANSI Escape Code Processing | Custom Text Rendering with Simple Text Output Protocol fallback |
| Keyboard Input  | Simple Text Input Protocol   |                                  |                                                                 |
| Graphics Output | Graphics Output Protocol BLT | Custom Image Format              | Graphics Output Protocol BLT with Console fallback              |
| Mouse Input     | *Not Implemented*            |                                  | Simple Pointer Protocol                                         |
| Sound Output    | *Not Implemented*            | UEFI Has no Sound Protocol       | Custom Universal Sound Driver                                   |
