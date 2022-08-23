# Treebox
It is the only challenge I solved during the Google CTF 2022, and it took me approximately 5 hours to solve (considering the fact that it is my second live CTF experience, I am proud of the result). The challenge itself is available by [link](https://capturetheflag.withgoogle.com/challenges/sandbox-treebox)
## Description and code
Description: `I think I finally got Python sandboxing right.`

treebox.py
```python
#!/usr/bin/python3 -u
#
# Flag is in a file called "flag" in cwd.
#
# Quote from Dockerfile:
#   FROM ubuntu:22.04
#   RUN apt-get update && apt-get install -y python3
#
import ast
import sys
import os

def verify_secure(m):
  for x in ast.walk(m):
    match type(x):
      case (ast.Import|ast.ImportFrom|ast.Call):
        print(f"ERROR: Banned statement {x}")
        return False
  return True

abspath = os.path.abspath(__file__)
dname = os.path.dirname(abspath)
os.chdir(dname)

print("-- Please enter code (last line must contain only --END)")
source_code = ""
while True:
  line = sys.stdin.readline()
  if line.startswith("--END"):
    break
  source_code += line

tree = compile(source_code, "input.py", 'exec', flags=ast.PyCF_ONLY_AST)
if verify_secure(tree):  # Safe to execute!
  print("-- Executing safe code:")
  compiled = compile(source_code, "input.py", 'exec')
  exec(compiled)
```

## Thoughts flow during the CTF
The first thing I've done is generate the `flag` file inside the same directory as `treebox.py` to ensure that I have the environment as close as possible.
While looking through the code, it's easy to spot the `ast` module. The [module](https://docs.python.org/3/library/ast.html) allows for the generation of Abstract Syntax Trees. At the time of CTF and even now, IDK what the hell it is. I undersend that it converts all the fancy Python code to a more abstract one that is more suitable for code flow charts. So, based on that, I added the print statement that allowed me to see the abstract tree produced and spot anything interesting. 
The third and final piece of the puzzle - is banned statements. Code does not allow any `Import`, `ImportFrom`, and `Call` in the abstract syntax tree (produced only from ~user~ hacker input). `Import` and `InputFrom` are pretty self-explanatory. However, we have the third statement - `Call`. This statement appears in the abstract much more often than I wanted. It is mainly because of its ties with the Python syntax. The `Call` shows during functions call (who would have thought!) and during the methods call.

To sum up the first thoughts, we have:
- **abstract syntax trees**
- **hacker-controlled input**
- **banned Import, ImportFrom, and Call** 

After all the magic modifications (and some debugging with `ast`), the code looked like this:
```python
```
