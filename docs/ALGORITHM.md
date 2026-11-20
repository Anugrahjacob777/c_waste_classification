# Algorithm

## Main program

1. START
2. Show the main menu: 1 Text based, 2 File based, 3 Camera/Image, 4 View summary, 0 Exit
3. Read the choice (using `fgets` + `sscanf`, so wrong input never crashes the program)
4. Use a `switch` on the choice
   - **1 Text based**: repeat the steps in *Text mode* below
   - **2 File based**: follow *File mode* below
   - **3 Camera / Image**: show "Under construction" and wait for `0` to go back
   - **4**: show the summary screen
   - **0**: print a thank-you message and STOP
   - anything else: print "Invalid choice" and show the menu again
5. Go back to step 2

## Text mode

1. Read a line typed by the user
2. Convert it to lowercase and remove extra spaces
3. If it is empty, ask the user to type something
4. If it is `back` or `0`, return to the main menu
5. If it is `finish`, open the summary screen
6. Otherwise call **classify item** (below) and go back to step 1

## File mode

1. Show: 1 all items file, 2 choose a test file, 3 type my own file name, 0 back
2. Open the chosen file (a typed name is searched as typed, then in `data/`, then in `tests/`)
3. If it cannot be opened, show an error message and return to the file menu
4. Read the file line by line until the end of the file
   - skip blank lines
   - clean the line (lowercase, trim spaces) and call **classify item**
5. Show "n lines processed" and ask: 1 show summary, 0 back

## Classify item (rule based, not AI)

1. Set `i = 0`
2. While `i` is less than the number of keyword rules
   - if keyword `i` appears in the item as a **whole word** (plurals such as `bottles` are accepted, but `carpet` does not match `pet`), the class is the class of rule `i`; stop searching
   - otherwise `i = i + 1`
3. If no rule matched: unknown count + 1, show a help message, STOP
4. Count + 1 for the class
5. Decide the type
   - if the item contains a special word (`broken`, `bulb`, `blade`, `tube`, `sharp` -> HANDLE WITH CARE; `polythene`, `wrapper`, `straw`, `thermocol`, `plastic bag` -> NON-RECYCLABLE) use that type
   - otherwise use the default type of the class (Paper, Plastic, Glass, Metal -> RECYCLABLE; Organic -> COMPOSTABLE; E-waste -> HANDLE WITH CARE)
6. Show the class, the type and the disposal tip

Keyword order matters: material words (plastic, glass, steel ...) are checked first, so
`steel bottle` is Metal and `plastic bottle` is Plastic, even though `bottle` alone means Glass.

## Summary screen

1. Show total classified and unknown items
2. Show the count per class and per type **only for entries with at least one item**
3. For every class with at least one item:
   - quantity is **Small** if the count is 10 or less, otherwise **Large (bulk)**
   - show the Bengaluru waste management contact for that class and quantity
   - show the disposal tip
4. Show the list of items with their class and type
5. Menu: 1 save to `export/`, 2 start a new session, 0 back

## Save

1. Open `export/waste_summary.txt` for writing and print the same summary into it
2. Open `export/classified_items.csv` and write one line per item: `item,class,type`
3. Close both files
