#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <mysql/mysql.h>
#include "books.h"

#ifndef _countof
#define _countof(x) sizeof(x) / sizeof(*(x))
#endif

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

std::vector<int> print_xrefs(MYSQL *xrefcon, const int vid)
{
        std::vector<int> v;
        return v;
}

void print_refs(MYSQL *con, const char *book, const int nchap,
		 const int nverse)
{
	char vbuf[32] = {'\0'};
	char qbuf[128] = {'\0'};
        char out[128]={'\0'};
	const char *fmt =
		"SELECT ref FROM reftable where book=('%s') AND verse=('%s') ORDER BY verse ASC, ref ASC";
	sprintf(vbuf, "%d:%d", nchap, nverse);
        sprintf(qbuf, fmt, book, vbuf);
        mysql_query(con, qbuf);
        auto res = mysql_store_result(con);
        auto num_fields = mysql_num_fields(res);

        MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		for (int i = 0; i < num_fields; i++) {
			if (row[i]) 
                                fprintf(stdout, "%-*s %*s\n", 14, vbuf, 8, row[0]);
                }
        }
	if (res)
		mysql_free_result(res);
}
void print_book(const char *version, const char *book, const int nbook,
		MYSQL *conbib, MYSQL *conref, MYSQL *conxref, bool bxrefs=false)
{
        assert(version && book && conbib && conref && conxref);
        char qbuf[128] = {'\0'};
        const char * fmt = "SELECT id,c,v FROM %s WHERE b=('%d')";
        sprintf(qbuf, fmt, version, nbook);
        
        mysql_query(conbib, qbuf);
        auto res = mysql_store_result(conbib);
        auto num_fields = mysql_num_fields(res);

        puts(book);

        MYSQL_ROW row;

        while((row = mysql_fetch_row(res))) {
                if(row[0]) {
                        print_refs(conref, book, atoi(row[1]), atoi(row[2]));
                
                        if(bxrefs)
                                print_xrefs(conxref, atoi(row[0]));
                }
        }

}
void print_master(const char *version, bool bxrefs=false)
{
	mydb ref("reformedrefs");
	mydb xref("crossrefs");
	mydb bib("bible");

	ref.connect();
	xref.connect();
	bib.connect();
       size_t i; 
        for(i=0; i < _countof(books); i++)
                if(strcmp("Psalm", books[i]) == 0)
                        break;

	for (size_t i = 0; i < _countof(books); i++) 
		print_book(version, books[i], i + 1, bib.con, ref.con,
			   xref.con, bxrefs);
}
int main(const int argc, const char *argv[])
{
	print_master("t_asv");
}
