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
Usage: todome [directory path]

Search for TODOs, FIXMEs, BUGs, NOTEs and DONEs in projects.
Bishr Ghalil.
```
