# TODOME

Language agnostic tool that collects TODOs, FIXMEs, BUGs, NOTEs and DONEs in the source code. 

Urgency of "TODO", "FIXME", "BUG" and "NOTE" is indicated by repetitions of the final character.
For example, one might write FIXMEEEEEEEE for an important issue. Output will be sorted by priority then priority level (BUG, FIXME, TODO, NOTE, DONE).

## Install
```
git clone https://github.com/BishrGhalil/todome.git
cd todome
sudo make install
```
--- 
#### Arch linux
Arch linux users can install it from the [aur](https://aur.archlinux.org/packages/todome-git/).

Or using an aur helper such as `yay, paru, ...etc`.
```
yay -S todome-git
```

## Usage

```
Usage: todome [OPTION...] PATH
todome -- Language agnostic tool that collects TODOs, FIXMEs, BUGs, NOTEs and
DONEs in the source code.

  -b, --bug                  Collects BUGs
  -d, --done                 Collects DONEs
  -f, --fixme                Collects FIXMEs
  -h, --hidden               Searches hiddens
  -i, --input[=FILE]         Search in a provided file
  -n, --note                 Collects NOTEs
  -o, --output[=FILE]        Output to FILE
  -s, --no-colors            Don't output colors
  -t, --todo                 Collects TODOs
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to beshrghalil@protonmail.com.
```
