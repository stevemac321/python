#include <stdio.h>
#include <assert.h>
#include <mysql/mysql.h>
#include "books.h"

void print_book(MYSQL *con, const char *book)
{
        char qbuf[256]={'\0'};
        sprintf(qbuf, "select verse,ref from reftable where book=('%s') order by ABS(verse)", book);
        mysql_query(con, qbuf);
        MYSQL_RES *res = mysql_store_result(con);
        int num_fields = mysql_num_fields(res);

        printf("The Book of %s\n", book);
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res))) {
                if(row[0]) {
                        fprintf(stdout, "%-*s %*s\n", 14, row[0], 8, row[1]);
                }
        }
        puts("");
        if(res)
                mysql_free_result(res);
}
int main()
{
        MYSQL * con = mysql_init(NULL);
        assert(mysql_real_connect(con, "localhost", "root", "sqlpw", "reformedrefs", 0, NULL, 0));
       
        for(size_t i=0; i < _countof(books); i++)
                print_book(con, books[i]);

        if(con)
                mysql_close(con);
}
