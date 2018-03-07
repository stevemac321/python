all:
	clang -std=c11 -ggdb  main.c tree.c -o dex
san:
	scan-build clang -std=c11 -ggdb -fsanitize=address main.c tree.c -o dex
rel:
	
	clang -std=c11 -O3 main.c tree.c -o dex
