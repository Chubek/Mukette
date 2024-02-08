# Mukette: A Pager for Markdown, for UNIX-based Systems


A 'pager' is a utility that takes a markup text (say, Roff) and displays that text, formatted, in the terminal. The first pager was `nroff(1)` for the aforementioned Roff, and you may still find `nroff(1)` on UNIX-based systems.

Mukette follows the same template. You pass it one, or several Markdown files, and it pages them. For example, you may use `example-document.md` to test Mukette.

```
mukette example-document.md
```

## Installing

You need ncurses and Flex to build Mukette. ncurses is probably already on your system. But just to be safe:

```
sudo apt-get install ncurses-dev flex
```

And then in the the root directory, run `make` and `make install`:

```
make && sudo make install
```

The man page will be installed alongside. The man page contains all the info about using Mukette:

```
man 1 mukette
```

## Disclaimer

Mukette is a v1 product, it still has a lot of rough edges. Report any bugs to Issues tab in the Github repository.


## Windows

On Windows you can use WSL2 or CygWin/MSys.


