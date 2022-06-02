# edit
Some filters for editing; center, align etc.

## Align
Some code style guides insist on aligning like so:

    some_variable       = "isn't this nice?"
    another_variable    = "look how the equal signs line up!"
    the_meaning_of_life = 42
    better_not_add      = "another longer one!"

which align makes easy to do. In vi you can mark the block
and pipe it to `align =` to format it like above.  
The default alignment character is space.

## Center
This centers a line on stdin. The default width is 80 characters
which you can change with a parameter.

