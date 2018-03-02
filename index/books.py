f = open('books.txt')

quotes = '"';

for line in f:
    print(quotes + line.rstrip() + quotes + ',', end='')

f.close()
