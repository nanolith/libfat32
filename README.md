libfat32
========

This is a minimal library for reading and writing FAT32 partitions in raw image
files. This library is opinionated and partial. It assumes GPT partitioning
based on the the UEFI standard, Release 2.11. It exists for creating simple UEFI
compatible partitions for booting under emulation and cloud instances.

Library Layout
--------------

This library is broken into three pieces:
* a GPT library.
* a FAT32 library.
* a simplified interface for creating a raw disk image with a bootable EFI
  partition.

GPT Library
-----------

The GPT library allows the user to create a GPT partitioning scheme for a raw
image and to read partition entries from this scheme. With a propertly formatted
GPT partitioned disk, it can enumerate all partitions, including a properly
clamped FAT32 partition, needed to create an instance of the FAT32 Library.

FAT32 Library
-------------

The FAT32 Library "mounts" a FAT32 filesystem given a clamped partition provided
by the GPT Library. It can read directory or file entries from this partition,
or write directory or file entries to this partition.

Helper Library
--------------

The helper library allows the user to create a raw image file containing a GPT
partitioned disk with a bootable EFI partition. The user can add files to this
partition given a path and the file data. This provides a quick interface for
creating boot images for various operating systems that is sufficient for
bootstrapping installation code.
