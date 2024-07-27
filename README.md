# sussy
𐐘 sussy is my programming language that allows you to express your inner imposter ඞ

## syntax
sussy's syntax is inspired by my schizophrenia.
- **Every line must end with `;`!!!!!**
- Both single- and multiline **comments** are made like this: `# Hello #`
- **Variables** are defined just like in python: `a = 5;`
- **Functions** are defined much more sussier: `func = (arg1, arg2) { # body # }`
- - As for the `body` part, if the body is just a single line, it can be made like this: `func = (arg1, arg2) -> return arg1 * arg2;`
- - > **Hint!**  
    >> In the prevous example the use of `return` is unnecessary! The result of the last operation will always be the return value automatically
- **Conditions** - `if`, `elseif` and `else`:
```c++
...
if(a == 5 && b != 10) ->
    println("a is 5 and b is not 10...");
elseif(a > 10)
{
    b *= a;
    println(b);
}
else -> println("amogus");
```
- **Loops** - sussy currently supports `for` and `while` loops
```cpp
for(i => 0, 10) # operator => is called `range` #
{
    println(i);
}
while(condition) -> ...;
```
- **Arrays** - currently they are defined like this: `array = arraySize:valueToFillWith;` or `array = 1, 2, 3, 4, 5;`, as for example: `array = 10:0; # an array of 10 zeroes #`. You can access elements of arrays and strings with `:`: `a = array:0;`, `a = "hello":4`
- **Strings** can be either defined with '' or "" with no differences
- **Multiple files** - you can use `import "filename.sus"` to include something into your main file
- **Default functions**:
```c++
print(arg); # write a single line, no return #
println(arg); # write a single line with a new line, no return #
input(); # ask for the input, returns string #
int(arg); # convert arg to int #
float(arg); # convert arg to float #
string(arg); # convert arg to string #
size(arg); # returns the size of the arg (array or string) #
```

## examples
rle algorithm:
```c++
input = "UUUUUUKKKKKKRRRRRREEEEEEYYYYYYYYYBBBBBBBB";
compressed = "";
prev = input:0;
length = 0;

println("Input data: " + input);

for(i => 0, size(input) + 1)
{
    if(prev != input:i)
    {
        compressed += string(length) + prev;
        length = 0;
    }
    length++;
    prev = input:i;
}

println("Output data: " + compressed);
```
a simple calculator (see test.sus, input.sus and math.sus)
