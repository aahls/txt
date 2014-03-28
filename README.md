txt
===
*txt* is a simple program for creating and editing short text notes using a command-line interface. *txt* runs on any \*nix operating system (GNU/Linux, BSD, Mac OS X...). *txt* is licensed under the MIT license.

Creating a new note, saying, for instance, "hello world", is as simple as typing `txt hello world` and hitting enter. To view all notes, just type `txt`.

Beyond this, *txt* supports tags, notes specific for the current working directory, specifying importance (notes with high importance levels will be highlighted) and sorting (by creation time etc.). This is a pretty spartan set of features, however, this is what *txt* tries to be: simple.

Creating notes
--------------
To **create a note** type `txt` followed by whatever text you want your note to contain. For instance, if you want to make a note to remind yourself to buy milk, type `txt buy milk`. Any **tags can be added** by specifying them using hashtags, for instance `txt buy milk #shopping`. Any number of tags can be added and the tags are automatically colored.

When you create a note you can **set the importance** of the note by using the `-i` option followed by a number specifying the importance (from 1 to 10). Notes with high importances will automatically appear as red.

Listing notes
-------------
To **list all notes** just type `txt` with no arguments.

To **filter notes by their tags** use the `-t` option followed by the tag you want filter by, e.g. `txt -t #bug`.

To **sort the listing** use the `-sc` to sort by creation time and `-si` to sort by importance. The `-I` option can be used to **invert the sorting order**.

Directory-specific notes
------------------------
By default, *txt* uses a database stored in the user's home directory. This is called the *global database* and is used whenever a *directory database* isn't found in the current working directory or any of its parent directories.

To **create a directory database** use the `-D` option.

To **access the global database** when a directory database exists use the `-G` option.

The directory database is stored in the directory in which it is created and thus it will automatically be copied if you copy the directory from one computer to another.

Other
-----
To **remove a note**, use the `-r` option followed by any number of note identifiers. For instance, to remove notes with IDs 1, 8 and 14 type `txt -r 1 8 14`.

To **add tags to an existing note** use the `-A` option followed by the tags you want to add prefixed with hashtags and the IDs of the notes you want to edit. The `-R` option works similarly to **remove existing tags**. For instance, to add the tags #butter and #cheese to notes 2, 3, 5 and 7 type `txt -A 2 3 5 7 #butter #cheese`. Had you used `-R` instead those tags would have been removed from those notes.
