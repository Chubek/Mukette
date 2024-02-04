%{
#include <stdio.h>
%}

%option noyywrap

%x LineStart Line ImagePath Header1 Header2 Header3 Header4 Header5 Header6 CodeListing ImageAlt ListNumber ListBullet ListItem LinkName LinkURL FormatStart Italic Bold InlineCode Strikethrough TableCell TableSep Error

%%

^\.              {BEGIN LineStart; printf("LoadText\n");}
^#[ \t]+        {BEGIN Header1; printf("-\n");}
^##[ \t]+       {BEGIN Header2; printf("-\n");}
^###[ \t]+      {BEGIN Header3; printf("-\n");}
^####[ \t]+     {BEGIN Header4; printf("-\n");}
^#####[ \t]+    {BEGIN Header5; printf("-\n");}
^######[ \t]+   {BEGIN Header6; printf("-\n");}
^```$          {BEGIN CodeListing; printf("-\n");}
!\[             {BEGIN ImageAlt; printf("-\n");}
[1-9]+          {BEGIN ListNumber; printf("-\n");}
[*-]            {BEGIN ListBullet; printf("-\n");}
\|             {BEGIN TableCell; printf("-\n");}
.              {BEGIN Line; printf("-\n");}

<Line>{
    "["         {BEGIN LinkName; printf("-\n");}
    [-~_*`]     {BEGIN FormatStart; printf("-\n");}
    [\n]+       {BEGIN LineStart; printf("LineEndAction\n");}
    .+          {printf("AccumulateText\n");}
}

<Header1>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header1Action\n");}
}

<Header2>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header2Action\n");}
}

<Header3>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header3Action\n");}
}

<Header4>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header4Action\n");}
}

<Header5>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header5Action\n");}
}

<Header6>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("Header6Action\n");}
}

<ListNumber>{
    [0-9]       {printf("AccumulateText\n");}
    [\s\t-]     {BEGIN ListItem; printf("ListNumberAction\n");}
}

<ListBullet>{
    [*-]        {printf("AccumulateText\n");}
    [\s\t]      {BEGIN ListItem; printf("ListBulletAction\n");}
}

<ListItem>{
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN LineStart; printf("ListItemAction\n");}
}

<ImageAlt>{
    "]("        {BEGIN ImagePath; printf("ImageAltAction\n");}
    .+          {printf("AccumulateText\n");}
}

<ImagePath>{
    ")"         {BEGIN LineStart; printf("ImagePathAction\n");}
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
}

<LinkName>{
    "]("        {BEGIN LinkURL; printf("LinkNameAction\n");}
    .+          {printf("AccumulateText\n");}
}

<LinkURL>{
    ")"         {BEGIN Line; printf("LinkURLAction\n");}
   .+          {printf("AccumulateText\n");}
   [\n]+       {BEGIN Error; printf("-\n");}
}

<CodeListing>{
    ^.+$        {printf("AccumulateText\n");}
    "```"       {BEGIN LineStart; printf("CodeListingAction\n");}
}

<FormatStart>{
    "*"         {BEGIN Italic; printf("-\n");}
    "_"         {BEGIN Italic; printf("-\n");}
    "__"        {BEGIN Bold; printf("-\n");}
    "**"        {BEGIN Bold; printf("-\n");}
    "`"         {BEGIN InlineCode; printf("-\n");}
    "~~"        {BEGIN Strikethrough; printf("-\n");}
    .+          {printf("AccumulateText\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
}

<Italic>{
    "*"         {BEGIN Line; printf("ItalicAction\n");}
    "_"         {BEGIN Line; printf("ItalicAction\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
    .+          {printf("AccumulateText\n");}
}

<Bold>{
    "**"        {BEGIN Line; printf("BoldAction\n");}
    "__"        {BEGIN Line; printf("BoldAction\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
    .+          {printf("AccumulateText\n");}
}

<InlineCode>{
    "`"         {BEGIN Line; printf("CodeAction\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
    .+          {printf("AccumulateText\n");}
}

<Strikethrough>{
    "~~"        {BEGIN Line; printf("StrikethroughAction\n");}
    [\n]+       {BEGIN Error; printf("-\n");}
    .+          {printf("AccumulateText\n");}
}

<TableCell>{
    "|"         {printf("TableCellAction\n");}
    [\n]+       {printf("-\n");}
    .+          {printf("AccumulateText\n");}
}

<TableSep>{
    [\n]+       {BEGIN LineStart; printf("-\n");}
}

<Error>{
    [\n]+       {BEGIN LineStart; printf("-\n");}
    .+          {printf("-\n");}
}

%%

int main() {
    yylex();
    return 0;
}

