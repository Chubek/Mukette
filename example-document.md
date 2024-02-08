## Header1

This is a **bold text**. This is a normal text. This is an *italitc text*. And supposably, this is a [hyperlink to example.com](https://example.com). You can navigate to it by pressing `tab`. Then, you will cycle through all the hyperlinks (and images turned hyperlink) in the document.


Below, you can see a table:

| Cell1 | Cell2 | Cell3 |
|-------|-------|-------|
| Cell4 | Cell5 | Cell6 |


This is `(char*)"inline code"` --- and this is a code listing:

```
/e/ { print $0; }
function abs(i,j,c)
{
   printf "%d%d%d", i, j, c;
}
```

And this is a code listing with a tag:

```python
def abs(num: int):
    return num if num >= 0 else -num
```

These will be marked witn the tag, and the number of the code listing.


And now, this is [another link to gnu.org](https://gnu.org). This website is about raising cattle, especially Bison. I actually used to think Bison is pronounced as 'Bee-zon'! It's 'by-cen' lol.

Did you know there used to be an species of Bison-related cattle known as 'Aurochs' who lived all across Europe and Asia Minor, until they went extint in the 17th century? I have named my compiler, [Orax](https://github.com/Chubek/Orax) after them.


Another project I am working on is [Oryx](https://github.com/Chubek/Oryx). Orax and Oryx are sisters. Oryx is a non-compliant operating system. That means I am planning to make it neither POSIX --- nor SuS. I think POSIX and SuS are outdated standards.

It's funny, I ask ChatGPT to generate me data sturctures and x86-64 Assembly codoes fo Oryx, and it does! Also, I had ChatGPT generated the SSA for Orax. Not that I haven't learned nothing.


Anyways. Let's see how Mukette handles lists?

- Unordered List 1-1
- Unordered List 1-2

* Unordered List 2-1
* Unordered List 2-2

1. Ordered List 1
2. Ordered List 2

For now, Mukette does nothing with lists. This won't be the case forever.


A bold+italic text -> ***texxxxxt***.



