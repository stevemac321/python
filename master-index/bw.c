#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <mysql/mysql.h>
#include "books.h"

#ifndef _countof
#define _countof(x) sizeof(x) / sizeof(*(x))
#endif

enum cmdmode { ONE, CHAPTER, BOOK, ENTIRE };
struct tuple {
	const char *book;
	int chapter;
	int verse;
};
struct cmdargs {
	const char *version;
	const char *book;
	int nbook;
	int nchapter;
	enum cmdmode mode;
};

char *trim_leading(char *line);
char *trim_end(char *line);
void print_one(const char *version, char *verse, MYSQL *conbib, MYSQL *conref);
void print_chapter(const char *version, const char *book, const int nbook,
		   const int nchapter, MYSQL *conbib, MYSQL *conref);
void print_book(const char *version, const char *book, const int nbook,
		 MYSQL *conbib, MYSQL *conref);
void print_entire(const char *version, MYSQL *conbib, MYSQL *conref);
void interactive(const char *version, MYSQL *conbib, MYSQL *conref);
struct tuple tokenize(char *bookverse);
struct cmdargs parsecmdline(int argc, const char *argv[]);
int exit_usage(const char *msg);
size_t book_lookup(const char *book);

int main(int argc, const char *argv[])
{
	struct cmdargs args = parsecmdline(argc, argv);

	MYSQL *conbib = mysql_init(NULL);
	assert(conbib);

	assert(mysql_real_connect(conbib, "localhost", "root", "sqlpw", "bible",
				  0, NULL, 0));

	MYSQL *conref = mysql_init(NULL);
	assert(conref);

	assert(mysql_real_connect(conref, "localhost", "root", "sqlpw",
				  "reformedrefs", 0, NULL, 0));

	switch (args.mode) {
	case CHAPTER:
		print_chapter(args.version, args.book, args.nbook,
			      args.nchapter, conbib, conref);
		puts("");
		break;
        case BOOK:
                print_book(args.version, args.book, args.nbook, conbib, conref);
                puts("");
                break;
        case ENTIRE:
                print_entire(args.version, conbib, conref);
                puts("");
                break;
	default:
		interactive(args.version, conbib, conref);
		break;
	}

	mysql_close(conbib);
	mysql_close(conref);
}

char *trim_leading(char *line)
{
	char *p = line;

	while (isspace(*p) && *p != '\0')
		++p;

	line = p;
	return line;
}
char *trim_end(char *line)
{
	char *end = line;
	while (*end != '\0')
		++end;

	while (!isalnum(*end) && end > line) {
		*end = '\0';
		--end;
	}
	if (*end == '\r' || *end == '\n')
		*end = '\0';

	return line;
}
struct tuple tokenize(char *bookverse)
{
	struct tuple tup;
	char *p = strstr(bookverse, ":");
	assert(p);

	--p;
	while (isdigit(*p) && p > bookverse)
		--p;

	assert(p > bookverse);
	assert(isspace(*p));

	char *versepart = p + 1;

	*p = '\0';
	bookverse = trim_leading(bookverse);
	bookverse = trim_end(bookverse);

	tup.book = bookverse;

	char *chap = strtok(versepart, ":");
	char *vs = strtok(NULL, ":");
	assert(chap && vs);
	chap = trim_leading(chap);
	chap = trim_end(chap);
	vs = trim_leading(vs);
	vs = trim_end(vs);

	int nch = atoi(chap);
	assert(nch > 0);

	int nv = atoi(vs);
	assert(nv > 0);

	tup.chapter = nch;
	tup.verse = nv;

	return tup;
}
void print_one(const char *version, char *verse, MYSQL *conbib, MYSQL *conref)
{
	struct tuple tup = tokenize(verse);

	size_t len = strlen(tup.book);
	size_t book = 0;
	for (size_t i = 0; i < _countof(books); i++)
		if (strncmp(tup.book, books[i], len) == 0) {
			book = i + 1;
			break;
		}

	if (book == 0)
		return;

	char qbuf[128] = {'\0'};

	const char *fmtbib =
		"SELECT b,c,v,t  FROM t_kjv where b=('%d') AND c=('%d') AND v=('%d')";
	sprintf(qbuf, fmtbib, book, tup.chapter, tup.verse);
	mysql_query(conbib, qbuf);
	MYSQL_RES *resbib = mysql_store_result(conbib);

	int num_fields = mysql_num_fields(resbib);
	assert(num_fields == 4);

	MYSQL_ROW row;

	row = mysql_fetch_row(resbib);
	int dex = atoi(row[0]) - 1;
	printf("%s %s:%s %s\n", books[dex], row[1], row[2], row[3]);

	const char *fmtref =
		"SELECT ref FROM reftable where book=('%s') AND verse REGEXP('^%s')";
	char buf[12] = {'\0'};
	sprintf(buf, "%d:%d", tup.chapter, tup.verse);
	sprintf(qbuf, fmtref, books[dex], buf);
	mysql_query(conref, qbuf);
	MYSQL_RES *resref = mysql_store_result(conref);

	num_fields = mysql_num_fields(resref);

	while ((row = mysql_fetch_row(resref))) {
		for (int i = 0; i < num_fields; i++)
			if (row[i])
				printf("%s ", row[i]);
	}
	puts("\n--------------------------------------------------------------------------------------");

	if (resbib)
		mysql_free_result(resbib);

	if (resref)
		mysql_free_result(resref);
}
void print_chapter(const char *version, const char *book, const int nbook,
		   const int nchapter, MYSQL *conbib, MYSQL *conref)
{
	char bibbuf[128] = {'\0'};
	char refbuf[128] = {'\0'};
	char versebuf[12] = {'\0'};

	const char *fmtbib =
		"SELECT v,t  FROM t_kjv where b=('%d') AND c=('%d')";
	const char *fmtref =
		"SELECT ref FROM reftable where book=('%s') AND verse REGEXP('^%s')";

	sprintf(bibbuf, fmtbib, nbook, nchapter);
	mysql_query(conbib, bibbuf);

	MYSQL_RES *resbib = mysql_store_result(conbib);
	MYSQL_ROW row;
	MYSQL_RES *resref;
	int num_fields = mysql_num_fields(resbib);

	puts(book);
	printf("Chapter %d\n", nchapter);
	while ((row = mysql_fetch_row(resbib))) {
		printf("\n%s %s\t", row[0], row[1]);
		sprintf(versebuf, "%d:%d", nchapter, atoi(row[0]));
		sprintf(refbuf, fmtref, book, versebuf);
		mysql_query(conref, refbuf);
		resref = mysql_store_result(conref);
		num_fields = mysql_num_fields(resref);

		while ((row = mysql_fetch_row(resref))) {
			for (int i = 0; i < num_fields; i++)
				if (row[i])
					printf("%s ", row[i]);
		}
	}
	if (resbib)
		mysql_free_result(resbib);
	if (resref)
		mysql_free_result(resref);
}
void interactive(const char *version, MYSQL *conbib, MYSQL *conref)
{
	char *verse = NULL;
	size_t len = 0;
	ssize_t nread;

	while ((nread = getline(&verse, &len, stdin)) != -1) {
		if (strncmp("\\q", verse, 2) == 0) {
			break;
		} else {
			print_one(version, verse, conbib, conref);
		}
	}

	free(verse);
}

struct cmdargs parsecmdline(int argc, const char *argv[])
{
	struct cmdargs args = {"t_kjv", NULL, 0, 0, ONE};
	if (argc > 1) {
		if (*argv[1] != 't')
			exit_usage(NULL);

		args.version = argv[1];
	}

	if (argc > 2) {
		if (strcmp("entire", argv[2]) == 0) {
			args.mode = ENTIRE;
			return args;
		}

		int i = book_lookup(argv[2]);
		if (i == -1)
			return args;

		args.nbook = i + 1;
		args.book = books[i];
		args.mode = BOOK;
	}

	if (argc > 3) {
		args.nchapter = atoi(argv[3]);
		args.mode = CHAPTER;
	}

	return args;
}

int exit_usage(const char *msg)
{
	if (msg == NULL)
		puts("first arg is version, second arg is chapter, book, or entire (bible)");
	else
		puts(msg);

	exit(-1);
}
size_t book_lookup(const char *book)
{
	size_t len = strlen(book);
	for (size_t i = 0; i < _countof(books); i++) {
		if (strncmp(book, books[i], len) == 0)
			return i;
	}
	return -1;
}
void print_book(const char *version, const char *book, const int nbook,
		 MYSQL *conbib, MYSQL *conref)
{
        char bibbuf[128] = {'\0'};
	char refbuf[128] = {'\0'};
	char versebuf[12] = {'\0'};

	const char *fmtbib =
		"SELECT c,v,t  FROM t_kjv where b=('%d')";
	const char *fmtref =
		"SELECT ref FROM reftable where book=('%s') AND verse REGEXP('^%s')";

	sprintf(bibbuf, fmtbib, nbook);
	mysql_query(conbib, bibbuf);

	MYSQL_RES *resbib = mysql_store_result(conbib);
	MYSQL_ROW row;
	MYSQL_RES *resref;

	int num_fields = mysql_num_fields(resbib);
        int oldchapter=1;
        int newchapter=0;

        puts(book);
        printf("Chapter %d\n", oldchapter);
	while ((row = mysql_fetch_row(resbib))) {
                if((newchapter= atoi(row[0])) != oldchapter) {
                        printf("\nChapter %d\n", newchapter);
                        oldchapter = newchapter;
                }
                                        
		printf("\n%s %s\t", row[1], row[2]);
		sprintf(versebuf, "%d:%d", newchapter, atoi(row[1]));
		sprintf(refbuf, fmtref, book, versebuf);
		mysql_query(conref, refbuf);
		resref = mysql_store_result(conref);
		num_fields = mysql_num_fields(resref);

		while ((row = mysql_fetch_row(resref))) {
			for (int i = 0; i < num_fields; i++)
				if (row[i])
					printf("%s ", row[i]);
		}
	}
	if (resbib)
		mysql_free_result(resbib);
	if (resref)
		mysql_free_result(resref);

}
void print_entire(const char *version, MYSQL *conbib, MYSQL *conref)
{
       for(size_t i=0; i < _countof(books); i++) 
                print_book(version, books[i], i+1, conbib, conref);
       
}
