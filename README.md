# Mukette: A Markdown Pager (WIP, but works!)


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


## Update (7:21PM - Feb 6)

I added tables to it. Mind you rendering is very simple. Ncurses is not known for being a breeze to work with.




