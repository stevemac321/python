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
char *trim_leading(char *line)
{
	char *p = line;

	while (isspace(*p) && *p != '\0')
		++p;

	line = p;
	return line;
}
char *remove_comma_ws(char *line)
{
	char *p = line;

	while (*p != '\0') {
		if (isspace(*p)) {
			char *end = p;
			while (*(end + 1) != '\0') {
				*end = *(end + 1);
				++end;
			}
			*end = '\0';
		}
		++p;
	}
	return line;
}
char *trim_verse_end(char *line)
{
	char *end = line;
	while (*end != '\0')
		++end;

	while (isspace(*end) && end > line) {
		*end = '\0';
		--end;
	}
	return line;
}
int cmp(const char *left, const char *right)
{
	char *dupl = strdup(left);
	char *dupr = strdup(right);

	std::deque<const char *> qleft;
	std::deque<const char *> qright;
	const char *delims = ":-,";

	char *tok = strtok(dupl, delims);
	while (tok) {
		qleft.push_front(tok);
		tok = strtok(NULL, delims);
	}

	tok = strtok(dupr, delims);
	while (tok) {
		qright.push_front(tok);
		tok = strtok(NULL, delims);
	}


	const char *pl = NULL;
	const char *pr = NULL;
	int il = 0;
	int ir = 0;

	while (!qleft.empty() && !qright.empty()) {
		pl = qleft.front();
		qleft.pop_front();
		pr = qright.front();
		qright.pop_front();
		il = atoi(pl);
		ir = atoi(pr);

		if (il != ir)
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
		return cmp(const_cast<char *>(left.c_str()),
			   const_cast<char *>(right.c_str()))
		       < 0;
	}
};
void parseverses(std::vector<book> &v, const size_t &question, char *verse)
{
	char *trimmed = trim_leading(verse);
	trimmed = trim_verse_end(trimmed);

	char *p = strstr(trimmed, ":");
	if (p == NULL)
		return;
	--p;

	while (isdigit(*p) && p > trimmed)
		--p;
	*p = '\0';
	++p;
	while (isspace(*p) && *p != '\0')
		++p;

	char *verses = remove_comma_ws(p);
	size_t len = strlen(trimmed);

	char refbuf[20];
	sprintf(refbuf, "LBCat Q. %zu", question);

	for (size_t i = 0; i < 66; i++) {
		if (strncmp(trimmed, v[i].name.c_str(), len) == 0) {
			v[i].count++;
			auto entry = std::make_pair(verses, refbuf);
			v[i].verses.push_back(entry);
			break;
		}
	}
}
void parseparens(std::vector<book> &v, const size_t &question,
		 std::string &line)
{
	char *begin = NULL;
	char *end = NULL;
	char *ln = const_cast<char *>(line.c_str());
	while ((begin = strstr(ln, "(")) && (end = strstr(ln, ")"))) {
		if (begin && end) {
			*end = '\0';
			if (strstr(begin, ":")) {
				begin++;
				break;
			} else {
				end++;
				ln = end;
			}
		}
	}

	if (begin == NULL || end == NULL)
		return;

	char *dup = strdup(begin);

	char *queue[20];
	size_t dex = 0;

	char *tok = strtok(dup, ";");
	while (tok) {
		queue[dex++] = tok;
		assert(dex < 20);
		tok = strtok(nullptr, ";");
	}

	for (size_t i = 0; i < dex; i++)
		parseverses(v, question, queue[i]);

	free(dup);
}
void parsefile(std::vector<book> &v, const std::string &filename)
{
	std::fstream fs(filename);
	std::regex re("^Q. ([0-9]+)");
	std::regex rev("\\((#[0-9]+.*?)\\)");
	std::smatch m;
	std::string line = "";
	size_t question = 0;
	std::string parens = "";

	if (fs.is_open()) {
		while (!fs.eof()) {
			std::getline(fs, line);
			if (strstr(line.c_str(), "Q.")) {
				std::regex_search(line, m, re);
				std::string t = m[1];
				question = atoi(t.c_str());

			} else if (strstr(line.c_str(), "(")
				   && strstr(line.c_str(), ":")) {
				std::regex_search(line, m, rev);
				parens = m[1];
				parseparens(v, question, line);
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

	parsefile(v, "catechism");
	char insertbuf[256] = {'\0'};
	MYSQL *con = mysql_init(nullptr);
	assert(con);
	int sqlret = 0;

	assert(mysql_real_connect(con, "localhost", "root", "sqlpw",
				  "reformedrefs", 0, nullptr, 0));

	const char *fmt = "INSERT INTO reftable VALUES('%s','%s','%s')";


	for (size_t i = 0; i < 66; i++) {
		if (!v[i].verses.empty()) {
			std::sort(std::begin(v[i].verses),
				  std::end(v[i].verses), verseless());

			char *bookname = const_cast<char *>(v[i].name.c_str());
			for (auto i : v[i].verses) {
				sprintf(insertbuf, fmt, bookname,
					i.first.c_str(), i.second.c_str());
				puts(insertbuf);
				sqlret = mysql_query(con, insertbuf);
				if (sqlret != 0) {
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
	printf("Seconds: %.4lf\n", (stop - start) / ((double)CLOCKS_PER_SEC));
}

