#include <cstdio>
#include <cassert>
#include <cstring>
#include <mysql/mysql.h>
#include "books.h"

struct mydb {
	MYSQL *con = nullptr;

	const char *dbname = nullptr;
	const char *server = "localhost";
	const char *user = "root";
	const char *pw = "sqlpw";

	mydb(const char *name)
	{
		dbname = name;
	}
	~mydb()
	{
		if (con)
			mysql_close(con);
	}

	MYSQL *connect()
	{
		con = mysql_init(nullptr);
		assert(con);
		return mysql_real_connect(con, server, user, pw, dbname, 0,
					  nullptr, 0);
	}
};
void print_one(const int nbook, const int nchap, const int nverse)
{
        char verse[64];
        sprintf(verse, "%s %d:%d ", books[nbook-1], nchap, nverse);
        printf("%s",verse);
        
}
void get_refs(MYSQL *con, const int id, const int sv, const int ev)
{
        char buf[128];
        sprintf(buf, "select id,b,c,v from t_kjv where id between %d and %d", sv, ev);

	mysql_query(con, buf);
	MYSQL_RES *res = mysql_store_result(con);
	int nfields = mysql_num_fields(res);

	MYSQL_ROW row;
        while((row= mysql_fetch_row(res))) 
                if(row[0]) 
                        print_one(atoi(row[1]), atoi(row[2]), atoi(row[3]));                
                
        if(res)
                mysql_free_result(res);

}
void print_refs(MYSQL *bibcon, MYSQL *conref, const int vid)
{

	char bufref[128];
	sprintf(bufref, "select vid,r,sv,ev from cross_reference where vid=%d order by r DESC", vid);
	mysql_query(conref, bufref);

	MYSQL_RES *res = mysql_store_result(conref);
	int nfields = mysql_num_fields(res);

	MYSQL_ROW row; 

	while((row = mysql_fetch_row(res))) {
                if(row[0])
                        get_refs(bibcon, atoi(row[0]), atoi(row[2]), atoi(row[3]));
        }
	if (res)
		mysql_free_result(res);
}

void print_verse(MYSQL *conbib, MYSQL *conref, const char *book,
		 const int nbook, const int nchap, const int nverse,
		 const char *version = "t_kjv")
{
	char bufbib[128];
	sprintf(bufbib, "select id,t from %s where b=%d and c=%d and v=%d", version, nbook, nchap, nverse);

	mysql_query(conbib, bufbib);

	MYSQL_RES *res = mysql_store_result(conbib);
	int nfields = mysql_num_fields(res);

	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		if (row[0]) {
                        printf("%s %d:%d %s\n", book, nchap, nverse, row[1]);
                        print_refs(conbib, conref, atoi(row[0]));
		}
	}
        puts("");
	if (res)
		mysql_free_result(res);
}
int getdex(const char *book)
{
	for (size_t i = 0; i < _countof(books); i++)
		if (strcmp(book, books[i]) == 0)
			return i;

	return -1;
}
int main(const int argc, const char *argv[])
{
        if(argc < 5) {
                puts("pass version, bookname, chapter, and verse deliniated by spaces");
                return -1;
        }
	const char *version = argv[1];

	mydb bib("bible");
	mydb ref("crossrefs");

	assert(bib.connect());
	assert(ref.connect());

	int dex = getdex(argv[2]);
	print_verse(bib.con, ref.con, books[dex], dex + 1, atoi(argv[3]), atoi(argv[4]), version);
}
