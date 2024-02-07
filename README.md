# Mukette: A Markdown Pager

**IT HAS BROKE. INTENTIONALLY. DOES NOT WORK UNTIL I FIX IT IN A FEW HOURS. DON'T USE UNLESS AN OLDER BRANCH.**

Mukette is a Markdown pager. It uses ncurses to format Makrodwn files in your terminal. There still remains truckload of things and features to be added (most importantly, link highlighting) but for the moment, you can build it.


```
sudo apt-get install ncurses flex
make
```

Then you can:

```
./mukette example-document.md
cat example-document.md | ./mukette
```

You can also compile the automata as an Graphviz FSM using:

```
sudo apt-get install graphviz
make graph
```

This will create `parse.png` which contains the Finite State Machine which I am using to parse Markdown with in Flex. You will need Graphviz.


## What is a Pager?

This basically acts like the `man` or `more` and `less` utility. It will display Makrodwn, formatted, in your terminal.



