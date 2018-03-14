#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <deque>
#include <fstream>
#include <sstream>
#include <regex>
#include <cstring>
#include <algorithm>
#include <time.h>
#include <mysql/mysql.h>

struct book {
	std::string name = "";
	std::vector<std::pair<std::string, std::string>> verses;
        unsigned int count = 0;
};
int cmp(const char *left, const char *right)
{
        char * dupl = strdup(left);
        char * dupr = strdup(right);

        std::deque<const char *> qleft;
        std::deque<const char *> qright;
        const char *delims = ":-,";

        char *tok = strtok(dupl, delims);
        while(tok) {
                qleft.push_front(tok);
                tok = strtok(NULL, delims);
        }

        tok = strtok(dupr, delims);
        while(tok) {
                qright.push_front(tok);
                tok = strtok(NULL, delims);
        }
        

        const char *pl = NULL;
        const char * pr = NULL;
        int il = 0;
        int ir = 0;

        while(!qleft.empty() && !qright.empty()) {
                pl = qleft.front();
                qleft.pop_front();
                pr = qright.front();
                qright.pop_front();
                il = atoi(pl);
                ir = atoi(pr);

                if(il != ir) 
                        break;
        }
        
        free(dupl);
        free(dupr);
        /* else il still == ir, the queue that is not empty is greater*/
        return (il != ir) ? il - ir : qleft.size() - qright.size();
}
struct verseless {
	bool operator()(const std::pair<std::string, std::string> &l,
			const std::pair<std::string, std::string> &r) const
	{
                
		std::string left = l.first;
		std::string right = r.first;
		return cmp(const_cast<char *>(left.c_str()), const_cast<char *>(right.c_str())) < 0;
        }
};

void parsesemicolon(std::vector<book> &v, std::string &chapter,
		    std::string &paragraph, std::string &footnote,
		    std::string &token)
{
	char *book = const_cast<char *>(token.c_str());
        while(isspace(*book) && *book != '\0')
                ++book;

	char *end = strstr(book, ":");
	if (end == NULL)
		return;

	while (!isspace(*end) && end > book)
		--end;

	if (end <= book)
		return;

	char *verses = end + 1;
	*end = '\0';

	std::string bk(book);
	std::string verse(verses);
	verse.erase(remove_if(verse.begin(), verse.end(), isspace), verse.end());
        std::string ref("LBC ");
        ref += chapter + "." + paragraph + "." + footnote;
        auto p = std::make_pair(verse, ref);

        for(size_t i=0; i < 66; i++)
                if(bk == v[i].name) {
                        v[i].count++;
                        v[i].verses.push_back(p);
                        break;
                }
}
void parsehash(std::vector<book> &v, std::string &chapter,
	       std::string &paragraph, std::string &token)
{
	// get footnote number, verses
	std::regex re("(^[0-9]+) (.*)");
	std::smatch m;
	std::string footnote = "";
	std::string verses = "";

	if (std::regex_search(token, m, re)) {
		footnote = m[1];
		verses = m[2];

                // tokenize on ;
		std::vector<std::string> vtok;
		char *tok = strtok(const_cast<char *>(verses.c_str()), ";");
		while (tok) {
			vtok.push_back(tok);
			tok = strtok(NULL, ";");
		}

		for (auto s : vtok)
			parsesemicolon(v, chapter, paragraph, footnote, s);
	}
}
void parseline(std::vector<book> &v, std::string &chapter,
	       std::string &paragraph, std::string &line)
{
	std::regex rev("\\((#[0-9]+.*?)\\)");
	std::string parens = "";
	std::smatch m;

	if (strstr(line.c_str(), "(")) {
		// capture refs within parens
		if (std::regex_search(line, m, rev)) {
			parens = m[1];
			assert(parens != "");

			std::vector<std::string> vtok;
			char *tok =
				strtok(const_cast<char *>(parens.c_str()), "#");
			while (tok) {
				vtok.push_back(tok);
				tok = strtok(NULL, "#");
			}

			for (auto s : vtok)
				parsehash(v, chapter, paragraph, s);
		}
	}
}
void parsefile(std::vector<book> &v, const std::string &filename)
{
	std::fstream fs(filename);
	std::regex re("Chapter ([0-9]+)");
	std::regex rep("Paragraph ([0-9]+)");
	std::smatch m;
	std::string line = "";
	std::string chapter = "";
	std::string paragraph = "";

	if (fs.is_open()) {
		while (!fs.eof()) {
			std::getline(fs, line);
			if (strstr(line.c_str(), "Chapter")) {
				std::regex_search(line, m, re);
				chapter = m[1];
				assert(chapter != "");

			} else if (strstr(line.c_str(), "Paragraph")) {
				std::regex_search(line, m, rep);
				paragraph = m[1];
				assert(paragraph != "");
				parseline(v, chapter, paragraph, line);
			}
		}
		fs.close();
	}
}
int main()
{
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

        clock_t start = clock();

	std::vector<book> v(70);
	for (size_t i = 0; i < 66; i++)
		v[i].name = books[i];

	parsefile(v, "lbc.txt");
        
        char insertbuf[256]={'\0'};
        MYSQL *con = mysql_init(nullptr);
        assert(con);
        int sqlret=0;

        assert(mysql_real_connect(con, "localhost", "root", "sqlpw", "reformedrefs", 0, nullptr, 0));
        
        const char *fmt = "INSERT INTO reftable VALUES('%s','%s','%s')";

	for (size_t i = 0; i < 66; i++) {
		if (!v[i].verses.empty()) {
			std::sort(std::begin(v[i].verses),
				  std::end(v[i].verses), verseless());
                        
                        char * bookname = const_cast<char*>(v[i].name.c_str());
			for (auto i : v[i].verses) {
                                sprintf(insertbuf, fmt, bookname, i.first.c_str(), i.second.c_str());
                                puts(insertbuf);
                                sqlret = mysql_query(con, insertbuf);
                                if(sqlret != 0) {
                                        printf("failed: %d", sqlret);
                                        goto cleanup;
                                }
                        }
		}
	}
cleanup:
        mysql_close(con);

        clock_t stop = clock();
        printf("Ticks: %ld\n", stop - start);
        printf("Seconds: %.4lf\n", (stop - start) / ((double) CLOCKS_PER_SEC));
}

