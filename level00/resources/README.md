# Level00

## Flag

x24ti5gi3x0ol2eh4esiuxias

## Discovery

First, we need to find files related to the user `flag00`. We use:

```bash
find / -user "flag00" 2>/dev/null
```

`2>/dev/null` redirects the error stream to clean up output from permission errors (e.g., "Permission denied").
Command output:

```bash
/usr/sbin/john
/rofs/usr/sbin/john
```

Both files contain the string:

```text
cdiiddwpgswtgt
```

The string `cdiiddwpgswtgt` is not usable as-is to log in as flag00. Most likely, it is encrypted. Possible decryption methods:

* Caesar Cipher
* ROT
* Base64 Encoding
* XOR
* Simple character substitution
* Anagrams

Online tools like [dcode.fr](https://www.dcode.fr/) can help try different decoding algorithms.

On forums (GitHub, dcode.fr), several methods were tested:

* Chiffre ROT (Rotation)
* Caesar Code
* Base62 Encoding
* Monoalphabetic Substitution
* etc.

By trial, it was determined that the string is a Caesar Cipher with a shift. The first decoding attempt produced the password:

```text
nottoohardhere
```

This is the password of the user `flag00`.

## Use

Log in as user flag00 with the discovered password:

```bash
su flag00
Password: nottoohardhere
```

After successful login, the level flag can be retrieved.

## Prevention

Security measures to prevent such cases on real systems:

* **Do not store passwords in plain text**, even in restricted directories.
* **Use modern cryptographic methods** rather than simple ciphers (Caesar/ROT13).
* **Limit file access with least privilege principles**.
* **Perform regular file system audits** to detect suspicious data.
* **Use integrity monitoring** (AIDE, Tripwire) to detect unwanted changes in system files.

## Documentation

Methods used in the solution:

* **Caesar Cipher** — cipher with cyclic shift of each letter in the alphabet.
* **ROT** — variant of Caesar with a fixed shift (e.g., ROT13).
* **dcode.fr** — online tool for decryption, shift brute-forcing, frequency analysis.
* **`find`** — searching files based on file system attributes.

| If the string looks like…                   | Likely encoding method     |
| ------------------------------------------- | -------------------------- |
| Letters are shifted                         | Caesar, ROT13, Atbash      |
| Letters are mixed                           | Transposition Cipher       |
| Contains `=` , `/` , digits                 | Base64, Hex, Binary        |
| String seems random, length like a password | XOR                        |
| Very long string of characters              | Hash functions (MD5, SHA…) |
| Hidden inside file, image, or audio         | Steganography              |
