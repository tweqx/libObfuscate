## libObfuscate

libObfuscate is an open-source project from EmbeddedSW. [Home page](https://www.embeddedsw.net/libObfuscate_Cryptography_Home.html).

This project was modified to run on POSIX-compatible systems as a library exporting libObfuscate's cryptography functions. Furthermore, warnings emitted g++ were fixed.

## Building
Run `make`.

## Testing
This project features a test-suite to ensure compatibility with the original libObfuscate.

`wine` must be installed to run the tests. They can be ran with `make tests`

## License
EmbeddedSW's libObfuscate is part of this project. This code is released under LGPL 3.0 (http://www.gnu.org/licenses/lgpl.html).
Its license can be found in lib/libObfuscate/license.txt
The author (and copyright owner) is: http://www.embeddedsw.net/. Their homepage can be found at: http://www.embeddedsw.net/libobfuscate.html

Modifications to the original liObfuscate are release under GPL v3.
