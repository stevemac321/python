#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <mysql/mysql.h>

const char *books[] = {"Genesis",
		       "Exodus",
		       "Leviticus",
		       "Numbers",
		       "Deuteronomy",
		       "Joshua",
		       "Judges",
		       "Ruth",
		       "1 Samuel",
		       "2 Samuel",
		       "1 Kings",
		       "2 Kings",
		       "1 Chronicles",
		       "2 Chronicles",
		       "Ezra",
		       "Nehemiah",
		       "Esther",
		       "Job",
		       "Psalm",
		       "Proverbs",
		       "Ecclesiastes",
		       "Song of Solomon",
		       "Isaiah",
		       "Jeremiah",
		       "Lamentations",
		       "Ezekiel",
		       "Daniel",
		       "Hosea",
		       "Joel",
		       "Amos",
		       "Obadiah",
		       "Jonah",
		       "Micah",
		       "Nahum",
		       "Habakkuk",
		       "Zephaniah",
		       "Haggai",
		       "Zechariah",
		       "Malachi",
		       "Matthew",
		       "Mark",
		       "Luke",
		       "John",
		       "Acts",
		       "Romans",
		       "1 Corinthians",
		       "2 Corinthians",
		       "Galatians",
		       "Ephesians",
		       "Philippians",
		       "Colossians",
		       "1 Thessalonians",
		       "2 Thessalonians",
		       "1 Timothy",
		       "2 Timothy",
		       "Titus",
		       "Philemon",
		       "Hebrews",
		       "James",
		       "1 Peter",
		       "2 Peter",
		       "1 John",
		       "2 John",
		       "3 John",
		       "Jude",
		       "Revelation"};

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
void lookup(char *verse, MYSQL *con, MYSQL *con2)
{
	char *p = strstr(verse, ":");
	if (p == NULL)
		return;

	--p;
	while (isdigit(*p) && p > verse)
		--p;

	if (p <= verse)
		return;
	if (!isspace(*p))
		return;

        char * versepart = p + 1;

        *p = '\0';
        verse = trim_leading(verse);
        verse = trim_end(verse);

	/* now verse is the name of the book, p is the start of verse*/
        size_t len = strlen(verse);
	size_t book = 0;
        for(size_t i=0; i < 66; i++) 
                if(strncmp(verse, books[i], len) == 0) {
                        book = i + 1;
                        break;
                }

        if(book == 0)
                return;

        char * chap = strtok(versepart, ":");
        char * vs = strtok(NULL, ":");
        assert(chap && vs);
        chap = trim_leading(chap);
        chap = trim_end(chap);
        vs = trim_leading(vs);
        vs = trim_end(vs);

        int nch = atoi(chap);
        assert(nch > 0);

        int nv = atoi(vs);
        assert(nv > 0);
	
        char qbuf[128] = {'\0'};
	
	const char *fmt = "SELECT b,c,v,t  FROM t_kjv where b=('%d') AND c=('%d') AND v=('%d')";
        sprintf(qbuf, fmt, book, nch, nv);
        mysql_query(con, qbuf);
        MYSQL_RES *res = mysql_store_result(con);

        int num_fields = mysql_num_fields(res);
        assert(num_fields == 4);

        MYSQL_ROW row;

        row = mysql_fetch_row(res);
        int dex = atoi(row[0]) - 1;
        printf("%s %s:%s %s\n", books[dex], row[1], row[2], row[3]);

	const char *fmt2 = "SELECT ref FROM reftable where book=('%s') AND LOCATE('%s', verse)";
        char buf[12]={'\0'};
        sprintf(buf, "%d:%d", nch, nv);
        sprintf(qbuf, fmt2, books[dex], buf);
        mysql_query(con2, qbuf);
        MYSQL_RES *res2 = mysql_store_result(con2);

        num_fields = mysql_num_fields(res2);

        while((row = mysql_fetch_row(res2))) {
                for(int i=0; i < num_fields; i++)
                        if(row[i])
                                printf("%s ", row[i]);
        
        }
        puts("\n--------------------------------------------------------------------------------------");

        if(res)
                mysql_free_result(res);

        if(res)
                mysql_free_result(res2);
}
int main()
{
	char *verse = NULL;
        size_t len = 0;
        ssize_t nread;
        
        MYSQL *con = mysql_init(NULL);
	assert(con);

	assert(mysql_real_connect(con, "localhost", "root", "sqlpw",
				  "bible", 0, NULL, 0));

        MYSQL *con2 = mysql_init(NULL);
        assert(con2);

	assert(mysql_real_connect(con2, "localhost", "root", "sqlpw",
				  "reformedrefs", 0, NULL, 0));

	while ((nread = getline(&verse, &len, stdin)) != -1) {
		if (strncmp("\\q", verse, 2) == 0) {
			break;
                } else {
			lookup(verse, con, con2);
                }
	}

	free(verse);
        mysql_close(con);
        mysql_close(con2);
}
