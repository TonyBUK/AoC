# Day 7: No Space Left On Device

https://adventofcode.com/2022/day/7

Todays problem is essentially a tree problem, whereby we'll be asked to replicate a directory structure on a computer, and interrogate it in order to determine collective file sizes, and the collective sizes of files in sub-folders.

Note: For my sanity, I'm going to use directory/folder interchangeably.

The method for doing this is to ask us to interpret two commands:

cd (Change Directory)
ls (Listing)

The cd command will also have three flavours:

cd / (change directory to the top most (root) folder)
cd {name} (change directory to a named sub-directory)
cd .. (change directory up one level)

Whereas the ls command, when run will then tell us...

1. All the folders (if any) in the current folder.
2. All the files (if any), and their respective sizes in the current folder.

How hard can it be...


**Part 1**

So this all sounds pretty scary, but there are a few short-cuts that somewhat trivialise it.

But first off, what exactly is a tree... consider the following.

    root
    ├── child1
    ├─┬ child2
    │ ├── grandchild1
    │ └── grandchild2
    └── child3

This if we take this and break it down a bit.  We have the first level:

    root
    ├── child1
    ├─- child2
    └── child3

And from here we have a second:

    ├─┬ child2
    │ ├── grandchild1
    │ └── grandchild2

And those grandchildren could have children, and so-on.

Kinda looks like folders on a computer doesn't it!  So how would we express that using a program?  Well, let's start with easy mode.

Easy Mode:

One property folders on a computer have is uniqueness.  That is at any given level, all items need to have unique names.  i.e. if we take the classic, C:\Windows\, you only have *one* of these on your computer.  If there were two directories names Windows at the root of the drive, how would you know which to use.  So we can express the structure above as:

    /root/
    /root/child1/
    /root/child2/
    /root/child2/grandchild1/
    /root/child2/grandchild2/
    /root/child3/

I've added the "/" separators to guarentee uniqueness.  As consider:

    root
    ├── child2grandchild1
    ├─┬ child2
    │ ├── grandchild1
    │ └── grandchild2
    └── child3

We need to distinguish:

    /root/child2grandchild1/

from:

    /root/child2/grandchild1/

The first is a deliberately obtuse folder name written by a bad person but is only 1 folder deep, whereas the second is 2 folders deep.

So what we have is uniqueness of each of the items in the tree as text, which sounds an awful lot like a hash map to me!

Meaning you can store the data as a bog standard dictionary/map, and all you need to store within it are:

- Directories (so that you can construct a new string to point to any children)
- Files (so that you can solve the puzzle)

And traversing up/down the tree is a matter of adding/removing directory names from the text string.

So let's do some pseudo-code...

    FOR each command

      IF it's a cd command

        IF it's a cd / command

          Set the current target to "/"

        ELSE IF it's a cd .. command

          Remove one level of "???/" from the current target

        ELSE

          Append the directory name to the current target, and add a trailing "/"

        END

      ELSE IF it's a ls command

        Nothing to do... yet.

      ELSE (it's not a command, but the output of an ls command)

        IF the item is a directory (first item is "dir")

          Append the directory to the current targets list of directories

        ELSE (first item is a number)

          Append the file and file size to the current targets list of files

        END

      END

    NEXT

So to put this into action, let's walk through the example:

    $ cd /
    => Set the current target to "/"
    => Current Target = "/"

    $ ls
    => Nothing to do... yet.

    dir a
    => Append the directory to the current targets list of directories
    => Add "a" to Targets[Current Target].Directorys[]

    14848514 b.txt
    => Append the file and file size to the current targets list of files
    => Add ["b.txt", 14848514] to Targets[Current Target].Files[]

    8504156 c.dat
    => Append the file and file size to the current targets list of files
    => Add ["c.dat", 8504156] to Targets[Current Target].Files[]

    dir d
    => Append the directory to the current targets list of directories
    => Add "d" to Targets[Current Target].Directorys[]

    $ cd a
    => Append the directory name to the current target, and add a trailing "/"
    => Current Target = "/a/"

    $ ls
    => Nothing to do... yet.

    dir e
    => Append the directory to the current targets list of directories
    => Add "e" to Targets[Current Target].Directorys[]

    29116 f
    => Append the file and file size to the current targets list of files
    => Add ["f", 29116] to Targets[Current Target].Files[]

    2557 g
    => Append the file and file size to the current targets list of files
    => Add ["g", 2557] to Targets[Current Target].Files[]

    62596 h.lst
    => Append the file and file size to the current targets list of files
    => Add ["h.lst", 62596] to Targets[Current Target].Files[]

    $ cd e
    => Append the directory name to the current target, and add a trailing "/"
    => Current Target = "/a/e/"

    $ ls
    => Nothing to do... yet.

    584 i
    => Append the file and file size to the current targets list of files
    => Add ["i", 584] to Targets[Current Target].Files[]

    $ cd ..
    => Remove one level of "???/" from the current target
    => Current Target = "/a/"

    $ cd ..
    => Remove one level of "???/" from the current target
    => Current Target = "/"

    $ cd d
    => Append the directory name to the current target, and add a trailing "/"
    => Current Target = "/d/"

    $ ls
    => Nothing to do... yet.

    4060174 j
    => Append the file and file size to the current targets list of files
    => Add ["j", 4060174] to Targets[Current Target].Files[]

    8033020 d.log
    => Append the file and file size to the current targets list of files
    => Add ["d.log", 8033020] to Targets[Current Target].Files[]

    5626152 d.ext
    => Append the file and file size to the current targets list of files
    => Add ["d.ext", 5626152] to Targets[Current Target].Files[]

    7214296 k
    => Append the file and file size to the current targets list of files
    => Add ["k", 7214296] to Targets[Current Target].Files[]

So with this in place, we should have a workable tree, but neatly one we can traverse as a 1D Array as well, which simplifies solving part 1, as we need to be able to "walk" the entire tree (i.e. visit every single part of the tree we've created).

The puzzle requires the ability for any given folder, to find out the size of all the files within that folder, plus the size of files in any sub folders, or sub folders of sub folders etc.

This is really a recursive problem.  It goes something like:

    RECURSIVE SIZE FUNCTION (Current Directory)

      Calculate the combined size of all files in the current directory

      FOR each Sub Directory in the Current Directory

        Set the New Directory to the Current Directory + Sub Directory + "/"

        Add to the combined size the result of RECURSIVE SIZE FUNCTION (New Directory)

      NEXT

      Return the combined size

    END

In the example we've previously walked, if we fed the RECURSIVE FUNCTION the root directory ("/") it would go something like in terms of recursive calls:

    RECURSIVE SIZE FUNCTION("/")

      RECURSIVE SIZE FUNCTION("/a")

        RECURSIVE SIZE FUNCTION("/a/e/")
        END

      END

      RECURSIVE SIZE FUNCTION("/d/")
      END

    END

So the idea now is just to walk the entire folder list and get the recursive sizes of each directory.

    FOR Each Key in Targets (the key is what we indexed by, which is the full path)

      Use the RECURSIVE SIZE FUNCTION to get the size of the Item

    NEXT

And to solve it, we need to find the sum of all Recursive Sizes that are <= 100000.

Hard Mode:

Now a hash map is one way of doing this, you can also solve this with nodes.  A Node is simply a data structure that contains within it some data, but also, some way of referencing other nodes.  In truth the hashmap does qualify for this, but the more traditional way is something like:

    NODE
    {
      File Data   : Size, Name
      Directories : NODES
      Parent : NODE (Optional)
    }

i.e. Within the structure is references to other structures of it's parent type.  Or to be less computer sciency, it's a thing that contains information about other things (which contain information about other things (which contain information about other things)).

Loading the nodes isn't that different to the Hash Map solution:

    FOR each command

      IF it's a cd command

        IF it's a cd / command

          Set the current node to the root node

        ELSE IF it's a cd .. command

          Set the current node to its parent node

        ELSE

          Add a new sub-node for the current directory

          Set the parent of the new sub-node to the current node

          Set the current node to the new sub-node

        END

      ELSE IF it's a ls command

        Nothing to do... yet.

      ELSE (it's not a command, but the output of an ls command)

        IF the item is a directory (first item is "dir")

          Nothing to do

        ELSE (first item is a number)

          Append the file and file size to the current nodes list of files

        END

      END

    NEXT

There's one minor change, and that is now there's no real reason to list the directories, as the act of creating the node also implicitly stores a reference to that node.  So when listing, we now only care about files.

What makes this solution trickier is the process of "walking" the tree.  Where before, we had a nice FOR loop, as the hash map effectively gave us nodes, but also a 1D Array we can iterate on, a pure NODE solution needs to be walked, typically recursively.

i.e.

    RECURSIVE NODE FUNCTION (Current Node)

      Do Something
      
      FOR Each Directory (Sub Node)

        RECURSIVE NODE FUNCTION (Sub Node)

      NEXT

    END

So if we wanted to get the Cursive Size of any Node...

    RECURSIVE NODE SIZE FUNCTION (Current Node)

      Set Total Size to 0
      
      Add the Current Folders file sizes to the Total Size
      
      FOR Each Directory (Sub Node)

        Add the results of RECURSIVE NODE SIZE FUNCTION (Sub Node) to the Total Size

      NEXT

      RETURN the Total Size

    END

We'd then want another recursive function to walk the node stack for the 100000 check.

    RECURSIVE NODE SOLVER FUNCTION (Current Node, Old Small Node Size)

      Set the Current Node Size to RECURSIVE NODE SIZE FUNCTION for the Current Node

      Set the New Small Node Size to the Old Small Node Size

      IF the Current Node Size <= 100000

        Add the Current Node Size to the New Small Node Size

      END

      FOR Each Directory (Sub Node)

        Add the results of RECURSIVE NODE SOLVER FUNCTION (Sub Node, New Small Node Size) to the New Small Node Size

      NEXT

      RETURN New Small Node Size

    END

So much recursion!  But ultimately you get the answer by running:

    Answer = RECURSIVE NODE SOLVER FUNCTION (Root Node, 0)

**Part 2**

Note: This assumes easy mode.  Hard Mode follows the same principles as Part 1, but with a different target.

We've done all the heavy lifting now, this is very similar to the Part 1 solution, except now we're looking for the smallest Recursive Size that will free up enough space on the disk.

To calculate how much space needs to be freed up, we need to:

1. Calculate how much space is used already, which is the recursive size of "/"
2. Subtract from it: (70000000 - 30000000), which is (Total Disk Space - Neccesary Unused Space)

So it will go something like:

    Calculate how much space needs to be free'd

    FOR Each Key in Targets (the key is what we indexed by, which is the full path)

      Use the RECURSIVE SIZE FUNCTION to get the size of the Item

      IF this size is >= the needed space to be free'd

        IF the size is smaller than any of the other sizes that were deemed to be big enough

          Use this size (for now)
        
        END

      ENX

    NEXT

Problem solved.

**Python vs C++ vs C**

**Python**

Very much easy mode here, pretty much verbatum.  The main difference is instead of string manipulation to change paths, it uses an array stack, and the "".join() method whenever it's needed as a string.  This makes it easy to pop paths.

i.e. instead of "/a/b/c/" it would be:

["/", "a/", "b/", "c/"]

**C++**

TBD - I'm prioritising Python/C for my first pass of this year.

**C**

The is hard mode but both easier and much much harder.  What this solution takes advantage of is:

1. No input lists the same folder more than once, meaning we don't need duplicate detection.
2. We don't need to store folder names.
3. We don't need to store files, just total sizes for the folders.
4. As we add each new folder, we can update the sizes of the parent folders by walking the new size delta up to all the parent nodes until we hit the root node, that's because we never need anything other than the recursive size.
5. Because this is C and we can do what we want, it's possible to store pointers to every node in a 1D array as well, so that we can traverse by node, but also by a simple array.

Essentially this is a tree solution that never recurses.