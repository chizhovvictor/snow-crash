# Level00

## Flag

x24ti5gi3x0ol2eh4esiuxias

## Discovery

First, we need to find files associated with the `flag00` user. Use:

```bash
find / -user "flag00" 2>/dev/null
```

`2>/dev/null` — redirecting the error stream to clean the output from error messages (e.g., "Permission denied").
Command output:

```bash
/usr/sbin/john
/rofs/usr/sbin/john
```

Both files contain the string:

```text
cdiiddwpgswtgt
```

The obtained string `cdiiddwpgswtgt` does not work as‑is to log in as flag00. It is likely an encrypted string. Possible decoding methods:

* Caesar Cipher
* ROT
* Base64 Encoding
* XOR
* Simple substitution cipher
* Anagrams

Websites like [https://www.dcode.fr/](https://www.dcode.fr/) are also useful, where you can try different decoding algorithms.

**ROT11**
```bash
echo "cdiiddwpgswtgt" | tr 'a-z' 'l-za-k' 
```
On forums (GitHub, dcode.fr) several methods were tested:

* Chiffre ROT (Rotation)
* Code César
* Codage Base62
* Substitution Mono‑alphabétique
  and others.

Through trial and error, it turned out to be a Caesar Cipher with a shift, and in the first decoding attempt the password was obtained:

```text
nottoohardhere
```

This turned out to be the password for the `flag00` user.

## Use

Logging in as flag00 with the found password:

```bash
su flag00
Password: nottoohardhere
```

After a successful login, you can retrieve the level flag.

## Prevention

What security measures prevent such cases on real systems:

* **Do not store passwords in plain text** — even if they are placed in restricted directories.
* **Use modern cryptographic methods**, not simple ciphers (Caesar/ROT13).
* **Limit access to files based on the principle of least privilege**.
* **Perform regular filesystem audits** to detect suspicious data.
* **Use integrity checking tools** (AIDE, Tripwire) to detect unwanted changes in system files.

## Documentation

Methods used in the solution:

* **Caesar Cipher** — a cipher with a cyclic shift of each letter in the alphabet.
* **ROT** — a variety of Caesar cipher with a fixed shift (e.g., ROT13).
* **dcode.fr** — an online tool for decoding, shift brute‑forcing, frequency analysis.
* **`find`** — searching for files by filesystem attributes.

| If the string looks like…                   | Likely encoding method     |
| ------------------------------------------- | -------------------------- |
| Letters are shifted                         | Caesar, ROT13, Atbash      |
| Letters are mixed                           | Transposition Cipher       |
| Contains `=` , `/` , digits                 | Base64, Hex, Binary        |
| String seems random, length like a password | XOR                        |
| Very long string of characters              | Hash functions (MD5, SHA…) |
| Hidden inside file, image, or audio         | Steganography              |
